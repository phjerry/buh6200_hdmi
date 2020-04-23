/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: implement of ao driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"

#include "drv_ao_engine.h"

#if defined(HI_AUDIO_AI_SUPPORT)
#include "drv_ai_private.h"
#endif

/* aiao drv config */
#include "drv_ao_cfg.h"
#include "drv_ao.h"

#include "hal_tianlai_adac.h"

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
#include "drv_aiao_veri.h"
#endif

#ifdef HI_AIAO_TIMER_SUPPORT
#include "drv_timer_private.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static hi_void *g_filp = HI_NULL;

static osal_semaphore g_ao_mutex = {HI_NULL};

#ifdef HI_SND_ADVANCED_SUPPORT
static hi_s32 ao_snd_dma_destory(hi_void *file, ao_snd_id sound);
#endif

hi_void ao_snd_free_handle(ao_snd_id sound, hi_void *file);
hi_s32 ao_snd_alloc_handle(ao_snd_id sound, hi_void *file);
hi_s32 ao_close_dev(hi_void);
hi_s32 ao_snd_close(ao_snd_id sound, hi_bool suspend);
hi_s32 ao_snd_open(ao_snd_id sound, hi_ao_attr *attr, hi_bool resume);

/* ao global management */
static ao_mgmt g_ao_drv;

ao_mgmt *ao_get_mgmt(hi_void)
{
    return &g_ao_drv;
}

/*
 * ao_drv_get_pll_cfg - read EPLL frac and int register cfg
 * @frac_cfg:    EPLL frac register cfg.
 * @int_cfg:     EPLL int  register cfg.
 * @return:      HI_SUCCESS or HI_FAILURE.
 */
hi_s32 ao_drv_get_pll_cfg(hi_u32 *frac_cfg, hi_u32 *int_cfg)
{
    CHECK_AO_NULL_PTR(frac_cfg);
    CHECK_AO_NULL_PTR(int_cfg);

    return autil_get_pll_cfg(frac_cfg, int_cfg);
}

hi_void ao_lock(hi_void)
{
    if (osal_sem_down_interruptible(&g_ao_mutex) != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
    }
}

hi_void ao_unlock(hi_void)
{
    osal_sem_up(&g_ao_mutex);
}

