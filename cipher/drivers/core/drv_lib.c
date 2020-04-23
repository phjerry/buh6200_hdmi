/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_lib
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_osal_lib.h"
#include "cryp_trng.h"
#include "cryp_symc.h"

/*********************** Internal Structure Definition ***********************/
/** \addtogroup      base type */
/** @{ */ /** <!-- [base] */

/* crg register addr size */
#define CRG_REG_ADDR_SIZE (0x100)

/* set a bit within a word */
#undef SET_BIT
#define SET_BIT(src, bit) ((src) |= (1 << (bit)))

/* clear a bit within a word */
#undef CLEAR_BIT
#define CLEAR_BIT(src, bit) ((src) &= ~(1 << (bit)))

/* ! module already initialize or not */
static hi_u32 module_initialize = HI_FALSE;

/* ! \struct resource_channel
 * the tbale of base info of module, such as addr/reset/clk/ver.
 */
struct sys_arch_hardware_info {
    /* the name of module, used for debug print and request interrupt */
    const char *name;

    /* smoe field may be needn't to used */
    hi_u32 reset_valid : 1; /* bis[0], reset availability, 0-valid, 1-invalid */
    hi_u32 clk_valid : 1;   /* bis[1], clock availability, 0-valid, 1-invalid */
    hi_u32 phy_valid : 1;   /* bis[2], system address availability, 0-valid, 1-invalid */
    hi_u32 crg_valid : 1;   /* bis[3], CRG availability, 0-valid, 1-invalid */
    hi_u32 ver_valid : 1;   /* bis[4], version reg availability, 0-valid, 1-invalid */
    hi_u32 int_valid : 1;   /* bis[5], interrupt availability, 0-valid, 1-invalid */
    hi_u32 res_valid : 25;  /* bis[6..31] */

    /* module base addr */
    hi_u32 reg_addr_phy;

    /* base logic addr size */
    hi_u32 reg_addr_size;

    /* crg register addr, which provide the switch of reset and clock */
    hi_u32 crg_addr_phy;

    /* the clk switch bit index within the crg register,
     *if the switch bis is provided by second crg register, you need to add 32 */
    hi_u32 clk_bit : 8;

    /* the reset switch bit index within the crg register,
     * if the switch bis is provided by second crg register, you need to add 32 */
    hi_u32 reset_bit : 8;

    /* the interrupt number */
    hi_u32 int_num : 8;

    /* the reserve bits */
    hi_u32 res : 8;

    /* the offset of version register */
    hi_u32 version_reg;

    /* the value of version register, you can used it to check the active module */
    hi_u32 version_val;

    /* cpu address of module register */
    void *reg_addr_via;

    /* cpu address of crg register */
    void *crg_addr_via;
};

static struct sys_arch_hardware_info hard_info_table[CRYPTO_MODULE_ID_CNT] = {
    HARD_INFO_CIPHER,
    HARD_INFO_CIPHER_KEY,
    HARD_INFO_HASH,
    HARD_INFO_IFEP_RSA,
    HARD_INFO_SIC_RSA,
    HARD_INFO_TRNG,
    HARD_INFO_PKE,
    HARD_INFO_SM4,
    HARD_INFO_SMMU,
};

#ifdef CRYPTO_SWITCH_CPU
static struct sys_arch_hardware_info nsec_hard_info_table[CRYPTO_MODULE_ID_CNT] = {
    NSEC_HARD_INFO_CIPHER,
    NSEC_HARD_INFO_CIPHER_KEY,
    NSEC_HARD_INFO_HASH,
    NSEC_HARD_INFO_IFEP_RSA,
    NSEC_HARD_INFO_SIC_RSA,
    NSEC_HARD_INFO_TRNG,
    NSEC_HARD_INFO_PKE,
    NSEC_HARD_INFO_SM4,
    NSEC_HARD_INFO_SMMU,
};

static hi_u32 s_secure_cpu = HI_FALSE;
#endif

