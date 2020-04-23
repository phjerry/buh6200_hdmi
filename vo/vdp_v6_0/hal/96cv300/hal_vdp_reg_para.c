/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_reg_para.h"
#include "hal_vdp_comm.h"

#define VDP_PARAUP_UP_OFFSET 96
hi_void vdp_para_setparaupdispchn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_disp para_up_disp;

    para_up_disp.u32 = (1 << (u32chnnum - VDP_PARAUP_UP_OFFSET));
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_disp.u32)), para_up_disp.u32);

    return;
}

hi_void vdp_para_setparaupdispchnall(vdp_regs_type *vdp_reg)
{
    u_para_up_disp para_up_disp;

    para_up_disp.u32 = 0xfff;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_disp.u32)), para_up_disp.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn00 para_addr_disp_chn00;

    para_addr_disp_chn00.bits.para_addr_disp_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn00.u32)), para_addr_disp_chn00.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn01 para_addr_disp_chn01;

    para_addr_disp_chn01.bits.para_addr_disp_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn01.u32)), para_addr_disp_chn01.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn02 para_addr_disp_chn02;

    para_addr_disp_chn02.bits.para_addr_disp_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn02.u32)), para_addr_disp_chn02.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn03 para_addr_disp_chn03;

    para_addr_disp_chn03.bits.para_addr_disp_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn03.u32)), para_addr_disp_chn03.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn04 para_addr_disp_chn04;

    para_addr_disp_chn04.bits.para_addr_disp_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn04.u32)), para_addr_disp_chn04.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn05 para_addr_disp_chn05;

    para_addr_disp_chn05.bits.para_addr_disp_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn05.u32)), para_addr_disp_chn05.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn06 para_addr_disp_chn06;

    para_addr_disp_chn06.bits.para_addr_disp_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn06.u32)), para_addr_disp_chn06.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn07 para_addr_disp_chn07;

    para_addr_disp_chn07.bits.para_addr_disp_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn07.u32)), para_addr_disp_chn07.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn08 para_addr_disp_chn08;

    para_addr_disp_chn08.bits.para_addr_disp_chn08 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn08.u32)), para_addr_disp_chn08.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn09 para_addr_disp_chn09;

    para_addr_disp_chn09.bits.para_addr_disp_chn09 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn09.u32)), para_addr_disp_chn09.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn10 para_addr_disp_chn10;

    para_addr_disp_chn10.bits.para_addr_disp_chn10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn10.u32)), para_addr_disp_chn10.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn11 para_addr_disp_chn11;

    para_addr_disp_chn11.bits.para_addr_disp_chn11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn11.u32)), para_addr_disp_chn11.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn12 para_addr_disp_chn12;

    para_addr_disp_chn12.bits.para_addr_disp_chn12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn12.u32)), para_addr_disp_chn12.u32);

    return;
}

hi_void vdp_para_setparaaddrdispchn13(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_disp_chn13 para_addr_disp_chn13;

    para_addr_disp_chn13.bits.para_addr_disp_chn13 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_disp_chn13.u32)), para_addr_disp_chn13.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn00 para_haddr_disp_chn00;

    para_haddr_disp_chn00.bits.para_haddr_disp_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn00.u32)), para_haddr_disp_chn00.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn01 para_haddr_disp_chn01;

    para_haddr_disp_chn01.bits.para_haddr_disp_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn01.u32)), para_haddr_disp_chn01.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn02 para_haddr_disp_chn02;

    para_haddr_disp_chn02.bits.para_haddr_disp_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn02.u32)), para_haddr_disp_chn02.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn03 para_haddr_disp_chn03;

    para_haddr_disp_chn03.bits.para_haddr_disp_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn03.u32)), para_haddr_disp_chn03.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn04 para_haddr_disp_chn04;

    para_haddr_disp_chn04.bits.para_haddr_disp_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn04.u32)), para_haddr_disp_chn04.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn05 para_haddr_disp_chn05;

    para_haddr_disp_chn05.bits.para_haddr_disp_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn05.u32)), para_haddr_disp_chn05.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn06 para_haddr_disp_chn06;

    para_haddr_disp_chn06.bits.para_haddr_disp_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn06.u32)), para_haddr_disp_chn06.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn07 para_haddr_disp_chn07;

    para_haddr_disp_chn07.bits.para_haddr_disp_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn07.u32)), para_haddr_disp_chn07.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn08 para_haddr_disp_chn08;

    para_haddr_disp_chn08.bits.para_haddr_disp_chn08 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn08.u32)), para_haddr_disp_chn08.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn09 para_haddr_disp_chn09;

    para_haddr_disp_chn09.bits.para_haddr_disp_chn09 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn09.u32)), para_haddr_disp_chn09.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn10 para_haddr_disp_chn10;

    para_haddr_disp_chn10.bits.para_haddr_disp_chn10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn10.u32)), para_haddr_disp_chn10.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn11 para_haddr_disp_chn11;

    para_haddr_disp_chn11.bits.para_haddr_disp_chn11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn11.u32)), para_haddr_disp_chn11.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn12 para_haddr_disp_chn12;

    para_haddr_disp_chn12.bits.para_haddr_disp_chn12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn12.u32)), para_haddr_disp_chn12.u32);

    return;
}

