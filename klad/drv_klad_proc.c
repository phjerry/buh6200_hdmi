/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
 * Description :Add proc node in virtual fs.
 * Author : Linux SDK team
 * Created : 2019-06-22
 */
#include "drv_klad_proc.h"
#include <linux/uaccess.h>
#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "drv_klad_sw_define.h"
#include "drv_klad_sw.h"
#include "drv_hkl.h"
#include "drv_klad_hw_func.h"
#include "drv_klad_sw_func.h"
#include "drv_klad_timestamp.h"
#include "drv_klad_timestamp_logic.h"

#define NAME_LEN  16

typedef enum {
    HI_PROC_DEFAULT = 0,
    HI_PROC_HW_COM = 1,
    HI_PROC_HW_TA = 2,
    HI_PROC_HW_FP = 3,
    HI_PROC_HW_NONCE = 4,
    HI_PROC_HW_CLR = 5,
    HI_PROC_SW_COM = 6,
    HI_PROC_SW_TA = 7,
    HI_PROC_SW_FP = 8,
    HI_PROC_SW_NONCE = 9,
    HI_PROC_SW_CLR = 10,
    HI_PROC_REG = 11,
    HI_PROC_LOCK_STAT = 12,
    HI_PROC_UNLOCK = 13,
    HI_PROC_LOCK = 14,
    HI_PROC_TIMESTAMP = 15,
    HI_PROC_TIME_QUEUE = 16,
    HI_PROC_TIME_Q_CLR = 16,
    HI_PROC_TIME_Q_HW = 17,
    HI_PROC_HW_OBJ = 18,
} hi_proc_cmd;

typedef enum {
    HI_PROC_KLAD_COM_KLAD0                 = 0x0,
    HI_PROC_KLAD_COM_KLAD1                 = 0x1,
    HI_PROC_KLAD_COM_KLAD2                 = 0x2,
    HI_PROC_KLAD_COM_KLAD3                 = 0x3,
    HI_PROC_KLAD_COM_KLAD4                 = 0x4,
    HI_PROC_KLAD_COM_KLAD5                 = 0x5,
    HI_PROC_KLAD_COM_KLAD6                 = 0x6,
    HI_PROC_KLAD_COM_KLAD7                 = 0x7,
    HI_PROC_KLAD_TA                        = 0x8,
    HI_PROC_KLAD_FP                        = 0x9,
    HI_PROC_KLAD_NONCE                     = 0xA,
    HI_PROC_KLAD_CLR                       = 0xB,
    HI_PROC_KLAD_ALL                       = 0xC,
    HI_PROC_KLAD_SEL_MAX
} hi_proc_klad_index;

typedef hi_s32(*func_proc_read)(struct seq_file *p, hi_void *v);
typedef hi_s32(*func_proc_write)(hi_char *buf, hi_u32 len);
struct cmd_map {
    hi_char name[NAME_LEN];
    hi_proc_cmd cmd;
    func_proc_read func;
};

hi_proc_cmd g_cmd = HI_PROC_DEFAULT;

/* macro definition */
#define for_each_cmd_node(size, node) \
    for ((size) = 0, (node) = &g_proc_cmd[0]; \
        (size) < sizeof(g_proc_cmd) / sizeof(g_proc_cmd[0]); \
        (size)++, (node) = &g_proc_cmd[size]) \

static hi_s32 __klad_proc_cmd_lock_stat(struct seq_file *p, hi_void *v);

static hi_void __klad_proc_get_help(hi_void)
{
    hi_drv_proc_echo_helper("\nUsage as following: \n");
    hi_drv_proc_echo_helper("    cat /proc/msp/klad               --Display all proc information. \n");
    hi_drv_proc_echo_helper("    echo default > /proc/msp/klad; cat /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo reg > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo hkl_stat > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo hw_com > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo hw_ta > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo hw_fp > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo hw_nonce > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo hw_clr > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo sw_com > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo sw_ta > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo sw_fp > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo sw_nonce > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo sw_clr > /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo timestamp > /proc/msp/klad; cat /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo time_queue > /proc/msp/klad; cat /proc/msp/klad \n");
    hi_drv_proc_echo_helper("    echo time_clean > /proc/msp/klad; cat /proc/msp/klad  \n");
    hi_drv_proc_echo_helper("    echo time_logic > /proc/msp/klad; cat /proc/msp/klad  \n");
    hi_drv_proc_echo_helper(
        "    echo hw_unlock xx > /proc/msp/klad 0~7:com,8:ta, 9:fp, 10:nonce, 11:clr, all:all the hkl\n");
    hi_drv_proc_echo_helper(
        "    echo hw_lock xx > /proc/msp/klad 0~7 com(only lock a random slot), 8:ta, 9:fp, 10:nonce, 11:clr\n");
    hi_drv_proc_echo_helper("    echo hw_obj > /proc/msp/klad; cat /proc/msp/klad\n");
    return;
}

static hi_s32 __klad_proc_cmd_reg(struct seq_file *p, hi_void *v)
{
    hi_s32 i = 0;

    /* dump all the HKL register. */
    for (i = 0; i < 0x4c0; i += 0x4) {
        if ((i & 0xf) == 0) {
            HI_PROC_PRINT(p, "\n%04x: ", KLAD_REG_BASE + i);
        }
        HI_PROC_PRINT(p, "%08x ", hkl_read_reg(KLAD_REG_BASE + i));
    }
    HI_PROC_PRINT(p, "\n");
    return HI_SUCCESS;
}

hi_u8 *g_ca_vendor_id[HI_CA_VENDORID_MAX + 1] = {
    "COMMON   ", "NAGRA    ", "IRDETO   ", "CONAX    ",
    "SYNAMEDIA", "SUMA     ", "NOVEL    ", "VMX      ",
    "CTI      ", "SAFEVIEW ", "LATENSE  ", "TELECOM  ",
    "DCAS     ", "VIACCEESS", "PANACCESS", "ABV      ",
    "UNKNOWN  ",
};

static hi_u8 *__klad_proc_ca_vendor_id(hi_ca_vendor_id vendor_id)
{
    if (vendor_id >= HI_CA_VENDORID_MAX) {
        return g_ca_vendor_id[HI_CA_VENDORID_MAX];
    }
    return g_ca_vendor_id[vendor_id];
}