compat_addr compat_addr_zero = { .phy = 0 };

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      base */
/** @{ */ /** <!--[base] */

static hi_s32 module_addr_map_reg(void)
{
    hi_u32 i;
    hi_u32 ret = HI_SUCCESS;
    struct sys_arch_hardware_info *table = HI_NULL;

    hi_log_func_enter();

    if (module_initialize == HI_TRUE) {
        return HI_SUCCESS;
    }

    for (i = 0; i < CRYPTO_MODULE_ID_CNT; i++) {
        table = &hard_info_table[i];

        hi_log_info("[%d] %s\n", i, table->name);

        /* io remap crg register */
        if (table->crg_valid) {
            table->crg_addr_via = crypto_ioremap_nocache((uintptr_t)table->crg_addr_phy, CRG_REG_ADDR_SIZE);
            if (table->crg_addr_via == HI_NULL) {
                hi_log_error("iomap reg of module failed\n");
                module_addr_unmap();
                ret = HI_FAILURE;
                break;
            }
            hi_log_info("crg phy 0x%x, via 0x%p\n", table->crg_addr_phy, table->crg_addr_via);
        }

        /* io remap module register */
        if (table->phy_valid) {
            table->reg_addr_via = crypto_ioremap_nocache(table->reg_addr_phy,
                                                         table->reg_addr_size);
            if (table->reg_addr_via == HI_NULL) {
                hi_log_error("iomap reg of module failed\n");
                module_addr_unmap();
                ret = HI_FAILURE;
                break;
            }
            hi_log_info("reg phy 0x%x, via 0x%p, size 0x%x\n", table->reg_addr_phy,
                        table->reg_addr_via, table->reg_addr_size);
        }
    }

    module_initialize = HI_TRUE;

    hi_log_func_exit();

    return ret;
}

/**
\brief  unmap the physics addr to cpu within the base table, contains the base addr and crg addr.
 */
hi_s32 module_addr_unmap(void)
{
    hi_u32 i;
    struct sys_arch_hardware_info *table = HI_NULL;

    hi_log_func_enter();

    if (module_initialize == HI_FALSE) {
        return HI_SUCCESS;
    }

    for (i = 0; i < CRYPTO_MODULE_ID_CNT; i++) {
        table = &hard_info_table[i];

        hi_log_info("[%d] %s\n", i, table->name);

        /* io unmap crg register */
        if (table->crg_valid && table->crg_addr_via) {
            hi_log_info("crg via addr 0x%p\n", table->crg_addr_via);
            crypto_iounmap(table->crg_addr_via, CRG_REG_ADDR_SIZE);
            table->crg_addr_via = HI_NULL;
        }

        /* io unmap module register */
        if (table->phy_valid && table->reg_addr_via) {
            hi_log_info("reg via addr 0x%p\n", table->reg_addr_via);
            crypto_iounmap(table->reg_addr_via, table->reg_addr_size);
            table->reg_addr_via = HI_NULL;
        }
    }

    module_initialize = HI_FALSE;

    hi_log_func_exit();
    return HI_SUCCESS;
}

/**
\brief  map the physics addr to cpu within the base table, contains the base addr and crg addr.
 */
hi_s32 module_addr_map(void)
{
    hi_s32 ret;

    hi_log_func_enter();

    ret = module_addr_map_reg();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(module_addr_map_reg, ret);
        return ret;
    }

#ifdef CRYPTO_SWITCH_CPU
    s_secure_cpu = drv_symc_is_secure();
    if (s_secure_cpu == HI_TRUE) {
        /* default secure cpu, do nothing */
        return HI_SUCCESS;
    }

    hi_log_info("non-secure CPU need to remap reg addr\n");

    /* use non-secure info */
    ret = memcpy_s(&hard_info_table, sizeof(hard_info_table),
                   &nsec_hard_info_table, sizeof(nsec_hard_info_table));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memcpy_s, ret);
        return ret;
    }

    /* remap module addr */
    ret = module_addr_unmap();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(module_addr_unmap, ret);
        return ret;
    }
    ret = module_addr_map_reg();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(module_addr_map, ret);
        return ret;
    }
