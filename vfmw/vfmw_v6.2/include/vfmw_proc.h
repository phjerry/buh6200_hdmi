#ifndef __VFMW_PROC__
#define __VFMW_PROC__

#include "vfmw.h"

#define FILE_NAME_MAX_LEN 100

#define CMD_PARAM_MAX_COUNT 5
#define CMD_PARAM_MAX_LEN   16

#define CMD_STR_MAX_LEN 128

#define LOG_MAX_WAIT_TIME  200  /*  ms  */
#define LOG_MAX_SLEEP_TIME 2000 /*  ms  */

#define LOG_FMT_HEAD  "----------------------------- %-3s -----------------------------\n"
#define LOG_FMT_D     "  %-15s: %-10d\n"
#define LOG_FMT_X     "  %-15s: 0x%-9x\n"
#define LOG_FMT_P     "  %-15s: %-10p\n"
#define LOG_FMT_S     "  %-15s: %-10s\n"
#define LOG_FMT_L     "  %-15s: %-10lld\n"
#define LOG_FMT_I_D   "  %-15s: %d.%d\n"
#define LOG_FMT_I_D_D "  %-15s: %d/%d\n"
#define LOG_FMT_D_D   "  %-15s: %-10d  |  %-15s: %-10d\n"
#define LOG_FMT_X_X   "  %-15s: 0x%-9x |  %-15s: 0x%-9x\n"
#define LOG_FMT_P_P   "  %-15s: %-10p  |  %-15s: %-10p\n"
#define LOG_FMT_S_S   "  %-15s: %-10s  |  %-15s: %-10s\n"
#define LOG_FMT_U_U   "  %-15s: %-10u  |  %-15s: %-10u\n"
#define LOG_FMT_D_X   "  %-15s: %-10d  |  %-15s: %-10x\n"
#define LOG_FMT_D_P   "  %-15s: %-10d  |  %-15s: %-10p\n"
#define LOG_FMT_D_S   "  %-15s: %-10d  |  %-15s: %-10s\n"
#define LOG_FMT_D_U   "  %-15s: %-10d  |  %-15s: %-10u\n"
#define LOG_FMT_X_D   "  %-15s: 0x%-9x |  %-15s: %-10d\n"
#define LOG_FMT_X_P   "  %-15s: 0x%-9x |  %-15s: %-10p\n"
#define LOG_FMT_X_S   "  %-15s: 0x%-9x |  %-15s: %-10s\n"
#define LOG_FMT_X_U   "  %-15s: 0x%-9x |  %-15s: %-10u\n"
#define LOG_FMT_P_D   "  %-15s: %-10p  |  %-15s: %-10d\n"
#define LOG_FMT_P_X   "  %-15s: %-10p  |  %-15s: 0x%-9x\n"
#define LOG_FMT_P_S   "  %-15s: %-10p  |  %-15s: %-10s\n"
#define LOG_FMT_P_U   "  %-15s: %-10p  |  %-15s: %-10u\n"
#define LOG_FMT_S_D   "  %-15s: %-10s  |  %-15s: %-10d\n"
#define LOG_FMT_S_X   "  %-15s: %-10s  |  %-15s: 0x%-9x\n"
#define LOG_FMT_S_P   "  %-15s: %-10s  |  %-15s: %-10p\n"
#define LOG_FMT_S_U   "  %-15s: %-10s  |  %-15s: %-10u\n"
#define LOG_FMT_U_D   "  %-15s: %-10u  |  %-15s: %-10d\n"
#define LOG_FMT_U_X   "  %-15s: %-10u  |  %-15s: 0x%-9x\n"
#define LOG_FMT_U_P   "  %-15s: %-10u  |  %-15s: %-10p\n"
#define LOG_FMT_U_S   "  %-15s: %-10u  |  %-15s: %-10s\n"
#define LOG_FMT_D_F   "  %-15s: %-10d  |  %-15s: %d.%d\n"
#define LOG_FMT_X_D_D "  %-15s: 0x%-9x |  %-15s: %d/%d\n"
#define LOG_FMT_LX_LX "  %-15s: 0x%-9llx |  %-15s: 0x%-9llx\n"
#define LOG_FMT_LX_X  "  %-15s: 0x%-9llx |  %-15s: 0x%-9x\n"


