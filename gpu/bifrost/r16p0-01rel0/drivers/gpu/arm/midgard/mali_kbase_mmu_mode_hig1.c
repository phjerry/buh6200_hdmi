/*
 *
 * (C) COPYRIGHT 2010-2016 Hisilicon Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "mali_kbase.h"
#include "mali_midg_regmap.h"
#include "mali_kbase_hmmu_hw.h"

#define ENTRY_TYPE_MASK     3ULL
#define ENTRY_IS_ATE        1ULL
#define ENTRY_IS_INVAL      2ULL
#define ENTRY_IS_PTE        3ULL

#define ENTRY_ATTR_BITS (7ULL << 2)	/* bits 4:2 */
#define ENTRY_RD_BIT (1ULL << 6)
#define ENTRY_WR_BIT (1ULL << 7)
#define ENTRY_SHARE_BITS (3ULL << 8)	/* bits 9:8 */
#define ENTRY_ACCESS_BIT (1ULL << 10)
#define ENTRY_NX_BIT (1ULL << 54)

#define ENTRY_FLAGS_MASK (ENTRY_ATTR_BITS | ENTRY_RD_BIT | ENTRY_WR_BIT | \
		ENTRY_SHARE_BITS | ENTRY_ACCESS_BIT | ENTRY_NX_BIT)

#define KBASE_MMU_PAGE_ENTRIES 512

#if defined(CONFIG_HIG1_TILE_64X4_V0)	/* 64x4 v1r0 */
#define GMMU_C			(1ULL << 38)
#define GMMU_S_SHIFT	39
#define GMMU_ASID(asid) (((u64)(asid)) << 35)
#define GMMU_ADDR_MASK	0x07FFFFFFFFULL
#define GMMU_ASID_MASK	0x3800000000ULL
#elif defined(CONFIG_HIG1_TILE_64X4_V1)	/* 64x4 v1r2 */
#define GMMU_C			(1ULL << 39)
#define GMMU_S_SHIFT	35
#define GMMU_ASID(asid) (((u64)(asid)) << 36)
#define GMMU_ADDR_MASK	0x07FFFFFFFFULL
#define GMMU_ASID_MASK	0x7000000000ULL
#else /* 64x2 */
#define GMMU_C			(1ULL << 39)
#define GMMU_ASID(asid) (((u64)(asid)) << 36)
#define GMMU_ADDR_MASK	0x0FFFFFFFFFULL
#define GMMU_ASID_MASK	0x7000000000ULL
#define GMMU_S_SHIFT	35
#endif

/* Max support 24 HiG1 textures per context, limmit to 24 for loop performance consideration
*  Caution: HIG1_MAX_TEXTURE_PER_CONTEXT * sizeof(struct eglp_hiice_desc) should less than 4096 */
#define HIG1_MAX_TEXTURE_PER_CONTEXT	24

#define DESC_ALIGN_64(x) (((x + 63) >> 6) << 6)

/* Helper Function to perform assignment of page table entries, to
 * ensure the use of strd, which is required on LPAE systems.
 */
static inline void page_table_entry_set(u64 * pte, u64 phy)
{
#if KERNEL_VERSION(3, 18, 13) <= LINUX_VERSION_CODE
	WRITE_ONCE(*pte, phy);
#else
#ifdef CONFIG_64BIT
	barrier();
	*pte = phy;
	barrier();
#elif defined(CONFIG_ARM)
	barrier();
	asm volatile("ldrd r0, [%1]\n\t"
		     "strd r0, %0\n\t"
		     : "=m" (*pte)
		     : "r" (&phy)
		     : "r0", "r1");
	barrier();
#else
#error "64-bit atomic write must be implemented for your architecture"
#endif
#endif
}

