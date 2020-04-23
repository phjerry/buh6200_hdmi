/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function decl.
 * Author: sdk
 * Create: 2017-12-04
 */
#ifndef __DRV_DEMUX_RMXFCT_H__
#define __DRV_DEMUX_RMXFCT_H__

#include "drv_demux_define.h"
#include "hi_drv_demux.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/***********************rmx_pump begin******************************************/
extern struct dmx_r_rmx_pump_ops g_dmx_rmx_pump_ops;

struct dmx_r_rmx_pump_ops {
    hi_s32(*create)(struct dmx_r_rmx_pump *rrmx_pump, const dmx_rmx_pump_attrs *attrs);
    hi_s32(*get_attrs)(struct dmx_r_rmx_pump *rrmx_pump, dmx_rmx_pump_attrs *attr);
    hi_s32(*set_attrs)(struct dmx_r_rmx_pump *rrmx_pump, const dmx_rmx_pump_attrs *attr);
    hi_s32(*destroy)(struct dmx_r_rmx_pump *rrmx_pump);

    hi_s32(*suspend)(struct dmx_r_rmx_pump *rrmx_pump);
    hi_s32(*resume)(struct dmx_r_rmx_pump *rrmx_pump);
};

#define IS_RMXPUMP(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_rmx_fct *rrmx_fct = container_of(base, struct dmx_r_rmx_pump, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_rmx_fct_ops == rrmx_fct->ops) \
        ret = HI_TRUE; \
    ret; \
})

struct dmx_r_rmx_pump {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_rmx_pump_ops  *ops;
    hi_bool                    staled;
    osal_mutex                 lock;
    struct list_head           node0;
    struct list_head           node1;  /* list in dmx_r_rmx_fct to record the pump added to rmxfct */

    hi_handle                  rmx_pump_handle;
    hi_u32                     pump_total_id;

    struct dmx_r_rmx_fct       *rrmx_fct;  /* the rmxfct that this pump has been added */

    hi_u32                     rmx_port_id; /* index of 4 source portid */

    dmx_rmx_pump_type          pump_type;
    dmx_port                   in_port_id;
    hi_u32                     pid;
    hi_u32                     remap_pid;
};

/* rmx_pump interface */
hi_s32 dmx_rmx_pump_get_attrs(hi_handle rmx_pump_handle, dmx_rmx_pump_attrs *pump_attrs);
hi_s32 dmx_rmx_pump_set_attrs(hi_handle rmx_pump_handle, const dmx_rmx_pump_attrs *pump_attrs);

/* rmx_pump mgmt  */
hi_s32 dmx_mgmt_create_rmx_pump(const dmx_rmx_pump_attrs *attrs, struct dmx_r_rmx_pump **rrmx_pump);
hi_s32 dmx_mgmt_destroy_rmx_pump(struct dmx_r_base *obj);

/* rmx_fct begin */
extern struct dmx_r_rmx_fct_ops g_dmx_rmx_fct_ops;

struct dmx_r_rmx_fct_ops {
    hi_s32(*create)(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_attrs *attrs);
    hi_s32(*open)(struct dmx_r_rmx_fct *rrmx_fct);
    hi_s32(*get_attrs)(struct dmx_r_rmx_fct *rrmx_fct, dmx_rmx_attrs *attr);
    hi_s32(*set_attrs)(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_attrs *attr);
    hi_s32(*get_status)(struct dmx_r_rmx_fct *rrmx_fct, dmx_rmx_status *status);
    hi_s32(*add_pump)(struct dmx_r_rmx_fct *rrmx_fct, const dmx_rmx_pump_attrs *pump_attrs,
        struct dmx_r_rmx_pump *rrmx_pump);
    hi_s32(*del_pump)(struct dmx_r_rmx_fct *rrmx_fct, struct dmx_r_rmx_pump *rrmx_pump);
    hi_s32(*del_all_pump)(struct dmx_r_rmx_fct *rrmx_fct);

    hi_s32(*close)(struct dmx_r_rmx_fct *rrmx_fct);
    hi_s32(*destroy)(struct dmx_r_rmx_fct *rrmx_fct);

    hi_s32(*suspend)(struct dmx_r_rmx_fct *rrmx_fct);
    hi_s32(*resume)(struct dmx_r_rmx_fct *rrmx_fct);
};

#define IS_RMXFCT(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_rmx_fct *rrmx_fct = container_of(base, struct dmx_r_rmx_fct, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_rmx_fct_ops == rrmx_fct->ops) \
        ret = HI_TRUE; \
    ret; \
})

typedef struct {
    dmx_port port_id;
    hi_u32 ref;
    hi_u32 overflow_count;
} rmx_port_info;


struct dmx_r_rmx_fct {
    struct dmx_r_base          base;   /* !!! it must be first entry. */
    struct dmx_r_rmx_fct_ops   *ops;
    hi_bool                    staled;
    osal_mutex                 lock;
    struct list_head           node;

    hi_handle                  rmx_fct_handle;

    dmx_port                   out_port_id;
    hi_u32                     out_port_cnt;

    /* static src port array */
    hi_u32                     src_port_cnt;
    rmx_port_info              port_info[DMX_RMX_PORT_CNT];
    DECLARE_BITMAP(port_bitmap, DMX_RMX_PORT_CNT);

    /* pump list */
    hi_u32                     pump_cnt;
    struct list_head           pump_head;      /* recode the pump that add to the rmxfct  */
    osal_mutex                 pump_bitmap_lock;
    DECLARE_BITMAP(pump_bitmap, DMX_RMXPUMP_CNT);
};

/* rmx_fct interface */
hi_s32 dmx_rmx_fct_create(const dmx_rmx_attrs *attrs, hi_handle *rmx_handle, struct dmx_session *session);
hi_s32 dmx_rmx_fct_open(hi_handle rmx_handle);
hi_s32 dmx_rmx_fct_get_attrs(hi_handle rmx_handle, dmx_rmx_attrs *attrs);
hi_s32 dmx_rmx_fct_set_attrs(hi_handle rmx_handle, const dmx_rmx_attrs *attrs);
hi_s32 dmx_rmx_fct_get_status(hi_handle rmx_handle, dmx_rmx_status *status);

hi_s32 dmx_rmx_fct_add_pump(hi_handle rmx_handle, dmx_rmx_pump_attrs *pump_attrs, hi_handle *rmx_pump_handle,
    struct dmx_session *session);
hi_s32 dmx_rmx_fct_del_pump(hi_handle rmx_pump_handle);
hi_s32 dmx_rmx_fct_del_all_pump(hi_handle rmx_handle);

hi_s32 dmx_rmx_fct_close(hi_handle rmx_handle);
hi_s32 dmx_rmx_fct_destroy(hi_handle rmx_handle);

/* rmx_fct mgmt  */
hi_s32 dmx_mgmt_create_rmx_fct(const dmx_rmx_attrs *attrs, struct dmx_r_rmx_fct **rrmx_fct);
hi_s32 dmx_mgmt_destroy_rmx_fct(struct dmx_r_base *obj);


/***********************rmx_fct end******************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_RMXFCT_H__

