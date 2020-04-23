/*
 *
 * (C) COPYRIGHT 2014-2016 Hisilicon Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 */
#include <linux/bitops.h>
#include <linux/delay.h>
#include <mali_kbase.h>
#include <mali_kbase_mem.h>
#include <mali_kbase_hmmu_hw.h>
#include <mali_kbase_tlstream.h>
#include <backend/gpu/mali_kbase_device_internal.h>
#include <mali_kbase_as_fault_debugfs.h>

#define HIG1_CFG_CONTROL_BASE        		0x4000
#define HIG1_CFG_CONTROL_REG(r)      		(HIG1_CFG_CONTROL_BASE + (r))

#define HIG1_CFG_Core_ID             		0x000	/* (RO) HIG1 ID identifier */
#define HIG1_Core_ID                 		0x31476948	/* Core ID */

#define HIG1_CFG_Phase_ID            		0x004	/* (RO) HIG1 Phase identifier. 0x1 - Conception Protype */
#define HIG1_Phase_ID                		0x01	/* Phase ID */

#define HIG1_CFG_HYPERVISOR_DISABLE		0x008	/* (RW) HIG1 module disable. set: 0x0: HIG1 enable; 0x1: HIG1 bypass. */
#define HIG1_CFG_ICE_PRESENT			0x00C	/* (RW) HIG1 ICE present mode. set: 0x0: HIG1 ice no present; 0x1: HIG1 ice present */
#define HIG1_CFG_MMU_FEATURE			0x010	/* (RO) HIG1 mmu capability is xMB */
#define HIG1_CFG_TLB_CACHE_MODE			0x014	/* (RW) HIG1 tlb bypass. set:0x0: tlb enable; 0x1: HIG1 tlb bypass. */
#define HIG1_CFG_MMU_CACHE_MODE			0x018	/* (RW) HIG1 MMU cache bypass. bit0 for cache0, bit1 for cache1. 
								set:0x0: cache enable; 0x1: cache bypass. */

#define HIG1_CFG_ICE_DCACHE_MODE		0x01C
#define HIG1_CFG_ICE_L0_HCACHE_MODE		0x020
#define HIG1_CFG_ICE_L1_HCACHE_MODE		0x024

#define HIG1_CFG_COMMAND			0x030	/* (RW) HIG1 command configurations:0x0: no operation occurs;
								0x1: soft reset;0x2: invalidates cache. */

#define HIG1_DCACHE_CFG				0x038	/* (RW) [3:0]  invalidate_type (0:all invalidate 1:texid invalidate
								2:asid invalidate) */
							/* (RW) [7:4]  texid */
							/* (RW) [11:8] asid  */
#define HIG1_CFG_STATUS              		0x034	/* (RO) HIG1 module status:0x0: HIG1 is free;0x1: HIG1 is active. */

#define HIG1_CFG_TEXDC_LOADED			0x80

#define HIG1_CFG_PIXMAP_DISPATCH_MODE		0xE0    /* (RW) 0x0: hash mode, 0x7: without routing mode */

#define MEMORY_MANAGEMENT_BASE  		0x2000
#define HIG1_MMU_REG(r)              		(HIG1_CFG_CONTROL_BASE + MEMORY_MANAGEMENT_BASE + (r))

#define HIG1_CFG_MMU_IRQ_RAWSTAT      		0x000	/* (RW) Raw interrupt status register */
#define HIG1_CFG_MMU_IRQ_CLEAR          	0x004	/* (WO) Interrupt clear register */
#define HIG1_CFG_MMU_IRQ_MASK           	0x008	/* (RW) Interrupt mask register */
#define HIG1_CFG_MMU_IRQ_STATUS         	0x00C	/* (RO) Interrupt status register */

/*In theory, all the fault should be captured in the front end of the G-MMU and there is nothing we can do if a fault is happening on the HiMMU side, it is only an debug-aidded register somehow*/

