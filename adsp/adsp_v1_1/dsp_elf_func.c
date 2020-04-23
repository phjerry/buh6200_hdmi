/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: adsp elf
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#include <asm/io.h>

#include "hi_adsp_debug.h"
#include "hi_reg_common.h"
#include "dsp_elf_func.h"

hi_s32 reset_elf_section(hi_u32 base_addr, hi_u32 size)
{
    hi_void *elf_addr = HI_NULL;

    if (base_addr > HI_REG_PERI_BASE_ADDR) {
        elf_addr = ioremap_nocache(base_addr, size);
    } else {
        elf_addr = phys_to_virt(base_addr);
    }

    if (elf_addr == HI_NULL) {
        HI_LOG_ERR("memory mmap failed\n");
        return HI_FAILURE;
    }

    memset(elf_addr, 0, size);

    if (base_addr > HI_REG_PERI_BASE_ADDR) {
        iounmap(elf_addr);
    }

    return HI_SUCCESS;
}

hi_s32 memcpy_section(hi_u32 *pdst, hi_u32 *psrc, hi_u32 ifilesize)
{
    hi_u32 value;
    hi_s32 i;

    hi_u32 *udst = pdst;
    hi_uchar *usrc = (hi_uchar *)psrc;
    uintptr_t flag = (uintptr_t)pdst;

    if (flag & 3) {
        HI_LOG_WARN("warning dst address is not align in 32bit\n");
    }

    for (i = 0; i < ((ifilesize + 3) >> 2); i++) {
        value = usrc[0] | (usrc[1] << 8) | (usrc[2] << 16) | (usrc[3] << 24);
        udst[i] = value;
        usrc += 4;
    }

    return HI_SUCCESS;
}

hi_s32 copy_elf_section(hi_uchar *elf_addr)
{
    hi_s32 j;
    Elf32_Ehdr *phdr = (Elf32_Ehdr *)elf_addr;
    Elf32_Phdr *ppdr = (Elf32_Phdr *)(elf_addr + phdr->e_phoff);
    hi_void *addr = HI_NULL;

    for (j = 0; j < phdr->e_phnum; j++) {
        if ((ppdr[j].p_type == PT_LOAD) && (ppdr[j].p_filesz != 0)) {
            if (ppdr[j].p_paddr > HI_REG_PERI_BASE_ADDR) {
                addr = ioremap_nocache(ppdr[j].p_paddr, ppdr[j].p_filesz + 3);
            } else {
                addr = phys_to_virt(ppdr[j].p_paddr);
            }

            if (addr == HI_NULL) {
                return HI_FAILURE;
            }

            HI_LOG_INFO("phy addr: 0x%.8x;size: 0x%.8x, flag(%d)\n", ppdr[j].p_paddr,
                        ppdr[j].p_filesz, ppdr[j].p_paddr > HI_REG_PERI_BASE_ADDR);
            memcpy_section((hi_u32 *)addr, (hi_u32 *)(elf_addr + ppdr[j].p_offset), ppdr[j].p_filesz);

            if (ppdr[j].p_paddr > HI_REG_PERI_BASE_ADDR) {
                iounmap(addr);
            }
        }
    }

    return 0;
}

hi_s32 check_elf_paser(hi_uchar *elf_addr)
{
    hi_s32 i, j;
    Elf32_Ehdr *phdr = (Elf32_Ehdr *)elf_addr;
    Elf32_Phdr *ppdr = (Elf32_Phdr *)(elf_addr + phdr->e_phoff);
    hi_u32 *addr = HI_NULL;

    for (j = 0; j < phdr->e_phnum; j++) {
        if ((ppdr[j].p_type == PT_LOAD) && (ppdr[j].p_filesz != 0)) {
            /* map the phys address in elf file to virt address */
            if (ppdr[j].p_paddr > HI_REG_PERI_BASE_ADDR) {
                addr = ioremap_nocache(ppdr[j].p_paddr, ppdr[j].p_filesz + 3);
            } else {
                addr = phys_to_virt(ppdr[j].p_paddr);
            }

            if (addr == HI_NULL) {
                return HI_FAILURE;
            }

            for (i = 0; i < (ppdr[j].p_filesz >> 2); i++) {
                /* compare the value stored in ddr with value read from elf file */
                if (addr[i] != ((hi_u32 *)(elf_addr + ppdr[j].p_offset))[i]) {
                    HI_LOG_ERR(
                            "error addr 0x%x: 0x%x - 0x%x:0x%x \r\n", ppdr[j].p_paddr,
                            addr[i], ((hi_u32 *)(elf_addr + ppdr[j].p_offset))[i],
                            ((hi_u32 *)(elf_addr + ppdr[j].p_offset))[i]);
                    if (ppdr[j].p_paddr > HI_REG_PERI_BASE_ADDR) {
                        iounmap(addr);
                    }
                    return HI_FAILURE;
                }
            }
            if (ppdr[j].p_paddr > HI_REG_PERI_BASE_ADDR) {
                iounmap(addr);
            }
        }
    }

    return HI_SUCCESS;
}