hi_void vdp_para_setparahaddrdispchn13(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_disp_chn13 para_haddr_disp_chn13;

    para_haddr_disp_chn13.bits.para_haddr_disp_chn13 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_disp_chn13.u32)), para_haddr_disp_chn13.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn00 para_haddr_v0_chn00;

    para_haddr_v0_chn00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn00.u32)));
    para_haddr_v0_chn00.bits.para_haddr_v0_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn00.u32)), para_haddr_v0_chn00.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn00 para_addr_v0_chn00;

    para_addr_v0_chn00.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn00.u32)));
    para_addr_v0_chn00.bits.para_addr_v0_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn00.u32)), para_addr_v0_chn00.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn01 para_haddr_v0_chn01;

    para_haddr_v0_chn01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn01.u32)));
    para_haddr_v0_chn01.bits.para_haddr_v0_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn01.u32)), para_haddr_v0_chn01.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn01 para_addr_v0_chn01;

    para_addr_v0_chn01.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn01.u32)));
    para_addr_v0_chn01.bits.para_addr_v0_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn01.u32)), para_addr_v0_chn01.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn02 para_haddr_v0_chn02;

    para_haddr_v0_chn02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn02.u32)));
    para_haddr_v0_chn02.bits.para_haddr_v0_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn02.u32)), para_haddr_v0_chn02.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn02 para_addr_v0_chn02;

    para_addr_v0_chn02.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn02.u32)));
    para_addr_v0_chn02.bits.para_addr_v0_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn02.u32)), para_addr_v0_chn02.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn03 para_haddr_v0_chn03;

    para_haddr_v0_chn03.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn03.u32)));
    para_haddr_v0_chn03.bits.para_haddr_v0_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn03.u32)), para_haddr_v0_chn03.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn03 para_addr_v0_chn03;

    para_addr_v0_chn03.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn03.u32)));
    para_addr_v0_chn03.bits.para_addr_v0_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn03.u32)), para_addr_v0_chn03.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn04 para_haddr_v0_chn04;

    para_haddr_v0_chn04.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn04.u32)));
    para_haddr_v0_chn04.bits.para_haddr_v0_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn04.u32)), para_haddr_v0_chn04.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn04 para_addr_v0_chn04;

    para_addr_v0_chn04.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn04.u32)));
    para_addr_v0_chn04.bits.para_addr_v0_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn04.u32)), para_addr_v0_chn04.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn05 para_haddr_v0_chn05;

    para_haddr_v0_chn05.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn05.u32)));
    para_haddr_v0_chn05.bits.para_haddr_v0_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn05.u32)), para_haddr_v0_chn05.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn05 para_addr_v0_chn05;

    para_addr_v0_chn05.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn05.u32)));
    para_addr_v0_chn05.bits.para_addr_v0_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn05.u32)), para_addr_v0_chn05.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn06 para_haddr_v0_chn06;

    para_haddr_v0_chn06.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn06.u32)));
    para_haddr_v0_chn06.bits.para_haddr_v0_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn06.u32)), para_haddr_v0_chn06.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn06 para_addr_v0_chn06;

    para_addr_v0_chn06.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn06.u32)));
    para_addr_v0_chn06.bits.para_addr_v0_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn06.u32)), para_addr_v0_chn06.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn07 para_haddr_v0_chn07;

    para_haddr_v0_chn07.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn07.u32)));
    para_haddr_v0_chn07.bits.para_haddr_v0_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn07.u32)), para_haddr_v0_chn07.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn07 para_addr_v0_chn07;

    para_addr_v0_chn07.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn07.u32)));
    para_addr_v0_chn07.bits.para_addr_v0_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn07.u32)), para_addr_v0_chn07.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn08 para_haddr_v0_chn08;

    para_haddr_v0_chn08.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn08.u32)));
    para_haddr_v0_chn08.bits.para_haddr_v0_chn08 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn08.u32)), para_haddr_v0_chn08.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn08 para_addr_v0_chn08;

    para_addr_v0_chn08.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn08.u32)));
    para_addr_v0_chn08.bits.para_addr_v0_chn08 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn08.u32)), para_addr_v0_chn08.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn09 para_haddr_v0_chn09;

    para_haddr_v0_chn09.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn09.u32)));
    para_haddr_v0_chn09.bits.para_haddr_v0_chn09 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn09.u32)), para_haddr_v0_chn09.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn09 para_addr_v0_chn09;

    para_addr_v0_chn09.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn09.u32)));
    para_addr_v0_chn09.bits.para_addr_v0_chn09 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn09.u32)), para_addr_v0_chn09.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn10 para_haddr_v0_chn10;

    para_haddr_v0_chn10.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn10.u32)));
    para_haddr_v0_chn10.bits.para_haddr_v0_chn10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn10.u32)), para_haddr_v0_chn10.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn10 para_addr_v0_chn10;

    para_addr_v0_chn10.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn10.u32)));
    para_addr_v0_chn10.bits.para_addr_v0_chn10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn10.u32)), para_addr_v0_chn10.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn11 para_haddr_v0_chn11;

    para_haddr_v0_chn11.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn11.u32)));
    para_haddr_v0_chn11.bits.para_haddr_v0_chn11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn11.u32)), para_haddr_v0_chn11.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn11 para_addr_v0_chn11;

    para_addr_v0_chn11.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn11.u32)));
    para_addr_v0_chn11.bits.para_addr_v0_chn11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn11.u32)), para_addr_v0_chn11.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn12 para_haddr_v0_chn12;

    para_haddr_v0_chn12.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn12.u32)));
    para_haddr_v0_chn12.bits.para_haddr_v0_chn12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn12.u32)), para_haddr_v0_chn12.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn12 para_addr_v0_chn12;

    para_addr_v0_chn12.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn12.u32)));
    para_addr_v0_chn12.bits.para_addr_v0_chn12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn12.u32)), para_addr_v0_chn12.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn13(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn13 para_haddr_v0_chn13;

    para_haddr_v0_chn13.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn13.u32)));
    para_haddr_v0_chn13.bits.para_haddr_v0_chn13 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn13.u32)), para_haddr_v0_chn13.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn13(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn13 para_addr_v0_chn13;

    para_addr_v0_chn13.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn13.u32)));
    para_addr_v0_chn13.bits.para_addr_v0_chn13 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn13.u32)), para_addr_v0_chn13.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn14(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn14 para_haddr_v0_chn14;

    para_haddr_v0_chn14.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn14.u32)));
    para_haddr_v0_chn14.bits.para_haddr_v0_chn14 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn14.u32)), para_haddr_v0_chn14.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn14(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn14 para_addr_v0_chn14;

    para_addr_v0_chn14.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn14.u32)));
    para_addr_v0_chn14.bits.para_addr_v0_chn14 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn14.u32)), para_addr_v0_chn14.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn15(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn15 para_haddr_v0_chn15;

    para_haddr_v0_chn15.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn15.u32)));
    para_haddr_v0_chn15.bits.para_haddr_v0_chn15 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn15.u32)), para_haddr_v0_chn15.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn15(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn15 para_addr_v0_chn15;

    para_addr_v0_chn15.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn15.u32)));
    para_addr_v0_chn15.bits.para_addr_v0_chn15 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn15.u32)), para_addr_v0_chn15.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn16(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn16 para_haddr_v0_chn16;

    para_haddr_v0_chn16.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn16.u32)));
    para_haddr_v0_chn16.bits.para_haddr_v0_chn16 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn16.u32)), para_haddr_v0_chn16.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn16(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn16 para_addr_v0_chn16;

    para_addr_v0_chn16.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn16.u32)));
    para_addr_v0_chn16.bits.para_addr_v0_chn16 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn16.u32)), para_addr_v0_chn16.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn17(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn17 para_haddr_v0_chn17;

    para_haddr_v0_chn17.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn17.u32)));
    para_haddr_v0_chn17.bits.para_haddr_v0_chn17 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn17.u32)), para_haddr_v0_chn17.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn17(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn17 para_addr_v0_chn17;

    para_addr_v0_chn17.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn17.u32)));
    para_addr_v0_chn17.bits.para_addr_v0_chn17 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn17.u32)), para_addr_v0_chn17.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn18(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn18 para_haddr_v0_chn18;

    para_haddr_v0_chn18.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn18.u32)));
    para_haddr_v0_chn18.bits.para_haddr_v0_chn18 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn18.u32)), para_haddr_v0_chn18.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn18(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn18 para_addr_v0_chn18;

    para_addr_v0_chn18.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn18.u32)));
    para_addr_v0_chn18.bits.para_addr_v0_chn18 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn18.u32)), para_addr_v0_chn18.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn19(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn19 para_haddr_v0_chn19;

    para_haddr_v0_chn19.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn19.u32)));
    para_haddr_v0_chn19.bits.para_haddr_v0_chn19 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn19.u32)), para_haddr_v0_chn19.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn19(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn19 para_addr_v0_chn19;

    para_addr_v0_chn19.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn19.u32)));
    para_addr_v0_chn19.bits.para_addr_v0_chn19 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn19.u32)), para_addr_v0_chn19.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn20(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn20 para_haddr_v0_chn20;

    para_haddr_v0_chn20.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn20.u32)));
    para_haddr_v0_chn20.bits.para_haddr_v0_chn20 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn20.u32)), para_haddr_v0_chn20.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn20(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn20 para_addr_v0_chn20;

    para_addr_v0_chn20.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn20.u32)));
    para_addr_v0_chn20.bits.para_addr_v0_chn20 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn20.u32)), para_addr_v0_chn20.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn21(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn21 para_haddr_v0_chn21;

    para_haddr_v0_chn21.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn21.u32)));
    para_haddr_v0_chn21.bits.para_haddr_v0_chn21 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn21.u32)), para_haddr_v0_chn21.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn21(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn21 para_addr_v0_chn21;

    para_addr_v0_chn21.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn21.u32)));
    para_addr_v0_chn21.bits.para_addr_v0_chn21 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn21.u32)), para_addr_v0_chn21.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn22(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn22 para_haddr_v0_chn22;

    para_haddr_v0_chn22.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn22.u32)));
    para_haddr_v0_chn22.bits.para_haddr_v0_chn22 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn22.u32)), para_haddr_v0_chn22.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn22(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn22 para_addr_v0_chn22;

    para_addr_v0_chn22.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn22.u32)));
    para_addr_v0_chn22.bits.para_addr_v0_chn22 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn22.u32)), para_addr_v0_chn22.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn23(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn23 para_haddr_v0_chn23;

    para_haddr_v0_chn23.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn23.u32)));
    para_haddr_v0_chn23.bits.para_haddr_v0_chn23 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn23.u32)), para_haddr_v0_chn23.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn23(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn23 para_addr_v0_chn23;

    para_addr_v0_chn23.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn23.u32)));
    para_addr_v0_chn23.bits.para_addr_v0_chn23 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn23.u32)), para_addr_v0_chn23.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn24(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn24 para_haddr_v0_chn24;

    para_haddr_v0_chn24.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn24.u32)));
    para_haddr_v0_chn24.bits.para_haddr_v0_chn24 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn24.u32)), para_haddr_v0_chn24.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn24(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn24 para_addr_v0_chn24;

    para_addr_v0_chn24.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn24.u32)));
    para_addr_v0_chn24.bits.para_addr_v0_chn24 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn24.u32)), para_addr_v0_chn24.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn25(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn25 para_haddr_v0_chn25;

    para_haddr_v0_chn25.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn25.u32)));
    para_haddr_v0_chn25.bits.para_haddr_v0_chn25 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn25.u32)), para_haddr_v0_chn25.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn25(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn25 para_addr_v0_chn25;

    para_addr_v0_chn25.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn25.u32)));
    para_addr_v0_chn25.bits.para_addr_v0_chn25 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn25.u32)), para_addr_v0_chn25.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn26(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn26 para_haddr_v0_chn26;

    para_haddr_v0_chn26.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn26.u32)));
    para_haddr_v0_chn26.bits.para_haddr_v0_chn26 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn26.u32)), para_haddr_v0_chn26.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn26(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn26 para_addr_v0_chn26;

    para_addr_v0_chn26.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn26.u32)));
    para_addr_v0_chn26.bits.para_addr_v0_chn26 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn26.u32)), para_addr_v0_chn26.u32);

    return;
}

