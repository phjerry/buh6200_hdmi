/******************************************************************************
  * Copyright (C), 2009-2019, Hisilicon Tech. Co., Ltd.
  * Description   :
  * Author        : Hisilicon multimedia software group
  * Create        : 2009/12/21
  * History       :
 *******************************************************************************/
#include "drv_avplay.h"

#include "hi_drv_module.h"
#include "hi_drv_demux.h"
#include "hi_drv_sync.h"
#include "drv_vdec_ext.h"

#define BIG_ENDIAN_WRITE(a, b) (((a) << 32) | (b))

typedef enum {
    AVPLAY_PARAM_INC = 0, /* 递增 */
    AVPLAY_PARAM_ADD,     /* 累加 */
    AVPLAY_PARAM_RES      /* 覆盖 */
} avplay_param_type;

typedef struct {
    demux_func_export *dmx_func;
    drv_vdec_export_func *vdec_func;
    void *win_func;
} avplay_import_func;

typedef struct {
    hi_bool update;
    hi_u64  param;
    avplay_param_type param_type;
} avplay_event_info;

typedef struct {
    hi_handle self;

    hi_handle vdec;
    hi_handle vid_dmx;
    hi_handle aud_dmx;
    hi_handle pcr_dmx;
    hi_handle win;
    hi_handle sync;

    hi_u64 event_mask; /* 一个bit对应一种事件, 用于控制是否立即上报事件, 0: disable; 1: enable */
    avplay_event_info events[AVPLAY_EVENT_TYPE_MAX]; /* protected by instance->spin */
    hi_bool wait_cond;
    osal_wait wq;
} avplay_drv_ctx;

typedef struct {
    hi_handle handle;
    avplay_drv_ctx *ctx;
    const void       *unique;
    avplay_drv_spin_t  spin;    /* used in the context of interruption */
    avplay_drv_mutex_t mutex;
} avplay_drv_instance;

static hi_bool g_avplay_inited = HI_FALSE;
static hi_u32 g_avplay_instance_cnt = 0;
static avplay_drv_instance g_avplay_instance[AVPLAY_MAX_CNT];
static avplay_drv_spin_t g_avplay_spinlock;

static avplay_import_func g_avplay_import_func = {HI_NULL};
static avplay_param_type g_avplay_param_types[AVPLAY_EVENT_TYPE_MAX] = {
    [AVPLAY_EVENT_VID_RENDER_UNDERLOAD] = AVPLAY_PARAM_ADD,
    [AVPLAY_EVENT_VID_ERR_FRAME] = AVPLAY_PARAM_RES,
    [AVPLAY_EVENT_VID_TYPE_ERR] = AVPLAY_PARAM_RES,

    [AVPLAY_EVENT_VID_DECODE_FORMAT_CHANGE] = AVPLAY_PARAM_RES,
    [AVPLAY_EVENT_VID_DECODE_PACKING_CHANGE] = AVPLAY_PARAM_RES,
    [AVPLAY_EVENT_VID_FIRST_PTS] = AVPLAY_PARAM_RES,
    [AVPLAY_EVENT_VID_SECOND_PTS] = AVPLAY_PARAM_RES,
    [AVPLAY_EVENT_VID_DECODE_EOS] = AVPLAY_PARAM_RES
};

static hi_s32 drv_wakeup(avplay_drv_ctx *ctx);

static void ctx_init(avplay_drv_ctx *ctx, hi_handle avplay)
{
    hi_u32 i = 0;

    ctx->self = avplay;
    ctx->vdec = HI_INVALID_HANDLE;
    ctx->vid_dmx = HI_INVALID_HANDLE;
    ctx->aud_dmx = HI_INVALID_HANDLE;
    ctx->win = HI_INVALID_HANDLE;
    ctx->sync = HI_INVALID_HANDLE;

    ctx->event_mask = 0LL;
    for (i = 0; i < AVPLAY_EVENT_TYPE_MAX; i++) {
        ctx->events[i].update = HI_FALSE;
        ctx->events[i].param = 0LL;
        ctx->events[i].param_type = g_avplay_param_types[i];
    }

    ctx->wait_cond = HI_FALSE;
    osal_wait_init(&ctx->wq);
}