static hi_void __klad_proc_cmd_default_cas_slot(struct seq_file *p)
{
    hi_u32 slot;
    rkp_slotx_crc_rst reg;
    rkp_rk_cas_flag cas_slot;
    hi_u8 upper_bit = (otp_shadow_read(OTP_SHW_OWNER_UPPER_BIT) >> 0x8) & 0x0f; /* upper bit 0x0d[3-0] */

    for (slot = 0; slot <= HI_RKP_SLOT_CHOOSE_SLOT7; slot++) {
        reg.u32 = rkp_read_reg(RKP_SLOT0_CRC_RST + slot * 0x4);
        cas_slot.u32 = otp_shadow_read(OTP_SHW_CAS_SLOT0_FLAG + slot * 0x4);
        HI_PROC_PRINT(p, "%s%d  %-12s%-12s(%02x)%-8s(%02x)%-8s(%02x)%-10s%-s\n", "cas_slot_", slot,
                      (reg.bits.slotx_rk_all_zero != 0) ? "zero" : ((reg.bits.slotx_rk_all_one != 0) ? "one" : "ok"),
                      (reg.bits.slotx_crc_ok != 0) ? "ok" : "nok",
                      cas_slot.bits.bb_owner_id,
                      (reg.bits.slotx_owner_id_all_zero != 0) ? "zero" : \
                      ((reg.bits.slotx_owner_id_all_one != 0) ? "one" : "ok"),
                      upper_bit,
                      (reg.bits.slotx_owner_id_correct != 0) ? "ok" : "nok",
                      cas_slot.bits.ca_owner_id,
                      __klad_proc_ca_vendor_id((hi_ca_vendor_id)cas_slot.bits.ca_owner_id),
                      (cas_slot.bits.rk_disable != 0) ? "no" : "yes");
    }
}

static hi_void __klad_proc_cmd_default_boot_slot(struct seq_file *p)
{
    rkp_slotx_crc_rst reg;
    rkp_rk_boot_flag boot_slot;
    hi_u8 upper_bit = (otp_shadow_read(OTP_SHW_OWNER_UPPER_BIT) >> 0x8) & 0x0f; /* upper bit 0x0d[3-0] */

    reg.u32 = rkp_read_reg(RKP_BOOT_CRC_RST);
    boot_slot.u32 = otp_shadow_read(OTP_SHW_BOOT_SLOT_FLAG);
    HI_PROC_PRINT(p, "%-12s%-12s%-12s(%02x)%-8s(%02x)%-8s(%02x)%-10s%-s\n", "boot_slot",
                  (reg.bits.slotx_rk_all_zero != 0) ? "zero" : ((reg.bits.slotx_rk_all_one != 0) ? "one" : "ok"),
                  (reg.bits.slotx_crc_ok != 0) ? "ok" : "nok",
                  boot_slot.bits.bb_owner_id,
                  (reg.bits.slotx_owner_id_all_zero != 0) ? "zero" : \
                  ((reg.bits.slotx_owner_id_all_one != 0) ? "one" : "ok"),
                  upper_bit,
                  (reg.bits.slotx_owner_id_correct != 0) ? "ok" : "nok",
                  boot_slot.bits.ca_owner_id,
                  __klad_proc_ca_vendor_id((hi_ca_vendor_id)boot_slot.bits.ca_owner_id),
                  (boot_slot.bits.rk_disable != 0) ? "no" : "yes");
}

static hi_void __klad_proc_cmd_default_hisi_slot(struct seq_file *p)
{
    rkp_slotx_crc_rst reg;

    reg.u32 = rkp_read_reg(RKP_HISI_CRC_RST);
    HI_PROC_PRINT(p, "%-12s%-12s%-12s%-12s%-12s%-14s%-s\n", "hisi_slot",
                  (reg.bits.slotx_rk_all_zero != 0) ? "zero" : ((reg.bits.slotx_rk_all_one != 0) ? "one" : "ok"),
                  (reg.bits.slotx_crc_ok != 0) ? "ok" : "nok",
                  "--", "--", "--", "--");
}

static hi_void __klad_proc_cmd_default_stbm_slot(struct seq_file *p)
{
    rkp_slotx_crc_rst reg;
    rkp_rk_stbm_flag stbm_slot;

    reg.u32 = rkp_read_reg(RKP_STBM_CRC_RST);
    stbm_slot.u32 = otp_shadow_read(OTP_SHW_STBM_SLOT_FLAG);
    HI_PROC_PRINT(p, "%-12s%-12s%-12s%-12s%-12s%-14s%-s\n", "oem_slot",
                  (reg.bits.slotx_rk_all_zero != 0) ? "zero" : ((reg.bits.slotx_rk_all_one != 0) ? "one" : "ok"),
                  (reg.bits.slotx_crc_ok != 0) ? "ok" : "nok",
                  "--", "--", "--", (stbm_slot.bits.rk_disable != 0) ? "no" : "yes");
}

static hi_s32 __klad_proc_cmd_default(struct seq_file *p, hi_void *v)
{
    HI_PROC_PRINT(p, "\n=============================Rootkey status=====================================\n");
    HI_PROC_PRINT(p, "slot        rootkey     crc         bb_owner_id upper_bit   ca_owner_id enabled\n");
    HI_PROC_PRINT(p, "            zero/one/ok ok/nok      zero/one/ok ok/nok                  yes/no \n");
    HI_PROC_PRINT(p, "================================================================================\n");
    __klad_proc_cmd_default_cas_slot(p);
    __klad_proc_cmd_default_boot_slot(p);
    __klad_proc_cmd_default_hisi_slot(p);
    __klad_proc_cmd_default_stbm_slot(p);
    HI_PROC_PRINT(p, "================================================================================\n");
    __klad_proc_cmd_lock_stat(p, v);
    return 0;
}

static hi_u8 *__klad_cpu_type(hi_u8 reg)
{
    if (reg == 0x00) {
        return "unlocked";
    }
    if (reg == 0x55) {
        return "tpp";
    }
    if (reg == 0x95) {
        return "hpp";
    }
    if (reg == 0xa5) {
        return "tee";
    }
    if (reg == 0xaa) {
        return "ree";
    }
    return "unknown";
}

static hi_bool __klad_is_locked(hi_u8 reg)
{
#ifdef  HI_TEE_SUPPORT
    if (reg == 0xaa) {
        return HI_TRUE;
    }
#else
    if (reg == 0xa5) {
        return HI_TRUE;
    }
#endif
    return HI_FALSE;
}