hi_s32 ao_snd_close_by_state(drv_ao_state *ao_state, hi_u32 snd_id)
{
    hi_s32 ret;
    hi_u32 user_open_cnt;
    hi_u32 mgmt_open_cnt;
    ao_mgmt *mgmt = ao_get_mgmt();

    user_open_cnt = osal_atomic_read(&ao_state->atm_user_open_cnt[snd_id]);
    mgmt_open_cnt = osal_atomic_read(&mgmt->snd_entity[snd_id].atm_use_total_cnt);

    osal_atomic_set(&mgmt->snd_entity[snd_id].atm_use_total_cnt, mgmt_open_cnt - user_open_cnt);

    if (user_open_cnt == mgmt_open_cnt) {
        ret = ao_snd_close(snd_id, HI_FALSE);
        if (ret != HI_SUCCESS) {
            osal_atomic_inc_return(&mgmt->snd_entity[snd_id].atm_use_total_cnt);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 ao_snd_free(hi_void *filp)
{
    hi_s32 ret;
    hi_u32 snd_id;
    hi_u32 snd_open_cnt;
    drv_ao_state *ao_state = *((drv_ao_state **)filp);
    ao_mgmt *mgmt = ao_get_mgmt();

    if (ao_state == HI_NULL) {
        return HI_SUCCESS;
    }

    for (snd_id = 0; snd_id < AO_MAX_TOTAL_SND_NUM; snd_id++) {
        if (mgmt->snd_entity[snd_id].card == HI_NULL) {
            continue;
        }

        for (snd_open_cnt = 0; snd_open_cnt < SND_MAX_OPEN_NUM; snd_open_cnt++) {
            hi_void *file_cur_snd_open = mgmt->snd_entity[snd_id].file[snd_open_cnt];
            if (file_cur_snd_open != filp) {
                continue;
            }

            ret = ao_snd_close_by_state(ao_state, snd_id);
            if (ret != HI_SUCCESS) {
                HI_FATAL_AO("ao_snd_close_by_state failed(0x%x), snd_id = %d!\n", ret, snd_id);
                return ret;
            }

            ao_snd_free_handle(snd_id, file_cur_snd_open);
        }
    }

    return HI_SUCCESS;
}

hi_s32 ao_open_dev(hi_void)
{
    hi_u32 i;
    hi_s32 ret;
    ao_mgmt *mgmt = ao_get_mgmt();

    /* init global track parameter */
    for (i = 0; i < AO_MAX_TOTAL_TRACK_NUM; i++) {
        osal_atomic_set(&mgmt->track_entity[i].atm_use_cnt, 0);
    }

    /* init global snd parameter */
    for (i = 0; i < AO_MAX_TOTAL_SND_NUM; i++) {
        osal_atomic_set(&mgmt->snd_entity[i].atm_use_total_cnt, 0);
    }

    /* hal_aiao_init, init aiao hardware */
    hal_aiao_init();

    /* hal_aoe_init , init aoe hardware */
    ret = hal_aoe_init();
    if (ret != HI_SUCCESS) {
        HI_FATAL_AO("call hal_aoe_init failed(0x%x)\n", ret);
        hal_aiao_de_init();
        return ret;
    }

#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
    /* hal_cast_init , init cast hardware */
    hal_cast_init();
#endif

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
    aiao_veri_open();
#endif

    /* set ready flag */
    mgmt->ready = HI_TRUE;

    snd_op_register_driver();

    HI_INFO_AO("ao_open_dev OK.\n");
    return HI_SUCCESS;
}

hi_s32 ao_drv_open(hi_void *private_data)
{
    hi_s32 ret;
    hi_u32 cnt;
    drv_ao_state *ao_state = HI_NULL;
    drv_ao_state **priv_state = (drv_ao_state **)private_data;
    ao_mgmt *mgmt = ao_get_mgmt();

    if (private_data == HI_NULL) {
        HI_FATAL_AO("file handle is null.\n");
        return HI_FAILURE;
    }

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return HI_ERR_AO_INTR;
    }

    ao_state = osal_kmalloc(HI_ID_AO, sizeof(drv_ao_state), OSAL_GFP_KERNEL);
    if (ao_state == HI_NULL) {
        HI_FATAL_AO("malloc ao_state failed.\n");
        osal_sem_up(&g_ao_mutex);
        return HI_FAILURE;
    }

    ret = memset_s(ao_state, sizeof(drv_ao_state), 0, sizeof(drv_ao_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        goto out1;
    }

    for (cnt = 0; cnt < AO_MAX_TOTAL_SND_NUM; cnt++) {
        osal_atomic_init(&(ao_state->atm_user_open_cnt[cnt]));
        osal_atomic_set(&(ao_state->atm_user_open_cnt[cnt]), 0);
        ao_state->file_id[cnt] = AO_SND_FILE_NOUSE_FLAG;
    }

    if (osal_atomic_inc_return(&mgmt->atm_open_cnt) == 1) {
        /* init device */
        ret = ao_open_dev();
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(ao_open_dev, ret);
            goto out2;
        }
    }

    *priv_state = ao_state;

    osal_sem_up(&g_ao_mutex);
    return HI_SUCCESS;

out2:
    osal_atomic_dec_return(&mgmt->atm_open_cnt);
    for (cnt = 0; cnt < AO_MAX_TOTAL_SND_NUM; cnt++) {
        osal_atomic_destory(&(ao_state->atm_user_open_cnt[cnt]));
    }

out1:
    osal_kfree(HI_ID_AO, ao_state);
    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 ao_drv_release(hi_void *private_data)
{
    hi_s32 ret;
    drv_ao_state *ao_state = *((drv_ao_state **)private_data);
    ao_mgmt *mgmt = ao_get_mgmt();

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return HI_ERR_AO_INTR;
    }

    /* not the last close, only close the cast, track & snd which matches filp */
    if (osal_atomic_dec_return(&mgmt->atm_open_cnt) != 0) {
#ifdef HI_SND_CAST_SUPPORT
        ret = ao_snd_free_cast(private_data);
        if (ret != HI_SUCCESS) {
            goto ERR_AO_DRV_RELEASE;
        }
#endif

        ret = ao_snd_free_track(private_data);
        if (ret != HI_SUCCESS) {
            goto ERR_AO_DRV_RELEASE;
        }

        /* free all snd */
        ret = ao_snd_free(private_data);
        if (ret != HI_SUCCESS) {
            goto ERR_AO_DRV_RELEASE;
        }
    } else {
        ao_close_dev();
    }

    if (ao_state != HI_NULL) {
        hi_u32 cnt;
        for (cnt = 0; cnt < AO_MAX_TOTAL_SND_NUM; cnt++) {
            osal_atomic_destory(&(ao_state->atm_user_open_cnt[cnt]));
        }

        osal_kfree(HI_ID_AO, ao_state);
    }
    osal_sem_up(&g_ao_mutex);
    return HI_SUCCESS;

ERR_AO_DRV_RELEASE:
    osal_atomic_inc_return(&mgmt->atm_open_cnt);
    osal_sem_up(&g_ao_mutex);
    return HI_FAILURE;
}

#if defined(HI_SND_AR_SUPPORT)
static hi_void ao_continue_close(hi_void *filp)
{
    snd_card_state *card = HI_NULL;
    ao_mgmt *mgmt = ao_get_mgmt();

    if (mgmt->snd_entity[AO_SND_0].card == HI_NULL) {
        return;
    }

    card = mgmt->snd_entity[AO_SND_0].card;

    if (card->render_attr.con_output_fp != filp) {
        return;
    }

    card->render_attr.continue_status = HI_FALSE;
    card->render_attr.output_mode = HI_AO_OUTPUT_LATENCY_NORMAL;
}
#endif

hi_s32 ao_close_dev(hi_void)
{
    hi_u32 i, j;
    ao_mgmt *mgmt = ao_get_mgmt();
    drv_ao_state *ao_state = HI_NULL;

    /* reentrant */
    if (mgmt->ready == HI_FALSE) {
        return HI_SUCCESS;
    }

    /* set ready flag */
    mgmt->ready = HI_FALSE;

#ifdef HI_SND_CAST_SUPPORT
    /* free all cast */
    for (i = 0; i < AO_MAX_CAST_NUM; i++) {
        if (osal_atomic_read(&mgmt->cast_entity[i].atm_use_cnt)) {
            (hi_void) ao_cast_destory(i);
            ao_cast_free_handle(i);
        }
    }
#endif

    /* free all track */
    for (i = 0; i < AO_MAX_TOTAL_TRACK_NUM; i++) {
        if (osal_atomic_read(&mgmt->track_entity[i].atm_use_cnt)) {
            (hi_void) ao_track_destory(i);
            ao_track_free_handle_by_id(i);
        }
    }

#if defined(HI_SND_AR_SUPPORT)
    if (mgmt->snd_entity[AO_SND_0].card != HI_NULL) {
        ao_continue_close(mgmt->snd_entity[AO_SND_0].card->render_attr.con_output_fp);
    }
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
    for (i = 0; i < AO_MAX_TOTAL_SND_NUM; i++) {
        if (mgmt->snd_dma_entity[i].file != HI_NULL) {
            ao_snd_dma_destory(mgmt->snd_dma_entity[i].file, i);
        }
    }
#endif

    /* free all snd */
    for (i = 0; i < AO_MAX_TOTAL_SND_NUM; i++) {
        if (mgmt->snd_entity[i].card != HI_NULL) {
            for (j = 0; j < SND_MAX_OPEN_NUM; j++) {
                if (mgmt->snd_entity[i].file[j] != 0) {
                    hi_u32 user_open_cnt = 0;
                    hi_u32 mgmt_open_cnt = 0;
                    ao_state = *((drv_ao_state **)(mgmt->snd_entity[i].file[j]));

                    user_open_cnt = osal_atomic_read(&ao_state->atm_user_open_cnt[i]);
                    mgmt_open_cnt = osal_atomic_read(&mgmt->snd_entity[i].atm_use_total_cnt);
                    osal_atomic_set(&mgmt->snd_entity[i].atm_use_total_cnt, mgmt_open_cnt - user_open_cnt);
                    if (mgmt_open_cnt == user_open_cnt) {
                        (hi_void) ao_snd_close(i, HI_FALSE);
                    }
                    ao_snd_free_handle(i, mgmt->snd_entity[i].file[j]);
                }
            }
        }
    }

    /* hal_aoe_deinit */
    hal_aoe_deinit();

#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
    /* hal_cast_deinit */
    hal_cast_deinit();
#endif

    /* hal_aiao_de_init */
    hal_aiao_de_init();

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
    aiao_veri_release();
#endif

    return HI_SUCCESS;
}

snd_card_state *snd_card_get_card(ao_snd_id sound)
{
    ao_mgmt *mgmt = ao_get_mgmt();
    if (sound >= AO_SND_MAX) {
        return HI_NULL;
    }

    return mgmt->snd_entity[sound].card;
}

ao_snd_id snd_card_get_snd(snd_card_state *card)
{
    hi_u32 i;
    ao_mgmt *mgmt = ao_get_mgmt();

    for (i = 0; i < AO_MAX_TOTAL_SND_NUM; i++) {
        if (mgmt->snd_entity[i].card == card) {
            return i;
        }
    }

    return AO_MAX_TOTAL_SND_NUM;
}

hi_void ao_snd_free_handle(ao_snd_id sound, hi_void *filp)
{
    ao_mgmt *mgmt = ao_get_mgmt();
    snd_card_state *card = mgmt->snd_entity[sound].card;
    hi_u32 file_id;
    drv_ao_state *ao_state = HI_NULL;

    if (card == HI_NULL) {
        return;
    }

    if (osal_atomic_read(&mgmt->snd_entity[sound].atm_use_total_cnt) == 0) {
        osal_kfree(HI_ID_AO, card);
        mgmt->snd_entity[sound].card = HI_NULL;
    }

    if (filp == HI_NULL) {
        return;
    }

    ao_state = *((drv_ao_state **)filp);
    if (ao_state == HI_NULL) {
        return;
    }

    file_id = ao_state->file_id[sound];
    if (file_id < SND_MAX_OPEN_NUM) {
        mgmt->snd_entity[sound].file[file_id] = HI_NULL;
    }

    ao_state->file_id[sound] = AO_SND_FILE_NOUSE_FLAG;
}

hi_s32 snd_get_free_file_id(ao_snd_id sound)
{
    hi_u32 i;
    ao_mgmt *mgmt = ao_get_mgmt();

    for (i = 0; i < SND_MAX_OPEN_NUM; i++) {
        if (mgmt->snd_entity[sound].file[i] == HI_NULL) {
            return i;
        }
    }

    return SND_MAX_OPEN_NUM;
}

hi_s32 ao_snd_alloc_handle(ao_snd_id sound, hi_void *file)
{
    hi_s32 ret;
    hi_u32 free_id;
    snd_card_state *card = HI_NULL;
    drv_ao_state *ao_state = HI_NULL;
    ao_mgmt *mgmt = ao_get_mgmt();

    if (sound >= AO_SND_MAX) {
        HI_ERR_AO("bad param!\n");
        return HI_ERR_AO_INVALID_ID;
    }

    /* check ready flag */
    if (mgmt->ready != HI_TRUE) {
        HI_ERR_AO("need open first!\n");
        return HI_ERR_AO_DEV_NOT_OPEN;
    }

    free_id = snd_get_free_file_id(sound);
    if (free_id >= SND_MAX_OPEN_NUM) {
        HI_ERR_AO("get free file id failed!\n");
        return HI_FAILURE;
    }

    ao_state = *((drv_ao_state **)file);

    if (ao_state->file_id[sound] == AO_SND_FILE_NOUSE_FLAG) {
        ao_state->file_id[sound] = free_id;
        mgmt->snd_entity[sound].file[free_id] = file;
    }

    if (osal_atomic_read(&mgmt->snd_entity[sound].atm_use_total_cnt) > 0) {
        return HI_SUCCESS;
    }

    /* allocate new snd resource */
    card = (snd_card_state *)osal_kmalloc(HI_ID_AO, sizeof(snd_card_state), OSAL_GFP_KERNEL);
    if (card == HI_NULL) {
        HI_ERR_AO("osal_kmalloc snd_card_state failed\n");
        mgmt->snd_entity[sound].file[free_id] = HI_NULL;
        return HI_ERR_AO_MALLOC_FAILED;
    }

    ret = memset_s(card, sizeof(snd_card_state), 0, sizeof(snd_card_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        osal_kfree(HI_ID_AO, card);
        mgmt->snd_entity[sound].file[free_id] = HI_NULL;
        return ret;
    }

    card->sound = sound;
    mgmt->snd_entity[sound].card = card;

    return HI_SUCCESS;
}

static hi_bool ao_check_out_port_is_attached(ao_snd_id sound, hi_ao_port out_port)
{
    hi_u32 snd;
    hi_u32 port;
    snd_card_state *card = HI_NULL;

    /* check if port is attached by another sound */
    for (snd = 0; snd < (hi_u32)AO_SND_MAX; snd++) {
        if (snd == (hi_u32)sound) {
            continue;
        }

        card = snd_card_get_card((ao_snd_id)snd);
        if (card == HI_NULL) {
            continue;
        }

        for (port = 0; port < card->user_open_param.port_num; port++) {
            if (out_port == card->user_open_param.outport[port].port) {
                return HI_TRUE;
            }
        }
    }

    return HI_FALSE;
}

hi_s32 ao_get_snd_def_open_attr(ao_snd_id sound, hi_ao_attr *attr)
{
    hi_s32 ret;
    hi_u32 port_num = 0;

    ret = ao_get_open_attr_from_bootargs(sound, attr);
    if (ret == HI_SUCCESS) {
        HI_INFO_AO("call ao_get_open_attr_from_bootargs success\n");
        return HI_SUCCESS;
    }

    ret = ao_get_open_attr_from_pdm(sound, attr);
    if (ret == HI_SUCCESS) {
        HI_INFO_AO("call ao_get_open_attr_from_pdm success\n");
        return HI_SUCCESS;
    }

    if (sound == AO_SND_0) {
        attr->outport[port_num].port = HI_AO_PORT_DAC0;
        attr->outport[port_num].un_attr.dac_attr.reserved = HI_NULL;
        port_num++;

        attr->outport[port_num].port = HI_AO_PORT_SPDIF0;
        attr->outport[port_num].un_attr.spdif_attr.reserved = HI_NULL;
        port_num++;

        attr->outport[port_num].port = HI_AO_PORT_HDMI0;
        attr->outport[port_num].un_attr.hdmi_attr.reserved = HI_NULL;
        port_num++;
    } else {
        attr->outport[port_num].port = HI_AO_PORT_HDMI1;
        attr->outport[port_num].un_attr.hdmi_attr.reserved = HI_NULL;
        port_num++;
    }

    attr->port_num = port_num;
    attr->sample_rate = HI_SAMPLE_RATE_48K;

    return HI_SUCCESS;
}

static hi_bool ao_check_out_port_is_valid(hi_ao_port out_port)
{
    if ((out_port != HI_AO_PORT_DAC0) &&
        (out_port != HI_AO_PORT_SPDIF0) &&
        (out_port != HI_AO_PORT_HDMI0) &&
        (out_port != HI_AO_PORT_HDMI1) &&
        (out_port != HI_AO_PORT_I2S0) &&
        (out_port != HI_AO_PORT_I2S1)) {
        HI_ERR_AO("just support I2S0, I2S1, DAC0, SPDIF0, HDMI0, HDMI1 port!\n");
        return HI_FALSE;
    }

    return HI_TRUE;
}

static hi_s32 ao_init_card_state(snd_card_state *card)
{
    hi_s32 ret;

    ret = memset_s(card, sizeof(snd_card_state), 0, sizeof(snd_card_state));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        return ret;
    }

    OSAL_INIT_LIST_HEAD(&card->op);
    OSAL_INIT_LIST_HEAD(&card->track);
    OSAL_INIT_LIST_HEAD(&card->engine);
    OSAL_INIT_LIST_HEAD(&card->cast);

    card->hdmi_port = HI_AO_PORT_MAX;
    card->hdmi_passthrough = TYPE_MAX;
    card->spdif_passthrough = TYPE_MAX;
    card->cast_simulate_op = HI_FALSE;

    card->other_track_mute = HI_FALSE;

#ifdef HI_PROC_SUPPORT
    card->save_state = SND_DEBUG_CMD_CTRL_STOP;
#endif

#ifdef HI_SND_AVC_SUPPORT
    ao_get_def_avc_attr(&card->avc_attr);
#endif

    return HI_SUCCESS;
}

hi_s32 ao_check_out_port_attr(ao_snd_id sound, hi_ao_port_attr *out_port)
{
    if (ao_check_out_port_is_valid(out_port->port) == HI_FALSE) {
        HI_ERR_AO("port(%d) is invalid.\n", out_port->port);
        return HI_FAILURE;
    }

    if (ao_check_out_port_is_attached(sound, out_port->port) == HI_TRUE) {
        HI_ERR_AO("port(%d) is aready attatched.\n", out_port->port);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#if defined(HI_SND_AR_SUPPORT)
static hi_void ao_release_isb_buf(snd_card_state *card)
{
    hi_s32 isb_num;
    ao_mgmt *mgmt = ao_get_mgmt();

    for (isb_num = 0; isb_num < AO_RENDER_TRACK_NUM; isb_num++) {
        if (mgmt->snd_entity[AO_SND_0].isb_mmz[isb_num].phys_addr) {
            hi_drv_audio_mmz_release(&mgmt->snd_entity[AO_SND_0].isb_mmz[isb_num]);
            mgmt->snd_entity[AO_SND_0].isb_mmz[isb_num].phys_addr = 0;
        }
    }
}
#endif

hi_s32 ao_snd_open(ao_snd_id sound, hi_ao_attr *attr, hi_bool resume)
{
    hi_s32 ret;
    hi_u32 port_id;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(attr);
    CHECK_AO_PORTNUM(attr->port_num);
    CHECK_AO_SAMPLERATE(attr->sample_rate);

    ret = ao_init_card_state(card);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_init_card_state, ret);
        return ret;
    }

    card->user_sample_rate = attr->sample_rate;
    card->sound = sound;

    for (port_id = 0; port_id < attr->port_num; port_id++) {
        ret = ao_check_out_port_attr(sound, &attr->outport[port_id]);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("check outport attr failed(0x%x)\n", ret);
            return ret;
        }
    }

    ret = ao_snd_create_op(card, attr, resume);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_snd_create_op, ret);
        return ret;
    }

    ret = ao_engine_init(card);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_engine_init, ret);
        goto ERR_EXIT0;
    }