static void ctx_deinit(avplay_drv_instance *instance)
{
    hi_ulong flags = 0;
    AVPLAY_MUTEX_LOCK(&instance->mutex);
    AVPLAY_SPIN_LOCK(&instance->spin, flags);
    if (instance->ctx != HI_NULL) {
        drv_wakeup(instance->ctx);
        AVPLAY_FREE(instance->ctx);
        instance->ctx = HI_NULL;
    }
    AVPLAY_SPIN_UNLOCK(&instance->spin, flags);
    AVPLAY_MUTEX_UNLOCK(&instance->mutex);
}

static void instance_init(void)
{
    hi_u32 i = 0;
    if (g_avplay_inited) {
        return;
    }

    for (; i < AVPLAY_MAX_CNT; i++) {
        g_avplay_instance[i].handle = HI_INVALID_HANDLE;
        g_avplay_instance[i].ctx = HI_NULL;
        g_avplay_instance[i].unique = HI_NULL;
        AVPLAY_SPIN_INIT(&g_avplay_instance[i].spin);
        AVPLAY_MUTEX_INIT(&g_avplay_instance[i].mutex);
    }

    g_avplay_instance_cnt = 0;
    g_avplay_inited = HI_TRUE;
}

static avplay_drv_instance *instance_get(hi_handle handle)
{
    avplay_drv_instance *instance = HI_NULL;
    hi_u32 index = AVPLAY_GET_ID(handle);
    if (index >= AVPLAY_MAX_CNT || AVPLAY_GET_MODID(handle) != HI_ID_AVPLAY) {
        HI_ERR_AVPLAY("Invalid avplay handle[0x%x]\n", handle);
        return HI_NULL;
    }

    if (!g_avplay_inited) {
        HI_ERR_AVPLAY("AVPLAY not inited\n");
        return HI_NULL;
    }

    if (g_avplay_instance[index].ctx == HI_NULL) {
        HI_ERR_AVPLAY("Invalid avplay handle[0x%x], instance context is NULL\n", handle);
    } else if (g_avplay_instance[index].handle != handle) {
        HI_ERR_AVPLAY("Invalid avplay handle[0x%x], instance handle[0x%x] check failed\n", handle, instance->handle);
    } else {
        instance = &g_avplay_instance[index];
    }

    return instance;
}

static hi_s32 instance_alloc(hi_handle *handle, const void *unique)
{
    hi_u32 i = 0;
    if (!g_avplay_inited) {
        HI_ERR_AVPLAY("AVPLAY not inited\n");
        return HI_FAILURE;
    }

    for (; i < AVPLAY_MAX_CNT; i++) {
        if (g_avplay_instance[i].handle == HI_INVALID_HANDLE) {
            break;
        }
    }

    if (i >= AVPLAY_MAX_CNT) {
        HI_ERR_AVPLAY("Too many avplays are created, instance cnt = %u\n", g_avplay_instance_cnt);
        return HI_ERR_AVPLAY_CREATE_ERR;
    }

    if (g_avplay_instance[i].ctx == HI_NULL) {
        g_avplay_instance[i].ctx = (avplay_drv_ctx*)AVPLAY_MALLOC(sizeof(avplay_drv_ctx));
    }
    CHECK_NULL(g_avplay_instance[i].ctx);

    ctx_init(g_avplay_instance[i].ctx, AVPLAY_GET_HANDLE(i));

    g_avplay_instance_cnt++;
    g_avplay_instance[i].handle = AVPLAY_GET_HANDLE(i);
    g_avplay_instance[i].unique = unique;
    *handle = g_avplay_instance[i].handle;

    HI_INFO_AVPLAY("Alloc drv avplay, index = 0x%x, unique = %p, instance_cnt = %u\n",
        i, unique, g_avplay_instance_cnt);
    return HI_SUCCESS;
}