static hi_void __klad_proc_hkl_stat(hi_void)
{
    struct hkl_lock_stat stat = {{0}};
    hi_u32 kl_index;

    hkl_get_klad_lock_info(&stat);
    HI_PRINT("0x00: unlock, 0x55:tpp, 0x95:hpp, 0xa5:tee, 0xaa:ree.\n");
    for (kl_index = 0; kl_index < 0x8; kl_index++) {
        HI_PRINT("com keyladder  %d:%s.\n", kl_index, __klad_cpu_type(stat.com_lock_stat[kl_index]));
    }
    HI_PRINT("ta keyladder    :%s.\n", __klad_cpu_type(stat.ta_lock_stat));
    HI_PRINT("fp keyladder    :%s.\n", __klad_cpu_type(stat.fp_lock_stat));
    HI_PRINT("nonce keyladder :%s.\n", __klad_cpu_type(stat.nonce_lock_stat));
    HI_PRINT("clr keyladder   :%s.\n\n", __klad_cpu_type(stat.clr_lock_stat));
    return ;
}

static hi_s32 __klad_proc_hkl_lock_stat(struct seq_file *p, hi_void *v)
{
    struct hkl_lock_stat stat = {{0}};
    hi_u32 kl_index;

    HI_PROC_PRINT(p, "\n====================Keyladder status=======================\n");
    hkl_get_klad_lock_info(&stat);
    HI_PROC_PRINT(p, "0x00: unlock, 0x55:tpp, 0x95:hpp, 0xa5:tee, 0xaa:ree.\n");
    for (kl_index = 0; kl_index < 0x8; kl_index++) {
        HI_PROC_PRINT(p, "com keyladder %d:    %s.\n", kl_index, __klad_cpu_type(stat.com_lock_stat[kl_index]));
    }
    HI_PROC_PRINT(p, "ta keyladder   :    %s.\n", __klad_cpu_type(stat.ta_lock_stat));
    HI_PROC_PRINT(p, "fp keyladder   :    %s.\n", __klad_cpu_type(stat.fp_lock_stat));
    HI_PROC_PRINT(p, "nonce keyladder:    %s.\n", __klad_cpu_type(stat.nonce_lock_stat));
    HI_PROC_PRINT(p, "clr keyladder  :    %s.\n\n", __klad_cpu_type(stat.clr_lock_stat));
    HI_PROC_PRINT(p, "===========================================================\n");
    return HI_SUCCESS;
}

static hi_s32 __klad_proc_cmd_lock_stat(struct seq_file *p, hi_void *v)
{
    return __klad_proc_hkl_lock_stat(p, v);
}


#ifdef HI_KLAD_PERF_SUPPORT

static hi_void __timetamp_print1(struct seq_file *p, const hi_char *str, struct time_ns *time_b)
{
    HI_PROC_PRINT(p, "%-16s,%06ld.%06ld s\n", str, time_b->tv_sec, time_b->tv_nsec);
}

static hi_void __timetamp_print2(struct seq_file *p, const hi_char *str, struct time_ns *time_b,
                                 struct time_ns *time_e, struct time_ns *time_new)
{
    /* if time_e < time_new, means time_e is a inivalid time, drop it. */
    if ((time_e->tv_sec < time_new->tv_sec) ||
            ((time_e->tv_sec == time_new->tv_sec) && (time_e->tv_nsec < time_new->tv_nsec))) {
        HI_PROC_PRINT(p, "%-16s,%06ld.%06ld s, invalid timetamp.\n", str, time_e->tv_sec, time_e->tv_nsec);
        return;
    }
    /* if time_new < time_b, means lost part of the time. */
    if ((time_new->tv_sec < time_b->tv_sec) ||
            ((time_new->tv_sec == time_b->tv_sec) && (time_new->tv_nsec < time_b->tv_nsec))) {
        HI_PROC_PRINT(p, "%-16s,%06ld.%06ld s->%06ld.%06ld s time lost.\n", "",
                      time_b->tv_sec, time_b->tv_nsec, time_new->tv_sec, time_new->tv_nsec);
    }

    if (time_new->tv_sec ==  time_e->tv_sec) {
        HI_PROC_PRINT(p, "%-16s,%06ld.%06ld s, cost:%ld.%03ld ms\n",
                      str, time_e->tv_sec, time_e->tv_nsec,
                      (time_e->tv_nsec - time_new->tv_nsec) / TIME_MS2US,
                      (time_e->tv_nsec - time_new->tv_nsec) % TIME_MS2US);
    } else {
        HI_PROC_PRINT(p, "%-16s,%06ld.%09ld s, cost:%ld.%03ld ms\n",
                      str, time_e->tv_sec, time_e->tv_nsec,
                      ((time_e->tv_sec - time_new->tv_sec) * TIME_S2US +
                       time_e->tv_nsec - time_new->tv_nsec) / TIME_MS2US,
                      ((time_e->tv_sec - time_new->tv_sec) * TIME_S2US +
                       time_e->tv_nsec - time_new->tv_nsec) % TIME_MS2US);
    }
    /* update time_new */
    time_new->tv_sec = time_e->tv_sec;
    time_new->tv_nsec = time_e->tv_nsec;
}

static hi_void __klad_proc_timetamp_dump(struct seq_file *p, hi_void *v, struct klad_r_base *base)
{
    struct klad_timestamp *t = &base->timestamp;
    struct time_ns time_new;

    time_new.tv_sec = t->hw_in.tv_sec;
    time_new.tv_nsec = t->hw_in.tv_nsec;

    __timetamp_print1(p, "hw_in", &t->hw_in);

    __timetamp_print2(p, "create_in", &t->hw_in, &t->create_in, &time_new);

    __timetamp_print2(p, "create_out", &t->create_in, &t->create_out, &time_new);

    __timetamp_print2(p, "open_in", &t->create_out, &t->open_in, &time_new);

    __timetamp_print2(p, "open_out", &t->open_in, &t->open_out, &time_new);

    __timetamp_print2(p, "rkp_in", &t->open_out, &t->rkp_in, &time_new);

    __timetamp_print2(p, "rkp_out", &t->rkp_in, &t->rkp_out, &time_new);

    __timetamp_print2(p, "start_in", &t->rkp_out, &t->start_in, &time_new);

    __timetamp_print2(p, "start_out", &t->start_in, &t->start_out, &time_new);

    __timetamp_print2(p, "close_in", &t->start_out, &t->close_in, &time_new);

    __timetamp_print2(p, "close_out", &t->close_in, &t->close_out, &time_new);

    __timetamp_print2(p, "destroy_in", &t->close_out, &t->destroy_in, &time_new);

    __timetamp_print2(p, "destroy_out", &t->destroy_in, &t->destroy_out, &time_new);

    __timetamp_print2(p, "hw_out", &t->destroy_out, &t->hw_out, &time_new);

    /* get the whole time. */
    time_new.tv_sec = t->hw_in.tv_sec;
    time_new.tv_nsec = t->hw_in.tv_nsec;
    __timetamp_print2(p, "whole time", &t->hw_in, &t->hw_out, &time_new);
}