#ifdef HI_PROC_SUPPORT
    ret = ao_reg_proc(card);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_reg_proc, ret);
        goto ERR_EXIT1;
    }
#endif

    return HI_SUCCESS;

#ifdef HI_PROC_SUPPORT
ERR_EXIT1:
    ao_engine_deinit(card);
#endif

ERR_EXIT0:
    ao_snd_destroy_op(card, HI_FALSE);

    return ret;
}

hi_s32 ao_snd_close(ao_snd_id sound, hi_bool suspend)
{
    snd_card_state *card = snd_card_get_card(sound);

#ifdef HI_PROC_SUPPORT
    ao_unreg_proc(card);
#endif

    ao_engine_deinit(card);

#if defined(HI_SND_AR_SUPPORT)
    ao_release_isb_buf(card);
#endif

    ao_snd_destroy_op(card, suspend);

    return HI_SUCCESS;
}

hi_s32 ao_snd_set_mute(ao_snd_id sound, hi_ao_port out_port, hi_bool mute)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    ret = snd_set_op_mute(card, out_port, mute);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("call snd_set_op_mute failed(0x%x)\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 ao_snd_get_mute(ao_snd_id sound, hi_ao_port out_port, hi_bool *mute)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(mute);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_mute(card, out_port, mute);
}

hi_s32 ao_snd_set_volume(ao_snd_id sound, hi_ao_port out_port, hi_ao_gain *gain)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(gain);
    CHECK_AO_NULL_PTR(card);

    if (gain->linear_mode == HI_TRUE) {
        CHECK_AO_LINEARVOLUME(gain->gain);
    } else {
        CHECK_AO_ABSLUTEVOLUME(gain->gain);
    }

    return snd_set_op_volume(card, out_port, gain);
}