static hi_s32 instance_del(hi_handle handle)
{
    hi_u32 index = AVPLAY_GET_ID(handle);
    if (index >= AVPLAY_MAX_CNT) {
        HI_ERR_AVPLAY("Invalid handle, index = %u\n", index);
        return HI_FAILURE;
    }

    ctx_deinit(&g_avplay_instance[index]);
    g_avplay_instance[index].handle = HI_INVALID_HANDLE;
    g_avplay_instance[index].unique = HI_NULL;
    g_avplay_instance_cnt--;

    HI_INFO_AVPLAY("Delete drv avplay, index = 0x%x\n", index);
    return HI_SUCCESS;
}

static hi_s32 instance_clear(const void *unique)
{
    hi_u32 i = 0;
    for (; i < AVPLAY_MAX_CNT; i++) {
        if (g_avplay_instance[i].unique == unique) {
            ctx_deinit(&g_avplay_instance[i]);
            g_avplay_instance[i].handle = HI_INVALID_HANDLE;
            g_avplay_instance[i].unique = HI_NULL;
            HI_INFO_AVPLAY("Clear drv avplay, index = 0x%x, unique = %p\n", i, unique);
        }
    }

    return HI_SUCCESS;
}

static void instance_deinit(void)
{
    hi_u32 i = 0;
    for (; i < AVPLAY_MAX_CNT; i++) {
        ctx_deinit(&g_avplay_instance[i]);
        AVPLAY_SPIN_DEINIT(&g_avplay_instance[i].spin);
        AVPLAY_MUTEX_DEINIT(&g_avplay_instance[i].mutex);
        g_avplay_instance[i].handle = HI_INVALID_HANDLE;
        g_avplay_instance[i].unique = HI_NULL;
    }
}

static void drv_instance_init(void)
{
    hi_ulong flags = 0;
    AVPLAY_SPIN_LOCK(&g_avplay_spinlock, flags);
    instance_init();
    AVPLAY_SPIN_UNLOCK(&g_avplay_spinlock, flags);
}

static avplay_drv_instance *drv_instance_get(hi_handle handle)
{
    hi_ulong flags = 0;
    avplay_drv_instance *instance = HI_NULL;
    AVPLAY_SPIN_LOCK(&g_avplay_spinlock, flags);
    instance = instance_get(handle);
    AVPLAY_SPIN_UNLOCK(&g_avplay_spinlock, flags);
    return instance;
}

static hi_s32 drv_instance_alloc(hi_handle *handle, const void *unique)
{
    hi_s32 ret;
    hi_ulong flags = 0;
    AVPLAY_SPIN_LOCK(&g_avplay_spinlock, flags);
    ret = instance_alloc(handle, unique);
    AVPLAY_SPIN_UNLOCK(&g_avplay_spinlock, flags);
    return ret;
}

static hi_s32 drv_instance_del(hi_handle handle)
{
    hi_s32 ret;
    hi_ulong flags = 0;
    AVPLAY_SPIN_LOCK(&g_avplay_spinlock, flags);
    ret = instance_del(handle);
    AVPLAY_SPIN_UNLOCK(&g_avplay_spinlock, flags);
    return ret;
}

static hi_s32 drv_instance_clear(const void *unique)
{
    hi_s32 ret;
    hi_ulong flags = 0;
    AVPLAY_SPIN_LOCK(&g_avplay_spinlock, flags);
    ret = instance_clear(unique);
    AVPLAY_SPIN_UNLOCK(&g_avplay_spinlock, flags);
    return ret;
}

static void drv_instance_deinit(void)
{
    hi_ulong flags = 0;
    AVPLAY_SPIN_LOCK(&g_avplay_spinlock, flags);
    instance_deinit();
    AVPLAY_SPIN_UNLOCK(&g_avplay_spinlock, flags);
}

#define AVPLAY_INSTANCE (instance)
#define AVPLAY_DRV_CTX (AVPLAY_INSTANCE->ctx)
#define AVPLAY_DRV_DO_FUNC(avplay, func) do {   \
    hi_s32 ret = HI_SUCCESS;                    \
    avplay_drv_instance *instance = drv_instance_get(avplay); \
    CHECK_NULL(instance);                                       \
    AVPLAY_MUTEX_LOCK(&AVPLAY_INSTANCE->mutex);                 \
                                                                \
    if (AVPLAY_DRV_CTX != HI_NULL) {                            \
        ret = func;                                             \
    } else {                                                    \
        ret = HI_ERR_AVPLAY_NULL_PTR;                           \
        HI_ERR_AVPLAY("Instance ctx is NULL\n");                \
    }                                                           \
                                                                \
    AVPLAY_MUTEX_UNLOCK(&AVPLAY_INSTANCE->mutex);               \
    return ret;                                                 \
} while (0);