#endif

static hi_s32 __klad_proc_cmd_timetamp(struct seq_file *p, hi_void *v)
{
#ifdef HI_KLAD_PERF_SUPPORT
    struct list_head *com_hkl_node = HI_NULL;
    struct klad_mgmt *mgmt = __get_klad_mgmt();

    __mutex_lock(&mgmt->com_hkl_lock);
    list_for_each(com_hkl_node, &mgmt->com_hkl_head) {
        struct klad_r_com_hkl *hkl = list_entry(com_hkl_node, struct klad_r_com_hkl, node);
        HI_PROC_PRINT(p, "\ncom   keyladder   :handle 0x%x, index=%d.\n", hkl->base.handle, hkl->base.hw_id);
        __klad_proc_timetamp_dump(p, v, (struct klad_r_base *)&hkl->base);
    }

    HI_PROC_PRINT(p, "\nta    keyladder   \n");
    __klad_proc_timetamp_dump(p, v, (struct klad_r_base *)&mgmt->ta_hkl.base);

    HI_PROC_PRINT(p, "\nfp    keyladder   \n");
    __klad_proc_timetamp_dump(p, v, (struct klad_r_base *)&mgmt->fp_hkl.base);

    HI_PROC_PRINT(p, "\nnonce keyladder   \n");
    __klad_proc_timetamp_dump(p, v, (struct klad_r_base *)&mgmt->nonce_hkl.base);

    HI_PROC_PRINT(p, "\nclear keyladder   \n");
    __klad_proc_timetamp_dump(p, v, (struct klad_r_base *)&mgmt->clr_route.base);

    __mutex_unlock(&mgmt->com_hkl_lock);
#else
    HI_PROC_PRINT(p, "\nNot support this feature.\n");
#endif
    return 0;
}

static hi_s32 __klad_proc_cmd_timestamp_queue(struct seq_file *p, hi_void *v)
{
    return klad_timestamp_dump(p);
}

static hi_s32 __klad_proc_cmd_timestamp_queue_logic(struct seq_file *p, hi_void *v)
{
    return  klad_timestamp_logic_dump(p);
}

static hi_void __klad_proc_com_hkl_obj(struct seq_file *p, struct klad_mgmt *mgmt)
{
    struct list_head *com_hkl_node = HI_NULL;
    struct klad_r_com_hkl *hkl = HI_NULL;
    hi_s32 i;

    list_for_each(com_hkl_node, &mgmt->com_hkl_head) {
        hkl = list_entry(com_hkl_node, struct klad_r_com_hkl, node);

        __mutex_lock(&hkl->lock);
        HI_PROC_PRINT(p, "\nID        %d \n", hkl->base.id);
        HI_PROC_PRINT(p, "Handle    0x%08x \n", hkl->base.handle);
        HI_PROC_PRINT(p, "Ref count 0x%08x \n", atomic_read(&hkl->base.ref_count));

        HI_PROC_PRINT(p, "HW ID     %d \n", hkl->base.hw_id);
        HI_PROC_PRINT(p, "LOCKED    %d \n", hkl->base.is_locked);

        HI_PROC_PRINT(p, "RKP OK    %d \n", hkl->rkp_ready);

        HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
        HI_PROC_PRINT(p, "Klad type %d \n", hkl->base_attr.klad_type);
        HI_PROC_PRINT(p, "Vendor id %d \n", hkl->base_attr.vendor_id);
        HI_PROC_PRINT(p, "Rk Slot   %d \n", hkl->base_attr.root_slot);
        HI_PROC_PRINT(p, "Unique    %d \n", hkl->base_attr.unique);
        HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
        HI_PROC_PRINT(p, "Module ID 0x%08x 0x%08x 0x%08x 0x%08x\n", hkl->base_attr.module_id[0x0],
                      hkl->base_attr.module_id[0x1], hkl->base_attr.module_id[0x2], hkl->base_attr.module_id[0x3]);
        __mutex_unlock(&hkl->lock);
    }
    HI_PROC_PRINT(p, "-------------------------------------------\n");

    for (i = 0; i < mgmt->com_hkl_cnt; i++) {
        hkl = &mgmt->com_hkl[i];
        __mutex_lock(&hkl->lock);
        HI_PROC_PRINT(p, "\nID        %d \n", hkl->base.id);
        HI_PROC_PRINT(p, "Handle    0x%08x \n", hkl->base.handle);
        HI_PROC_PRINT(p, "Ref count 0x%08x \n", atomic_read(&hkl->base.ref_count));

        HI_PROC_PRINT(p, "HW ID     %d \n", hkl->base.hw_id);
        HI_PROC_PRINT(p, "LOCKED    %d \n", hkl->base.is_locked);

        HI_PROC_PRINT(p, "RKP OK    %d \n", hkl->rkp_ready);

        HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
        HI_PROC_PRINT(p, "Klad type %d \n", hkl->base_attr.klad_type);
        HI_PROC_PRINT(p, "Vendor id %d \n", hkl->base_attr.vendor_id);
        HI_PROC_PRINT(p, "Rk Slot   %d \n", hkl->base_attr.root_slot);
        HI_PROC_PRINT(p, "Unique    %d \n", hkl->base_attr.unique);
        HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
        HI_PROC_PRINT(p, "Module ID 0x%08x 0x%08x 0x%08x 0x%08x\n", hkl->base_attr.module_id[0x0],
                      hkl->base_attr.module_id[0x1], hkl->base_attr.module_id[0x2], hkl->base_attr.module_id[0x3]);
        __mutex_unlock(&hkl->lock);
    }
}