#endif

    hi_log_func_exit();
    return HI_SUCCESS;
}

/**
\brief  get secure cpu type.
 */
hi_u32 module_get_secure(void)
{
#ifdef CRYPTO_SEC_CPU
    return HI_TRUE;
#elif defined(CRYPTO_SWITCH_CPU)
    return s_secure_cpu;
#else
    return HI_FALSE;
#endif
}

/**
\brief  open clock.
 */
static void module_clock(module_id id, hi_u32 open)
{
    hi_u32 val = 0;
    hi_u32 *addr = HI_NULL;
    struct sys_arch_hardware_info *table = HI_NULL;

    hi_log_func_enter();

    table = &hard_info_table[id];

    if (table->clk_valid) {
        if (table->crg_addr_via == HI_NULL) {
            hi_log_error("\033[0;1;31m"
                         "%s clock failed, crg addr is null\n"
                         "\033[0m",
                         table->name);
            return;
        }

        /* open clk, the addr may be need to add 1 */
        addr = table->crg_addr_via;
        addr += table->clk_bit / WORD_BIT_WIDTH;

        val = crypto_read((uintptr_t)addr);

        if (open) {
            SET_BIT(val, table->clk_bit % WORD_BIT_WIDTH);
        } else {
            CLEAR_BIT(val, table->clk_bit % WORD_BIT_WIDTH);
        }

        crypto_write((uintptr_t)addr, val);

        /* wait hardware clock active */
        crypto_udelay(1);

        hi_log_debug("%s clock, open   %d, bit %d, phy 0x%x, via 0x%p\n",
                     table->name, open, table->clk_bit,
                     table->crg_addr_phy, table->crg_addr_via);
    }

    hi_log_func_exit();

    return;
}

static void module_reset(module_id id, hi_u32 enable)
{
    hi_u32 val = 0;
    hi_u32 *addr = HI_NULL;
    hi_u32 expect = 0;
    struct sys_arch_hardware_info *table = HI_NULL;

    hi_log_func_enter();

    table = &hard_info_table[id];

    if (table->reset_valid) {
        if (table->crg_addr_via == HI_NULL) {
            hi_log_error("\033[0;1;31m"
                         "%s reset failed, crg addr is null\n"
                         "\033[0m",
                         table->name);
            return;
        }

        /* the addr may be need to add 1 */
        addr = table->crg_addr_via;
        addr += table->reset_bit / WORD_BIT_WIDTH;

        val = crypto_read((uintptr_t)addr);

        if (enable) {
            SET_BIT(val, table->reset_bit % WORD_BIT_WIDTH);
            expect = 0;
        } else {
            CLEAR_BIT(val, table->reset_bit % WORD_BIT_WIDTH);
            expect = table->version_val;
        }

        crypto_write((uintptr_t)addr, val);

        /* wait hardware reset finish */
        crypto_msleep(1);

        hi_log_debug("%s reset, enable %d, bit %d, phy 0x%x, via 0x%p\n",
                     table->name, enable, table->reset_bit,
                     table->crg_addr_phy, table->crg_addr_via);

        /* check the value of version reg to make sure reset success */
        if (table->ver_valid && table->reg_addr_via) {
            val = crypto_read((uintptr_t)(table->reg_addr_via + table->version_reg));
            if (val != expect) {
                hi_log_error("\033[0;1;31m"
                             "%s reset failed, version reg should be 0x%x but 0x%x\n"
                             "\033[0m",
                             table->name, expect, val);
                return;
            }

            hi_log_debug("%s version reg, offset 0x%x, expect val 0x%x, real val 0x%x\n",
                         table->name, table->version_reg, expect, val);
        }
    }

    hi_log_func_exit();

    return;
}

/**
\brief  enable a module, open clock  and remove reset signal.
 */