#define AVPLAY_DRV_DO_FUNC_WITH_SPIN(avplay, func) do {         \
    hi_s32 ret = HI_SUCCESS;                                    \
    hi_ulong flags = 0;                                         \
    avplay_drv_instance *AVPLAY_INSTANCE = drv_instance_get(avplay); \
    CHECK_NULL(AVPLAY_INSTANCE);                                       \
    AVPLAY_SPIN_LOCK(&AVPLAY_INSTANCE->spin, flags);            \
                                                                \
    if (AVPLAY_DRV_CTX != HI_NULL) {                            \
        ret = func;                                             \
    } else {                                                    \
        ret = HI_ERR_AVPLAY_NULL_PTR;                           \
        HI_ERR_AVPLAY("Instance ctx is NULL\n");                \
    }                                                           \
                                                                \
    AVPLAY_SPIN_UNLOCK(&AVPLAY_INSTANCE->spin, flags);          \
    return ret;                                                 \
} while (0);


static hi_s32 event_process(avplay_drv_ctx *ctx, avplay_event_type event, hi_u64 param)
{
    CHECK_NULL(ctx);
    CHECK_PARAM(event < AVPLAY_EVENT_TYPE_MAX);

    if (ctx->events[event].param_type == AVPLAY_PARAM_ADD) {
        ctx->events[event].param += param;
    } else if (ctx->events[event].param_type == AVPLAY_PARAM_RES) {
        ctx->events[event].param = param;
    } else {
        ctx->events[event].param++;
    }

    ctx->events[event].update = HI_TRUE;

    if (IS_BIT_ENABLE(ctx->event_mask, event)) {
        return drv_wakeup(ctx);
    }

    return HI_SUCCESS;
}

static hi_s32 vid_dmx_callback(hi_handle avplay, hi_dmx_evt_id evt_id, const void *param, hi_u32 param_size)
{
    hi_s32 ret;
    hi_ulong flags = 0;
    avplay_event_type event_type = AVPLAY_EVENT_TYPE_MAX;
    avplay_drv_instance *instance = HI_NULL;

    switch (evt_id) {
        case HI_DMX_EVT_ERR:
            event_type = AVPLAY_EVENT_VID_DMX_ERR;
            break;
        case HI_DMX_EVT_NEW_PES:
            event_type = AVPLAY_EVENT_VID_NEW_PES;
            break;
        case HI_DMX_EVT_EOS:
            event_type = AVPLAY_EVENT_VID_DMX_EOS;
            break;
        default:
            HI_ERR_AVPLAY("Unsupport video demux event id[0x%x].\n", evt_id);
            return HI_ERR_AVPLAY_NOT_SUPPORT;
    }

    HI_TRACE_AVPLAY("Receive video demux event id[0x%x], avplay event id = 0x%x\n", evt_id, event_type);
    instance = drv_instance_get(avplay);
    CHECK_NULL(instance);

    AVPLAY_SPIN_LOCK(&instance->spin, flags);
    ret = event_process(instance->ctx, event_type, 0LL);
    AVPLAY_SPIN_UNLOCK(&instance->spin, flags);

    return ret;
}