static hi_void __klad_proc_ta_hkl_obj(struct seq_file *p, struct klad_mgmt *mgmt)
{
    struct klad_r_ta_hkl *hkl = &mgmt->ta_hkl;

    if (mgmt->ta_hkl_state == KLAD_CLOSED) {
        return;
    }
    __mutex_lock(&hkl->lock);
    HI_PROC_PRINT(p, "\nID        %d \n", hkl->base.id);
    HI_PROC_PRINT(p, "Handle    0x%08x \n", hkl->base.handle);
    HI_PROC_PRINT(p, "Ref count 0x%08x \n", atomic_read(&hkl->base.ref_count));

    HI_PROC_PRINT(p, "HW ID     %d \n", hkl->base.hw_id);
    HI_PROC_PRINT(p, "LOCKED    %d \n", hkl->base.is_locked);

    HI_PROC_PRINT(p, "RKP OK    %d \n", hkl->rkp_ready);

    HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
    HI_PROC_PRINT(p, "Klad type %d \n", hkl->base_attr.klad_type);
    HI_PROC_PRINT(p, "Vendor id %d \n", hkl->base_attr.vendor_id);
    HI_PROC_PRINT(p, "Rk Slot   %d \n", hkl->base_attr.root_slot);
    HI_PROC_PRINT(p, "Unique    %d \n", hkl->base_attr.unique);
    HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
    HI_PROC_PRINT(p, "Module ID 0x%08x 0x%08x 0x%08x 0x%08x\n", hkl->base_attr.module_id[0x0],
                  hkl->base_attr.module_id[0x1], hkl->base_attr.module_id[0x2], hkl->base_attr.module_id[0x3]);
    __mutex_unlock(&hkl->lock);
}

static hi_void __klad_proc_fp_hkl_obj(struct seq_file *p, struct klad_mgmt *mgmt)
{
    struct klad_r_fp_hkl *hkl = &mgmt->fp_hkl;

    if (mgmt->fp_hkl_state == KLAD_CLOSED) {
        return;
    }
    __mutex_lock(&hkl->lock);
    HI_PROC_PRINT(p, "\nID        %d \n", hkl->base.id);
    HI_PROC_PRINT(p, "Handle    0x%08x \n", hkl->base.handle);
    HI_PROC_PRINT(p, "Ref count 0x%08x \n", atomic_read(&hkl->base.ref_count));

    HI_PROC_PRINT(p, "HW ID     %d \n", hkl->base.hw_id);
    HI_PROC_PRINT(p, "LOCKED    %d \n", hkl->base.is_locked);

    HI_PROC_PRINT(p, "RKP OK    %d \n", hkl->rkp_ready);

    HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
    HI_PROC_PRINT(p, "Klad type %d \n", hkl->base_attr.klad_type);
    HI_PROC_PRINT(p, "Vendor id %d \n", hkl->base_attr.vendor_id);
    HI_PROC_PRINT(p, "Rk Slot   %d \n", hkl->base_attr.root_slot);
    HI_PROC_PRINT(p, "Unique    %d \n", hkl->base_attr.unique);
    HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
    HI_PROC_PRINT(p, "Module ID 0x%08x 0x%08x 0x%08x 0x%08x\n", hkl->base_attr.module_id[0x0],
                  hkl->base_attr.module_id[0x1], hkl->base_attr.module_id[0x2], hkl->base_attr.module_id[0x3]);
    __mutex_unlock(&hkl->lock);
}

static hi_void __klad_proc_nonce_hkl_obj(struct seq_file *p, struct klad_mgmt *mgmt)
{
    struct klad_r_nonce_hkl *hkl = &mgmt->nonce_hkl;

    if (mgmt->nonce_hkl_state == KLAD_CLOSED) {
        return;
    }
    __mutex_lock(&hkl->lock);
    HI_PROC_PRINT(p, "\nID        %d \n", hkl->base.id);
    HI_PROC_PRINT(p, "Handle    0x%08x \n", hkl->base.handle);
    HI_PROC_PRINT(p, "Ref count 0x%08x \n", atomic_read(&hkl->base.ref_count));

    HI_PROC_PRINT(p, "HW ID     %d \n", hkl->base.hw_id);
    HI_PROC_PRINT(p, "LOCKED    %d \n", hkl->base.is_locked);

    HI_PROC_PRINT(p, "RKP OK    %d \n", hkl->rkp_ready);

    HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
    HI_PROC_PRINT(p, "Klad type %d \n", hkl->base_attr.klad_type);
    HI_PROC_PRINT(p, "Vendor id %d \n", hkl->base_attr.vendor_id);
    HI_PROC_PRINT(p, "Rk Slot   %d \n", hkl->base_attr.root_slot);
    HI_PROC_PRINT(p, "Unique    %d \n", hkl->base_attr.unique);
    HI_PROC_PRINT(p, "Secure    %d \n", hkl->base_attr.is_secure_key);
    HI_PROC_PRINT(p, "Module ID 0x%08x 0x%08x 0x%08x 0x%08x\n", hkl->base_attr.module_id[0x0],
                  hkl->base_attr.module_id[0x1], hkl->base_attr.module_id[0x2], hkl->base_attr.module_id[0x3]);
    __mutex_unlock(&hkl->lock);
}

static hi_s32 __klad_proc_cmd_hw_obj(struct seq_file *p, hi_void *v)
{
    struct klad_mgmt *mgmt = __get_klad_mgmt();

    __mutex_lock(&mgmt->lock);
    __klad_proc_com_hkl_obj(p, mgmt);
    __klad_proc_ta_hkl_obj(p, mgmt);
    __klad_proc_fp_hkl_obj(p, mgmt);
    __klad_proc_nonce_hkl_obj(p, mgmt);
    __mutex_unlock(&mgmt->lock);
    return 0;
}

hi_s32 hi_drv_hw_com_klad_start_dbg(struct klad_entry_key *attrs, hi_klad_com_entry *entry)
{
    hi_s32 ret;
    struct klad_r_com_hkl *obj = HI_NULL;

    ret = klad_hw_com_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_open, ret);
        goto out;
    }
    ret = klad_hw_com_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_start, ret);
        goto out1;
    }
out1:
    if (klad_hw_com_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw com hkl instance failed.\n");
    }
out:
    return ret;
}

hi_s32 hi_drv_hw_clr_route_start_dbg(struct klad_entry_key *attrs, hi_klad_clr_entry *entry)
{
    hi_s32 ret;
    struct klad_r_clr_route *obj = HI_NULL;

    ret = klad_hw_clr_route_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    __klad_proc_hkl_stat();
    ret = klad_hw_clr_route_start(obj, entry);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
out1:
    if (klad_hw_clr_route_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw clear route instance failed.\n");
    }
out:
    return ret;
}