/*   com log command  */
#define LOG_CMD_HELP           "help"
#define LOG_CMD_CUR_CHAN       "chan"
#define LOG_CMD_SAVE_YUV       "saveyuv"
#define LOG_CMD_SAVE_MTDT      "savemtdt"
#define LOG_CMD_PRINT          "print"
#define LOG_CMD_DPS_SER        "dpsser"
#define LOG_CMD_DPS_NUM        "dpsnum"
#define LOG_CMD_ERR_HOLD       "errhold"
#define LOG_CMD_CMP_MODE       "cmpmode"
#define LOG_CMD_UVMOS          "uvmos"
#define LOG_CMD_OUT_ORDER      "outorder"
#define LOG_CMD_DEC_MODE       "decmode"
#define LOG_CMD_DROP_HOLD      "drophold"
#define LOG_CMD_STD_OBED       "stdobed"
#define LOG_CMD_SED_ONLY       "sedonly"
#define LOG_CMD_MASK_SMMU_IRQ  "mskmmuirq"
#define LOG_CMD_TUNL_LINE      "tunlline"
#define LOG_CMD_MOD_LOW_DLY_EN "modlowdly"
#define LOG_CMD_SAVE_BEGIN     "savebegin"
#define LOG_CMD_SAVE_END       "saveend"
#define LOG_CMD_SUSPEND        "suspend"
#define LOG_CMD_RESUME         "resume"
#define LOG_CMD_START          "start"
#define LOG_CMD_STOP           "stop"
#define LOG_CMD_RESET          "reset"
#define LOG_CMD_RESTART        "restart"
#define LOG_CMD_FIX_NUM        "fixnum"
#define LOG_CMD_FIX_RES        "fixres"
#define LOG_CMD_SET_CLK        "setclk"
#define LOG_CMD_PRINT_LOG      "log"
#define LOG_CMD_READ_LOG       "readlog"
#define LOG_CMD_SHOW_DFLT      "showdflt"
#define LOG_CMD_PRT_STM_BLK    "stm_pr_block"
#define LOG_CMD_PRT_USD_BLK    "usd_pr_block"
#define LOG_CMD_VPP_BYPASS     "vpp_bypass"
#define LOG_CMD_VPP_PR_FRM     "vpp_pr_frm"
#define LOG_CMD_FRM_FENCE      "frm_fence_en"
#define LOG_CMD_FRM_SRC_TIM    "frm_src_time"
#define LOG_CMD_FRM_SINK_TIM   "frm_sink_time"

/*   stm log command  */
#define LOG_CMD_SAVE_RAW      "saveraw"
#define LOG_CMD_SAVE_SEEK_RAW "saveseekraw"
#define LOG_CMD_SAVE_SEG      "saveseg"
#define LOG_CMD_STM_SLEEP     "stmsleep"
#define LOG_CMD_STM_WAIT      "stmwait"

/*   syn log command  */
#define LOG_CMD_SYN_SLEEP "synsleep"
#define LOG_CMD_SYN_WAIT  "synwait"
#define LOG_CMD_SAVE_EL   "saveel"

/*   dec log command  */
#define LOG_CMD_SLICE_BIN "slicebin"
#define LOG_CMD_DEC_SLEEP "decsleep"
#define LOG_CMD_DEC_WAIT  "decwait"
/*   pts log command  */
#define LOG_CMD_SAMPLE_NUM "samplenum"

/*   hdr log command  */
#define LOG_CMD_READ_HDR   "readhdr"

/*   cs log command  */
#define LOG_CMD_MDC_LOG    "mdclog"

#define CMD_STR_LEN (CMD_PARAM_MAX_COUNT * CMD_PARAM_MAX_LEN * sizeof(hi_s8))

typedef hi_s8 (*cmd_str_ptr)[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];
typedef hi_void (*cmd_handler)(cmd_str_ptr, hi_u32);

typedef enum {
    LOG_MODULE_STM = 0,
    LOG_MODULE_SYN,
    LOG_MODULE_DEC,
    LOG_MODULE_COM,
    LOG_MODULE_FSP,
    LOG_MODULE_SHR,
    LOG_MODULE_MDC,
    LOG_MODULE_MAX,
} log_module;

typedef struct {
    hi_char cmd_name[CMD_PARAM_MAX_LEN];
    cmd_handler handler;
} log_cmd;

#endif
