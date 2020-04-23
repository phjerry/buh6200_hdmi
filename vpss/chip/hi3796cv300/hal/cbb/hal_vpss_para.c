/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal para
 * Author: zhangjunyu
 * Create: 2019/11/01
 */

#include "hal_vpss_para.h"
#include "vpss_comm.h"

hi_void vpss_hal_set_para_addr_vhd_chn00(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn00)
{
    u_vpss_para_addr_vhd_chn00 vpss_para_addr_vhd_chn00;

    vpss_para_addr_vhd_chn00.bits.para_addr_vhd_chn00 = para_addr_vhd_chn00;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn00.u32), vpss_para_addr_vhd_chn00.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn01(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn01)
{
    u_vpss_para_addr_vhd_chn01 vpss_para_addr_vhd_chn01;

    vpss_para_addr_vhd_chn01.bits.para_addr_vhd_chn01 = para_addr_vhd_chn01;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn01.u32), vpss_para_addr_vhd_chn01.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn02(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn02)
{
    u_vpss_para_addr_vhd_chn02 vpss_para_addr_vhd_chn02;

    vpss_para_addr_vhd_chn02.bits.para_addr_vhd_chn02 = para_addr_vhd_chn02;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn02.u32), vpss_para_addr_vhd_chn02.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn03(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn03)
{
    u_vpss_para_addr_vhd_chn03 vpss_para_addr_vhd_chn03;

    vpss_para_addr_vhd_chn03.bits.para_addr_vhd_chn03 = para_addr_vhd_chn03;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn03.u32), vpss_para_addr_vhd_chn03.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn04(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn04)
{
    u_vpss_para_addr_vhd_chn04 vpss_para_addr_vhd_chn04;

    vpss_para_addr_vhd_chn04.bits.para_addr_vhd_chn04 = para_addr_vhd_chn04;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn04.u32), vpss_para_addr_vhd_chn04.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn05(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn05)
{
    u_vpss_para_addr_vhd_chn05 vpss_para_addr_vhd_chn05;

    vpss_para_addr_vhd_chn05.bits.para_addr_vhd_chn05 = para_addr_vhd_chn05;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn05.u32), vpss_para_addr_vhd_chn05.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn06(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn06)
{
    u_vpss_para_addr_vhd_chn06 vpss_para_addr_vhd_chn06;

    vpss_para_addr_vhd_chn06.bits.para_addr_vhd_chn06 = para_addr_vhd_chn06;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn06.u32), vpss_para_addr_vhd_chn06.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn07(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn07)
{
    u_vpss_para_addr_vhd_chn07 vpss_para_addr_vhd_chn07;

    vpss_para_addr_vhd_chn07.bits.para_addr_vhd_chn07 = para_addr_vhd_chn07;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn07.u32), vpss_para_addr_vhd_chn07.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn08(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn08)
{
    u_vpss_para_addr_vhd_chn08 vpss_para_addr_vhd_chn08;

    vpss_para_addr_vhd_chn08.bits.para_addr_vhd_chn08 = para_addr_vhd_chn08;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn08.u32), vpss_para_addr_vhd_chn08.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn09(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn09)
{
    u_vpss_para_addr_vhd_chn09 vpss_para_addr_vhd_chn09;

    vpss_para_addr_vhd_chn09.bits.para_addr_vhd_chn09 = para_addr_vhd_chn09;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn09.u32), vpss_para_addr_vhd_chn09.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn10(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn10)
{
    u_vpss_para_addr_vhd_chn10 vpss_para_addr_vhd_chn10;

    vpss_para_addr_vhd_chn10.bits.para_addr_vhd_chn10 = para_addr_vhd_chn10;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn10.u32), vpss_para_addr_vhd_chn10.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn11(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn11)
{
    u_vpss_para_addr_vhd_chn11 vpss_para_addr_vhd_chn11;

    vpss_para_addr_vhd_chn11.bits.para_addr_vhd_chn11 = para_addr_vhd_chn11;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn11.u32), vpss_para_addr_vhd_chn11.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn12(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn12)
{
    u_vpss_para_addr_vhd_chn12 vpss_para_addr_vhd_chn12;

    vpss_para_addr_vhd_chn12.bits.para_addr_vhd_chn12 = para_addr_vhd_chn12;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn12.u32), vpss_para_addr_vhd_chn12.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn13(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn13)
{
    u_vpss_para_addr_vhd_chn13 vpss_para_addr_vhd_chn13;

    vpss_para_addr_vhd_chn13.bits.para_addr_vhd_chn13 = para_addr_vhd_chn13;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn13.u32), vpss_para_addr_vhd_chn13.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn14(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn14)
{
    u_vpss_para_addr_vhd_chn14 vpss_para_addr_vhd_chn14;

    vpss_para_addr_vhd_chn14.bits.para_addr_vhd_chn14 = para_addr_vhd_chn14;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn14.u32), vpss_para_addr_vhd_chn14.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn15(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn15)
{
    u_vpss_para_addr_vhd_chn15 vpss_para_addr_vhd_chn15;

    vpss_para_addr_vhd_chn15.bits.para_addr_vhd_chn15 = para_addr_vhd_chn15;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn15.u32), vpss_para_addr_vhd_chn15.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn16(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn16)
{
    u_vpss_para_addr_vhd_chn16 vpss_para_addr_vhd_chn16;

    vpss_para_addr_vhd_chn16.bits.para_addr_vhd_chn16 = para_addr_vhd_chn16;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn16.u32), vpss_para_addr_vhd_chn16.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn17(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn17)
{
    u_vpss_para_addr_vhd_chn17 vpss_para_addr_vhd_chn17;

    vpss_para_addr_vhd_chn17.bits.para_addr_vhd_chn17 = para_addr_vhd_chn17;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn17.u32), vpss_para_addr_vhd_chn17.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn18(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn18)
{
    u_vpss_para_addr_vhd_chn18 vpss_para_addr_vhd_chn18;

    vpss_para_addr_vhd_chn18.bits.para_addr_vhd_chn18 = para_addr_vhd_chn18;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn18.u32), vpss_para_addr_vhd_chn18.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn19(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn19)
{
    u_vpss_para_addr_vhd_chn19 vpss_para_addr_vhd_chn19;

    vpss_para_addr_vhd_chn19.bits.para_addr_vhd_chn19 = para_addr_vhd_chn19;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn19.u32), vpss_para_addr_vhd_chn19.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn20(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn20)
{
    u_vpss_para_addr_vhd_chn20 vpss_para_addr_vhd_chn20;

    vpss_para_addr_vhd_chn20.bits.para_addr_vhd_chn20 = para_addr_vhd_chn20;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn20.u32), vpss_para_addr_vhd_chn20.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn21(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn21)
{
    u_vpss_para_addr_vhd_chn21 vpss_para_addr_vhd_chn21;

    vpss_para_addr_vhd_chn21.bits.para_addr_vhd_chn21 = para_addr_vhd_chn21;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn21.u32), vpss_para_addr_vhd_chn21.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn22(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn22)
{
    u_vpss_para_addr_vhd_chn22 vpss_para_addr_vhd_chn22;

    vpss_para_addr_vhd_chn22.bits.para_addr_vhd_chn22 = para_addr_vhd_chn22;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn22.u32), vpss_para_addr_vhd_chn22.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn23(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn23)
{
    u_vpss_para_addr_vhd_chn23 vpss_para_addr_vhd_chn23;

    vpss_para_addr_vhd_chn23.bits.para_addr_vhd_chn23 = para_addr_vhd_chn23;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn23.u32), vpss_para_addr_vhd_chn23.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn24(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn24)
{
    u_vpss_para_addr_vhd_chn24 vpss_para_addr_vhd_chn24;

    vpss_para_addr_vhd_chn24.bits.para_addr_vhd_chn24 = para_addr_vhd_chn24;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn24.u32), vpss_para_addr_vhd_chn24.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn25(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn25)
{
    u_vpss_para_addr_vhd_chn25 vpss_para_addr_vhd_chn25;

    vpss_para_addr_vhd_chn25.bits.para_addr_vhd_chn25 = para_addr_vhd_chn25;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn25.u32), vpss_para_addr_vhd_chn25.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn26(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn26)
{
    u_vpss_para_addr_vhd_chn26 vpss_para_addr_vhd_chn26;

    vpss_para_addr_vhd_chn26.bits.para_addr_vhd_chn26 = para_addr_vhd_chn26;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn26.u32), vpss_para_addr_vhd_chn26.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn27(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn27)
{
    u_vpss_para_addr_vhd_chn27 vpss_para_addr_vhd_chn27;

    vpss_para_addr_vhd_chn27.bits.para_addr_vhd_chn27 = para_addr_vhd_chn27;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn27.u32), vpss_para_addr_vhd_chn27.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn28(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn28)
{
    u_vpss_para_addr_vhd_chn28 vpss_para_addr_vhd_chn28;

    vpss_para_addr_vhd_chn28.bits.para_addr_vhd_chn28 = para_addr_vhd_chn28;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn28.u32), vpss_para_addr_vhd_chn28.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn29(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn29)
{
    u_vpss_para_addr_vhd_chn29 vpss_para_addr_vhd_chn29;

    vpss_para_addr_vhd_chn29.bits.para_addr_vhd_chn29 = para_addr_vhd_chn29;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn29.u32), vpss_para_addr_vhd_chn29.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn30(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn30)
{
    u_vpss_para_addr_vhd_chn30 vpss_para_addr_vhd_chn30;

    vpss_para_addr_vhd_chn30.bits.para_addr_vhd_chn30 = para_addr_vhd_chn30;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn30.u32), vpss_para_addr_vhd_chn30.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vhd_chn31(vpss_reg_type *vpss_regs, hi_u32 para_addr_vhd_chn31)
{
    u_vpss_para_addr_vhd_chn31 vpss_para_addr_vhd_chn31;

    vpss_para_addr_vhd_chn31.bits.para_addr_vhd_chn31 = para_addr_vhd_chn31;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vhd_chn31.u32), vpss_para_addr_vhd_chn31.u32);

    return;
}