hi_s32 ao_snd_get_volume(ao_snd_id sound, hi_ao_port out_port, hi_ao_gain *gain)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(gain);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_volume(card, out_port, gain);
}

#if defined (HI_SOUND_PORT_DELAY_SUPPORT)
hi_s32 ao_snd_set_delay_compensation(ao_snd_id sound, hi_ao_port out_port, hi_u32 delay_ms)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    if (delay_ms > AO_MAX_DELAYMS) {
        HI_ERR_AO("invalid delay_ms(%d), max(%d)\n", delay_ms, AO_MAX_DELAYMS);
        return HI_ERR_AO_INVALID_PARA;
    }

    ret = snd_set_delay_compensation(card, out_port, delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("snd_set_delay_compensation (out_port = 0x%x) failed(0x%x)\n", out_port, ret);
    }

    return ret;
}

hi_s32 ao_snd_get_delay_compensation(ao_snd_id sound, hi_ao_port out_port, hi_u32 *delay_ms)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(delay_ms);

    ret = snd_get_delay_compensation(card, out_port, delay_ms);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("snd_get_delay_compensation (out_port = 0x%x) failed(0x%x)\n", out_port, ret);
    }

    return ret;
}
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 ao_snd_set_low_latency(ao_snd_id sound, hi_ao_port out_port, hi_u32 latency_ms)
{
    snd_card_state *card = snd_card_get_card(sound);
    CHECK_AO_NULL_PTR(card);

    return snd_set_low_latency(card, out_port, latency_ms);
}

hi_s32 ao_snd_get_low_latency(ao_snd_id sound, hi_ao_port out_port, hi_u32 *latency_ms)
{
    snd_card_state *card = snd_card_get_card(sound);
    CHECK_AO_NULL_PTR(card);

    return snd_get_low_latency(card, out_port, latency_ms);
}

hi_s32 ao_snd_set_ext_delay_ms(ao_snd_id sound, hi_u32 delay_ms)
{
    snd_card_state *card = snd_card_get_card(sound);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    card->ext_delay_ms = delay_ms;

    return HI_SUCCESS;
}

hi_s32 ao_snd_alloc_dma(hi_void *file, ao_snd_id sound)
{
    ao_mgmt *mgmt = ao_get_mgmt();
    /* DMA only support single user */
    if (osal_atomic_read(&mgmt->snd_dma_entity[sound].atm_use_cnt) != 0) {
        return HI_ERR_AO_USED;
    }

    if (mgmt->low_latency_created == HI_TRUE) {
        HI_ERR_AO("cannot enable SND DMA mode when low_latency track created!\n");
        return HI_ERR_AO_USED;
    }

    mgmt->snd_dma_entity[sound].file = file;
    osal_atomic_inc_return(&mgmt->snd_dma_entity[sound].atm_use_cnt);
    return HI_SUCCESS;
}

hi_s32 ao_snd_free_dma(hi_void *file, ao_snd_id sound)
{
    ao_mgmt *mgmt = ao_get_mgmt();
    if (file != mgmt->snd_dma_entity[sound].file) {
        HI_ERR_AO("invalid file handle!\n");
        return HI_ERR_AO_INVALID_PARA;
    }
    mgmt->snd_dma_entity[sound].file = HI_NULL;
    osal_atomic_set(&mgmt->snd_dma_entity[sound].atm_use_cnt, 0);
    return HI_SUCCESS;
}

static hi_s32 ao_snd_dma_destory(hi_void *file, ao_snd_id sound)
{
    hi_s32 ret = HI_SUCCESS;
    snd_card_state *card = snd_card_get_card(sound);

    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    if (card->dma_mode == HI_TRUE) {
        ret = snd_set_port_sample_rate(card, HI_AO_PORT_ALL, (hi_u32)(card->user_sample_rate));
        if (ret != HI_SUCCESS) {
            HI_ERR_AO ("snd_set_port_sample_rate(%d) HI_AO_PORT_ALL failed!\n", (hi_u32)(card->user_sample_rate));
            return ret;
        }
        ret = ao_snd_free_dma(file, sound);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("ao_snd_free_dma failed!\n");
            return ret;
        }
        card->dma_mode = HI_FALSE;
    }
    return ret;
}

hi_s32 ao_snd_dma_create(hi_void *file, ao_snd_id sound, snd_port_kernel_attr *port_k_attr)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);
    CHECK_AO_NULL_PTR(card);

    ret = ao_snd_alloc_dma(file, sound);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("SND DMA mode occupied!\n");
        return ret;
    }
    card->dma_mode = HI_TRUE;

    ret = track_force_all_to_pcm(card);  /* warning: might sleep */
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("track_route_all failed!\n");
        (hi_void) ao_snd_free_dma(file, sound);
        card->dma_mode = HI_FALSE;
        return ret;
    }

    ret = snd_get_port_info(card, HI_AO_PORT_ALL, port_k_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("snd_get_port_info failed!\n");
        (hi_void) ao_snd_free_dma(file, sound);
        card->dma_mode = HI_FALSE;
    }

    return ret;
}

hi_s32 ao_snd_dma_mode_set_sample_rate(ao_snd_id sound, hi_sample_rate sample_rate)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    if (card->dma_mode == HI_FALSE) {
        return HI_ERR_AO_NOTSUPPORT;
    }

    ret = snd_set_port_sample_rate(card, HI_AO_PORT_ALL, (hi_u32)(sample_rate));
    if (ret != HI_SUCCESS) {
        HI_ERR_AO ("snd_set_port_sample_rate(%d) HI_AO_PORT_ALL failed!\n", (hi_u32)(sample_rate));
    }

    return ret;
}
#endif

hi_s32 ao_snd_set_preci_vol(ao_snd_id sound, hi_ao_port out_port, hi_ao_preci_gain *preci_gain)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_ABSLUTEPRECIVOLUME(preci_gain->integer, preci_gain->decimal);
    CHECK_AO_NULL_PTR(card);

    return snd_set_op_preci_vol(card, out_port, preci_gain);
}

hi_s32 ao_snd_get_preci_vol(ao_snd_id sound, hi_ao_port out_port, hi_ao_preci_gain *preci_gain)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(preci_gain);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_preci_vol(card, out_port, preci_gain);
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 ao_snd_set_balance(ao_snd_id sound, hi_ao_port out_port, hi_s32 balance)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_BALANCE(balance);
    CHECK_AO_NULL_PTR(card);

    return snd_set_op_balance(card, out_port, balance);
}

hi_s32 ao_snd_get_balance(ao_snd_id sound, hi_ao_port out_port, hi_s32 *balance)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(balance);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_balance(card, out_port, balance);
}

hi_s32 ao_snd_get_x_run_count(ao_snd_id sound, hi_u32 *count)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    snd_get_x_run_count(card, count);

    return HI_SUCCESS;
}

hi_s32 ao_snd_set_ad_output_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    return snd_set_op_ad_output_enable(card, out_port, enable);
}

hi_s32 ao_snd_get_ad_output_enable(ao_snd_id sound, hi_ao_port out_port, hi_bool *enable)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_NULL_PTR(enable);

    return snd_get_op_ad_output_enable(card, out_port, enable);
}
#endif

static hi_s32 ao_snd_set_track_mode(ao_snd_id sound, hi_ao_port out_port, hi_track_mode mode)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_TRACKMODE(mode);
    CHECK_AO_NULL_PTR(card);

    return snd_set_op_track_mode(card, out_port, mode);
}

static hi_s32 ao_snd_get_track_mode(ao_snd_id sound, hi_ao_port out_port, hi_track_mode *mode)
{
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(mode);
    CHECK_AO_NULL_PTR(card);

    return snd_get_op_track_mode(card, out_port, mode);
}

hi_s32 ao_ioctl_get_snd_def_open_attr(hi_void *file, hi_void *arg)
{
    ao_snd_open_default_param_p param = (ao_snd_open_default_param_p)arg;

    return ao_get_snd_def_open_attr(param->sound, &param->attr);
}