static void mmu_get_as_setup(struct kbase_mmu_table *mmut, struct kbase_mmu_setup *const setup)
{
	/* Set up the required caching policies at the correct indices
	 * in the memattr register. */
	setup->memattr =
		(AS_MEMATTR_LPAE_IMPL_DEF_CACHE_POLICY <<
		(AS_MEMATTR_INDEX_IMPL_DEF_CACHE_POLICY * 8)) |
		(AS_MEMATTR_LPAE_FORCE_TO_CACHE_ALL    <<
		(AS_MEMATTR_INDEX_FORCE_TO_CACHE_ALL * 8))    |
		(AS_MEMATTR_LPAE_WRITE_ALLOC           <<
		(AS_MEMATTR_INDEX_WRITE_ALLOC * 8))           |
		(AS_MEMATTR_LPAE_OUTER_IMPL_DEF        <<
		(AS_MEMATTR_INDEX_OUTER_IMPL_DEF * 8))        |
		(AS_MEMATTR_LPAE_OUTER_WA              <<
		(AS_MEMATTR_INDEX_OUTER_WA * 8))              |
		0; /* The other indices are unused for now */

	setup->transtab = ((u64) mmut->pgd &
			   ((0xFFFFFFFFULL << 32) |
			    AS_TRANSTAB_LPAE_ADDR_SPACE_MASK)) |
	    AS_TRANSTAB_LPAE_ADRMODE_TABLE | AS_TRANSTAB_LPAE_READ_INNER;

	setup->transcfg = 0;

}

#ifdef CONFIG_HIG1_SECURE_SIM
#include <backend/gpu/mali_kbase_device_internal.h>

static void hig1_smmu_setup(struct kbase_device *kbdev, struct kbase_mmu_table *mmut)
{
	/* configure the smmu pgd */
	printk("HIG1_SEC:config smmu pgd:0x%x\n", (u32) mmut->hmmu.pgd_smmu);
	kbase_reg_write(kbdev, 0xF000, 0);
	kbase_reg_write(kbdev, 0xF100, mmut->hmmu.pgd_smmu);
	kbase_reg_write(kbdev, 0xF104, 0);
	kbase_reg_write(kbdev, 0xF110, 1);
	kbase_reg_write(kbdev, 0xF114, 0xffffffff);
	kbase_reg_write(kbdev, 0xF120, 0x1);
	kbase_reg_write(kbdev, 0xF124, 0xffffffff);
	kbase_reg_write(kbdev, 0xF118, 0xffffffff);
	kbase_reg_write(kbdev, 0xF128, 0xffffffff);

	return;
}
#endif

#if HIG1_DDK_TEST==0

/* Get the hig1 entry in GMMU */
static u64 mmu_get_L1_31_L2_0_pgd(struct kbase_mmu_table *mmut)
{
	u64 *pgd_page;
	u64 target_pgd;
	struct page *p;
	struct kbase_mmu_mode const *mmu_mode = mmut->kctx->kbdev->mmu_mode;

	p = pfn_to_page(PFN_DOWN(mmut->pgd));
	pgd_page = kmap_atomic(p);
	if (NULL == pgd_page) {
		dev_warn(mmut->kctx->kbdev->dev, "mmu_get_l0_31_l1_511_pgd: kmap_atomic failure\n");
		return 0;
	}

	target_pgd = mmu_mode->pte_to_phy_addr(pgd_page[0]);
	kunmap_atomic(pgd_page);

	if (0 == target_pgd) {
		return 0;
	}

	p = pfn_to_page(PFN_DOWN(target_pgd));
	pgd_page = kmap_atomic(p);
	if (NULL == pgd_page) {
		dev_warn(mmut->kctx->kbdev->dev, "mmu_get_l0_31_l1_511_pgd: kmap_atomic failure\n");
		return 0;
	}

	target_pgd = mmu_mode->pte_to_phy_addr(pgd_page[31]);
	kunmap_atomic(pgd_page);

	return target_pgd;
}
#endif

