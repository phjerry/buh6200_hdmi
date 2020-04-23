#ifndef __DRV_PDM_EXT_H__
#define __DRV_PDM_EXT_H__

#include "hi_drv_pdm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef hi_s32(*fn_pdm_get_disp_param)(hi_drv_display disp, hi_disp_param *disp_param);
typedef hi_s32(*fn_pdm_release_reserve_mem)(const hi_char *buf_name);
typedef hi_s32(*fn_pdm_get_reserve_mem)(const hi_char *buf_name, hi_u32 *resever_addr, hi_u32 *resever_buf_size);
typedef hi_s32(*fn_pdm_get_data)(const hi_char *buf_name, hi_u8 **data_addr, hi_u32 *data_len);
typedef hi_s32(*fn_pdm_get_sound_param)(ao_snd_id sound, hi_drv_pdm_sound_param *sound_param);

typedef struct {
    fn_pdm_get_disp_param             pdm_get_disp_param;
    fn_pdm_release_reserve_mem        pdm_release_reserve_mem;
    fn_pdm_get_reserve_mem            pdm_get_reserve_mem;
    fn_pdm_get_data                   pdm_get_data;
    fn_pdm_get_sound_param            pdm_get_sound_param;
} hi_pdm_export_func;

hi_s32 pdm_drv_mod_init(hi_void);
hi_void pdm_drv_mod_exit(hi_void);


#ifdef __cplusplus
}
#endif

#endif