hi_void vdp_para_setparahaddrv0chn27(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_v0_chn27 para_haddr_v0_chn27;

    para_haddr_v0_chn27.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_haddr_v0_chn27.u32)));
    para_haddr_v0_chn27.bits.para_haddr_v0_chn27 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_v0_chn27.u32)), para_haddr_v0_chn27.u32);

    return;
}

hi_void vdp_para_setparaaddrv0chn27(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_v0_chn27 para_addr_v0_chn27;

    para_addr_v0_chn27.u32 = vdp_regread((uintptr_t)(&(vdp_reg->para_addr_v0_chn27.u32)));
    para_addr_v0_chn27.bits.para_addr_v0_chn27 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_v0_chn27.u32)), para_addr_v0_chn27.u32);

    return;
}

#define VDP_V0_PARAUP_OFFSET 64
hi_void vdp_para_setparaupv0chn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_v0 para_up_v0;

    para_up_v0.u32 = (1 << (u32chnnum - VDP_V0_PARAUP_OFFSET));
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_v0.u32)), para_up_v0.u32);

    return;
}

hi_void vdp_para_setparaupv0chnall(vdp_regs_type *vdp_reg)
{
    u_para_up_v0 para_up_v0;

    para_up_v0.u32 = 0x7ffffff;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_v0.u32)), para_up_v0.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn00 para_addr_vhd_chn00;

    para_addr_vhd_chn00.bits.para_addr_vhd_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn00.u32)), para_addr_vhd_chn00.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn01 para_addr_vhd_chn01;

    para_addr_vhd_chn01.bits.para_addr_vhd_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn01.u32)), para_addr_vhd_chn01.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn02 para_addr_vhd_chn02;

    para_addr_vhd_chn02.bits.para_addr_vhd_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn02.u32)), para_addr_vhd_chn02.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn03 para_addr_vhd_chn03;

    para_addr_vhd_chn03.bits.para_addr_vhd_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn03.u32)), para_addr_vhd_chn03.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn04 para_addr_vhd_chn04;

    para_addr_vhd_chn04.bits.para_addr_vhd_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn04.u32)), para_addr_vhd_chn04.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn05 para_addr_vhd_chn05;

    para_addr_vhd_chn05.bits.para_addr_vhd_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn05.u32)), para_addr_vhd_chn05.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn06 para_addr_vhd_chn06;

    para_addr_vhd_chn06.bits.para_addr_vhd_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn06.u32)), para_addr_vhd_chn06.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn07 para_addr_vhd_chn07;

    para_addr_vhd_chn07.bits.para_addr_vhd_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn07.u32)), para_addr_vhd_chn07.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn08 para_addr_vhd_chn08;

    para_addr_vhd_chn08.bits.para_addr_vhd_chn08 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn08.u32)), para_addr_vhd_chn08.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn09 para_addr_vhd_chn09;

    para_addr_vhd_chn09.bits.para_addr_vhd_chn09 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn09.u32)), para_addr_vhd_chn09.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn10 para_addr_vhd_chn10;

    para_addr_vhd_chn10.bits.para_addr_vhd_chn10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn10.u32)), para_addr_vhd_chn10.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn11 para_addr_vhd_chn11;

    para_addr_vhd_chn11.bits.para_addr_vhd_chn11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn11.u32)), para_addr_vhd_chn11.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn12 para_addr_vhd_chn12;

    para_addr_vhd_chn12.bits.para_addr_vhd_chn12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn12.u32)), para_addr_vhd_chn12.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn13(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn13 para_addr_vhd_chn13;

    para_addr_vhd_chn13.bits.para_addr_vhd_chn13 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn13.u32)), para_addr_vhd_chn13.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn14(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn14 para_addr_vhd_chn14;

    para_addr_vhd_chn14.bits.para_addr_vhd_chn14 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn14.u32)), para_addr_vhd_chn14.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn15(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn15 para_addr_vhd_chn15;

    para_addr_vhd_chn15.bits.para_addr_vhd_chn15 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn15.u32)), para_addr_vhd_chn15.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn16(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn16 para_addr_vhd_chn16;

    para_addr_vhd_chn16.bits.para_addr_vhd_chn16 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn16.u32)), para_addr_vhd_chn16.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn17(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn17 para_addr_vhd_chn17;

    para_addr_vhd_chn17.bits.para_addr_vhd_chn17 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn17.u32)), para_addr_vhd_chn17.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn18(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn18 para_addr_vhd_chn18;

    para_addr_vhd_chn18.bits.para_addr_vhd_chn18 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn18.u32)), para_addr_vhd_chn18.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn19(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn19 para_addr_vhd_chn19;

    para_addr_vhd_chn19.bits.para_addr_vhd_chn19 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn19.u32)), para_addr_vhd_chn19.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn20(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn20 para_addr_vhd_chn20;

    para_addr_vhd_chn20.bits.para_addr_vhd_chn20 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn20.u32)), para_addr_vhd_chn20.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn21(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn21 para_addr_vhd_chn21;

    para_addr_vhd_chn21.bits.para_addr_vhd_chn21 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn21.u32)), para_addr_vhd_chn21.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn22(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn22 para_addr_vhd_chn22;

    para_addr_vhd_chn22.bits.para_addr_vhd_chn22 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn22.u32)), para_addr_vhd_chn22.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn23(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn23 para_addr_vhd_chn23;

    para_addr_vhd_chn23.bits.para_addr_vhd_chn23 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn23.u32)), para_addr_vhd_chn23.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn24(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn24 para_addr_vhd_chn24;

    para_addr_vhd_chn24.bits.para_addr_vhd_chn24 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn24.u32)), para_addr_vhd_chn24.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn25(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn25 para_addr_vhd_chn25;

    para_addr_vhd_chn25.bits.para_addr_vhd_chn25 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn25.u32)), para_addr_vhd_chn25.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn26(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn26 para_addr_vhd_chn26;

    para_addr_vhd_chn26.bits.para_addr_vhd_chn26 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn26.u32)), para_addr_vhd_chn26.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn27(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn27 para_addr_vhd_chn27;

    para_addr_vhd_chn27.bits.para_addr_vhd_chn27 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn27.u32)), para_addr_vhd_chn27.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn28(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn28 para_addr_vhd_chn28;

    para_addr_vhd_chn28.bits.para_addr_vhd_chn28 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn28.u32)), para_addr_vhd_chn28.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn29(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn29 para_addr_vhd_chn29;

    para_addr_vhd_chn29.bits.para_addr_vhd_chn29 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn29.u32)), para_addr_vhd_chn29.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn30(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn30 para_addr_vhd_chn30;

    para_addr_vhd_chn30.bits.para_addr_vhd_chn30 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn30.u32)), para_addr_vhd_chn30.u32);

    return;
}