/*
*************************************************************************
****************|  39   |38:36|35:12 |11:0    |******************************
****************|VFLAG|ASID |PAGE | OFFSET|******************************
*************************************************************************
*/
int gmmu_update_asid(struct kbase_device *kbdev, struct kbase_mmu_table *mmut, int as_nr)
{
	u64 *pgd_page;
	u64 *pgd_page_bottom;
	u64 target_pgd;
	struct page *p;
	struct page *p_bottom;
	struct kbase_mmu_mode const *mmu_mode;
	int i, j;
	u64 asid = GMMU_ASID(as_nr);
#if HIG1_DDK_TEST
	u64 pgd_entry_hig1 = mmut->pgd_hmmu;
#else
	u64 pgd_entry_hig1 = mmu_get_L1_31_L2_0_pgd(mmut);
#endif

	if (0 == pgd_entry_hig1) {
		return -1;
	}

	p = pfn_to_page(PFN_DOWN(pgd_entry_hig1));
	pgd_page = kmap_atomic(p);
	if (NULL == pgd_page) {
		dev_warn(kbdev->dev, "mmu_update_flag_as: kmap_atomic failure\n");
		return -EINVAL;
	}

	mmu_mode = kbdev->mmu_mode;

	for (i = 0; i < KBASE_MMU_PAGE_ENTRIES; i++) {
		target_pgd = mmu_mode->pte_to_phy_addr(pgd_page[i]);

		if (target_pgd) {
			p_bottom = pfn_to_page(PFN_DOWN(target_pgd));
			pgd_page_bottom = kmap_atomic(p_bottom);

			for (j = 0; j < KBASE_MMU_PAGE_ENTRIES; j++) {
				/* only 40bit valid */
				pgd_page_bottom[j] = ((pgd_page_bottom[j] & (~GMMU_ASID_MASK)) | asid);
			}

			/* sync pages */
			if (kbdev->system_coherency != COHERENCY_ACE)
				dma_sync_single_for_device(kbdev->dev,
							kbase_dma_addr(p_bottom),
							KBASE_MMU_PAGE_ENTRIES * sizeof(u64), DMA_TO_DEVICE);

			kunmap_atomic(pgd_page_bottom);
		}
	}

	kunmap_atomic(pgd_page);

	return 0;
}

static void mmu_update(struct kbase_device *kbdev, struct kbase_mmu_table *mmut, int as_nr)
{
	struct kbase_as *as;
	struct kbase_mmu_setup *current_setup;

	if (WARN_ON(as_nr == KBASEP_AS_NR_INVALID))
		return;

	as = &kbdev->as[as_nr];
	current_setup = &as->current_setup;

#ifdef CONFIG_HIG1_SECURE_SIM
	if (kbdev->protected_mode) {
		hig1_smmu_setup(kbdev, mmut);
	}
#endif

	kbase_hmmu_setup(kbdev, mmut, as_nr);

#ifdef CONFIG_HIG1_DEBUG
	printk("HIG1_DBG: mmut=%p, as_nr=%d, ice_desc_mask=0x%x, asid_changed = %d\n ", mmut, as_nr,
	       mmut->hmmu.ice_desc_mask, mmut->hmmu.asid_changed);
#endif

	mmu_get_as_setup(mmut, current_setup);

	/* Apply the address space setting */
	kbase_mmu_hw_configure(kbdev, as);
}

static void mmu_disable_as(struct kbase_device *kbdev, int as_nr)
{
	struct kbase_as *const as = &kbdev->as[as_nr];
	struct kbase_mmu_setup *const current_setup = &as->current_setup;

	current_setup->transtab = AS_TRANSTAB_LPAE_ADRMODE_UNMAPPED;

	/* Apply the address space setting */
	kbase_mmu_hw_configure(kbdev, as);
}

static phys_addr_t pte_to_phy_addr(u64 entry)
{
	if (!(entry & 1))
		return 0;

	return entry & ~0xFFF;
}

static int ate_is_valid(u64 ate, unsigned int level)
{
	return ((ate & ENTRY_TYPE_MASK) == ENTRY_IS_ATE);
}

static int pte_is_valid(u64 pte, unsigned int level)
{
	return ((pte & ENTRY_TYPE_MASK) == ENTRY_IS_PTE);
}

/*
 * Map KBASE_REG flags to MMU flags
 */
static u64 get_mmu_flags(unsigned long flags)
{
	u64 mmu_flags;
	unsigned long memattr_idx;

	memattr_idx = KBASE_REG_MEMATTR_VALUE(flags);
	if (WARN(memattr_idx == AS_MEMATTR_INDEX_NON_CACHEABLE,
		 "Legacy Mode MMU cannot honor GPU non-cachable memory, will use default instead\n"))
		memattr_idx = AS_MEMATTR_INDEX_DEFAULT;
	/* store mem_attr index as 4:2, noting that:
	 * - macro called above ensures 3 bits already
	 * - all AS_MEMATTR_INDEX_<...> macros only use 3 bits
	 */
	mmu_flags = memattr_idx << 2;

	/* write perm if requested */
	mmu_flags |= (flags & KBASE_REG_GPU_WR) ? ENTRY_WR_BIT : 0;
	/* read perm if requested */
	mmu_flags |= (flags & KBASE_REG_GPU_RD) ? ENTRY_RD_BIT : 0;
	/* nx if requested */
	mmu_flags |= (flags & KBASE_REG_GPU_NX) ? ENTRY_NX_BIT : 0;

	if (flags & KBASE_REG_SHARE_BOTH) {
		/* inner and outer shareable */
		mmu_flags |= SHARE_BOTH_BITS;
	} else if (flags & KBASE_REG_SHARE_IN) {
		/* inner shareable coherency */
		mmu_flags |= SHARE_INNER_BITS;
	}

	return mmu_flags;
}