hi_void vpss_hal_set_para_up_vhd_chn(vpss_reg_type *vpss_regs, hi_u32 chn_num)
{
    u_vpss_para_up_vhd vpss_para_up_vhd;

    if (chn_num > (1 << 31)) { /* 31:reg bit max */
        vpss_error("chan num bigger than 31 %d \n", chn_num);
        return;
    }

    vpss_para_up_vhd.u32 = vpss_reg_read((&(vpss_regs->vpss_para_up_vhd.u32)));
    vpss_para_up_vhd.u32 = vpss_para_up_vhd.u32 | chn_num;
    vpss_reg_write(&(vpss_regs->vpss_para_up_vhd.u32), vpss_para_up_vhd.u32);

    return;
}

hi_void vpss_hal_set_para_up_vhd_chn_all(vpss_reg_type *vpss_regs)
{
    u_vpss_para_up_vhd vpss_para_up_vhd;

    vpss_para_up_vhd.u32 = 0xffffffff;
    vpss_reg_write(&(vpss_regs->vpss_para_up_vhd.u32), vpss_para_up_vhd.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn00(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn00)
{
    u_vpss_para_addr_vsd_chn00 vpss_para_addr_vsd_chn00;

    vpss_para_addr_vsd_chn00.bits.para_addr_vsd_chn00 = para_addr_vsd_chn00;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn00.u32), vpss_para_addr_vsd_chn00.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn01(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn01)
{
    u_vpss_para_addr_vsd_chn01 vpss_para_addr_vsd_chn01;

    vpss_para_addr_vsd_chn01.bits.para_addr_vsd_chn01 = para_addr_vsd_chn01;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn01.u32), vpss_para_addr_vsd_chn01.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn02(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn02)
{
    u_vpss_para_addr_vsd_chn02 vpss_para_addr_vsd_chn02;

    vpss_para_addr_vsd_chn02.bits.para_addr_vsd_chn02 = para_addr_vsd_chn02;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn02.u32), vpss_para_addr_vsd_chn02.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn03(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn03)
{
    u_vpss_para_addr_vsd_chn03 vpss_para_addr_vsd_chn03;

    vpss_para_addr_vsd_chn03.bits.para_addr_vsd_chn03 = para_addr_vsd_chn03;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn03.u32), vpss_para_addr_vsd_chn03.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn04(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn04)
{
    u_vpss_para_addr_vsd_chn04 vpss_para_addr_vsd_chn04;

    vpss_para_addr_vsd_chn04.bits.para_addr_vsd_chn04 = para_addr_vsd_chn04;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn04.u32), vpss_para_addr_vsd_chn04.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn05(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn05)
{
    u_vpss_para_addr_vsd_chn05 vpss_para_addr_vsd_chn05;

    vpss_para_addr_vsd_chn05.bits.para_addr_vsd_chn05 = para_addr_vsd_chn05;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn05.u32), vpss_para_addr_vsd_chn05.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn06(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn06)
{
    u_vpss_para_addr_vsd_chn06 vpss_para_addr_vsd_chn06;

    vpss_para_addr_vsd_chn06.bits.para_addr_vsd_chn06 = para_addr_vsd_chn06;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn06.u32), vpss_para_addr_vsd_chn06.u32);

    return;
}