#define HIG1_MMU_AS0                 		0x400	/* Configuration registers for address space 0 */
#define HIG1_MMU_AS1                 		0x440	/* Configuration registers for address space 1 */
#define HIG1_MMU_AS2                 		0x480	/* Configuration registers for address space 2 */
#define HIG1_MMU_AS3                 		0x4C0	/* Configuration registers for address space 3 */
#define HIG1_MMU_AS4                 		0x500	/* Configuration registers for address space 4 */
#define HIG1_MMU_AS5                 		0x540	/* Configuration registers for address space 5 */
#define HIG1_MMU_AS6                 		0x580	/* Configuration registers for address space 5 */
#define HIG1_MMU_AS7                 		0x5C0	/* Configuration registers for address space 7 */

#define HIG1_MMU_AS_REG(n, r)        		(HIG1_MMU_REG(HIG1_MMU_AS0 + ((n) << 6)) + (r))

#define HIG1_CFG_MMUAS_TRANSTAB_LO         	0x00	/* (RW) Translation Table Base Address for address space n, low word */
#define HIG1_CFG_MMUAS_TRANSTAB_HI         	0x04	/* (RW) Translation Table Base Address for address space n, high word */
#define HIG1_CFG_MMUAS_COMMAND             	0x18	/* (WO) MMU command register for address space n */
#define HIG1_CFG_MMUAS_STATUS              	0x28	/* (RO) Status flags for address space n */

#define HIG1_ICE_BASE				0x3000
#define HIG1_ICE_REG(r)              		(HIG1_CFG_CONTROL_BASE + HIG1_ICE_BASE + (r))

#define HIG1_ICE_TEXID0				0x400	/* configuration registers for texture slot 0 */
#define HIG1_ICE_TEXID1				0x440	/* configuration registers for texture slot 1 */
#define HIG1_ICE_TEXID2				0x480	/* configuration registers for texture slot 2 */
#define HIG1_ICE_TEXID3				0x4C0	/* configuration registers for texture slot 3 */
#define HIG1_ICE_TEXID4				0x500	/* configuration registers for texture slot 4 */
#define HIG1_ICE_TEXID5				0x540	/* configuration registers for texture slot 5 */
#define HIG1_ICE_TEXID6				0x580	/* configuration registers for texture slot 6 */
#define HIG1_ICE_TEXID7				0x5C0	/* configuration registers for texture slot 7 */

#define HIG1_ICE_TEXID_REG(n, r)		(HIG1_ICE_REG(HIG1_ICE_TEXID0 + ((n) << 6)) + (r))

#define HIG1_ICE_TEXID_CFG_COMMAND		0x0	/* texid command */
#define HIG1_ICE_TEXID_CFG_STATUS		0x4	/* texid status, RO */
#define HIG1_ICE_TEXID_CFG_DESC_ADDRLOW		0x8	/* texture state descriptor base address for TEXIDn,low word */
#define HIG1_ICE_TEXID_CFG_DESC_ADDRHIGH	0xC	/* texture state descriptor base address for TEXIDn,high word */
#define HIG1_ICE_TEXID_CFG_GPUVA_ADDRLOW	0x10	/* texture buffer GPU VA address for TEXIDn,low word */
#define HIG1_ICE_TEXID_CFG_GPUVA_ADDRHIGH	0x14	/* texture buffer GPU VA address for TEXIDn,low word */

#define HIG1_CFG_TEXDC_LOADED_CNT		100000
#define HIG1_CFG_TEXDC_LOADING			0x0

#define HIG1_HW_ISSUE_DCACHE_INVALIDATION

#define HIG1_HW_CACHE_ENABLE			1
#define HIG1_HW_DCACHE_ENABLE			1

enum hig1_tlb_cache_mode {
	HIG1_TLB_CACHE_ENABLE = 0,
	HIG1_TLB_CACHE_DISABLE = 1,
};

enum hig1_mmu_cache_mode {
	HIG1_MMU_CACHE_ENABLE = 0,
	HIG1_MMU_CACHE_DISABLE = 1,
};

enum hig1_ice_dcache_mode {
	HIG1_ICE_DCACHE_ENABLE = 0,
	HIG1_ICE_DCACHE_DISABLE = 1,
	HIG1_ICE_DCACHE_DISABLE_COMPATIBLE = 9,
};

enum hig1_ice_l0_hcache_mode {
	HIG1_ICE_L0_HCACHE_ENABLE = 0,
	HIG1_ICE_L0_HCACHE_DISABLE = 1,
};