static void entry_set_ate(u64 * entry, struct tagged_addr phy, unsigned long flags, unsigned int level)
{
	page_table_entry_set(entry, as_phys_addr_t(phy) | get_mmu_flags(flags) | ENTRY_IS_ATE);
}

static void entry_set_pte(u64 * entry, phys_addr_t phy)
{
	page_table_entry_set(entry, (phy & ~0xFFF) | ENTRY_IS_PTE);
}

static void entry_invalidate(u64 * entry)
{
	page_table_entry_set(entry, ENTRY_IS_INVAL);
}

static struct kbase_mmu_mode const hig1_mode = {
	.update = mmu_update,
	.get_as_setup = mmu_get_as_setup,
	.disable_as = mmu_disable_as,
	.pte_to_phy_addr = pte_to_phy_addr,
	.ate_is_valid = ate_is_valid,
	.pte_is_valid = pte_is_valid,
	.entry_set_ate = entry_set_ate,
	.entry_set_pte = entry_set_pte,
	.entry_invalidate = entry_invalidate,
	.flags = 0
};

struct kbase_mmu_mode const *kbase_mmu_mode_get_hig1(void)
{
	return &hig1_mode;
}

static void kbase_ice_desc_update_asid(struct kbase_device *kbdev, struct kbase_hmmu_table *hmmu, int as_nr)
{
	struct eglp_hiice_desc *ice_desc;
	int i;
	int struct_size_align64 = DESC_ALIGN_64(sizeof(struct eglp_hiice_desc));
	u64 c_asid = (GMMU_C | GMMU_ASID(as_nr));

	for (i = 0; i < HIG1_MAX_TEXTURE_PER_CONTEXT; i++) {
		ice_desc = (struct eglp_hiice_desc *)(hmmu->virt_ice_desc + i * struct_size_align64);
		ice_desc->asid_s.bits.asid = as_nr & 0x7;

		if (test_bit(i, (unsigned long *)&hmmu->ice_desc_mask)) {

			/* update asid and vflag */
			ice_desc->luma_header_addr =
			    (ice_desc->luma_header_addr & GMMU_ADDR_MASK) | c_asid
			    | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
			ice_desc->luma_data_addr =
			    (ice_desc->luma_data_addr & GMMU_ADDR_MASK) | c_asid
			    | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
			ice_desc->chroma_header_addr =
			    (ice_desc->chroma_header_addr & GMMU_ADDR_MASK) |
			    c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
			ice_desc->chroma_data_addr =
			    (ice_desc->chroma_data_addr & GMMU_ADDR_MASK) |
			    c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
			ice_desc->luma_data2_addr =
			    (ice_desc->luma_data2_addr & GMMU_ADDR_MASK) |
			    c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
			ice_desc->chroma_data2_addr =
			    (ice_desc->chroma_data2_addr & GMMU_ADDR_MASK) |
			    c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
			ice_desc->chroma_judge_base =
			    (ice_desc->chroma_judge_base & GMMU_ADDR_MASK) |
			    c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);

			if (kbdev->system_coherency != COHERENCY_ACE) {
				dma_sync_single_for_device(kbdev->dev,
							   kbase_dma_addr(hmmu->page_ice_desc), 4096, DMA_TO_DEVICE);
			}

		}
	}

	hmmu->ice_desc_changed = true;
}

void kbase_mmu_mode_update_asid(struct kbase_device *kbdev, struct kbase_mmu_table *mmut, int as_nr)
{
	if (mmut->hmmu.ice_desc_mask) {
		kbase_ice_desc_update_asid(kbdev, &mmut->hmmu, as_nr);
	}
	mmut->hmmu.asid_changed = true;
#ifdef CONFIG_HIG1_DEBUG
	printk("HIG1_DBG:asid_changed, mmut=%p!\n", mmut);
#endif
}