hi_s32 ao_ioctl_snd_open(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    ao_snd_id sound;
    drv_ao_state *ao_state = *((drv_ao_state **)file);
    ao_snd_open_param_p param = (ao_snd_open_param_p)arg;
    ao_mgmt *mgmt = ao_get_mgmt();

    sound = param->sound;
    CHECK_AO_SNDCARD(sound);

    ret = ao_snd_alloc_handle(sound, file);
    if (ret == HI_SUCCESS) {
        if (osal_atomic_read(&mgmt->snd_entity[sound].atm_use_total_cnt) == 0) {
            ret = ao_snd_open(sound, &param->attr, HI_FALSE);
            if (ret != HI_SUCCESS) {
                ao_snd_free_handle(sound, file);
                return ret;
            }
        }
    }

    osal_atomic_inc_return(&mgmt->snd_entity[sound].atm_use_total_cnt);
    osal_atomic_inc_return(&ao_state->atm_user_open_cnt[sound]);

    return ret;
}

hi_s32 ao_ioctl_snd_close(hi_void *file, hi_void *arg)
{
    ao_snd_id sound;
    hi_u32 track_idx;
    drv_ao_state *ao_state = *((drv_ao_state **)file);
    ao_mgmt *mgmt = ao_get_mgmt();

    sound = *(ao_snd_id *)arg;
    CHECK_AO_SNDCARD_OPEN(sound);

    if (osal_atomic_dec_return(&ao_state->atm_user_open_cnt[sound]) == 0) {
        if (osal_atomic_dec_return(&mgmt->snd_entity[sound].atm_use_total_cnt) == 0) {
            hi_s32 ret;

            /* destory all track before close sound */
            for (track_idx = 0; track_idx < AO_MAX_TOTAL_TRACK_NUM; track_idx++) {
                if (osal_atomic_read(&mgmt->track_entity[track_idx].atm_use_cnt)) {
                    if (track_card_get_card(track_idx) != snd_card_get_card(sound)) {
                        continue;
                    }
                    (hi_void) ao_track_destory(track_idx);
                    ao_track_free_handle_by_id(track_idx);
                }
            }

            ret = ao_snd_close(sound, HI_FALSE);
            if (ret != HI_SUCCESS) {
                osal_atomic_inc_return(&mgmt->snd_entity[sound].atm_use_total_cnt);
                osal_atomic_inc_return(&ao_state->atm_user_open_cnt[sound]);
                return ret;
            }

            ao_snd_free_handle(sound, file);
        }
    } else {
        osal_atomic_dec_return(&mgmt->snd_entity[sound].atm_use_total_cnt);
    }

    return HI_SUCCESS;
}