enum hig1_ice_l1_hcache_mode {
	HIG1_ICE_L1_HCACHE_ENABLE = 0,
	HIG1_ICE_L1_HCACHE_DISABLE = 1,
};

enum hig1_ice_pixmap_router_mode {
	HIG1_ICE_HASH_MODE = 0x0,
	HIG1_ICE_WITHOUT_ROUTING_MODE = 0x7,
};

enum hig1_command {
	HIG1_CMD_NOP = 0,
	HIG1_CMD_SOFT_RESET = 1,
	HIG1_CMD_TLB_CACHE_INVALIDATE = 2,
	HIG1_CMD_MMU_CACHE_INVALIDATE = 3,
	HIG1_CMD_RESERVED = 4,
	HIG1_CMD_ICE_DCACHE_INVALIDATE = 5,
	HIG1_CMD_ICE_L0_HCACHE_INVALIDATE = 6,
	HIG1_CMD_TIMER_RESTART = 7,
	HIG1_CMD_TIMER_ENABLE = 8,
	HIG1_CMD_TIMER_DISABLE = 9,
	HIG1_CMD_HARD_RESET = 0xa,
	HIG1_CMD_ICE_DESC_LOAD_STATUS_CLEAN = 0xb,
	HIG1_CMD_ICE_L1_HCACHE_INVALIDATE = 0xc,
	HIG1_CMD_TLB_CACHE_INVALIDATE_STATUS_CLEAN = 0xd,
	HIG1_CMD_MMU_CACHE_INVALIDATE_STATUS_CLEAN = 0xe,
	HIG1_CMD_ICE_DCACHE_INVALIDATE_STATUS_CLEAN = 0xf,
	HIG1_CMD_ICE_L0_HCACHE_INVALIDATE_STATUS_CLEAN = 0x10,
	HIG1_CMD_ICE_L1_HCACHE_INVALIDATE_STATUS_CLEAN = 0x11,
};

enum hig1_texid_cmd {
	HIG1_TEXID_CMD_NOP = 0,
	HIG1_TEXID_CMD_LOAD = 1,
	HIG1_TEXID_CMD_INVALIDATE = 2,
};

enum hig1_mmu_as_cmd {
	HIG1_MMU_AS_CMD_NOP = 0,
	HIG1_MMU_AS_CMD_UPDATE = 1,		/* Broadcasts the values in AS_TRANSTAB into the H-MMU */
	HIG1_MMU_AS_CMD_FLUSH_PT = 4,	/* Flush all L2 caches then issue a flush region command to all MMUs */
};

enum hig1_ice_dcache_invalidate_mode {
	HIG1_ICE_DCACHE_INVALIDATE_ALL = 0,
	HIG1_ICE_DCACHE_INVALIDATE_BY_TEXID = 1,
	HIG1_ICE_DCACHE_INVALIDATE_BY_ASID = 2,
};

static int kbase_ice_desc_load_clean(struct kbase_device *kbdev)
{
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_COMMAND), HIG1_CMD_ICE_DESC_LOAD_STATUS_CLEAN);
#ifdef CONFIG_HIG1_DEBUG
	printk("HIG1_DBG:load status = %d %s\n", kbase_reg_read(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_TEXDC_LOADED)),
	       __func__);
#endif
	return 0;
}

static int kbase_ice_desc_load_waitready(struct kbase_device *kbdev)
{
	unsigned int max_loops = HIG1_CFG_TEXDC_LOADED_CNT;
	unsigned int loops = 0;
	u32 val = kbase_reg_read(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_TEXDC_LOADED));

	/* Wait for the MMU status to indicate there is no active command, in
	 * case one is pending. Do not log remaining register accesses. */
	while (--max_loops && (val == HIG1_CFG_TEXDC_LOADING)) {
		val = kbase_reg_read(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_TEXDC_LOADED));
		loops++;
	}

	if (max_loops == 0) {
		printk("HIG1_DBG:%s failed\n", __func__);
		return -1;
	}

	/* If waiting in loop was performed, log last read value. */
	if (HIG1_CFG_TEXDC_LOADED_CNT - 1 > max_loops)
		kbase_reg_read(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_TEXDC_LOADED));

	return 0;
}

