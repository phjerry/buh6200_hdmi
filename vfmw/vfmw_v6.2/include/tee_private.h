#ifndef __TEE_PRIVATE_H__
#define __TEE_PRIVATE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */

typedef enum {
    VFMW_CMD_ID_INVALID = 0x0,
    VFMW_CMD_ID_INIT,
    VFMW_CMD_ID_EXIT,
    VFMW_CMD_ID_RESUME,
    VFMW_CMD_ID_SUSPEND,
    VFMW_CMD_ID_CONTROL,
    VFMW_CMD_ID_THREAD,
    VFMW_CMD_ID_GET_IMAGE,
    VFMW_CMD_ID_RELEASE_IMAGE,
    VFMW_CMD_ID_READ_PROC,
    VFMW_CMD_ID_WRITE_PROC,
    VFMW_CMD_ID_PARSE_CSD,
} tee_vfmw_cmd_id;

typedef struct {
    hi_s32 chan_id;
    hi_s32 cmd_id;
    UADDR arg_phy_addr;
    hi_u32 param_length;
    hi_u32 option;
    hi_u32 value;
} tee_vfmw_cmd_params;

typedef struct {
    hi_s32 module_id;
    hi_s32 chan_id;
    hi_s32 write_count;
} tee_vfmw_proc_params;

#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */

#endif