static hi_s32 aud_dmx_callback(hi_handle avplay, hi_dmx_evt_id evt_id, const void *param, hi_u32 param_size)
{
    hi_s32 ret;
    hi_ulong flags = 0;
    avplay_event_type event_type = AVPLAY_EVENT_TYPE_MAX;
    avplay_drv_instance *instance = HI_NULL;

    switch (evt_id) {
        case HI_DMX_EVT_ERR:
            event_type = AVPLAY_EVENT_AUD_DMX_ERR;
            break;
        case HI_DMX_EVT_NEW_PES:
            event_type = AVPLAY_EVENT_AUD_NEW_PES;
            break;
        case HI_DMX_EVT_EOS:
            event_type = AVPLAY_EVENT_AUD_DMX_EOS;
            break;
        default:
            HI_ERR_AVPLAY("Unsupport audio demux event id[0x%x].\n", evt_id);
            return HI_ERR_AVPLAY_NOT_SUPPORT;
    }

    HI_TRACE_AVPLAY("Receive audio demux event id[0x%x], avplay event id = 0x%x\n", evt_id, event_type);
    instance = drv_instance_get(avplay);
    CHECK_NULL(instance);

    AVPLAY_SPIN_LOCK(&instance->spin, flags);
    ret = event_process(instance->ctx, event_type, 0LL);
    AVPLAY_SPIN_UNLOCK(&instance->spin, flags);

    return ret;
}

static hi_s32 pcr_dmx_callback(hi_handle avplay, hi_dmx_evt_id evt_id, const void *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;
    const dmx_sync_pcr *info = HI_NULL;

    CHECK_NULL(param);

    if (evt_id == HI_DMX_EVT_PCR) {
        info = (const dmx_sync_pcr *)param;
        ret = hi_drv_sync_pcr_proc(info->handle, info->pcr_value, info->scr_value);
    }

    return ret;
}

static void err_frame_info_to_u64(const void *param, hi_u32 param_size, hi_u64 *event_param)
{
    const hi_vdec_err_frm *err_frame = (const hi_vdec_err_frm*)param;
    hi_u64 a = err_frame->err_frm_type;
    hi_u64 b = err_frame->err_ratio;
    *event_param = BIG_ENDIAN_WRITE(a, b);
}