static int kbase_hmmu_cfg_write_cmd(struct kbase_device *kbdev, int as_nr, u32 cmd)
{
	kbase_reg_write(kbdev, HIG1_MMU_AS_REG(as_nr, HIG1_CFG_MMUAS_COMMAND), cmd);
	return 0;
}

#ifdef CONFIG_G52_WARP
#define G52_WARP_REG   			0xf9200f10
#define G52_STANDARD_WARP_NUM	96
#define G52_SMALLCORE_WARP_NUM	64
static void kbase_hmmu_setwarp(struct kbase_device *kbdev, int warp_depth)
{
	struct gpu_raw_gpu_props *raw_props = &kbdev->gpu_props.props.raw_props;

	switch (warp_depth) {
	case G52_STANDARD_WARP_NUM:		//default
		writel(0x0, IO_ADDRESS(G52_WARP_REG));
		raw_props->thread_max_threads = 0x300;
		raw_props->thread_tls_alloc = 0x400;
		raw_props->thread_max_workgroup_size = 0x180;
		raw_props->thread_max_barrier_size = 0x180;
		raw_props->thread_features = 0x0a046000;
		break;

	case G52_SMALLCORE_WARP_NUM:
		writel(0x1, IO_ADDRESS(G52_WARP_REG));;
		raw_props->thread_max_threads = 0x200;
		raw_props->thread_tls_alloc = 0x400;
		raw_props->thread_max_workgroup_size = 0x100;
		raw_props->thread_max_barrier_size = 0x100;
		raw_props->thread_features = 0x0a044000;
		break;

	default:
		// no change
		break;
	}

	return;
}
#endif

void kbase_hmmu_cache_invalidate(struct kbase_device *kbdev, int as_nr)
{
#if HIG1_HW_DCACHE_ENABLE
	u32 dcache_cfg = (as_nr << 8) | HIG1_ICE_DCACHE_INVALIDATE_BY_ASID;
#endif

#if !HIG1_HW_CACHE_ENABLE
	return;
#endif

#ifdef HIG1_HW_ISSUE_DCACHE_INVALIDATION
	/* bypass data cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_DCACHE_MODE), HIG1_ICE_DCACHE_DISABLE_COMPATIBLE);
#endif
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_COMMAND), HIG1_CMD_ICE_L0_HCACHE_INVALIDATE);
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_COMMAND), HIG1_CMD_ICE_L1_HCACHE_INVALIDATE);

#if HIG1_HW_DCACHE_ENABLE
#ifdef HIG1_HW_ISSUE_DCACHE_INVALIDATION
	udelay(100);
#endif
	/* invalidate dcache by asid */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_DCACHE_CFG), dcache_cfg);
	/* invalidate dcache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_COMMAND), HIG1_CMD_ICE_DCACHE_INVALIDATE);
#ifdef HIG1_HW_ISSUE_DCACHE_INVALIDATION
	/* enable data cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_DCACHE_MODE), HIG1_ICE_DCACHE_ENABLE);
#endif
#endif
}

void kbase_hmmu_reset(struct kbase_device *kbdev, int hard)
{
	int i = 0;
	struct kbase_context *kctx;

#ifdef CONFIG_HIG1_DEBUG
	printk("HIG1_DBG:%s reset\n", hard ? "hard" : "soft");
#endif

	/* soft reset HIG1 */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_COMMAND),
			hard ? HIG1_CMD_HARD_RESET : HIG1_CMD_SOFT_RESET);

#if HIG1_HW_CACHE_ENABLE
#if HIG1_HW_DCACHE_ENABLE
	/* enable data cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_DCACHE_MODE), HIG1_ICE_DCACHE_ENABLE);
#else
	/* bypass data cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_DCACHE_MODE), HIG1_ICE_DCACHE_DISABLE_COMPATIBLE);
#endif

	/* enable mmu tlb in b1 version */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_TLB_CACHE_MODE), HIG1_TLB_CACHE_ENABLE);

	/* enable mmu cache in b1 version */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_MMU_CACHE_MODE), HIG1_MMU_CACHE_ENABLE);

	/* enable mmu header cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_L0_HCACHE_MODE), HIG1_ICE_L0_HCACHE_ENABLE);

	/* enable biu header cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_L1_HCACHE_MODE), HIG1_ICE_L1_HCACHE_ENABLE);
#else
	/* bypass data cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_DCACHE_MODE), HIG1_ICE_DCACHE_DISABLE_COMPATIBLE);

	/* bypass mmu tlb in b1 version */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_TLB_CACHE_MODE), HIG1_TLB_CACHE_DISABLE);

	/* bypass mmu cache in b1 version */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_MMU_CACHE_MODE), HIG1_MMU_CACHE_DISABLE);

	/* bypass header cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_L0_HCACHE_MODE), HIG1_ICE_L0_HCACHE_DISABLE);

	/* bypass biu header cache */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_ICE_L1_HCACHE_MODE), HIG1_ICE_L1_HCACHE_DISABLE);