static int kbase_ice_desc_alloc(u32 *ice_desc_hw_mask)
{
	unsigned long idx = find_first_zero_bit((const unsigned long*)ice_desc_hw_mask, HIG1_MAX_TEXTURE_PER_HW_ASID);

	if (idx == HIG1_MAX_TEXTURE_PER_HW_ASID) {
		printk("HIG1_DBG:No free texture slot left!\n");
		return -1;
	}

	__set_bit(idx, (unsigned long *)ice_desc_hw_mask);

	return idx;
}

void kbase_ice_desc_free(struct kbase_hmmu_table *hmmu, u64 jc)
{
	int i;

	if (hmmu->ice_desc_hw_mask == 0)
		return ;

	for (i = 0; i < HIG1_MAX_TEXTURE_PER_HW_ASID; i++) {
		if (jc == hmmu->ice_desc_slot[i].jc) {
			__clear_bit(i, (unsigned long *)&hmmu->ice_desc_hw_mask);
			hmmu->ice_desc_slot[i].jc = 0;
		}
	}

	return ;
}

int kbase_ice_desc_update(struct kbase_context *kctx, u32 valid, u32 secure, struct eglp_hiice_desc *descriptor)
{
	struct eglp_hiice_desc *ice_desc;
	int struct_size_align64 = DESC_ALIGN_64(sizeof(struct eglp_hiice_desc));
	int i;
	int as_nr = kctx->as_nr;
	u64 c_asid = (GMMU_C | GMMU_ASID(as_nr));

	kctx->mmu.hmmu.ice_desc_changed = true;

	if (valid) {
		/* find a free slot, and insert the descriptor */
		for (i = 0; i < HIG1_MAX_TEXTURE_PER_CONTEXT; i++) {
			if (0 == ((kctx->mmu.hmmu.ice_desc_mask >> i) & 0x1)) {
				ice_desc = (struct eglp_hiice_desc *)(kctx->mmu.hmmu.virt_ice_desc + i * struct_size_align64);

				__set_bit(i, (unsigned long *)&kctx->mmu.hmmu.ice_desc_mask);
				memcpy(ice_desc, descriptor, sizeof(*descriptor));
				ice_desc->asid_s.bits.secure = secure ? 1 : 0;
				ice_desc->asid_s.bits.asid = as_nr;

				/* update asid and vflag */
				ice_desc->luma_header_addr = (ice_desc->luma_header_addr & GMMU_ADDR_MASK)
				    | c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
				ice_desc->luma_data_addr = (ice_desc->luma_data_addr & GMMU_ADDR_MASK)
				    | c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
				ice_desc->chroma_header_addr =
				    (ice_desc->chroma_header_addr &
				     GMMU_ADDR_MASK) | c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
				ice_desc->chroma_data_addr =
				    (ice_desc->chroma_data_addr &
				     GMMU_ADDR_MASK) | c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
				ice_desc->luma_data2_addr = (ice_desc->luma_data2_addr & GMMU_ADDR_MASK)
				    | c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
				ice_desc->chroma_data2_addr =
				    (ice_desc->chroma_data2_addr &
				     GMMU_ADDR_MASK) | c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);
				ice_desc->chroma_judge_base =
				    (ice_desc->chroma_judge_base &
				     GMMU_ADDR_MASK) | c_asid | ((u64) ice_desc->asid_s.bits.secure << GMMU_S_SHIFT);

				if (kctx->kbdev->system_coherency != COHERENCY_ACE) {
					dma_sync_single_for_device(kctx->kbdev->dev,
								   kbase_dma_addr(kctx->mmu.hmmu.page_ice_desc), 4096,
								   DMA_TO_DEVICE);
				}
#ifdef CONFIG_HIG1_DEBUG
				printk("HIG1_DBG:%s, mmut=%p, ice_desc_mask = 0x%x, valid = %d!\n", __func__,
				       &kctx->mmu, kctx->mmu.hmmu.ice_desc_mask, valid);
#endif

				return 0;
			}
		}
	} else {
		/* find the slot, and invalid it */
		for (i = 0; i < HIG1_MAX_TEXTURE_PER_CONTEXT; i++) {
			ice_desc = (struct eglp_hiice_desc *)(kctx->mmu.hmmu.virt_ice_desc + i * struct_size_align64);
			if (((kctx->mmu.hmmu.ice_desc_mask >> i) & 0x1) && (secure == ice_desc->asid_s.bits.secure) &&
			    ((ice_desc->luma_header_addr & GMMU_ADDR_MASK) ==
			     (descriptor->luma_header_addr & GMMU_ADDR_MASK))) {
				__clear_bit(i, (unsigned long *)&kctx->mmu.hmmu.ice_desc_mask);
				memset(ice_desc, 0, sizeof(*ice_desc));
				if (kctx->kbdev->system_coherency != COHERENCY_ACE) {
					dma_sync_single_for_device(kctx->kbdev->dev,
								   kbase_dma_addr(kctx->mmu.hmmu.page_ice_desc), 4096,
								   DMA_TO_DEVICE);
				}

#ifdef CONFIG_HIG1_DEBUG
				printk("HIG1_DBG:%s, mmut=%p, ice_desc_mask = 0x%x, valid = %d!\n", __func__,
				       &kctx->mmu, kctx->mmu.hmmu.ice_desc_mask, valid);
#endif

				return 0;
			}
		}
	}

	/* should not reach here */
	return -1;
}