hi_void vdp_para_setparaaddrvhdchn31(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vhd_chn31 para_addr_vhd_chn31;

    para_addr_vhd_chn31.bits.para_addr_vhd_chn31 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vhd_chn31.u32)), para_addr_vhd_chn31.u32);

    return;
}

#define VDP_MAX_PARAUP_CHN 31
hi_void vdp_para_setparaupvhdchn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_vhd para_up_vhd;
    if (u32chnnum <= VDP_MAX_PARAUP_CHN) {
        para_up_vhd.u32 = (1 << u32chnnum);
        vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_vhd.u32)), para_up_vhd.u32);
    }

    return;
}

hi_void vdp_para_setparaupvhdchnall(vdp_regs_type *vdp_reg)
{
    u_para_up_vhd para_up_vhd;

    para_up_vhd.u32 = 0xffffffff;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_vhd.u32)), para_up_vhd.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn00 para_addr_vsd_chn00;

    para_addr_vsd_chn00.bits.para_addr_vsd_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn00.u32)), para_addr_vsd_chn00.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn01 para_addr_vsd_chn01;

    para_addr_vsd_chn01.bits.para_addr_vsd_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn01.u32)), para_addr_vsd_chn01.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn02 para_addr_vsd_chn02;

    para_addr_vsd_chn02.bits.para_addr_vsd_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn02.u32)), para_addr_vsd_chn02.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn03 para_addr_vsd_chn03;

    para_addr_vsd_chn03.bits.para_addr_vsd_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn03.u32)), para_addr_vsd_chn03.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn04 para_addr_vsd_chn04;

    para_addr_vsd_chn04.bits.para_addr_vsd_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn04.u32)), para_addr_vsd_chn04.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn05 para_addr_vsd_chn05;

    para_addr_vsd_chn05.bits.para_addr_vsd_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn05.u32)), para_addr_vsd_chn05.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn06 para_addr_vsd_chn06;

    para_addr_vsd_chn06.bits.para_addr_vsd_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn06.u32)), para_addr_vsd_chn06.u32);

    return;
}