void module_enable(module_id id)
{
    module_clock(id, HI_TRUE);
    module_reset(id, HI_FALSE);
    return;
}

/**
\brief  disable a module, close clock and set reset signal.
 */
void module_disable(module_id id)
{
    module_reset(id, HI_TRUE);
    module_clock(id, HI_FALSE);
    return;
}

/**
\brief  get attribute of module.
 */
void module_get_attr(module_id id, hi_u32 *int_valid, hi_u32 *int_num, const char **name)
{
    *int_valid = hard_info_table[id].int_valid;
    *int_num = hard_info_table[id].int_num;
    if (name != HI_NULL) {
        *name = hard_info_table[id].name;
    }

    return;
}

void module_get_info(module_id id, hi_u32 *info, module_info_option option)
{
    *info = 0x00;

    switch (option) {
        case MODULE_INFO_RESET_VALID:
            *info = hard_info_table[id].reset_valid;
            break;
    }

    return;
}

/**
\brief  read a register.
 */
hi_u32 module_reg_read(module_id id, hi_u32 offset)
{
    hi_u32 val;
    void *addr = HI_NULL;
    hi_s32 ret;

    /* check if module is valid */
    if (id >= CRYPTO_MODULE_ID_CNT) {
        hi_log_error("error, invalid module id %d\n", id);
        return 0;
    }

    if (offset >= hard_info_table[id].reg_addr_size) {
        hi_log_error("module_reg_read error, module %d, reg offset overflow 0x%x\n", id, offset);
        hi_log_print_err_code(0);
        return 0;
        ;
    }

    /* tee may be read trng before cipher module init */
    if (hard_info_table[id].reg_addr_via == HI_NULL) {
        ret = module_addr_map();
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(module_addr_map, ret);
            return 0;
        }
    }

    /* get the absolute address of reg */
    addr = (hi_u8 *)(hard_info_table[id].reg_addr_via) + offset;
    val = crypto_read((uintptr_t)addr);

    return val;
}

/**
\brief  write a register.
 */
void module_reg_write(module_id id, hi_u32 offset, hi_u32 val)
{
    void *addr = HI_NULL;
    hi_s32 ret;

    /* check if module is valid */
    if (id >= CRYPTO_MODULE_ID_CNT) {
        hi_log_error("error, invalid module id %d\n", id);
        return;
    }

    /* check if offset is valid */
    if (offset >= hard_info_table[id].reg_addr_size) {
        hi_log_error("module_reg_write error, module %d, reg offset overflow 0x%x\n", id, offset);
        return;
    }

    /* tee may be read trng before cipher module init */
    if (hard_info_table[id].reg_addr_via == HI_NULL) {
        ret = module_addr_map();
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(module_addr_map, ret);
            return;
        }
    }

    /* get the absolute address of reg */
    addr = (hi_u8 *)hard_info_table[id].reg_addr_via + offset;
    crypto_write((uintptr_t)addr, val);

    return;
}

/**
\brief  Initialize the channel list.
 */