int kbase_ice_desc_setup(struct kbase_device *kbdev, struct kbase_hmmu_table *hmmu, int as_nr)
{
	struct eglp_hiice_desc *ice_desc;
	int i;
	int struct_size_align64 = DESC_ALIGN_64(sizeof(struct eglp_hiice_desc));
	if (!hmmu->ice_desc_changed || (hmmu->ice_desc_hw_mask == 0)) {
		/* there are two conditions. One is the ice_desc has been configure before
		*  the other is the job has not been submitted so that the drawcall mask is 0 */
		return 0;
	}

	for (i = 0; i < HIG1_MAX_TEXTURE_PER_HW_ASID; i++) {
		if (test_bit(i, (unsigned long *)&hmmu->ice_desc_hw_mask)) {
			ice_desc = (struct eglp_hiice_desc *)(hmmu->virt_ice_desc + hmmu->ice_desc_slot[i].idx * struct_size_align64);

#ifdef CONFIG_HIG1_DEBUG
			printk("HIG1_DBG: Valid tex:%d in mask index %d, luma_header_addr=0x%llx\n",
				hmmu->ice_desc_slot[i].idx, i, ice_desc->luma_header_addr);
#endif
			/* Caution: GPU VA only 40bit valid */
			kbase_hmmu_update_ice_desc(kbdev, true, i,
						   hmmu->phy_ice_desc + struct_size_align64 * hmmu->ice_desc_slot[i].idx,
						   ice_desc->luma_header_addr);
		} else {
#ifdef CONFIG_HIG1_DEBUG
			printk("HIG1_DBG: InValid tex:%d\n", i);
#endif
			/* invalid texure id i descriptor in as as_nr, if we do not do this, the texture maybe stay valid */
			kbase_hmmu_update_ice_desc(kbdev, false, i, 0, GMMU_ASID(as_nr));
		}
	}

	hmmu->ice_desc_changed = false;

	kbase_hmmu_cache_invalidate(kbdev, as_nr);

	return 0;
}

