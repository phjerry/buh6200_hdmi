/*
 *
 * (C) COPYRIGHT 2014-2015 Hisilicon Limited. All rights reserved.
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



/**
 * @file
 * Interface file for accessing HMMU hardware functionality
 */

/**
 * @page mali_kbase_hmmu_hw_page MMU hardware interface
 *
 * @section mali_kbase_hmmu_hw_intro_sec Introduction
 * This module provides an abstraction for accessing the functionality provided
 * by the midgard MMU and thus allows all MMU HW access to be contained within
 * one common place and allows for different backends (implementations) to
 * be provided.
 */

#ifndef _MALI_KBASE_HMMU_HW_H_
#define _MALI_KBASE_HMMU_HW_H_

/* Forward declarations */
struct kbase_device;
struct kbase_as;
struct kbase_context;

/** @brief Configure an address space for use.
 *
 * Configure the HMMU using the address space details setup in the
 * @ref kbase_context structure.
 *
 * @param[in]  kbdev          kbase device to configure.
 * @param[in]  as             address space to configure.
 * @param[in]  kctx           kbase context to configure.
 */
void kbase_hmmu_hw_configure(struct kbase_device *kbdev,
		int as_nr, phys_addr_t pgd);

/** @brief Update the ICE descriptor.
 *
 * Update the ICE descriptor using the specified descriptor
 *
 * @param[in]  kbdev          		kbase device to configure.
 * @param[in]  valid            	descriptor valid or not.
 * @param[in]  desc_num		the descriptor number, only update the specified descriptor.
 * @param[in]  gpu_va   		gpu virtual address for this descriptor.
 */
void kbase_hmmu_update_ice_desc( struct kbase_device *kbdev, u32 valid, u32 desc_num, u64 desc_addr, u64 gpu_va);

void kbase_hmmu_reset(struct kbase_device *kbdev, int hard);

void kbase_hmmu_setup(struct kbase_device *kbdev, struct kbase_mmu_table *mmut, int as_nr);

int kbase_hmmu_ice_desc_clean(struct kbase_device *kbdev, int as_nr);

int kbase_hmmu_ice_desc_waitready(struct kbase_device *kbdev, int as_nr);

void kbase_hmmu_cache_invalidate(struct kbase_device *kbdev, int as_nr);

int kbase_ice_desc_update_by_job(struct kbase_hmmu_table *hmmu, u16 nr_extres, struct kbase_ext_res *extres, u64 jc);

void kbase_ice_desc_free(struct kbase_hmmu_table *hmmu, u64 jc);

/** @} *//* end group mali_kbase_hmmu_hw */
/** @} *//* end group base_kbase_api */

#endif	/* _MALI_KBASE_HMMU_HW_H_ */