static hi_s32 vdec_evt_map_with_param(hi_vdec_evt_id evt_id, const void *param, hi_u32 param_size,
                                      avplay_event_type *avplay_evt, hi_u64 *event_param)
{
    CHECK_NULL(param);

    switch (evt_id) {
        case HI_VDEC_EVT_FIRST_PTS: {
            *avplay_evt = AVPLAY_EVENT_VID_FIRST_PTS;
            *event_param = *(const hi_s64*)param;
            break;
        }
        case HI_VDEC_EVT_SECOND_PTS: {
            *avplay_evt = AVPLAY_EVENT_VID_SECOND_PTS;
            *event_param = *(const hi_s64*)param;
            break;
        }
        case HI_VDEC_EVT_EOS: {
            const hi_vdec_eos_param *eos = param;
            *avplay_evt = AVPLAY_EVENT_VID_DECODE_EOS;
            *event_param = eos->last_frame_flag;
            break;
        }
        case HI_VDEC_EVT_ERR_STANDARD: {
            *avplay_evt = AVPLAY_EVENT_VID_TYPE_ERR;
            *event_param = *(const hi_u32*)param;
            break;
        }
        case HI_VDEC_EVT_NORM_CHG: {
            const hi_vdec_norm_param *norm = param;
            *avplay_evt = AVPLAY_EVENT_VID_DECODE_FORMAT_CHANGE;
            *event_param = norm->norm_type;
            break;
        }
        case HI_VDEC_EVT_FRM_PCK_CHG: {
            *avplay_evt = AVPLAY_EVENT_VID_DECODE_PACKING_CHANGE;
            *event_param = *(const hi_u32*)param;
            break;
        }
        case HI_VDEC_EVT_ERR_FRAME: {
            *avplay_evt = AVPLAY_EVENT_VID_ERR_FRAME;
            err_frame_info_to_u64(param, param_size, event_param);
            break;
        }
        default:
            HI_ERR_AVPLAY("Unsupport vdec event id[0x%x]\n", evt_id);
            return HI_ERR_AVPLAY_NOT_SUPPORT;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_callback(hi_handle avplay, hi_vdec_evt_id evt_id, const void *param, hi_u32 param_size)
{
    hi_s32 ret = HI_SUCCESS;
    hi_ulong flags = 0;
    hi_u64 event_param = 0LL;
    avplay_event_type event_type = AVPLAY_EVENT_TYPE_MAX;
    avplay_drv_instance *instance = HI_NULL;

    switch (evt_id) {
        case HI_VDEC_EVT_NEW_FRAME:
            event_type = AVPLAY_EVENT_VID_NEW_FRAME;
            break;
        case HI_VDEC_EVT_NEW_STREAM:
            event_type = AVPLAY_EVENT_VID_RLS_PES;
            break;
        case HI_VDEC_EVT_NEW_USER_DATA:
            event_type = AVPLAY_EVENT_VID_NEW_USER_DATA;
            break;
        case HI_VDEC_EVT_UNSUPPORT:
            event_type = AVPLAY_EVENT_VID_UNSUPPORT;
            break;
        default:
            ret = vdec_evt_map_with_param(evt_id, param, param_size, &event_type, &event_param);
            RETURN_WHEN_ERROR(ret, vdec_evt_map_with_param);
    }

    HI_TRACE_AVPLAY("Receive vdec event id[0x%x], avplay event id = 0x%x\n", evt_id, event_type);
    instance = drv_instance_get(avplay);
    CHECK_NULL(instance);

    AVPLAY_SPIN_LOCK(&instance->spin, flags);
    ret = event_process(instance->ctx, event_type, event_param);
    AVPLAY_SPIN_UNLOCK(&instance->spin, flags);

    return ret;
}

static hi_bool pop_event(avplay_drv_ctx *ctx, avplay_event_type *event, hi_u64 *param)
{
    hi_s32 i = 0;
    avplay_event_info *event_info = HI_NULL;

    for (i = 0; i < AVPLAY_EVENT_TYPE_MAX; i++) {
        event_info = &ctx->events[i];
        if (event_info->update) {
            *event = i;
            *param = event_info->param;

            event_info->update = HI_FALSE;
            event_info->param = 0LL;
            return HI_TRUE;
        }
    }

    return HI_FALSE;
}

static hi_bool drv_pop_event(avplay_drv_instance *instance, avplay_event_type *event, hi_u64 *param)
{
    hi_bool ret;
    hi_ulong flags = 0;

    AVPLAY_SPIN_LOCK(&instance->spin, flags);
    ret = pop_event(instance->ctx, event, param);
    AVPLAY_SPIN_UNLOCK(&instance->spin, flags);

    return ret;
}

static hi_s32 drv_reset(avplay_drv_ctx *ctx, avplay_channel_type chn)
{
    hi_u32 i = 0;
    (void)chn;

    for (i = 0; i < AVPLAY_EVENT_TYPE_MAX; i++) {
        ctx->events[i].update = HI_FALSE;
        ctx->events[i].param = 0LL;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_set_dmx_callback(hi_handle *dmx_handle, hi_handle handle,
                                   hi_handle avplay, drv_dmx_callback callback)
{
    hi_s32 ret = HI_SUCCESS;

    if (g_avplay_import_func.dmx_func == HI_NULL ||
        g_avplay_import_func.dmx_func->dmx_set_callback == HI_NULL) {
        HI_ERR_AVPLAY("drv_dmx_set_callback is NULL, cannot set resrouce\n");
        return HI_ERR_AVPLAY_INVALID_OPT;
    }

    if (handle != HI_INVALID_HANDLE) {
        ret = g_avplay_import_func.dmx_func->dmx_set_callback(handle, avplay, callback);
    } else if (*dmx_handle != HI_INVALID_HANDLE) {
        ret = g_avplay_import_func.dmx_func->dmx_set_callback(*dmx_handle, avplay, HI_NULL);
    }

    *dmx_handle = handle;

    return ret;
}

static hi_s32 drv_set_vdec_callback(hi_handle *vdec_handle, hi_handle handle,
                                    hi_handle avplay, fn_drv_vdec_callback callback)
{
    hi_s32 ret = HI_SUCCESS;

    fn_drv_vdec_set_callback vdec_set_cb = HI_NULL;
    if (g_avplay_import_func.vdec_func != HI_NULL &&
        g_avplay_import_func.vdec_func->pfn_drv_vdec_set_callback != HI_NULL) {
        vdec_set_cb = g_avplay_import_func.vdec_func->pfn_drv_vdec_set_callback;
    } else {
        HI_ERR_AVPLAY("drv_vdec_set_callback is NULL, cannot set resrouce\n");
        return HI_ERR_AVPLAY_INVALID_OPT;
    }

    if (handle != HI_INVALID_HANDLE) {
        ret = vdec_set_cb(handle, avplay, vdec_callback);
    } else if (*vdec_handle != HI_INVALID_HANDLE) {
        ret = vdec_set_cb(*vdec_handle, avplay, HI_NULL);
    }

    *vdec_handle = handle;
    return ret;
}

static hi_s32 drv_set_resource(avplay_drv_ctx *ctx, hi_handle handle, avplay_resource_type type, hi_u32 param)
{
    hi_s32 ret = HI_SUCCESS;

    switch (type) {
        case AVPLAY_RESOURCE_TYPE_VID_DEMUX: {
            ret = drv_set_dmx_callback(&ctx->vid_dmx, handle, ctx->self, vid_dmx_callback);
            break;
        }
        case AVPLAY_RESOURCE_TYPE_AUD_DEMUX: {
            ret = drv_set_dmx_callback(&ctx->aud_dmx, handle, ctx->self, aud_dmx_callback);
            break;
        }
        case AVPLAY_RESOURCE_TYPE_PCR_DEMUX: {
            ret = drv_set_dmx_callback(&ctx->pcr_dmx, handle, ctx->self, pcr_dmx_callback);
            break;
        }
        case AVPLAY_RESOURCE_TYPE_VDEC: {
            ret = drv_set_vdec_callback(&ctx->vdec, handle, ctx->self, vdec_callback);
            break;
        }
        case AVPLAY_RESOURCE_TYPE_WINDOW:  {
            if (handle != HI_INVALID_HANDLE) {
            } else if (ctx->win != HI_INVALID_HANDLE) {
            }

            ctx->win = handle;
            break;
        }
        case AVPLAY_RESOURCE_TYPE_SYNC: {
            if (handle != HI_INVALID_HANDLE) {
            } else if (ctx->sync != HI_INVALID_HANDLE) {
            }

            ctx->sync = handle;
            break;
        }
        default: {
            HI_ERR_AVPLAY("Unsupport resource type[%d]\n", type);
            ret = HI_ERR_AVPLAY_NOT_SUPPORT;
            break;
        }
    }

    RETURN_WHEN_ERROR(ret, set_resource);
    return HI_SUCCESS;
}

static hi_s32 drv_wakeup(avplay_drv_ctx *ctx)
{
    ctx->wait_cond = HI_TRUE;
    osal_wait_wakeup(&ctx->wq);

    return HI_SUCCESS;
}

static int drv_wait_cond(const void *param)
{
    if (*(const hi_bool*)param) {
        return 1;
    }

    return 0;
}

static hi_s32 drv_wait_event(avplay_drv_instance *instance, hi_u64 event_mask,
                             avplay_event_type *event, hi_u64 *param, hi_s64 timeout)
{
    hi_s32 ret = HI_FAILURE;
    avplay_drv_ctx *ctx = instance->ctx;
    ctx->event_mask = event_mask;

    *event = AVPLAY_EVENT_TYPE_MAX;
    if (!drv_pop_event(instance, event, param)) {
        AVPLAY_MUTEX_UNLOCK(&instance->mutex);
        ret = osal_wait_timeout_interruptible(&ctx->wq, drv_wait_cond, &ctx->wait_cond, timeout);
        AVPLAY_MUTEX_LOCK(&instance->mutex);
        CHECK_NULL(instance->ctx);

        (void)drv_pop_event(instance, event, param);
        ctx->wait_cond = HI_FALSE;
    }

    if (*event != AVPLAY_EVENT_TYPE_MAX) {
        ret = HI_SUCCESS;
    } else if (ret == 0) {
        ret = HI_ERR_AVPLAY_TIMEOUT;
    }

    return ret;
}

static hi_s32 import_function(void)
{
    hi_s32 ret;
    hi_ulong flags = 0;
    AVPLAY_SPIN_LOCK(&g_avplay_spinlock, flags);

    ret = hi_drv_module_get_func(HI_ID_DEMUX, (void **)&(g_avplay_import_func.dmx_func));
    LOG_WHEN_ERROR(ret, hi_drv_module_get_func);

    ret = hi_drv_module_get_func(HI_ID_VDEC, (void **)&(g_avplay_import_func.vdec_func));
    LOG_WHEN_ERROR(ret, hi_drv_module_get_func);

    AVPLAY_SPIN_UNLOCK(&g_avplay_spinlock, flags);

    return HI_SUCCESS;
}

hi_s32 hi_drv_avplay_create(hi_handle *avplay, const void *unique)
{
    CHECK_NULL(avplay);
    return drv_instance_alloc(avplay, unique);
}

hi_s32 hi_drv_avplay_destroy(hi_handle avplay)
{
    return drv_instance_del(avplay);
}

hi_s32 hi_drv_avplay_reset(hi_handle avplay, avplay_channel_type chn)
{
    AVPLAY_DRV_DO_FUNC_WITH_SPIN(avplay, drv_reset(AVPLAY_DRV_CTX, chn));
}

hi_s32 hi_drv_avplay_set_resource(hi_handle avplay, hi_handle handle, avplay_resource_type type, hi_u32 param)
{
    AVPLAY_DRV_DO_FUNC(avplay, drv_set_resource(AVPLAY_DRV_CTX, handle, type, param));
}

hi_s32 hi_drv_avplay_wakeup(hi_handle avplay)
{
    AVPLAY_DRV_DO_FUNC(avplay, drv_wakeup(AVPLAY_DRV_CTX));
}

hi_s32 hi_drv_avplay_wait_event(hi_handle avplay, hi_u64 event_mask,
                                avplay_event_type *event, hi_u64 *param, hi_s64 timeout)
{
    AVPLAY_DRV_DO_FUNC(avplay, drv_wait_event(AVPLAY_INSTANCE, event_mask, event, param, timeout));
}

hi_s32 avplay_init(void)
{
    AVPLAY_SPIN_INIT(&g_avplay_spinlock);
    drv_instance_init();

    return HI_SUCCESS;
}

hi_s32 avplay_deinit(void)
{
    drv_instance_deinit();
    AVPLAY_SPIN_DEINIT(&g_avplay_spinlock);
    return HI_SUCCESS;
}

hi_s32 avplay_open(void *private_data)
{
    return import_function();
}

hi_s32 avplay_release(void *private_data)
{
    return drv_instance_clear(private_data);
}

hi_slong avplay_ioctl(void *file, hi_u32 cmd, hi_void *arg)
{
    hi_s32 ret = HI_SUCCESS;

    switch (cmd) {
        case CMD_AVPLAY_CREATE: {
            avplay_create_param *param = arg;
            ret = hi_drv_avplay_create(&param->avplay, file);
            break;
        }

        case CMD_AVPLAY_DESTROY: {
            hi_handle *param = arg;
            ret = hi_drv_avplay_destroy(*param);
            break;
        }

        case CMD_AVPLAY_RESET: {
            avplay_reset_param *param = arg;
            ret = hi_drv_avplay_reset(param->avplay, param->chn);
            break;
        }

        case CMD_AVPLAY_SET_RESOURCE: {
            avplay_resource_param *param = arg;
            ret = hi_drv_avplay_set_resource(param->avplay, param->handle, param->type, param->param);
            break;
        }

        case CMD_AVPLAY_WAIT_EVENT: {
            avplay_wait_event_param *param = arg;
            ret = hi_drv_avplay_wait_event(param->avplay, param->event_mask,
                &param->event, &param->param, param->timeout);
            break;
        }

        case CMD_AVPLAY_WAKEUP: {
            hi_handle *param = arg;
            ret = hi_drv_avplay_wakeup(*param);
            break;
        }

        default: {
            HI_ERR_AVPLAY("Unsupport cmd[%u]\n", cmd);
            ret = HI_ERR_AVPLAY_NOT_SUPPORT;
            break;
        }
    }

    return ret;
}