int kbase_ice_desc_update_by_job(struct kbase_hmmu_table *hmmu, u16 nr_extres, struct kbase_ext_res *extres, u64 jc)
{
	int i = 0;
	int j = 0;
	int tex_id;
	struct eglp_hiice_desc *ice_desc = NULL;
	int struct_size_align64 = DESC_ALIGN_64(sizeof(struct eglp_hiice_desc));

	if (nr_extres <= 1) {
		return 0;
	}

	/* find out which texture descriptor index each external resource loacated at */
	for (i = 0; i < nr_extres; i++) {
		for (j = 0; j < HIG1_MAX_TEXTURE_PER_CONTEXT; j++) {
			if (!test_bit(j, (unsigned long *)&hmmu->ice_desc_mask))
				continue;

			ice_desc = (struct eglp_hiice_desc *)(hmmu->virt_ice_desc + j * struct_size_align64);

#ifdef CONFIG_HIG1_DEBUG
			printk("HIG1_DBG: %d luma_header_addr=%llx, extres[%d]=%llx\n",
				j, ice_desc->luma_header_addr, i, extres[i].gpu_address);
#endif

			if ((ice_desc->gpu_address & GMMU_ADDR_MASK) != (extres[i].gpu_address & GMMU_ADDR_MASK))
				continue;

			tex_id = kbase_ice_desc_alloc(&hmmu->ice_desc_hw_mask);
			if (tex_id < 0)
				return -1;

			hmmu->ice_desc_slot[tex_id].idx = j;
			hmmu->ice_desc_slot[tex_id].jc = jc;
			hmmu->ice_desc_changed = true; /* reset the change flag */

			break;
		}
	}

#ifdef CONFIG_HIG1_DEBUG
	if (hmmu->ice_desc_changed) {
		printk("HIG1_DBG:%s, ice_desc_mask =0x%x, ice_desc_hw_mask=0x%x!\n", __func__,
							hmmu->ice_desc_mask, hmmu->ice_desc_hw_mask);
	}
#endif
	return 0;
}


void kbase_hmmu_setup(struct kbase_device *kbdev, struct kbase_mmu_table *mmut, int as_nr)
{
	if (mmut->hmmu.ice_desc_mask) {
		kbase_ice_desc_setup(kbdev, &mmut->hmmu, as_nr);
		kbase_hmmu_hw_configure(kbdev, as_nr, mmut->hmmu.pgd_hmmu);
	} else if (kbdev->protected_mode) {
		/* protected texture without compression */
		kbase_hmmu_hw_configure(kbdev, as_nr, mmut->hmmu.pgd_hmmu);
	}

	return;
}

#if 0
static struct file *mmu_savefile_open(char *filename)
{
	struct file *file_filp = NULL;

	file_filp = filp_open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
	if (IS_ERR(file_filp)) {
		printk("Open %s failed!\n", filename);
		return NULL;
	}

	return file_filp;
}

static void mmu_savefile_setkernelfs(mm_segment_t * old_fs)
{
	*old_fs = get_fs();
	set_fs(KERNEL_DS);
}

static void mmu_savefile_write(struct file *file_filp, char *buffer, int len)
{
	file_filp->f_op->write(file_filp, buffer, len, &file_filp->f_pos);
}

static void mmu_savefile_resetfs(mm_segment_t old_fs)
{
	set_fs(old_fs);
}

static void mmu_savefile_close(struct file *file_filp)
{
	filp_close(file_filp, NULL);
}

static int mmu_dump_mmu(char *filename, u64 l0_31_l1_511_pgd)
{
	struct file *file_filp;
	mm_segment_t old_fs;
	struct page *p;
	u64 *pgd_page;
	int i;
	u64 target_pgd;
	struct page *p_bottom;
	u64 *pgd_page_bottom;

	if (0 == l0_31_l1_511_pgd) {
		return -1;
	}

	file_filp = mmu_savefile_open(filename);
	if (NULL == file_filp) {
		return -1;
	}

	mmu_savefile_setkernelfs(&old_fs);

	p = pfn_to_page(PFN_DOWN(l0_31_l1_511_pgd));
	pgd_page = kmap(p);
	if (NULL == pgd_page) {
		printk("kmap %p failed\n", p);
		return -1;
	}

	for (i = 0; i < KBASE_MMU_PAGE_ENTRIES; i++) {
		target_pgd = pte_to_phy_addr(pgd_page[i]);
		if (target_pgd) {
			p_bottom = pfn_to_page(PFN_DOWN(target_pgd));
			pgd_page_bottom = kmap(p_bottom);
			mmu_savefile_write(file_filp, (char *)pgd_page_bottom, KBASE_MMU_PAGE_ENTRIES * sizeof(u64));
			kunmap(p_bottom);
		}
	}

	kunmap(p);

	mmu_savefile_resetfs(old_fs);
	mmu_savefile_close(file_filp);

	return 0;
}