hi_void vdp_para_setparaaddrvsdchn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_addr_vsd_chn07 para_addr_vsd_chn07;

    para_addr_vsd_chn07.bits.para_addr_vsd_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_addr_vsd_chn07.u32)), para_addr_vsd_chn07.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn00 para_haddr_vhd_chn00;

    para_haddr_vhd_chn00.bits.para_haddr_vhd_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn00.u32)), para_haddr_vhd_chn00.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn01 para_haddr_vhd_chn01;

    para_haddr_vhd_chn01.bits.para_haddr_vhd_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn01.u32)), para_haddr_vhd_chn01.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn02 para_haddr_vhd_chn02;

    para_haddr_vhd_chn02.bits.para_haddr_vhd_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn02.u32)), para_haddr_vhd_chn02.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn03 para_haddr_vhd_chn03;

    para_haddr_vhd_chn03.bits.para_haddr_vhd_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn03.u32)), para_haddr_vhd_chn03.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn04 para_haddr_vhd_chn04;

    para_haddr_vhd_chn04.bits.para_haddr_vhd_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn04.u32)), para_haddr_vhd_chn04.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn05 para_haddr_vhd_chn05;

    para_haddr_vhd_chn05.bits.para_haddr_vhd_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn05.u32)), para_haddr_vhd_chn05.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn06 para_haddr_vhd_chn06;

    para_haddr_vhd_chn06.bits.para_haddr_vhd_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn06.u32)), para_haddr_vhd_chn06.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn07 para_haddr_vhd_chn07;

    para_haddr_vhd_chn07.bits.para_haddr_vhd_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn07.u32)), para_haddr_vhd_chn07.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn08(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn08 para_haddr_vhd_chn08;

    para_haddr_vhd_chn08.bits.para_haddr_vhd_chn08 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn08.u32)), para_haddr_vhd_chn08.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn09(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn09 para_haddr_vhd_chn09;

    para_haddr_vhd_chn09.bits.para_haddr_vhd_chn09 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn09.u32)), para_haddr_vhd_chn09.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn10(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn10 para_haddr_vhd_chn10;

    para_haddr_vhd_chn10.bits.para_haddr_vhd_chn10 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn10.u32)), para_haddr_vhd_chn10.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn11(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn11 para_haddr_vhd_chn11;

    para_haddr_vhd_chn11.bits.para_haddr_vhd_chn11 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn11.u32)), para_haddr_vhd_chn11.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn12(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn12 para_haddr_vhd_chn12;

    para_haddr_vhd_chn12.bits.para_haddr_vhd_chn12 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn12.u32)), para_haddr_vhd_chn12.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn13(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn13 para_haddr_vhd_chn13;

    para_haddr_vhd_chn13.bits.para_haddr_vhd_chn13 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn13.u32)), para_haddr_vhd_chn13.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn14(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn14 para_haddr_vhd_chn14;

    para_haddr_vhd_chn14.bits.para_haddr_vhd_chn14 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn14.u32)), para_haddr_vhd_chn14.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn15(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn15 para_haddr_vhd_chn15;

    para_haddr_vhd_chn15.bits.para_haddr_vhd_chn15 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn15.u32)), para_haddr_vhd_chn15.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn16(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn16 para_haddr_vhd_chn16;

    para_haddr_vhd_chn16.bits.para_haddr_vhd_chn16 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn16.u32)), para_haddr_vhd_chn16.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn17(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn17 para_haddr_vhd_chn17;

    para_haddr_vhd_chn17.bits.para_haddr_vhd_chn17 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn17.u32)), para_haddr_vhd_chn17.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn18(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn18 para_haddr_vhd_chn18;

    para_haddr_vhd_chn18.bits.para_haddr_vhd_chn18 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn18.u32)), para_haddr_vhd_chn18.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn19(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn19 para_haddr_vhd_chn19;

    para_haddr_vhd_chn19.bits.para_haddr_vhd_chn19 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn19.u32)), para_haddr_vhd_chn19.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn20(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn20 para_haddr_vhd_chn20;

    para_haddr_vhd_chn20.bits.para_haddr_vhd_chn20 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn20.u32)), para_haddr_vhd_chn20.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn21(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn21 para_haddr_vhd_chn21;

    para_haddr_vhd_chn21.bits.para_haddr_vhd_chn21 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn21.u32)), para_haddr_vhd_chn21.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn22(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn22 para_haddr_vhd_chn22;

    para_haddr_vhd_chn22.bits.para_haddr_vhd_chn22 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn22.u32)), para_haddr_vhd_chn22.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn23(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn23 para_haddr_vhd_chn23;

    para_haddr_vhd_chn23.bits.para_haddr_vhd_chn23 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn23.u32)), para_haddr_vhd_chn23.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn24(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn24 para_haddr_vhd_chn24;

    para_haddr_vhd_chn24.bits.para_haddr_vhd_chn24 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn24.u32)), para_haddr_vhd_chn24.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn25(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn25 para_haddr_vhd_chn25;

    para_haddr_vhd_chn25.bits.para_haddr_vhd_chn25 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn25.u32)), para_haddr_vhd_chn25.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn26(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn26 para_haddr_vhd_chn26;

    para_haddr_vhd_chn26.bits.para_haddr_vhd_chn26 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn26.u32)), para_haddr_vhd_chn26.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn27(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn27 para_haddr_vhd_chn27;

    para_haddr_vhd_chn27.bits.para_haddr_vhd_chn27 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn27.u32)), para_haddr_vhd_chn27.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn28(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn28 para_haddr_vhd_chn28;

    para_haddr_vhd_chn28.bits.para_haddr_vhd_chn28 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn28.u32)), para_haddr_vhd_chn28.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn29(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn29 para_haddr_vhd_chn29;

    para_haddr_vhd_chn29.bits.para_haddr_vhd_chn29 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn29.u32)), para_haddr_vhd_chn29.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn30(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn30 para_haddr_vhd_chn30;

    para_haddr_vhd_chn30.bits.para_haddr_vhd_chn30 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn30.u32)), para_haddr_vhd_chn30.u32);

    return;
}