#endif

	/* reset the ice_desc_changed flag for every context so that we can reload ice descriptor after reset */
	for (i = 0; i < BASE_MAX_NR_AS; i++) {
		kctx = kbdev->as_to_kctx[i];
		if(kctx != NULL) {
			kctx->mmu.hmmu.ice_desc_changed = kctx->mmu.hmmu.ice_desc_mask ? true : false;
		}
	}
#ifdef CONFIG_HIG1_WARP
	kbase_hmmu_setwarp(kbdev, 0);
#endif

	/* hig1 pixmap dispatch mode */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_PIXMAP_DISPATCH_MODE), HIG1_ICE_WITHOUT_ROUTING_MODE);
}

void kbase_hmmu_hw_configure(struct kbase_device *kbdev, int as_nr, phys_addr_t pgd)
{
	/* TLB and MMU cache should be invalidated simutaneously when the GMMU page update */
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_COMMAND), HIG1_CMD_TLB_CACHE_INVALIDATE);
	kbase_reg_write(kbdev, HIG1_CFG_CONTROL_REG(HIG1_CFG_COMMAND), HIG1_CMD_MMU_CACHE_INVALIDATE);

	/* write as address */
	kbase_reg_write(kbdev, HIG1_MMU_AS_REG(as_nr, HIG1_CFG_MMUAS_TRANSTAB_LO), pgd);

	kbase_reg_write(kbdev, HIG1_MMU_AS_REG(as_nr, HIG1_CFG_MMUAS_TRANSTAB_HI), (((u64) pgd) >> 32));

	kbase_hmmu_cfg_write_cmd(kbdev, as_nr, HIG1_MMU_AS_CMD_UPDATE);
}

void kbase_hmmu_update_ice_desc(struct kbase_device *kbdev, u32 valid, u32 desc_num, u64 desc_addr, u64 gpu_va)
{
	lockdep_assert_held(&kbdev->hwaccess_lock);
	/* configure the GPU Virtual Address for texture N,  hardware will fetch the asid from gpu_va and invalid the texture id which matches with the asid */
	kbase_reg_write(kbdev, HIG1_ICE_TEXID_REG(desc_num, HIG1_ICE_TEXID_CFG_GPUVA_ADDRLOW), gpu_va);
	kbase_reg_write(kbdev, HIG1_ICE_TEXID_REG(desc_num, HIG1_ICE_TEXID_CFG_GPUVA_ADDRHIGH), gpu_va >> 32);

	if (valid) {
		kbase_ice_desc_load_clean(kbdev);
		/* configure the descriptor physical address for texture N  */
		kbase_reg_write(kbdev, HIG1_ICE_TEXID_REG(desc_num, HIG1_ICE_TEXID_CFG_DESC_ADDRLOW), desc_addr);
		kbase_reg_write(kbdev, HIG1_ICE_TEXID_REG(desc_num, HIG1_ICE_TEXID_CFG_DESC_ADDRHIGH), desc_addr >> 32);

		/* load the descriptor */
		kbase_reg_write(kbdev, HIG1_ICE_TEXID_REG(desc_num, HIG1_ICE_TEXID_CFG_COMMAND), HIG1_TEXID_CMD_LOAD);

		kbase_ice_desc_load_waitready(kbdev);
	} else {
		/* invalid the descriptor */
		kbase_reg_write(kbdev, HIG1_ICE_TEXID_REG(desc_num, HIG1_ICE_TEXID_CFG_COMMAND), HIG1_TEXID_CMD_INVALIDATE);
	}
}