static int mmu_dump_ice(char *filename, u64 phy_ice)
{
	struct file *file_filp;
	mm_segment_t old_fs;
	struct page *p;
	u64 *pgd_page;

	if (0 == phy_ice) {
		return -1;
	}

	file_filp = mmu_savefile_open(filename);
	if (NULL == file_filp) {
		return -1;
	}

	mmu_savefile_setkernelfs(&old_fs);

	p = pfn_to_page(PFN_DOWN(phy_ice));
	pgd_page = kmap(p);

	mmu_savefile_write(file_filp, (char *)pgd_page, 4096);

	kunmap(p);

	mmu_savefile_resetfs(old_fs);
	mmu_savefile_close(file_filp);

	return 0;
}
#endif
int mmu_print_mmu(char *filename, u64 l0_31_l1_511_pgd)
{
	struct page *p;
	u64 *pgd_page;
	int i;
	u64 target_pgd;
	struct page *p_bottom;
	u32 *pgd_page_bottom;

	if (0 == l0_31_l1_511_pgd) {
		return -1;
	}

	p = pfn_to_page(PFN_DOWN(l0_31_l1_511_pgd));
	pgd_page = kmap_atomic(p);
	if (NULL == pgd_page) {
		printk("kmap %p failed\n", p);
		return -1;
	}

	for (i = 0; i < KBASE_MMU_PAGE_ENTRIES; i++) {
		printk("---------------------------%d------------------------\n", i);
		target_pgd = pte_to_phy_addr(pgd_page[i]);
		if (target_pgd) {
			int j, k;
			p_bottom = pfn_to_page(PFN_DOWN(target_pgd));
			pgd_page_bottom = kmap_atomic(p_bottom);
			//mmu_savefile_write(file_filp, (char *)pgd_page_bottom, KBASE_MMU_PAGE_ENTRIES * sizeof(u64));
			for (j = 0; j < 16 * 4; j++) {
				for (k = 0; k < 16; k++) {
					printk("-%08x", pgd_page_bottom[j * 16 + k]);
				}
				printk("\n");
			}
			kunmap_atomic(pgd_page_bottom);
		}
	}

	kunmap_atomic(pgd_page);
	return 0;
}

int mmu_print_ice(char *filename, u64 phy_ice)
{
	struct page *p;
	u32 *pgd_page;
	int j, k;

	if (0 == phy_ice) {
		return -1;
	}

	p = pfn_to_page(PFN_DOWN(phy_ice));
	pgd_page = kmap_atomic(p);

	for (j = 0; j < 2 * 8; j++) {
		for (k = 0; k < 16; k++) {
			printk("-%08x", pgd_page[j * 16 + k]);
		}
		printk("\n");
	}
	kunmap_atomic(pgd_page);
	return 0;
}

int mmu_dump_gmmu_hmmu(struct kbase_mmu_table *mmut)
{
	char filename_gmmu[80];
	char filename_hmmu[80];
	char filename_ice[80];

	u64 pgd_entry_hig1 = mmut->hmmu.pgd_hmmu;
	u64 pgd_entry_gmmu = mmu_get_L1_31_L2_0_pgd(mmut);
	if (pgd_entry_gmmu == 0) {
		return 0;
	}

	printk("ice info:\n\n");
	mmu_print_ice(filename_ice, mmut->hmmu.phy_ice_desc);

	printk("\ngmmu info:\n\n");
	mmu_print_mmu(filename_gmmu, pgd_entry_gmmu);

	printk("\nhmmu info:\n\n");
	mmu_print_mmu(filename_hmmu, pgd_entry_hig1);

	return 0;
	//todo:complete hig1 vector dump
#if 0
	snprintf(filename_hmmu, sizeof(filename_hmmu), "HMMU_%d_%p_%llu.bin",
		 task_tgid_nr(current), kctx, pgd_entry_hig1);
	snprintf(filename_gmmu, sizeof(filename_gmmu), "GMMU_%d_%p_%llu.bin",
		 task_tgid_nr(current), kctx, pgd_entry_gmmu);
	snprintf(filename_ice, sizeof(filename_gmmu), "ICE_%d_%p_%x.bin",
		 task_tgid_nr(current), kctx, (u32) kctx->phy_ice_desc);

	mmu_dump_mmu(filename_hmmu, pgd_entry_hig1);
	mmu_dump_mmu(filename_gmmu, pgd_entry_gmmu);
	mmu_dump_ice(filename_ice, kctx->phy_ice_desc);
	return 0;
#endif
}