hi_s32 ao_ioctl_snd_set_mute(hi_void *file, hi_void *arg)
{
    ao_snd_mute_param_p mute = (ao_snd_mute_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(mute->sound);
    return ao_snd_set_mute(mute->sound, mute->out_port, mute->mute);
}

hi_s32 ao_ioctl_snd_get_mute(hi_void *file, hi_void *arg)
{
    ao_snd_mute_param_p mute = (ao_snd_mute_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(mute->sound);
    return ao_snd_get_mute(mute->sound, mute->out_port, &mute->mute);
}

hi_s32 ao_ioctl_snd_set_volume(hi_void *file, hi_void *arg)
{
    ao_snd_volume_param_p volume = (ao_snd_volume_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(volume->sound);
    return ao_snd_set_volume(volume->sound, volume->out_port, &volume->gain);
}

hi_s32 ao_ioctl_snd_get_volume(hi_void *file, hi_void *arg)
{
    ao_snd_volume_param_p volume = (ao_snd_volume_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(volume->sound);
    return ao_snd_get_volume(volume->sound, volume->out_port, &volume->gain);
}

hi_s32 ao_ioctl_snd_set_output_mode(hi_void *file, hi_void *arg)
{
    hi_s32 ret;
    ao_snd_output_mode_param_p mode = (ao_snd_output_mode_param_p)arg;
    snd_card_state *card = snd_card_get_card(mode->sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_OUTPORT(mode->ao_port);
    CHECK_AO_OUTPUT_MODE(mode->mode);

    ret = snd_op_set_output_mode(card, mode->ao_port, mode->mode);
#if defined(HI_SND_AR_SUPPORT)
    if (ret == HI_SUCCESS) {
        ao_snd_update_render_raw_info(mode->sound, mode->mode);
    }
#endif

    return ret;
}

hi_s32 ao_ioctl_snd_get_output_mode(hi_void *file, hi_void *arg)
{
    ao_snd_output_mode_param_p mode = (ao_snd_output_mode_param_p)arg;
    snd_card_state *card = snd_card_get_card(mode->sound);

    CHECK_AO_NULL_PTR(card);
    CHECK_AO_OUTPORT(mode->ao_port);

    return snd_op_get_output_mode(card, mode->ao_port, &mode->mode);
}

hi_s32 ao_ioctl_snd_set_preci_volume(hi_void *file, hi_void *arg)
{
    ao_snd_preci_volume_param_p preci_vol = (ao_snd_preci_volume_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(preci_vol->sound);
    return ao_snd_set_preci_vol(preci_vol->sound, preci_vol->out_port, &preci_vol->preci_gain);
}

hi_s32 ao_ioctl_snd_get_preci_volume(hi_void *file, hi_void *arg)
{
    ao_snd_preci_volume_param_p preci_vol = (ao_snd_preci_volume_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(preci_vol->sound);
    return ao_snd_get_preci_vol(preci_vol->sound, preci_vol->out_port, &preci_vol->preci_gain);
}

#ifdef __DPT__
hi_s32 ao_ioctl_snd_set_alsa_prescale(hi_void *file, hi_void *arg)
{
    ao_snd_set_alsa_prescale_p alsa_prescale = (ao_snd_set_alsa_prescale_p)arg;
    CHECK_AO_SNDCARD_OPEN(alsa_prescale->sound);
    return ao_snd_set_alsa_prescale(alsa_prescale->sound, &alsa_prescale->preci_gain);
}
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 ao_ioctl_snd_set_balance(hi_void *file, hi_void *arg)
{
    ao_snd_balance_param_p balance = (ao_snd_balance_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(balance->sound);
    return ao_snd_set_balance(balance->sound, balance->out_port, balance->balance);
}

hi_s32 ao_ioctl_snd_get_balance(hi_void *file, hi_void *arg)
{
    ao_snd_balance_param_p balance = (ao_snd_balance_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(balance->sound);
    return ao_snd_get_balance(balance->sound, balance->out_port, &balance->balance);
}

hi_s32 ao_ioctl_snd_get_x_run_count(hi_void *file, hi_void *arg)
{
    ao_snd_get_xrun_param_p xrun_status = (ao_snd_get_xrun_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(xrun_status->sound);
    return ao_snd_get_x_run_count(xrun_status->sound, &xrun_status->count);
}
#endif

#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
hi_s32 ao_ioctl_snd_set_delay_compensation(hi_void *file, hi_void *arg)
{
    ao_snd_op_delay_param_p set_delay = (ao_snd_op_delay_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(set_delay->sound);
    return ao_snd_set_delay_compensation(set_delay->sound, set_delay->out_port, set_delay->delay_ms);
}

hi_s32 ao_ioctl_snd_get_delay_compensation(hi_void *file, hi_void *arg)
{
    ao_snd_op_delay_param_p set_delay = (ao_snd_op_delay_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(set_delay->sound);
    return ao_snd_get_delay_compensation(set_delay->sound, set_delay->out_port, &set_delay->delay_ms);
}
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 ao_ioctl_snd_set_low_latency(hi_void *file, hi_void *arg)
{
    ao_snd_set_low_latency_param_p param = (ao_snd_set_low_latency_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_set_low_latency(param->sound, param->out_port, param->latency_ms);
}

hi_s32 ao_ioctl_snd_get_low_latency(hi_void *file, hi_void *arg)
{
    ao_snd_set_low_latency_param_p param = (ao_snd_set_low_latency_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_get_low_latency(param->sound, param->out_port, &param->latency_ms);
}

hi_s32 ao_ioctl_snd_set_ext_delay_ms(hi_void *file, hi_void *arg)
{
    ao_snd_set_ext_delay_ms_param_p ext_delay_ms_param = (ao_snd_set_ext_delay_ms_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(ext_delay_ms_param->sound);
    return ao_snd_set_ext_delay_ms(ext_delay_ms_param->sound, ext_delay_ms_param->delay_ms);
}

hi_s32 ao_ioctl_snd_dma_create(hi_void *file, hi_void *arg)
{
    ao_snd_dma_create_param_p param = (ao_snd_dma_create_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_dma_create(file, param->sound, param->port_k_attr);
}

hi_s32 ao_ioctl_snd_dma_destory(hi_void *file, hi_void *arg)
{
    ao_snd_dma_create_param_p param = (ao_snd_dma_create_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_dma_destory(file, param->sound);
}

hi_s32 ao_ioctl_snd_dma_mode_set_sample_rate(hi_void *file, hi_void *arg)
{
    ao_snd_dma_set_sample_rate_param_p param = (ao_snd_dma_set_sample_rate_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(param->sound);
    return ao_snd_dma_mode_set_sample_rate(param->sound, param->sample_rate);
}

hi_s32 ao_ioctl_snd_set_ad_output_enable(hi_void *file, hi_void *arg)
{
    ao_snd_ad_output_enable_p ad_output_eanble = (ao_snd_ad_output_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(ad_output_eanble->sound);
    return ao_snd_set_ad_output_enable(ad_output_eanble->sound,
        ad_output_eanble->out_port, ad_output_eanble->ad_output_enable);
}

hi_s32 ao_ioctl_snd_get_ad_output_enable(hi_void *file, hi_void *arg)
{
    ao_snd_ad_output_enable_p ad_output_eanble = (ao_snd_ad_output_enable_p)arg;
    CHECK_AO_SNDCARD_OPEN(ad_output_eanble->sound);
    return ao_snd_get_ad_output_enable(ad_output_eanble->sound,
        ad_output_eanble->out_port, &ad_output_eanble->ad_output_enable);
}
#endif

hi_s32 ao_ioctl_snd_set_track_mode(hi_void *file, hi_void *arg)
{
    ao_snd_track_mode_param_p mode = (ao_snd_track_mode_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(mode->sound);
    return ao_snd_set_track_mode(mode->sound, mode->out_port, mode->mode);
}

hi_s32 ao_ioctl_snd_get_track_mode(hi_void *file, hi_void *arg)
{
    ao_snd_track_mode_param_p mode = (ao_snd_track_mode_param_p)arg;
    CHECK_AO_SNDCARD_OPEN(mode->sound);
    return ao_snd_get_track_mode(mode->sound, mode->out_port, &mode->mode);
}

hi_s32 ao_snd_get_settings(ao_snd_id sound, snd_card_settings *snd_settings)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    snd_settings->all_track_mute = card->all_track_mute;

#if defined(HI_SND_AR_SUPPORT)
    ret = memcpy_s(&snd_settings->render_attr, sizeof(ao_render_attr),
        &card->render_attr, sizeof(ao_render_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }
#endif

    ret = snd_get_op_setting(card, snd_settings);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_get_op_setting, ret);
        return ret;
    }

    snd_settings->all_track_preci_gain.integer = card->all_track_preci_gain.integer;
    snd_settings->all_track_preci_gain.decimal = card->all_track_preci_gain.decimal;

    return HI_SUCCESS;
}

hi_s32 ao_snd_restore_settings(ao_snd_id sound, snd_card_settings *snd_settings)
{
    hi_s32 ret;
    snd_card_state *card = snd_card_get_card(sound);

    CHECK_AO_NULL_PTR(card);

    card->all_track_mute = snd_settings->all_track_mute;

    ret = snd_restore_op_setting(card, snd_settings);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(snd_restore_op_setting, ret);
        return ret;
    }

#if defined(HI_SND_AR_SUPPORT)
    ret = memcpy_s(&card->render_attr, sizeof(ao_render_attr),
        &snd_settings->render_attr, sizeof(ao_render_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    card->con_output_enable = snd_settings->con_output_enable;
    ret = ao_snd_set_con_output_enable(sound, card->con_output_enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_snd_set_con_output_enable, ret);
        return ret;
    }

    card->output_atmos_enable = snd_settings->output_atmos_enable;
    ret = ao_snd_set_output_atmos_enable(sound, card->output_atmos_enable);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_snd_set_output_atmos_enable, ret);
        return ret;
    }
#endif

    card->all_track_preci_gain.integer = snd_settings->all_track_preci_gain.integer;
    card->all_track_preci_gain.decimal = snd_settings->all_track_preci_gain.decimal;

    return HI_SUCCESS;
}

hi_s32 hi_drv_ao_init(hi_void)
{
    return ao_drv_init();
}

hi_void hi_drv_ao_deinit(hi_void)
{
    ao_drv_exit();
}

hi_s32 hi_drv_ao_snd_init(hi_void *file)
{
    file = (file == HI_NULL) ? &g_filp : file;

    HI_INFO_AO("hi_drv_ao_snd_init file = %p\n", file);

    return ao_drv_open(file);
}

hi_s32 hi_drv_ao_snd_deinit(hi_void *file)
{
    file = (file == HI_NULL) ? &g_filp : file;

    HI_INFO_AO("hi_drv_ao_snd_de_init file = %p\n", file);

    return ao_drv_release(file);
}

hi_s32 hi_drv_ao_snd_get_default_open_attr(ao_snd_id sound, hi_ao_attr *attr)
{
    hi_s32 ret;

    CHECK_AO_SNDCARD(sound);

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_get_snd_def_open_attr(sound, attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_get_snd_def_open_attr, ret);
    }

    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 hi_drv_ao_snd_open(ao_snd_id sound, hi_ao_attr *attr, hi_void *file)
{
    hi_s32 ret;
    drv_ao_state *ao_state = HI_NULL;
    ao_mgmt *mgmt = ao_get_mgmt();

    CHECK_AO_SNDCARD(sound);

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    file = (file == HI_NULL) ? &g_filp : file;
    ao_state = *((drv_ao_state **)file);

    HI_INFO_AO("hi_drv_ao_snd_open file = %p\n", file);

    ret = ao_snd_alloc_handle(sound, file);
    if (ret == HI_SUCCESS) {
        if (osal_atomic_read(&mgmt->snd_entity[sound].atm_use_total_cnt) == 0) {
            ret = ao_snd_open(sound, attr, HI_FALSE);
            if (ret != HI_SUCCESS) {
                HI_ERR_AO("ao_snd_open failed(0x%x)\n", ret);
                ao_snd_free_handle(sound, file);
                osal_sem_up(&g_ao_mutex);
                return ret;
            }
        }
    }

    osal_atomic_inc_return(&mgmt->snd_entity[sound].atm_use_total_cnt);
    osal_atomic_inc_return(&ao_state->atm_user_open_cnt[sound]);
    osal_sem_up(&g_ao_mutex);

    return ret;
}

hi_s32 hi_drv_ao_snd_close(ao_snd_id sound, hi_void *file)
{
    hi_s32 ret;
    hi_u32 track_idx = 0;
    drv_ao_state *ao_state = HI_NULL;
    ao_mgmt *mgmt = ao_get_mgmt();

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    file = (file == HI_NULL) ? &g_filp : file;
    ao_state = *((drv_ao_state **)file);

    CHECK_AO_SNDCARD_OPEN(sound);

    HI_INFO_AO("hi_drv_ao_snd_close file = %p\n", file);

    if (osal_atomic_dec_return(&ao_state->atm_user_open_cnt[sound]) == 0) {
        if (osal_atomic_dec_return(&mgmt->snd_entity[sound].atm_use_total_cnt) == 0) {
            /* destory all track before close sound */
            for (track_idx = 0; track_idx < AO_MAX_TOTAL_TRACK_NUM; track_idx++) {
                if (osal_atomic_read(&mgmt->track_entity[track_idx].atm_use_cnt)) {
                    if (track_card_get_card(track_idx) != snd_card_get_card(sound)) {
                        continue;
                    }
                    (hi_void) ao_track_destory(track_idx);
                    ao_track_free_handle_by_id(track_idx);
                }
            }

            ret = ao_snd_close(sound, HI_FALSE);
            if (ret != HI_SUCCESS) {
                osal_atomic_inc_return(&mgmt->snd_entity[sound].atm_use_total_cnt);
                osal_atomic_inc_return(&ao_state->atm_user_open_cnt[sound]);
                HI_ERR_AO("ao_snd_close failed(0x%x)\n", ret);
                osal_sem_up(&g_ao_mutex);
                return ret;
            }

            ao_snd_free_handle(sound, file);
        }
    } else {
        osal_atomic_dec_return(&mgmt->snd_entity[sound].atm_use_total_cnt);
    }
    osal_sem_up(&g_ao_mutex);

    return HI_SUCCESS;
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_s32 hi_drv_ao_snd_get_x_run_count(ao_snd_id sound, hi_u32 *count)
{
    CHECK_AO_SNDCARD_OPEN(sound);
    return ao_snd_get_x_run_count(sound, count);
}
#endif

hi_s32 hi_drv_ao_snd_set_volume(ao_snd_id sound, hi_ao_port ao_port, hi_ao_gain *gain)
{
    hi_s32 ret;
    CHECK_AO_SNDCARD_OPEN(sound);

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_snd_set_volume(sound, ao_port, gain);
    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 hi_drv_ao_snd_get_volume(ao_snd_id sound, hi_ao_port ao_port, hi_ao_gain *gain)
{
    hi_s32 ret;
    CHECK_AO_SNDCARD_OPEN(sound);
    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_snd_get_volume(sound, ao_port, gain);
    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 hi_drv_ao_snd_set_mute(ao_snd_id sound, hi_ao_port ao_port, hi_bool mute)
{
    hi_s32 ret;
    CHECK_AO_SNDCARD_OPEN(sound);

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_snd_set_mute(sound, ao_port, mute);
    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 hi_drv_ao_snd_get_mute(ao_snd_id sound, hi_ao_port ao_port, hi_bool *mute)
{
    hi_s32 ret;
    CHECK_AO_SNDCARD_OPEN(sound);

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_snd_get_mute(sound, ao_port, mute);
    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 hi_drv_ao_snd_get_alsa_track_delay(ao_snd_id sound, hi_u32 *delay)
{
    hi_s32 ret;
    CHECK_AO_SNDCARD_OPEN(sound);
    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_snd_get_alsa_track_delay_ms(sound, delay);
    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 hi_drv_ao_track_get_default_open_attr(hi_ao_track_type track_type, hi_ao_track_attr *attr)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    attr->track_type = track_type;
    ret = ao_track_get_def_attr(attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("ao_track_get_def_attr failed(0x%x)\n", ret);
        osal_sem_up(&g_ao_mutex);
        return ret;
    }
    osal_sem_up(&g_ao_mutex);

    return ret;
}

hi_s32 hi_drv_ao_track_create(ao_snd_id sound, hi_ao_track_attr *attr,
    hi_bool alsa_track, hi_void *file, hi_handle *track)
{
    hi_s32 ret;

    ao_track_create_param track_arg = {
        .sound = sound,
        .alsa_track = alsa_track,
        .h_track = HI_INVALID_HANDLE,
    };

    CHECK_AO_NULL_PTR(attr);
    CHECK_AO_NULL_PTR(track);

    file = (file == HI_NULL) ? &g_filp : file;

    HI_INFO_AO("hi_drv_ao_track_create alsa_track = %d, file = %p\n", alsa_track, file);

    ret = memcpy_s(&track_arg.attr, sizeof(hi_ao_track_attr), attr, sizeof(hi_ao_track_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_ioctl_track_create(file, &track_arg);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("ao_ioctl_track_create failed(0x%x)\n", ret);
        osal_sem_up(&g_ao_mutex);
        return ret;
    }

    ret = ao_track_set_used_by_kernel(track_arg.h_track);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("ao_track_set_used_by_kernel failed(0x%x)\n", ret);
        (hi_void)ao_ioctl_track_destory(file, &track_arg.h_track);
        osal_sem_up(&g_ao_mutex);
        return ret;
    }

    *track = track_arg.h_track;
    osal_sem_up(&g_ao_mutex);

    return ret;
}

hi_s32 hi_drv_ao_track_destroy(hi_handle track)
{
    hi_s32 ret;
    CHECK_AO_TRACK_OPEN(track);

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_track_destory(track);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("ao_track_destory failed(0x%x)\n", ret);
        osal_sem_up(&g_ao_mutex);
        return ret;
    }

    ao_track_free_handle(track);
    osal_sem_up(&g_ao_mutex);

    return ret;
}

hi_s32 hi_drv_ao_track_flush(hi_handle track)
{
    CHECK_AO_TRACK_OPEN(track);
    return ao_track_flush(track);
}

hi_s32 hi_drv_ao_track_start(hi_handle track)
{
    CHECK_AO_TRACK_OPEN(track);
    return ao_track_start(track);
}

hi_s32 hi_drv_ao_track_stop(hi_handle track)
{
    hi_s32 ret;
    CHECK_AO_TRACK_OPEN(track);

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    ret = ao_track_stop(track);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("ao_track_destory failed(0x%x)\n", ret);
        osal_sem_up(&g_ao_mutex);
        return ret;
    }

    osal_sem_up(&g_ao_mutex);
    return ret;
}

hi_s32 hi_drv_ao_track_get_delay(hi_handle track, hi_u32 *delay)
{
    CHECK_AO_TRACK_OPEN(track);
    return ao_track_get_delay_ms(track, delay);
}

hi_s32 hi_drv_ao_track_send_data(hi_handle track, ao_frame *frame)
{
    CHECK_AO_TRACK_OPEN(track);
    return ao_track_send_data(track, frame);
}

hi_s32 hi_drv_ao_track_atomic_send_data(hi_handle track, ao_frame *frame)
{
    hi_u32 track_id;
    snd_card_state *card = HI_NULL;

    CHECK_AO_TRACK_OPEN(track);

    track_id = track & AO_TRACK_CHNID_MASK;

    card = track_card_get_card(track_id);
    if (card == HI_NULL) {
        return HI_ERR_AO_SOUND_NOT_OPEN;
    }

    if (card->dma_mode == HI_TRUE) {
        return HI_SUCCESS;
    }

    return track_atomic_send_data(card, track_id, frame);
}

hi_s32 hi_drv_ao_track_attach_ai(hi_handle track, hi_handle ai)
{
    CHECK_AO_TRACK_OPEN(track);
    return ao_track_attach_ai(track, ai);
}

hi_s32 hi_drv_ao_track_detach_ai(hi_handle track, hi_handle ai)
{
    CHECK_AO_TRACK_OPEN(track);
    return ao_track_detach_ai(track, ai);
}

hi_s32 ao_drv_suspend(hi_void)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;
    ao_mgmt *mgmt = ao_get_mgmt();

#ifdef HI_AIAO_TIMER_SUPPORT
    ret = hi_drv_timer_suspend();
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("AO hi_drv_timer_suspend fail 0x%x\n", ret);
    }
#endif

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    if (mgmt->ready == HI_TRUE) {
#ifdef HI_SND_CAST_SUPPORT
        /* destory all cast */
        for (i = 0; i < AO_MAX_CAST_NUM; i++) {
            if (osal_atomic_read(&mgmt->cast_entity[i].atm_use_cnt)) {
                /* store cast settings */
                ao_cast_get_settings(i, &mgmt->cast_entity[i].suspend_attr);

                /* destory cast */
                ret = ao_cast_destory(i);
                if (ret != HI_SUCCESS) {
                    HI_FATAL_AO("ao_cast_destory fail\n");
                    osal_sem_up(&g_ao_mutex);
                    return HI_FAILURE;
                }
            }
        }
#endif

        /* destory all track */
        for (i = 0; i < AO_MAX_TOTAL_TRACK_NUM; i++) {
            if (osal_atomic_read(&mgmt->track_entity[i].atm_use_cnt)) {
                /* store track settings */
                ao_track_get_settings(i, &mgmt->track_entity[i].suspend_attr);

                /* destory track */
                ret = ao_track_destory(i);
                if (ret != HI_SUCCESS) {
                    HI_FATAL_AO("ao_track_destory fail\n");
                    osal_sem_up(&g_ao_mutex);
                    return HI_FAILURE;
                }
            }
        }

        /* destory all snd */
        for (i = 0; i < AO_MAX_TOTAL_SND_NUM; i++) {
            if (mgmt->snd_entity[i].card != HI_NULL) {
                /* store snd settings */
                ao_snd_get_settings(i, &mgmt->snd_entity[i].suspend_attr);

                /* destory snd */
                ret = ao_snd_close(i, HI_TRUE);
                if (ret != HI_SUCCESS) {
                    HI_FATAL_AO("ao_snd_close fail\n");
                    osal_sem_up(&g_ao_mutex);
                    return HI_FAILURE;
                }
            }
        }

        hal_aoe_deinit();

        ret = hal_aiao_suspend();
        if (ret != HI_SUCCESS) {
            HI_FATAL_AO("AIAO suspend fail\n");
            osal_sem_up(&g_ao_mutex);
            return HI_FAILURE;
        }

        adac_tianlai_flush_work();
    }

    osal_sem_up(&g_ao_mutex);

    HI_PRINT("AO suspend OK\n");
    return HI_SUCCESS;
}

hi_s32 ao_drv_resume(hi_void)
{
    hi_s32 i;
    hi_s32 ret;
    ao_mgmt *mgmt = ao_get_mgmt();
#ifdef HI_AUDIO_AI_SUPPORT
    hi_bool ai_enable = HI_FALSE;
#endif

    ret = osal_sem_down_interruptible(&g_ao_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("lock g_ao_mutex failed\n");
        return ret;
    }

    if (mgmt->ready == HI_TRUE) {
        ret = hal_aiao_resume();
        if (ret != HI_SUCCESS) {
            HI_FATAL_AO("AIAO resume fail\n");
            osal_sem_up(&g_ao_mutex);
            return HI_FAILURE;
        }

        /* hal_aoe_init , init aoe hardare */
        ret = hal_aoe_init();
        if (ret != HI_SUCCESS) {
            HI_FATAL_AO("call hal_aoe_init failed(0x%x)\n", ret);
            osal_sem_up(&g_ao_mutex);
            return ret;
        }

        /* restore all snd */
        for (i = 0; i < AO_MAX_TOTAL_SND_NUM; i++) {
            if (mgmt->snd_entity[i].card != HI_NULL) {
                /* recreate snd */
                ret = ao_snd_open(i, &mgmt->snd_entity[i].suspend_attr.user_open_param, HI_TRUE);
                if (ret != HI_SUCCESS) {
                    HI_FATAL_AO("ao_snd_open fail\n");
                    osal_sem_up(&g_ao_mutex);
                    return HI_FAILURE;
                }

                /* restore snd settings */
                ao_snd_restore_settings(i, &mgmt->snd_entity[i].suspend_attr);
            }
        }

        for (i = 0; i < AO_MAX_TOTAL_TRACK_NUM; i++) {
            if (osal_atomic_read(&mgmt->track_entity[i].atm_use_cnt)) {
                ao_snd_id sound = mgmt->track_entity[i].suspend_attr.sound;
                hi_ao_track_attr *attr = &mgmt->track_entity[i].suspend_attr.track_attr;
                hi_bool alsa_track = mgmt->track_entity[i].suspend_attr.alsa_track;

                /* recreate track */
                ret = ao_track_create(sound, attr, alsa_track, i);
                if (ret != HI_SUCCESS) {
                    HI_FATAL_AO("ao_track_create(%d) fail\n", i);
                    osal_sem_up(&g_ao_mutex);
                    return HI_FAILURE;
                }
#ifdef HI_AUDIO_AI_SUPPORT
                if (mgmt->track_entity[i].suspend_attr.att_ai) {
                    ret = ai_get_enable(mgmt->track_entity[i].suspend_attr.h_ai, &ai_enable);
                    if (ret != HI_SUCCESS) {
                        HI_FATAL_AO("AI get_enable fail\n");
                        osal_sem_up(&g_ao_mutex);
                        return HI_FAILURE;
                    }
                    if (ai_enable == HI_TRUE) {
                        ret = ai_set_enable(mgmt->track_entity[i].suspend_attr.h_ai, HI_FALSE, HI_TRUE);
                        if (ret != HI_SUCCESS) {
                            HI_FATAL_AO("AI set_enable failed!!\n");
                            osal_sem_up(&g_ao_mutex);
                            return HI_FAILURE;
                        }
                    }

                    ret = ao_track_attach_ai(i, mgmt->track_entity[i].suspend_attr.h_ai);
                    if (ret != HI_SUCCESS) {
                        HI_FATAL_AO("ao_track_attach_ai(%d) fail\n", i);
                        osal_sem_up(&g_ao_mutex);
                        return HI_FAILURE;
                    }

                    if (ai_enable == HI_TRUE) {
                        ret = ai_set_enable(mgmt->track_entity[i].suspend_attr.h_ai, HI_TRUE, HI_TRUE);
                        if (ret != HI_SUCCESS) {
                            HI_FATAL_AO("AI set_enable failed!!\n");
                            osal_sem_up(&g_ao_mutex);
                            return HI_FAILURE;
                        }
                    }
                    ret = ao_track_start(i);
                    if (ret != HI_SUCCESS) {
                        HI_FATAL_AO("ao_track_start(%d) fail\n", i);
                        osal_sem_up(&g_ao_mutex);
                        return HI_FAILURE;
                    }
                }
#endif
                /* restore track settings */
                ao_track_restore_settings(i, &mgmt->track_entity[i].suspend_attr);
            }
        }

#ifdef HI_SND_CAST_SUPPORT
        /* restore all cast */
        for (i = 0; i < AO_MAX_CAST_NUM; i++) {
            if (osal_atomic_read(&mgmt->cast_entity[i].atm_use_cnt)) {
                ao_snd_id sound = mgmt->cast_entity[i].suspend_attr.sound;
                hi_ao_cast_attr *attr = &mgmt->cast_entity[i].suspend_attr.cast_attr;

                /* recreate cast */
                ret = ao_cast_create(sound, attr, &mgmt->cast_entity[i].rbf_mmz, i);
                if (ret != HI_SUCCESS) {
                    HI_FATAL_AO("ao_cast_create(%d) fail\n", i);
                    osal_sem_up(&g_ao_mutex);
                    return HI_FAILURE;
                }

                /* restore cast settings */
                ao_cast_restore_settings(i, &mgmt->cast_entity[i].suspend_attr);
            }
        }
#endif
    }

    osal_sem_up(&g_ao_mutex);

#ifdef HI_AIAO_TIMER_SUPPORT
    ret = hi_drv_timer_resume();
    if (ret != HI_SUCCESS) {
        HI_FATAL_AO("hi_drv_timer_resume fail 0x%x\n", ret);
    }
#endif

    HI_PRINT("AO resume OK\n");
    return HI_SUCCESS;
}

hi_void ao_osal_init(hi_void)
{
    hi_u32 cnt;

    osal_sem_init(&g_ao_mutex, 1);

#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
    for (cnt = 0; cnt < AO_MAX_CAST_NUM; cnt++) {
        osal_atomic_init(&g_ao_drv.cast_entity[cnt].atm_use_cnt);
    }
#endif

    for (cnt = 0; cnt < AO_MAX_TOTAL_TRACK_NUM; cnt++) {
        osal_atomic_init(&g_ao_drv.track_entity[cnt].atm_use_cnt);
    }

    for (cnt = 0; cnt < AO_MAX_TOTAL_SND_NUM; cnt++) {
        osal_atomic_init(&g_ao_drv.snd_entity[cnt].atm_use_total_cnt);
        osal_atomic_init(&g_ao_drv.snd_dma_entity[cnt].atm_use_cnt);
    }

    osal_atomic_init(&g_ao_drv.atm_open_cnt);
}

hi_void ao_osal_deinit(hi_void)
{
    hi_u32 cnt;

    osal_atomic_destory(&g_ao_drv.atm_open_cnt);

#if defined(HI_SND_CAST_SUPPORT) || defined(HI_PROC_SUPPORT)
    for (cnt = 0; cnt < AO_MAX_CAST_NUM; cnt++) {
        osal_atomic_destory(&g_ao_drv.cast_entity[cnt].atm_use_cnt);
    }
#endif

    for (cnt = 0; cnt < AO_MAX_TOTAL_TRACK_NUM; cnt++) {
        osal_atomic_destory(&g_ao_drv.track_entity[cnt].atm_use_cnt);
    }

    for (cnt = 0; cnt < AO_MAX_TOTAL_SND_NUM; cnt++) {
        osal_atomic_destory(&g_ao_drv.snd_entity[cnt].atm_use_total_cnt);
        osal_atomic_destory(&g_ao_drv.snd_dma_entity[cnt].atm_use_cnt);
    }

    osal_sem_destory(&g_ao_mutex);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

