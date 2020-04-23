
#ifndef __DRV_VDEC_PROC_H__
#define __DRV_VDEC_PROC_H__

#include "hi_vdec_type.h"

#define VP_RET_UNSUPPORT (-2)

#define VP_HEAD  "--------------------------------------- %-3s --------------------------------------\n"
#define VP_D     "  %-15s: %-20d\n"
#define VP_X     "  %-15s: 0x%-19x\n"
#define VP_P     "  %-15s: %-20p\n"
#define VP_S     "  %-15s: %-20s\n"
#define VP_L     "  %-15s: %-20lld\n"
#define VP_I_D   "  %-15s: %d.%d\n"
#define VP_I_D_D "  %-15s: %d/%d\n"
#define VP_D_D   "  %-15s: %-20d  |  %-15s: %-20d\n"
#define VP_X_X   "  %-15s: 0x%-19x |  %-15s: 0x%-19x\n"
#define VP_P_P   "  %-15s: %-20p  |  %-15s: %-20p\n"
#define VP_S_S   "  %-15s: %-20s  |  %-15s: %-20s\n"
#define VP_U_U   "  %-15s: %-20u  |  %-15s: %-20u\n"
#define VP_D_X   "  %-15s: %-20d  |  %-15s: %-20x\n"
#define VP_D_P   "  %-15s: %-20d  |  %-15s: %-20p\n"
#define VP_D_S   "  %-15s: %-20d  |  %-15s: %-20s\n"
#define VP_D_U   "  %-15s: %-20d  |  %-15s: %-20u\n"
#define VP_X_D   "  %-15s: 0x%-19x |  %-15s: %-20d\n"
#define VP_X_P   "  %-15s: 0x%-19x |  %-15s: %-20p\n"
#define VP_X_S   "  %-15s: 0x%-19x |  %-15s: %-20s\n"
#define VP_X_U   "  %-15s: 0x%-19x |  %-15s: %-20u\n"
#define VP_P_D   "  %-15s: %-20p  |  %-15s: %-20d\n"
#define VP_P_X   "  %-15s: %-20p  |  %-15s: 0x%-19x\n"
#define VP_P_S   "  %-15s: %-20p  |  %-15s: %-20s\n"
#define VP_P_U   "  %-15s: %-20p  |  %-15s: %-20u\n"
#define VP_S_D   "  %-15s: %-20s  |  %-15s: %-20d\n"
#define VP_S_X   "  %-15s: %-20s  |  %-15s: 0x%-19x\n"
#define VP_S_P   "  %-15s: %-20s  |  %-15s: %-20p\n"
#define VP_S_U   "  %-15s: %-20s  |  %-15s: %-20u\n"
#define VP_U_D   "  %-15s: %-20u  |  %-15s: %-20d\n"
#define VP_U_X   "  %-15s: %-20u  |  %-15s: 0x%-19x\n"
#define VP_U_P   "  %-15s: %-20u  |  %-15s: %-20p\n"
#define VP_U_S   "  %-15s: %-20u  |  %-15s: %-20s\n"
#define VP_D_F   "  %-15s: %-20d  |  %-15s: %d.%d%\n"

#define VDEC_PROC_NAME "vdec"
#define VDEC_CMD_NUM   2
#define VDEC_CMD_LEN   128
#define VDEC_HELP_FMT  " %-15s %-15s %-15s %s\n"

#define VDEC_PROC_CMD_SET_CHAN     "set_proc_chan"
#define VDEC_PROC_CMD_HELP             "help"

typedef hi_char (*cmd_str)[VDEC_CMD_NUM][VDEC_CMD_LEN];

hi_s32 vdec_proc_read(hi_void *fp);
hi_s32 vdec_proc_get_cmd(hi_void **cmd_list, hi_u32 *cmd_cnt);

#endif