hi_s32 hi_drv_hw_fp_klad_start_dbg(struct klad_entry_key *attrs, hi_klad_fp_entry *entry)
{
    hi_s32 ret;
    struct klad_r_fp_hkl *obj = HI_NULL;

    ret = klad_hw_fp_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    __klad_proc_hkl_stat();
    ret = klad_hw_fp_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
out1:
    if (klad_hw_fp_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw com hkl instance failed.\n");
    }
out:
    return ret;
}

static hi_s32 __klad_proc_cmd_unlock(hi_proc_klad_index index)
{
    hi_u32 i;
    struct hkl_lock_stat stat = {{0}};

    hkl_get_klad_lock_info(&stat);

    if (index == HI_PROC_KLAD_ALL) {
        for (i = 0; i <= HI_PROC_KLAD_COM_KLAD7; i++) {
            if (__klad_is_locked(stat.com_lock_stat[i])) {
                hi_klad_com_unlock(i);
            }
        }
        if (__klad_is_locked(stat.ta_lock_stat)) {
            hi_klad_ta_unlock();
        }
        if (__klad_is_locked(stat.fp_lock_stat)) {
            hi_klad_fp_unlock();
        }
        if (__klad_is_locked(stat.nonce_lock_stat)) {
            hi_klad_nonce_unlock();
        }
        if (__klad_is_locked(stat.clr_lock_stat)) {
            hi_klad_clrcw_unlock();
        }
    } else if (index <= HI_PROC_KLAD_COM_KLAD7) {
        if (__klad_is_locked(stat.com_lock_stat[index])) {
            hi_klad_com_unlock(index);
        }
    } else if (index == HI_PROC_KLAD_TA) {
        if (__klad_is_locked(stat.ta_lock_stat)) {
            hi_klad_ta_unlock();
        }
    } else if (index == HI_PROC_KLAD_FP) {
        if (__klad_is_locked(stat.fp_lock_stat)) {
            hi_klad_fp_unlock();
        }
    } else if (index == HI_PROC_KLAD_NONCE) {
        if (__klad_is_locked(stat.nonce_lock_stat)) {
            hi_klad_nonce_unlock();
        }
    } else if (index == HI_PROC_KLAD_CLR) {
        if (__klad_is_locked(stat.clr_lock_stat)) {
            hi_klad_clrcw_unlock();
        }
    }
    return 0;
}

static hi_s32 __klad_proc_cmd_lock(hi_proc_klad_index index)
{
    hi_u32 i;
    hi_u32 com_kl_num;
    if (index == HI_PROC_KLAD_ALL) {
        for (i = 0; i <= HI_PROC_KLAD_COM_KLAD7; i++) {
            hi_klad_com_lock(&com_kl_num);
        }
        hi_klad_ta_lock();
        hi_klad_fp_lock();
        hi_klad_nonce_lock();
        hi_klad_clrcw_lock();
    } else if (index <= HI_PROC_KLAD_COM_KLAD7) {
        hi_klad_com_lock(&com_kl_num);
    } else if (index == HI_PROC_KLAD_TA) {
        hi_klad_ta_lock();
    } else if (index == HI_PROC_KLAD_FP) {
        hi_klad_fp_lock();
    } else if (index == HI_PROC_KLAD_NONCE) {
        hi_klad_nonce_lock();
    } else if (index == HI_PROC_KLAD_CLR) {
        hi_klad_clrcw_lock();
    }
    return 0;
}

static struct cmd_map g_proc_cmd[] = {
    { "default",       HI_PROC_DEFAULT,      __klad_proc_cmd_default },
    { "reg",           HI_PROC_REG,          __klad_proc_cmd_reg },
    { "hkl_stat",      HI_PROC_LOCK_STAT,    __klad_proc_cmd_lock_stat },
    { "timestamp",     HI_PROC_TIMESTAMP,    __klad_proc_cmd_timetamp },
    { "time_queue",    HI_PROC_TIME_QUEUE,   __klad_proc_cmd_timestamp_queue },
    { "time_logic",    HI_PROC_TIME_Q_HW,    __klad_proc_cmd_timestamp_queue_logic },
    { "hw_obj",        HI_PROC_HW_OBJ,       __klad_proc_cmd_hw_obj },
    { "time_clean",    HI_PROC_TIME_Q_CLR,   HI_NULL },
    { "hw_com",        HI_PROC_HW_COM,       __klad_proc_cmd_default },
    { "hw_clr",        HI_PROC_HW_CLR,       __klad_proc_cmd_default },
    { "hw_ta",         HI_PROC_HW_TA,        __klad_proc_cmd_default },
    { "hw_nonce",      HI_PROC_HW_NONCE,     __klad_proc_cmd_default },
    { "hw_fp",         HI_PROC_HW_FP,        __klad_proc_cmd_default },
    { "sw_com",        HI_PROC_SW_COM,       __klad_proc_cmd_default },
    { "sw_clr",        HI_PROC_SW_CLR,       __klad_proc_cmd_default },
    { "sw_fp",         HI_PROC_SW_FP,        __klad_proc_cmd_default },
    { "sw_ta",         HI_PROC_SW_TA,        __klad_proc_cmd_default },
    { "sw_nonce",      HI_PROC_SW_NONCE,     __klad_proc_cmd_default },
    { "hw_lock",       HI_PROC_LOCK,         __klad_proc_cmd_default },
    { "hw_unlock",     HI_PROC_UNLOCK,       __klad_proc_cmd_default },
};

hi_char *__drop_head_space(hi_char *buf, hi_u32 len)
{
    hi_s32 i;

    for (i = 0; i < len; i++) {
        if (buf[i] != ' ') {
            return buf + i;
        }
    }
    return HI_NULL;
}

static hi_s32 __klad_proc_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        goto out;
    }
    __klad_proc_get_help();

out:
    return ret;
}

static hi_s32 __klad_proc_default(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_DEFAULT;

out:
    return ret;
}

static hi_s32 __klad_proc_reg(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_REG;

out:
    return ret;
}

static hi_s32 __klad_proc_lock_stat(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_LOCK_STAT;

out:
    return ret;
}

static hi_s32 __klad_proc_time_stamp(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_TIMESTAMP;

out:
    return ret;
}

static hi_s32 __klad_proc_time_queue(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_TIME_QUEUE;

out:
    return ret;
}

