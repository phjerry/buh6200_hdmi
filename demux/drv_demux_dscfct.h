/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function decl.
 * Author: sdk
 * Create: 2017-06-05
 */
#ifndef __DRV_DEMUX_DSCFCT_H__
#define __DRV_DEMUX_DSCFCT_H__

#include "drv_demux_define.h"
#include "hi_drv_demux.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/***********************dsc_fct begin******************************************/
extern struct dmx_r_dsc_fct_ops g_dmx_dsc_fct_ops;

struct dmx_r_dsc_fct_ops {
    hi_s32(*create)(struct dmx_r_dsc_fct *rdsc_fct, const dmx_dsc_attrs *attrs);
    hi_s32(*get_attrs)(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_attrs *attrs);
    hi_s32(*set_attrs)(struct dmx_r_dsc_fct *rdsc_fct, const dmx_dsc_attrs *attrs);
    hi_s32(*attach_keyslot)(struct dmx_r_dsc_fct *rdsc_fct, hi_handle ks_handle);
    hi_s32(*detach_keyslot)(struct dmx_r_dsc_fct *rdsc_fct);
    hi_s32(*get_keyslot_handle)(struct dmx_r_dsc_fct *rdsc_fct, hi_handle *ks_handle);
    hi_s32(*attach)(struct dmx_r_dsc_fct *rdsc_fct, struct dmx_r_pid_ch *rpid_ch);
    hi_s32(*detach)(struct dmx_r_dsc_fct *rdsc_fct, struct dmx_r_pid_ch *rpid_ch);

    hi_s32(*set_key)(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_key_type key_type, const hi_u8 *key, hi_u32 len);
    hi_s32(*set_iv)(struct dmx_r_dsc_fct *rdsc_fct, dmx_dsc_key_type ivtype, const hi_u8 *iv, hi_u32 len);
    hi_s32(*destroy)(struct dmx_r_dsc_fct *rdsc_fct);

    hi_s32(*suspend)(struct dmx_r_dsc_fct *rdsc_fct);
    hi_s32(*resume)(struct dmx_r_dsc_fct *rdsc_fct);
};

#define IS_DSCFCT(obj) ({ \
    hi_bool ret = HI_FALSE; \
    struct dmx_r_base *base = (struct dmx_r_base*)obj; \
    struct dmx_r_dsc_fct *rdsc_fct = container_of(base, struct dmx_r_dsc_fct, base); \
    WARN_ON(get_dmx_rbase_ops() != base->ops); \
    if (&g_dmx_dsc_fct_ops == rdsc_fct->ops) \
        ret = HI_TRUE; \
    ret; \
})

struct dmx_r_dsc_fct {
    struct dmx_r_base        base;   /* !!! it must be first entry. */
    struct dmx_r_dsc_fct_ops *ops;
    hi_bool                  staled;
    osal_mutex               lock;
    struct list_head         node;

    hi_handle                dsc_fct_handle;

    /* attribute */
    dmx_dsc_ca_type          ca_type;
    dmx_dsc_entropy          entropy_reduction;
    dmx_dsc_key_alg          alg;

    /* IV information */
    dmx_dsc_key_type         ivtype;
    hi_u32                   ivlen;
    hi_u8                    IV[DMX_MAX_KEY_LEN];

    /* keyslot information */
    hi_handle                keyslot_handle;
    hi_bool                  keyslot_attached;
    hi_bool                  keyslot_create_en;

    /* key information */
    dmx_dsc_key_type         key_type;
    hi_u32                   key_len;
    hi_u8                    key[DMX_MAX_KEY_LEN];

    /* attached pidch */
    osal_mutex               pid_ch_list_lock;
    struct list_head         pid_ch_head;    /* recode the pidch that add to the dscfct  */
};

/* interface */
hi_s32 dmx_dsc_fct_create(const dmx_dsc_attrs *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 dmx_dsc_fct_open(hi_handle handle);
hi_s32 dmx_dsc_fct_get_attrs(hi_handle handle, dmx_dsc_attrs *attrs);
hi_s32 dmx_dsc_fct_set_attrs(hi_handle handle, const dmx_dsc_attrs *attrs);

hi_s32 dmx_dsc_fct_attach(hi_handle handle, hi_handle ch_handle);
hi_s32 dmx_dsc_fct_detach(hi_handle handle, hi_handle ch_handle);

hi_s32 dmx_dsc_fct_attach_keyslot(hi_handle handle, hi_handle ks_handle);
hi_s32 dmx_dsc_fct_detach_keyslot(hi_handle handle);
hi_s32 dmx_dsc_get_keyslot_handle(hi_handle handle, hi_handle *ks_handle);

hi_s32 dmx_dsc_fct_set_key(hi_handle handle, dmx_dsc_key_type key_type, const hi_u8 *key, hi_u32 len);

hi_s32 dmx_dsc_fct_set_iv(hi_handle handle, dmx_dsc_key_type ivtype, const hi_u8 *iv, hi_u32 len);

hi_s32 dmx_dsc_fct_destroy(hi_handle handle);

hi_s32 dmx_dsc_get_dsc_key_handle(hi_handle pid_ch_handle, hi_handle *dsc_handle);

hi_s32 dmx_dsc_get_free_cnt(hi_u32 *free_cnt);

/* recfct mgmt  */
hi_s32 dmx_mgmt_create_dsc_fct(const dmx_dsc_attrs *attrs, struct dmx_r_dsc_fct **rdsc_fct);
hi_s32 dmx_mgmt_destroy_dsc_fct(struct dmx_r_base *obj);


/***********************rec_fct end******************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_DSCFCT_H__