hi_void vpss_hal_set_para_addr_vsd_chn07(vpss_reg_type *vpss_regs, hi_u32 para_addr_vsd_chn07)
{
    u_vpss_para_addr_vsd_chn07 vpss_para_addr_vsd_chn07;

    vpss_para_addr_vsd_chn07.bits.para_addr_vsd_chn07 = para_addr_vsd_chn07;
    vpss_reg_write(&(vpss_regs->vpss_para_addr_vsd_chn07.u32), vpss_para_addr_vsd_chn07.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn00(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn00)
{
    u_vpss_para_haddr_vhd_chn00 vpss_para_haddr_vhd_chn00;

    vpss_para_haddr_vhd_chn00.bits.para_haddr_vhd_chn00 = para_haddr_vhd_chn00;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn00.u32), vpss_para_haddr_vhd_chn00.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn01(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn01)
{
    u_vpss_para_haddr_vhd_chn01 vpss_para_haddr_vhd_chn01;

    vpss_para_haddr_vhd_chn01.bits.para_haddr_vhd_chn01 = para_haddr_vhd_chn01;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn01.u32), vpss_para_haddr_vhd_chn01.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn02(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn02)
{
    u_vpss_para_haddr_vhd_chn02 vpss_para_haddr_vhd_chn02;

    vpss_para_haddr_vhd_chn02.bits.para_haddr_vhd_chn02 = para_haddr_vhd_chn02;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn02.u32), vpss_para_haddr_vhd_chn02.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn03(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn03)
{
    u_vpss_para_haddr_vhd_chn03 vpss_para_haddr_vhd_chn03;

    vpss_para_haddr_vhd_chn03.bits.para_haddr_vhd_chn03 = para_haddr_vhd_chn03;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn03.u32), vpss_para_haddr_vhd_chn03.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn04(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn04)
{
    u_vpss_para_haddr_vhd_chn04 vpss_para_haddr_vhd_chn04;

    vpss_para_haddr_vhd_chn04.bits.para_haddr_vhd_chn04 = para_haddr_vhd_chn04;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn04.u32), vpss_para_haddr_vhd_chn04.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn05(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn05)
{
    u_vpss_para_haddr_vhd_chn05 vpss_para_haddr_vhd_chn05;

    vpss_para_haddr_vhd_chn05.bits.para_haddr_vhd_chn05 = para_haddr_vhd_chn05;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn05.u32), vpss_para_haddr_vhd_chn05.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn06(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn06)
{
    u_vpss_para_haddr_vhd_chn06 vpss_para_haddr_vhd_chn06;

    vpss_para_haddr_vhd_chn06.bits.para_haddr_vhd_chn06 = para_haddr_vhd_chn06;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn06.u32), vpss_para_haddr_vhd_chn06.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn07(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn07)
{
    u_vpss_para_haddr_vhd_chn07 vpss_para_haddr_vhd_chn07;

    vpss_para_haddr_vhd_chn07.bits.para_haddr_vhd_chn07 = para_haddr_vhd_chn07;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn07.u32), vpss_para_haddr_vhd_chn07.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn08(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn08)
{
    u_vpss_para_haddr_vhd_chn08 vpss_para_haddr_vhd_chn08;

    vpss_para_haddr_vhd_chn08.bits.para_haddr_vhd_chn08 = para_haddr_vhd_chn08;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn08.u32), vpss_para_haddr_vhd_chn08.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn09(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn09)
{
    u_vpss_para_haddr_vhd_chn09 vpss_para_haddr_vhd_chn09;

    vpss_para_haddr_vhd_chn09.bits.para_haddr_vhd_chn09 = para_haddr_vhd_chn09;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn09.u32), vpss_para_haddr_vhd_chn09.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn10(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn10)
{
    u_vpss_para_haddr_vhd_chn10 vpss_para_haddr_vhd_chn10;

    vpss_para_haddr_vhd_chn10.bits.para_haddr_vhd_chn10 = para_haddr_vhd_chn10;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn10.u32), vpss_para_haddr_vhd_chn10.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn11(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn11)
{
    u_vpss_para_haddr_vhd_chn11 vpss_para_haddr_vhd_chn11;

    vpss_para_haddr_vhd_chn11.bits.para_haddr_vhd_chn11 = para_haddr_vhd_chn11;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn11.u32), vpss_para_haddr_vhd_chn11.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn12(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn12)
{
    u_vpss_para_haddr_vhd_chn12 vpss_para_haddr_vhd_chn12;

    vpss_para_haddr_vhd_chn12.bits.para_haddr_vhd_chn12 = para_haddr_vhd_chn12;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn12.u32), vpss_para_haddr_vhd_chn12.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn13(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn13)
{
    u_vpss_para_haddr_vhd_chn13 vpss_para_haddr_vhd_chn13;

    vpss_para_haddr_vhd_chn13.bits.para_haddr_vhd_chn13 = para_haddr_vhd_chn13;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn13.u32), vpss_para_haddr_vhd_chn13.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn14(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn14)
{
    u_vpss_para_haddr_vhd_chn14 vpss_para_haddr_vhd_chn14;

    vpss_para_haddr_vhd_chn14.bits.para_haddr_vhd_chn14 = para_haddr_vhd_chn14;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn14.u32), vpss_para_haddr_vhd_chn14.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn15(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn15)
{
    u_vpss_para_haddr_vhd_chn15 vpss_para_haddr_vhd_chn15;

    vpss_para_haddr_vhd_chn15.bits.para_haddr_vhd_chn15 = para_haddr_vhd_chn15;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn15.u32), vpss_para_haddr_vhd_chn15.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn16(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn16)
{
    u_vpss_para_haddr_vhd_chn16 vpss_para_haddr_vhd_chn16;

    vpss_para_haddr_vhd_chn16.bits.para_haddr_vhd_chn16 = para_haddr_vhd_chn16;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn16.u32), vpss_para_haddr_vhd_chn16.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn17(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn17)
{
    u_vpss_para_haddr_vhd_chn17 vpss_para_haddr_vhd_chn17;

    vpss_para_haddr_vhd_chn17.bits.para_haddr_vhd_chn17 = para_haddr_vhd_chn17;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn17.u32), vpss_para_haddr_vhd_chn17.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn18(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn18)
{
    u_vpss_para_haddr_vhd_chn18 vpss_para_haddr_vhd_chn18;

    vpss_para_haddr_vhd_chn18.bits.para_haddr_vhd_chn18 = para_haddr_vhd_chn18;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn18.u32), vpss_para_haddr_vhd_chn18.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn19(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn19)
{
    u_vpss_para_haddr_vhd_chn19 vpss_para_haddr_vhd_chn19;

    vpss_para_haddr_vhd_chn19.bits.para_haddr_vhd_chn19 = para_haddr_vhd_chn19;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn19.u32), vpss_para_haddr_vhd_chn19.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn20(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn20)
{
    u_vpss_para_haddr_vhd_chn20 vpss_para_haddr_vhd_chn20;

    vpss_para_haddr_vhd_chn20.bits.para_haddr_vhd_chn20 = para_haddr_vhd_chn20;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn20.u32), vpss_para_haddr_vhd_chn20.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn21(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn21)
{
    u_vpss_para_haddr_vhd_chn21 vpss_para_haddr_vhd_chn21;

    vpss_para_haddr_vhd_chn21.bits.para_haddr_vhd_chn21 = para_haddr_vhd_chn21;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn21.u32), vpss_para_haddr_vhd_chn21.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn22(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn22)
{
    u_vpss_para_haddr_vhd_chn22 vpss_para_haddr_vhd_chn22;

    vpss_para_haddr_vhd_chn22.bits.para_haddr_vhd_chn22 = para_haddr_vhd_chn22;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn22.u32), vpss_para_haddr_vhd_chn22.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn23(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn23)
{
    u_vpss_para_haddr_vhd_chn23 vpss_para_haddr_vhd_chn23;

    vpss_para_haddr_vhd_chn23.bits.para_haddr_vhd_chn23 = para_haddr_vhd_chn23;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn23.u32), vpss_para_haddr_vhd_chn23.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn24(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn24)
{
    u_vpss_para_haddr_vhd_chn24 vpss_para_haddr_vhd_chn24;

    vpss_para_haddr_vhd_chn24.bits.para_haddr_vhd_chn24 = para_haddr_vhd_chn24;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn24.u32), vpss_para_haddr_vhd_chn24.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn25(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn25)
{
    u_vpss_para_haddr_vhd_chn25 vpss_para_haddr_vhd_chn25;

    vpss_para_haddr_vhd_chn25.bits.para_haddr_vhd_chn25 = para_haddr_vhd_chn25;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn25.u32), vpss_para_haddr_vhd_chn25.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn26(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn26)
{
    u_vpss_para_haddr_vhd_chn26 vpss_para_haddr_vhd_chn26;

    vpss_para_haddr_vhd_chn26.bits.para_haddr_vhd_chn26 = para_haddr_vhd_chn26;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn26.u32), vpss_para_haddr_vhd_chn26.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn27(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn27)
{
    u_vpss_para_haddr_vhd_chn27 vpss_para_haddr_vhd_chn27;

    vpss_para_haddr_vhd_chn27.bits.para_haddr_vhd_chn27 = para_haddr_vhd_chn27;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn27.u32), vpss_para_haddr_vhd_chn27.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn28(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn28)
{
    u_vpss_para_haddr_vhd_chn28 vpss_para_haddr_vhd_chn28;

    vpss_para_haddr_vhd_chn28.bits.para_haddr_vhd_chn28 = para_haddr_vhd_chn28;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn28.u32), vpss_para_haddr_vhd_chn28.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn29(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn29)
{
    u_vpss_para_haddr_vhd_chn29 vpss_para_haddr_vhd_chn29;

    vpss_para_haddr_vhd_chn29.bits.para_haddr_vhd_chn29 = para_haddr_vhd_chn29;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn29.u32), vpss_para_haddr_vhd_chn29.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn30(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn30)
{
    u_vpss_para_haddr_vhd_chn30 vpss_para_haddr_vhd_chn30;

    vpss_para_haddr_vhd_chn30.bits.para_haddr_vhd_chn30 = para_haddr_vhd_chn30;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn30.u32), vpss_para_haddr_vhd_chn30.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vhd_chn31(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vhd_chn31)
{
    u_vpss_para_haddr_vhd_chn31 vpss_para_haddr_vhd_chn31;

    vpss_para_haddr_vhd_chn31.bits.para_haddr_vhd_chn31 = para_haddr_vhd_chn31;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vhd_chn31.u32), vpss_para_haddr_vhd_chn31.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn00(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn00)
{
    u_vpss_para_haddr_vsd_chn00 vpss_para_haddr_vsd_chn00;

    vpss_para_haddr_vsd_chn00.bits.para_haddr_vsd_chn00 = para_haddr_vsd_chn00;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn00.u32), vpss_para_haddr_vsd_chn00.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn01(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn01)
{
    u_vpss_para_haddr_vsd_chn01 vpss_para_haddr_vsd_chn01;

    vpss_para_haddr_vsd_chn01.bits.para_haddr_vsd_chn01 = para_haddr_vsd_chn01;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn01.u32), vpss_para_haddr_vsd_chn01.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn02(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn02)
{
    u_vpss_para_haddr_vsd_chn02 vpss_para_haddr_vsd_chn02;

    vpss_para_haddr_vsd_chn02.bits.para_haddr_vsd_chn02 = para_haddr_vsd_chn02;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn02.u32), vpss_para_haddr_vsd_chn02.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn03(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn03)
{
    u_vpss_para_haddr_vsd_chn03 vpss_para_haddr_vsd_chn03;

    vpss_para_haddr_vsd_chn03.bits.para_haddr_vsd_chn03 = para_haddr_vsd_chn03;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn03.u32), vpss_para_haddr_vsd_chn03.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn04(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn04)
{
    u_vpss_para_haddr_vsd_chn04 vpss_para_haddr_vsd_chn04;

    vpss_para_haddr_vsd_chn04.bits.para_haddr_vsd_chn04 = para_haddr_vsd_chn04;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn04.u32), vpss_para_haddr_vsd_chn04.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn05(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn05)
{
    u_vpss_para_haddr_vsd_chn05 vpss_para_haddr_vsd_chn05;

    vpss_para_haddr_vsd_chn05.bits.para_haddr_vsd_chn05 = para_haddr_vsd_chn05;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn05.u32), vpss_para_haddr_vsd_chn05.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn06(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn06)
{
    u_vpss_para_haddr_vsd_chn06 vpss_para_haddr_vsd_chn06;

    vpss_para_haddr_vsd_chn06.bits.para_haddr_vsd_chn06 = para_haddr_vsd_chn06;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn06.u32), vpss_para_haddr_vsd_chn06.u32);

    return;
}