static hi_s32 __klad_proc_time_logic(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_TIME_Q_HW;

out:
    return ret;
}

static hi_s32 __klad_proc_hw_obj(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_HW_OBJ;

out:
    return ret;
}

static hi_s32 __klad_proc_cmd_timestamp_clean(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                              hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        goto out;
    }

    g_cmd = HI_PROC_TIME_Q_CLR;

    klad_timestamp_logic_queue_clean();
    ret = klad_timestamp_queue_clean();

out:
    return ret;
}

static hi_s32 __klad_proc_cmd_hw_com(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {
        .session = HI_NULL,
        .hkl_base_attr.klad_type = HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_CSA2, HI_KLAD_COM, 0x00),
        .hkl_base_attr.vendor_id = 0x0f000000,
        .hkl_base_attr.module_id = {0x00000424, 0x00000101, 0x00000500, 0x0},
    };

    hi_klad_attr attr = {
        .klad_cfg = {
            0x0f000000, HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_CSA2, HI_KLAD_COM, 0x00)
        },
        .key_cfg = {HI_TRUE, HI_TRUE, HI_CRYPTO_ENGINE_ALG_AES_ECB_T},
    };

    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX] = {
        {
            .level = HI_KLAD_LEVEL1,
            .alg = HI_KLAD_ALG_TYPE_AES,
            .key_size = 0x10,
            .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
        },
        {
            .level = HI_KLAD_LEVEL2,
            .alg = HI_KLAD_ALG_TYPE_AES,
            .key_size = 0x10,
            .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
        },
    };

    hi_klad_content_key content_key = {
        .odd = HI_TRUE,
        .alg = HI_KLAD_ALG_TYPE_AES,
        .key_size = 0x10,
        .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
    };
    hi_klad_com_entry entry = {0};

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        goto out;
    }

    g_cmd = HI_PROC_HW_COM;
    ret = memcpy_s(&entry.attr, sizeof(hi_klad_attr), &attr, sizeof(hi_klad_attr));
    ret |= memcpy_s(entry.session_key, sizeof(hi_klad_session_key) * HI_KLAD_LEVEL_MAX,
                    session_key, sizeof(hi_klad_session_key) * HI_KLAD_LEVEL_MAX);
    ret |= memcpy_s(&entry.content_key, sizeof(hi_klad_content_key), &content_key, sizeof(hi_klad_content_key));
    if (ret != EOK) {
        print_err_func(memcpy_s, ret);
    }

    __klad_proc_hkl_stat();
    ret = hi_drv_hw_com_klad_start_dbg(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_com_klad_start_dbg, ret);
    }
    __klad_proc_hkl_stat();

out:
    return ret;
}

static hi_s32 __klad_proc_cmd_hw_clr(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {
        .hkl_base_attr.klad_type = HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_NULL, HI_KLAD_CLR, 0x00),
        .hkl_base_attr.vendor_id = 0x0f000000,
        .hkl_base_attr.module_id = {0x00000424, 0x00000101, 0x00000500, 0x0},
    };

    hi_klad_attr attr = {
        .klad_cfg = {
            0x0f000000, HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_NULL, HI_KLAD_CLR, 0x00)
        },
        .key_cfg = {HI_TRUE, HI_TRUE, HI_CRYPTO_ENGINE_ALG_AES_ECB_T},
    };

    hi_klad_clear_key clr_key = {
        .odd = HI_TRUE,
        .key_size = 0x10,
        .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
    };
    hi_klad_clr_entry entry = {0};

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        goto out;
    }

    g_cmd = HI_PROC_HW_CLR;
    ret = memcpy_s(&entry.attr, sizeof(hi_klad_attr), &attr, sizeof(hi_klad_attr));
    ret |= memcpy_s(&entry.clr_key, sizeof(hi_klad_clear_key), &clr_key, sizeof(hi_klad_clear_key));
    if (ret != EOK) {
        print_err_func(memcpy_s, ret);
    }

    __klad_proc_hkl_stat();
    ret = hi_drv_hw_clr_route_start_dbg(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_clr_route_start_dbg, ret);
    }
    __klad_proc_hkl_stat();

out:
    return ret;
}

static hi_s32 __klad_proc_hw_ta(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_HW_TA;

out:
    return ret;
}
static hi_s32 __klad_proc_hw_nonce(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_HW_NONCE;

out:
    return ret;
}

static hi_s32 __klad_proc_cmd_hw_fp(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {
        .hkl_base_attr.klad_type = HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_R2R, HI_KLAD_FP, 0x00),
        .hkl_base_attr.vendor_id = 0x0f000000,
        .hkl_base_attr.module_id = {0x00000424, 0x00000101, 0x00000500, 0x0},
    };

    hi_klad_attr attr = {
        .klad_cfg = {
            0x0f000000, HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_R2R, HI_KLAD_FP, 0x00)
        },
        .key_cfg = {HI_TRUE, HI_TRUE, HI_CRYPTO_ENGINE_ALG_AES_ECB_T},
    };

    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX] = {
        {
            .level = HI_KLAD_LEVEL1,
            .alg = HI_KLAD_ALG_TYPE_AES,
            .key_size = 0x10,
            .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
        },
        {
            .level = HI_KLAD_LEVEL2,
            .alg = HI_KLAD_ALG_TYPE_AES,
            .key_size = 0x10,
            .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
        },
    };

    hi_klad_fp_key fp_key = {
        .operation = HI_KLAD_FP_OPT_ENCRYPT,
        .alg = HI_KLAD_ALG_TYPE_AES,
        .key_size = 0x10,
        .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
    };
    hi_klad_fp_entry entry = {0};

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_HW_FP;
    ret = memcpy_s(&entry.attr, sizeof(hi_klad_attr), &attr, sizeof(hi_klad_attr));
    ret |= memcpy_s(entry.session_key, sizeof(hi_klad_session_key) * HI_KLAD_LEVEL_MAX,
                    session_key, sizeof(hi_klad_session_key) * HI_KLAD_LEVEL_MAX);
    ret |= memcpy_s(&entry.fp_key, sizeof(hi_klad_fp_key), &fp_key, sizeof(hi_klad_fp_key));
    if (ret != EOK) {
        print_err_func(memcpy_s, ret);
    }
    __klad_proc_hkl_stat();
    ret = hi_drv_hw_fp_klad_start_dbg(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_fp_klad_start_dbg, ret);
    }
    __klad_proc_hkl_stat();

out:
    return ret;
}