hi_void vdp_para_setparahaddrvhdchn31(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vhd_chn31 para_haddr_vhd_chn31;

    para_haddr_vhd_chn31.bits.para_haddr_vhd_chn31 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vhd_chn31.u32)), para_haddr_vhd_chn31.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn00(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn00 para_haddr_vsd_chn00;

    para_haddr_vsd_chn00.bits.para_haddr_vsd_chn00 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn00.u32)), para_haddr_vsd_chn00.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn01(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn01 para_haddr_vsd_chn01;

    para_haddr_vsd_chn01.bits.para_haddr_vsd_chn01 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn01.u32)), para_haddr_vsd_chn01.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn02(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn02 para_haddr_vsd_chn02;

    para_haddr_vsd_chn02.bits.para_haddr_vsd_chn02 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn02.u32)), para_haddr_vsd_chn02.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn03(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn03 para_haddr_vsd_chn03;

    para_haddr_vsd_chn03.bits.para_haddr_vsd_chn03 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn03.u32)), para_haddr_vsd_chn03.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn04(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn04 para_haddr_vsd_chn04;

    para_haddr_vsd_chn04.bits.para_haddr_vsd_chn04 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn04.u32)), para_haddr_vsd_chn04.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn05(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn05 para_haddr_vsd_chn05;

    para_haddr_vsd_chn05.bits.para_haddr_vsd_chn05 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn05.u32)), para_haddr_vsd_chn05.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn06(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn06 para_haddr_vsd_chn06;

    para_haddr_vsd_chn06.bits.para_haddr_vsd_chn06 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn06.u32)), para_haddr_vsd_chn06.u32);

    return;
}

hi_void vdp_para_setparahaddrvsdchn07(vdp_regs_type *vdp_reg, hi_u32 data)
{
    u_para_haddr_vsd_chn07 para_haddr_vsd_chn07;

    para_haddr_vsd_chn07.bits.para_haddr_vsd_chn07 = data;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_haddr_vsd_chn07.u32)), para_haddr_vsd_chn07.u32);

    return;
}

#define VDP_PARAUP_CHNNUM_OFFSET 32
hi_void vdp_para_setparaupvsdchn(vdp_regs_type *vdp_reg, hi_u32 u32chnnum)
{
    u_para_up_vsd para_up_vsd;

    para_up_vsd.u32 = (1 << (u32chnnum - VDP_PARAUP_CHNNUM_OFFSET));
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_vsd.u32)), para_up_vsd.u32);

    return;
}

hi_void vdp_para_setparaupvsdchnall(vdp_regs_type *vdp_reg)
{
    u_para_up_vsd para_up_vsd;

    para_up_vsd.u32 = 0xff;
    vdp_regwrite((uintptr_t)(&(vdp_reg->para_up_vsd.u32)), para_up_vsd.u32);

    return;
}