hi_void vpss_hal_set_para_haddr_vsd_chn07(vpss_reg_type *vpss_regs, hi_u32 para_haddr_vsd_chn07)
{
    u_vpss_para_haddr_vsd_chn07 vpss_para_haddr_vsd_chn07;

    vpss_para_haddr_vsd_chn07.bits.para_haddr_vsd_chn07 = para_haddr_vsd_chn07;
    vpss_reg_write(&(vpss_regs->vpss_para_haddr_vsd_chn07.u32), vpss_para_haddr_vsd_chn07.u32);

    return;
}

hi_void vpss_hal_set_para_up_vsd_chn(vpss_reg_type *vpss_regs, hi_u32 chn_num)
{
    u_vpss_para_up_vsd vpss_para_up_vsd;

    vpss_para_up_vsd.u32 = (1 << chn_num);
    vpss_reg_write(&(vpss_regs->vpss_para_up_vsd.u32), vpss_para_up_vsd.u32);

    return;
}

hi_void vpss_hal_set_para_up_vsd_chn_all(vpss_reg_type *vpss_regs)
{
    u_vpss_para_up_vsd vpss_para_up_vsd;

    vpss_para_up_vsd.u32 = 0xff;
    vpss_reg_write(&(vpss_regs->vpss_para_up_vsd.u32), vpss_para_up_vsd.u32);

    return;
}