static hi_s32 __klad_proc_cmd_sw_com(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_handle handle = 0;
    hi_handle ks_handle = 0;
    hi_klad_attr attr = {
        .klad_cfg = {
            0x0f000000, HI_KLAD_INSTANCE(HI_CA_ID_BASIC, HI_ROOTKEY_CSA2, HI_KLAD_COM, 0x00)
        },
        .key_cfg = {HI_TRUE, HI_TRUE, HI_CRYPTO_ENGINE_ALG_AES_ECB_T},
    };
    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX] = {
        {
            .level = HI_KLAD_LEVEL1,
            .alg = HI_KLAD_ALG_TYPE_AES,
            .key_size = 0x10,
            .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
        },
        {
            .level = HI_KLAD_LEVEL2,
            .alg = HI_KLAD_ALG_TYPE_AES,
            .key_size = 0x10,
            .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
        },
    };

    hi_klad_content_key content_key = {
        .odd = HI_TRUE,
        .alg = HI_KLAD_ALG_TYPE_AES,
        .key_size = 0x10,
        .key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x04, 0x24},
    };

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_SW_COM;
    ret = hi_drv_klad_create(&handle);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    HI_ERR_KLAD("handle =0x%x\n", handle);
    ret = hi_drv_klad_set_attr(handle, &attr);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_klad_set_attr, ret);
        goto out1;
    }
    ret = hi_drv_klad_attach(handle, ks_handle);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_klad_attach, ret);
        goto out1;
    }
    ret = hi_drv_klad_set_session_key(handle, &session_key[0]);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_klad_set_session_key, ret);
        goto out2;
    }
    ret = hi_drv_klad_set_session_key(handle, &session_key[1]);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_klad_set_session_key, ret);
        goto out2;
    }
    ret = hi_drv_klad_set_content_key(handle, &content_key);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_klad_set_content_key, ret);
        goto out2;
    }
out2:
    hi_drv_klad_detach(handle, ks_handle);
out1:
    hi_drv_klad_destroy(handle);
out:
    return ret;
}

static hi_s32 __klad_proc_sw_clr(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_SW_CLR;

out:
    return ret;
}

static hi_s32 __klad_proc_sw_fp(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_SW_FP;

out:
    return ret;
}

static hi_s32 __klad_proc_sw_ta(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_SW_FP;

out:
    return ret;
}

static hi_s32 __klad_proc_sw_nonce(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret = HI_SUCCESS;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_SW_FP;

out:
    return ret;
}

static hi_s32 __func_proc_cmd_hkl_lock(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 value;
    hi_proc_klad_index index;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }
    if (argc < 0x2) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_LOCK;

    if (strstr(argv[0x1], "all") != HI_NULL) {
        index = HI_PROC_KLAD_ALL;
    } else {
        value = (hi_u32)osal_strtoul(argv[0x1], HI_NULL, HI_NULL);
        if (value >= HI_PROC_KLAD_SEL_MAX) {
            __klad_proc_get_help();
            ret = HI_FAILURE;
            goto out;
        }
        index = value;
    }

    ret = __klad_proc_cmd_lock(index);

out:
    return ret;
}

static hi_s32 __func_proc_cmd_hkl_unlock(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 value;
    hi_proc_klad_index index;

    if (argv == HI_NULL) {
        ret = HI_ERR_KLAD_NULL_PTR;
        __klad_proc_get_help();
        goto out;
    }
    if (argc < 0x2) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        __klad_proc_get_help();
        goto out;
    }

    g_cmd = HI_PROC_UNLOCK;

    if (strstr(argv[0x1], "all") != HI_NULL) {
        index = HI_PROC_KLAD_ALL;
    } else {
        value = (hi_u32)osal_strtoul(argv[0x1], HI_NULL, HI_NULL);
        if (value >= HI_PROC_KLAD_SEL_MAX) {
            __klad_proc_get_help();
            return HI_FAILURE;
        }
        index = value;
    }

    ret = __klad_proc_cmd_unlock(index);

out:
    return ret;
}

static osal_proc_cmd g_cmd_ops[] = {
    { "help",        __klad_proc_help },
    { "default",     __klad_proc_default },
    { "reg",         __klad_proc_reg },
    { "hkl_stat",    __klad_proc_lock_stat },
    { "timestamp",   __klad_proc_time_stamp },
    { "time_queue",  __klad_proc_time_queue },
    { "time_logic",  __klad_proc_time_logic },
    { "hw_obj",      __klad_proc_hw_obj },

    { "time_clean",  __klad_proc_cmd_timestamp_clean },
    { "hw_com",      __klad_proc_cmd_hw_com },
    { "hw_clr",      __klad_proc_cmd_hw_clr },
    { "hw_ta",       __klad_proc_hw_ta },
    { "hw_nonce",    __klad_proc_hw_nonce },
    { "hw_fp",       __klad_proc_cmd_hw_fp },
    { "sw_com",      __klad_proc_cmd_sw_com },
    { "sw_clr",      __klad_proc_sw_clr },
    { "sw_fp",       __klad_proc_sw_fp },
    { "sw_ta",       __klad_proc_sw_ta },
    { "sw_nonce",    __klad_proc_sw_nonce },
    { "hw_lock",     __func_proc_cmd_hkl_lock },
    { "hw_unlock",   __func_proc_cmd_hkl_unlock },
};

static hi_s32 __klad_proc_read(void *seqfile, void *private)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 size;
    struct cmd_map *node  = HI_NULL_PTR;

    if (seqfile == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }

    for_each_cmd_node(size, node) {
        if (node->cmd != g_cmd) {
            continue;
        }
        if (node->func) {
            ret = node->func((struct seq_file *)seqfile, private);
        }
        break;
    }
    g_cmd = HI_PROC_DEFAULT;
    return ret;
}

hi_s32 klad_register_proc(hi_void)
{
    osal_proc_entry *item;

    item = osal_proc_add(HI_MOD_KLAD_NAME, sizeof(HI_MOD_KLAD_NAME));
    if (item == NULL) {
        print_err_func(osal_proc_add, HI_FAILURE);
        return HI_FAILURE;
    }
    item->read = __klad_proc_read;
    item->cmd_list = g_cmd_ops;
    item->cmd_cnt = sizeof(g_cmd_ops) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_void klad_remove_proc(hi_void)
{
    osal_proc_remove(HI_MOD_KLAD_NAME, sizeof(HI_MOD_KLAD_NAME));
    return;
}