hi_s32 crypto_channel_init(channel_context *ctx, hi_u32 num, hi_u32 ctx_size)
{
    hi_u32 ret;
    hi_u32 size;
    hi_u32 i = 0;
    hi_u8 *buf = HI_NULL;

    hi_log_func_enter();

    crypto_assert(ctx != HI_NULL);

    /* clear context */
    size = sizeof(channel_context) * num;
    ret = memset_s(ctx, size, 0, size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    /* set context buffer */
    if (ctx_size > 0) {
        buf = (hi_u8 *)crypto_malloc(ctx_size * num);
        if (buf == HI_NULL) {
            return HI_ERR_CIPHER_FAILED_MEM;
        }
        ret = memset_s(buf, ctx_size * num, 0, ctx_size * num);
        if (ret != HI_SUCCESS) {
            crypto_free(buf);
            buf = HI_NULL;
            hi_log_print_func_err(memset_s, ret);
            return ret;
        }

        /* the buffer addresss is stored at ctx[0].ctx */
        for (i = 0; i < num; i++) {
            ctx[i].ctx = buf;
            buf += ctx_size;
        }
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/**
\brief  denit the channel list.
 */
hi_s32 crypto_channel_deinit(channel_context *ctx, hi_u32 num)
{
    hi_s32 ret;
    hi_u32 size;

    hi_log_func_enter();

    crypto_assert(ctx != HI_NULL);

    /* the buffer addresss is stored at ctx[0].ctx */
    if (ctx[0].ctx != HI_NULL) {
        crypto_free(ctx[0].ctx);
        ctx[0].ctx = HI_NULL;
    }

    /* clear context */
    size = sizeof(channel_context) * num;
    ret = memset_s(ctx, size, 0, size);
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/**
\brief  allocate a channel.
 */
hi_s32 crypto_channel_alloc(channel_context *ctx, hi_u32 num, hi_u32 mask, hi_u32 *id)
{
    hi_s32 ret = HI_ERR_CIPHER_BUSY;
    hi_u32 i = 0;

    hi_log_func_enter();

    crypto_assert(ctx != HI_NULL);
    crypto_assert(id != HI_NULL);

    for (i = 0; i < num; i++) {
        /* check the valid channel */
        if (mask & (0x01 << i)) {
            if (!ctx[i].open) { /* found a free channel */
                ret = HI_SUCCESS;
                ctx[i].open = HI_TRUE; /* alloc channel */
                *id = i;
                break;
            }
        }
    }

    if (i == num) {
        hi_log_error("error, all channels are busy.\n");
    }

    hi_log_func_exit();

    return ret;
}

/**
\brief  free a  channel.
 */
void crypto_channel_free(channel_context *ctx, hi_u32 num, hi_u32 id)
{
    hi_log_func_enter();

    /* free channel */
    ctx[id].open = HI_FALSE;

    hi_log_func_exit();
    return;
}

/**
\brief  get the private data of channel.
 */
void *crypto_channel_get_context(channel_context *ctx, hi_u32 num, hi_u32 id)
{
    if (ctx == HI_NULL) {
        hi_log_error("crypto_channel_get_context() ctx is HI_NULL\n");
        hi_log_print_err_code(0);
        return HI_NULL;
    }

    if ((id >= num) || (!ctx[id].open)) {
        hi_log_error("crypto_channel_get_context()- error, id %d, open %d, num %d\n",
                     id, ctx[id].open, num);
        hi_log_print_err_code(0);
        return HI_NULL;
    }

    return ctx[id].ctx;
}
/******* proc function begin ********/
#ifdef HI_PROC_SUPPORT
void hex2str(char *buf, hi_u8 val)
{
    hi_u8 high, low;

    high = (val >> 4) & 0x0F; /* high 4 bit */
    low = val & 0x0F;

    if (high <= 9) {  /* 0~9 */
        buf[0] = high + '0';
    } else {
        buf[0] = (high - 0x0A) + 'A'; /* 0x0A~0x0F */
    }

    if (low <= 9) {  /* 0~9 */
        buf[1] = low + '0';
    } else {
        buf[1] = (low - 0x0A) + 'A'; /* 0x0A~0x0F */
    }
}
#endif
/******* proc function end ********/

void *crypto_calloc(size_t n, size_t size)
{
    hi_s32 ret;
    void *ptr = HI_NULL;

    ptr = crypto_malloc(n * size);

    if (ptr != HI_NULL) {
        ret = memset_s(ptr, n * size, 0, n * size);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memset_s, ret);
            crypto_free(ptr);
            ptr = HI_NULL;
            return HI_NULL;
        }
    }

    return ptr;
}

hi_u32 get_rand(void)
{
    hi_u32 randnum = 0;

    cryp_trng_get_random(&randnum, -1);

    return randnum;
}

/** @} */ /** <!-- ==== API Code end ==== */
