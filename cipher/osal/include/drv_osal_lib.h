/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_osal_lib
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#ifndef __DRV_OSAL_LIB_H__
#define __DRV_OSAL_LIB_H__
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/spinlock.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/ion.h>
#include <linux/dma-buf.h>
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_drv_module.h"
#include "hi_drv_osal.h"
#include "hi_drv_dev.h"
#include "hi_drv_mem.h"
#include "hi_drv_module.h"
#include "hi_drv_proc.h"
#include "hi_osal.h"
#include "drv_osal_chip.h"
#include "drv_cipher_kapi.h"
#include "drv_cipher_ioctl.h"
#include "hi_errno.h"
#include "linux/hisilicon/securec.h"

/**< allow modules to modify, default value is HI_ID_STB, the general module id*/
#define LOG_D_MODULE_ID             HI_ID_CIPHER
#define LOG_D_FUNCTRACE             0
#define LOG_D_UNFTRACE              0
#include "hi_drv_log.h"

/*! \check uuid, but misamtch with file system verify  */
#define check_owner(local)

/*! \max length module name */
#define CRYPTO_MODULE_NAME_LEN            16

/*! \the max cipher hard channel count */
#define CRYPTO_HARD_CHANNEL_MAX         0x10

/*! \serure mmz or not, not used */
#define MEM_TYPE_MMZ                     0x00
#define MEM_TYPE_SMMU                    0x01

#define KLAD_KEY_SIZE_IN_WORD           4
#define KLAD_KEY_SIZE_IN_BYTE           16

#ifndef cipher_max
#define cipher_max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef cipher_min
#define cipher_min(a, b) ((a) < (b) ? (a) : (b))
#endif

/*! \Size of array */
#ifndef array_size
#define array_size(array)  (sizeof(array) / sizeof(array[0]))
#endif

#define crypto_unused(x)    ((x) = (x))

/*! \big coding transform to litte coding*/
#define cipher_cpu_to_be16(v) (((v)<<8) | ((v)>>8))
#define cipher_cpu_to_be32(v) ((((hi_u32)(v))>>24) | ((((hi_u32)(v))>>8)&0xff00) \
                     | ((((hi_u32)(v))<<8)&0xff0000) | (((hi_u32)(v))<<24))

#define cipher_cpu_to_be64(x) ((hi_u64)(                         \
    (((hi_u64)(x) & (hi_u64)0x00000000000000ffULL) << 56) |   \
    (((hi_u64)(x) & (hi_u64)0x000000000000ff00ULL) << 40) |   \
    (((hi_u64)(x) & (hi_u64)0x0000000000ff0000ULL) << 24) |   \
    (((hi_u64)(x) & (hi_u64)0x00000000ff000000ULL) <<  8) |   \
    (((hi_u64)(x) & (hi_u64)0x000000ff00000000ULL) >>  8) |   \
    (((hi_u64)(x) & (hi_u64)0x0000ff0000000000ULL) >> 24) |   \
    (((hi_u64)(x) & (hi_u64)0x00ff000000000000ULL) >> 40) |   \
    (((hi_u64)(x) & (hi_u64)0xff00000000000000ULL) >> 56)))

/*! \defined the base error code */
#define HI_BASE_ERR_BASE                (0x400)
#define HI_BASE_ERR_BASE_SYMC           (HI_BASE_ERR_BASE + 0x100)
#define HI_BASE_ERR_BASE_HASH           (HI_BASE_ERR_BASE + 0x200)
#define HI_BASE_ERR_BASE_RSA            (HI_BASE_ERR_BASE + 0x300)
#define HI_BASE_ERR_BASE_TRNG           (HI_BASE_ERR_BASE + 0x400)
#define HI_BASE_ERR_BASE_SM2            (HI_BASE_ERR_BASE + 0x500)

/*! \enumeration module_id */
typedef enum {
    CRYPTO_MODULE_ID_SYMC,        /*!<  Symmetric Cipher */
    CRYPTO_MODULE_ID_SYMC_KEY,    /*!<  Symmetric Cipher key */
    CRYPTO_MODULE_ID_HASH,        /*!<  Message Digest */
    CRYPTO_MODULE_ID_IFEP_RSA,    /*!<  Asymmetric developed by IFEP */
    CRYPTO_MODULE_ID_SIC_RSA,     /*!<  Asymmetric developed by SIC */
    CRYPTO_MODULE_ID_TRNG,        /*!<  Random Data Generation */
    CRYPTO_MODULE_ID_PKE,         /*!<  Public Key Cryptographic Algorithm Based on Elliptic Curves */
    CRYPTO_MODULE_ID_SM4,         /*!<  SM4 */
    CRYPTO_MODULE_ID_SMMU,        /*!<  SMMU */
    CRYPTO_MODULE_ID_CNT,         /*!<  Count of module id */
} module_id;

/*! \struct channel
 * the context of hardware channel.
*/
typedef struct {
    /* the state of instance, open or closed. */
    hi_u32 open;

    /* the context of channel, which is defined by specific module */
    hi_void *ctx;
} channel_context;

/*! \struct of crypto_mem */
typedef struct {
    compat_addr dma_addr;    /*!<  dam addr, may be mmz or smmu */
    void *kernel_dma;
    hi_void *dma_virt;         /*!<  cpu virtual addr maped from dam addr */
    hi_u32 dma_size;           /*!<  dma memory size */
    hi_void *user_buf;         /*!<  buffer of user */
    hi_u32 mem_type;           /*!<  buffer of type */
} crypto_mem;

typedef enum {
    MODULE_INFO_RESET_VALID,
} module_info_option;

/** @} */  /** <!-- ==== Structure Definition end ==== */

/*! \****************************** API Declaration *****************************/
/*! \addtogroup    osal lib */
/** @{ */  /** <!--[osal] */

/**
\brief  init dma memory.
*/
hi_void crypto_mem_init(hi_void);

/**
\brief  deinit dma memory.
*/
hi_void crypto_mem_deinit(hi_void);

/**
\brief  allocate and map a dma memory.
\param[in] mem  The struct of crypto_mem.
\param[in] size The size of mem.
\param[in] name The name of mem.
\return         HI_SUCCESS if successful, or HI_BASE_ERR_MALLOC_FAILED.
*/
hi_s32 crypto_mem_create(crypto_mem *mem, hi_u32 type, const char *name, hi_u32 size);

/**
\brief  destory and unmap a dma memory.
\param[in] mem  The struct of crypto_mem.
\return         0 if successful, or HI_BASE_ERR_UNMAP_FAILED.
*/
hi_s32 crypto_mem_destory(crypto_mem *mem);

/**
\brief  map a dma memory.
\param[in] mem  The struct of crypto_mem.
\param[in] dma_ddr The address of dma mem.
\param[in] dma_size The size of dma mem.
\return         HI_SUCCESS if successful, or HI_BASE_ERR_MAP_FAILED.
*/
hi_s32 crypto_mem_open(crypto_mem *mem, compat_addr dma_ddr, hi_u32 dma_size);

/**
\brief  unmap a dma memory.
\param[in] mem  The struct of crypto_mem.
\param[in] dma_ddr The address of dma mem.
\return         HI_SUCCESS if successful, or HI_BASE_ERR_UNMAP_FAILED.
*/
hi_s32 crypto_mem_close(crypto_mem *mem);

/**
\brief  attach a cpu buffer with dma memory.
\param[in] mem  The struct of crypto_mem.
\param[in] buffer The user's buffer.
\return         HI_SUCCESS if successful, or HI_FAILURE.
*/
hi_s32 crypto_mem_attach(crypto_mem *mem, hi_void *buffer);

/**
\brief  flush dma memory,
\param[in] mem The struct of crypto_mem.
\param[in] dma2user 1-data from dma to user, 0-data from user to dma.
\param[in] offset The offset of data to be flush.
\param[in] data_size The size of data to be flush.
\return         HI_SUCCESS if successful, or HI_FAILURE.
*/
hi_s32 crypto_mem_flush(crypto_mem *mem, hi_u32 dma2user, hi_u32 offset, hi_u32 data_size);

/**
\brief  get dma memory physical address
\param[in] mem The struct of crypto_mem.
\return         HI_SUCCESS if successful, or HI_FAILURE.
*/
hi_s32 crypto_mem_phys(crypto_mem *mem, compat_addr *dma_addr);

/**
\brief  get dma memory virtual address
\param[in] mem The struct of crypto_mem.
\return         dma_addr if successful, or zero.
*/
hi_void *crypto_mem_virt(crypto_mem *mem);

/**
\brief  try to create max dma memory.
\param[in] name The name of crypto_mem.
\param[in] type The type of crypto_mem.
\param[in] max The max size of crypto_mem.
\param[out] mem The struct of crypto_mem.
\return         HI_SUCCESS if successful, or HI_FAILURE.
*/
hi_s32 crypto_mem_try_create_max(const char *name, hi_u32 type, hi_u32 max, crypto_mem *mem);

/**
\brief  check whether cpu is secure or not.
\retval secure cpu, true is returned otherwise false is returned.
*/
hi_u32 crypto_is_sec_cpu(hi_void);

/**
\brief  load symc clean key from klad.
\retval NA.
*/
hi_void crypto_load_symc_clean_key(hi_u32 chn_num, hi_u32 key[AES_IV_SIZE], hi_u32 odd);

/**
\brief  map the physics addr to cpu within the base table, contains the base addr and crg addr.
\retval    on success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned. */
hi_s32 module_addr_map(hi_void);

/**
\brief  unmap the physics addr to cpu within the base table, contains the base addr and crg addr.
\retval    on success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned. */
hi_s32 module_addr_unmap(hi_void);

/**
\brief  get secure cpu type.
*/
hi_u32 module_get_secure(hi_void);

/**
\brief  enable a module, open clock  and remove reset signal.
\param[in]  id The module id.
\retval    NA */
hi_void module_enable(module_id id);

/**
\brief  disable a module, close clock and set reset signal.
\param[in] id The module id.
\retval    NA */
hi_void module_disable(module_id id);

/**
\brief  get attribute of module.
\param[in]  id The module id.
\param[out] int_valid enable interrupt or not.
\param[out] int_num interrupt number of module.
\param[out] name name of module.
\retval    NA. */
hi_void module_get_attr(module_id id, hi_u32 *int_valid, hi_u32 *int_num, const char **name);

/**
\brief  get info of module.
\param[in]  id The module id.
\param[out] info info of module.
\param[in]  option .
\retval    NA. */
hi_void module_get_info(module_id id, hi_u32 *info, module_info_option option);

/**
\brief  read a register.
\param[in]  id The module id.
\param[in]  offset The module id.
\retval    the value of register */
hi_u32 module_reg_read(module_id id, hi_u32 offset);

/**
\brief  hex to string.
\param[in]  buf The string buffer.
\param[in]  val The value of hex.
\retval    NA */
hi_void hex2str(char *buf, hi_u8 val);

/**
\brief  write a register.
\param[in]  id The module id.
\retval    NA */
hi_void module_reg_write(module_id id, hi_u32 offset, hi_u32 val);

/* cipher module read and write a register */
#define symc_read(offset)         module_reg_read(CRYPTO_MODULE_ID_SYMC, offset)
#define symc_write(offset, val)   module_reg_write(CRYPTO_MODULE_ID_SYMC, offset, val)

/* hash module read and write a register */
#define hash_read(offset)         module_reg_read(CRYPTO_MODULE_ID_HASH, offset)
#define hash_write(offset, val)   module_reg_write(CRYPTO_MODULE_ID_HASH, offset, val)

/* rsa module read and write a register */
#define ifep_rsa_read(offset)       module_reg_read(CRYPTO_MODULE_ID_IFEP_RSA, offset)
#define ifep_rsa_write(offset, val) module_reg_write(CRYPTO_MODULE_ID_IFEP_RSA, offset, val)

/* trng module read and write a register */
#define trng_read(offset)         module_reg_read(CRYPTO_MODULE_ID_TRNG, offset)
#define trng_write(offset, val)   module_reg_write(CRYPTO_MODULE_ID_TRNG, offset, val)

/* sm2 module read and write a register */
#define pke_read(offset)         module_reg_read(CRYPTO_MODULE_ID_PKE, offset)
#define pke_write(offset, val)   module_reg_write(CRYPTO_MODULE_ID_PKE, offset, val)

/* smmu module read and write a register */
#define smmu_read(offset)         module_reg_read(CRYPTO_MODULE_ID_SMMU, offset)
#define smmu_write(offset, val)   module_reg_write(CRYPTO_MODULE_ID_SMMU, offset, val)

/**
\brief  Initialize the channel list.
\param[in]  ctx The context of channel.
\param[in]  num The channel numbers, max is 32.
\param[in]  ctx_size The size of context.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 crypto_channel_init(channel_context *ctx, hi_u32 num, hi_u32 ctx_size);

/**
\brief  Deinitialize the channel list.
\param[in]  ctx The context of channel.
\param[in]  num The channel numbers, max is 32.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 crypto_channel_deinit(channel_context *ctx, hi_u32 num);

/**
\brief  allocate a channel.
\param[in]  ctx The context of channel.
\param[in]  num The channel numbers, max is 32.
\param[in]  mask Mask whick channel allowed be alloc, max is 32.
\param[out] id The id of channel.
\retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_s32 crypto_channel_alloc(channel_context *ctx, hi_u32 num, hi_u32 mask, hi_u32 *id);

/**
\brief  free a channel.
\param[in]  ctx The context of channel.
\param[in]  num The channel numbers, max is 32.
\param[in] id The id of channel.
\retval    on success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
*/
hi_void crypto_channel_free(channel_context *ctx, hi_u32 num, hi_u32 id);

/**
\brief  get the private data of hard channel.
\param[in] ctx The context of channel.
\param[in] num The channel numbers, max is 32.
\param[in] id The id of channel.
\retval    on success, the address of context is returned.  On error, NULL is returned..
*/
hi_void *crypto_channel_get_context(channel_context *ctx, hi_u32 num, hi_u32 id);

/**
\brief  get the rang.
\retval    random number.
*/
hi_u32 get_rand(hi_void);

/*
 * brief  Trans bufhandle to phys.
 * retval phyaddr.
 */
hi_u64 crypto_bufhandle_to_phys(hi_cipher_mem_handle bufhandle);


/*
 * brief  Wait queue callback func.
 * retval if condition match, return HI_TRUE.
 */
hi_s32 crypto_queue_callback_func(const void *param);

#define crypto_iomap

#define crypto_ioremap_nocache(addr, size)  osal_ioremap_nocache(addr, size)
#define crypto_iounmap(addr, size)          osal_iounmap(addr)

#define crypto_read(addr)         (*(volatile unsigned int *)(addr))
#define crypto_write(addr, val)   (*(volatile unsigned int *)(addr) = (val))

#define crypto_msleep(msec)         osal_msleep_uninterruptible(msec)
#define crypto_udelay(msec)         osal_udelay(msec)

#define MAX_MALLOC_BUF_SIZE         0x10000
hi_void *crypto_calloc(size_t n, size_t size);
#define crypto_malloc(x)          ((x > 0) ? HI_KMALLOC(HI_ID_CIPHER, (x), GFP_KERNEL) : HI_NULL)
#define crypto_free(x)            {if ((x != HI_NULL)) HI_KFREE(HI_ID_CIPHER, (x));}

hi_s32 crypto_copy_from_user(hi_void *to, unsigned long to_len, const hi_void *from, unsigned long from_len);
hi_s32 crypto_copy_to_user(hi_void  *to, unsigned long to_len, const hi_void *from, unsigned long from_len);
hi_u32 get_rand(void);

/* ARM Memory barrier */
#ifdef CONFIG_ARCH_LP64_MODE
#define arm_memory_barrier()  do { osal_mb(); osal_isb(); osal_dsb();} while (0)
#else
#define arm_memory_barrier()  do { osal_mb(); osal_isb(); osal_dsb(); } while (0)

#endif

#define crypto_queue_head                          osal_wait
#define crypto_queue_init(x)                       osal_wait_init(x)
#define crypto_queue_wake_up(x)                    osal_wait_wakeup(x)
#define crypto_queue_wait_timeout(head, func, param, time) osal_wait_timeout_interruptible(head, func, param, time)

#define crypto_request_irq(irq, func, name) hi_drv_osal_request_irq((irq + 32), func, IRQF_SHARED, name, "crypto")
#define crypto_free_irq(irq, name)          hi_drv_osal_free_irq((irq + 32), name, "crypto")

typedef osal_semaphore                       crypto_mutex;
#define crypto_mutex_init(x)                 osal_sem_init(x, 1)
#define crypto_mutex_lock(x)                 osal_sem_down_interruptible(x)
#define crypto_mutex_unlock(x)               osal_sem_up(x)
#define crypto_mutex_destroy(x)              osal_sem_destory(x)

#define flush_dcache_area(start, size)             __flush_dcache_area(start, size)

#define crypto_owner                         pid_t
/* task_tgid_nr(current): Thread group ID
 * current->pid         : Process ID
 */
#define crypto_get_owner(x)                  *x = osal_get_pid()

hi_void crypto_zeroize(hi_void *buf, hi_u32 len);
hi_void smmu_get_table_addr(hi_ulong *rdaddr, hi_ulong *wraddr, hi_ulong *table);
#define LOG_D_MODULE_ID             HI_ID_CIPHER
#define LOG_D_FUNCTRACE             0
#define LOG_D_UNFTRACE              0

/* allow modules to define internel error code, from 0x1000 */
#define log_err_code_def(errid)      (hi_u32)(((LOG_D_MODULE_ID) << 16)  | (errid))

/* General Error Code, All modules can extend according to the rule */
#define HI_LOG_ERR_MEM              log_err_code_def(0x0001)      /* Memory Operation Error */
#define HI_LOG_ERR_SEM              log_err_code_def(0x0002)      /* Semaphore Operation Error */
#define HI_LOG_ERR_FILE             log_err_code_def(0x0003)      /* File Operation Error */
#define HI_LOG_ERR_LOCK             log_err_code_def(0x0004)      /* Lock Operation Error */
#define HI_LOG_ERR_PARAM            log_err_code_def(0x0005)      /* Invalid Parameter */
#define HI_LOG_ERR_TIMER            log_err_code_def(0x0006)      /* Timer error */
#define HI_LOG_ERR_THREAD           log_err_code_def(0x0007)      /* Thread Operation Error */
#define HI_LOG_ERR_TIMEOUT          log_err_code_def(0x0008)      /* Time Out Error */
#define HI_LOG_ERR_DEVICE           log_err_code_def(0x0009)      /* Device Operation Error */
#define HI_LOG_ERR_STATUS           log_err_code_def(0x0010)      /* Status Error */
#define HI_LOG_ERR_IOCTRL           log_err_code_def(0x0011)      /* IO Operation Error */
#define HI_LOG_ERR_INUSE            log_err_code_def(0x0012)      /* In use */
#define HI_LOG_ERR_EXIST            log_err_code_def(0x0013)      /* Have exist */
#define HI_LOG_ERR_NOEXIST          log_err_code_def(0x0014)      /* no exist */
#define HI_LOG_ERR_UNSUPPORTED      log_err_code_def(0x0015)      /* Unsupported */
#define HI_LOG_ERR_UNAVAILABLE      log_err_code_def(0x0016)      /* Unavailable */
#define HI_LOG_ERR_UNINITED         log_err_code_def(0x0017)      /* Uninited */
#define HI_LOG_ERR_DATABASE         log_err_code_def(0x0018)      /* Database Operation Error */
#define HI_LOG_ERR_OVERFLOW         log_err_code_def(0x0019)      /* Overflow */
#define HI_LOG_ERR_EXTERNAL         log_err_code_def(0x0020)      /* External Error */
#define HI_LOG_ERR_UNKNOWNED        log_err_code_def(0x0021)      /* Unknow Error */
#define HI_LOG_ERR_FLASH            log_err_code_def(0x0022)      /* Flash Operation Error*/
#define HI_LOG_ERR_ILLEGAL_IMAGE    log_err_code_def(0x0023)      /* Illegal Image */
#define HI_LOG_ERR_ILLEGAL_UUID     log_err_code_def(0x0023)      /* Illegal UUID */
#define HI_LOG_ERR_NOPERMISSION     log_err_code_def(0x0023)      /* No Permission */

#define hi_log_fatal(fmt...)        LOG_FATAL_PRINT(fmt)
#define hi_log_error(fmt...)        LOG_ERR_PRINT(fmt)
#define hi_log_warn(fmt...)         LOG_WARN_PRINT(fmt)
#define hi_log_info(fmt...)         LOG_INFO_PRINT(fmt)
#define hi_log_debug(fmt...)        LOG_DBG_PRINT(fmt)

/* Function trace log, strictly prohibited to expand */
#define hi_log_print_func_war(Func, ErrCode)   HI_WARN_PRINT_FUNC_RES(Func, ErrCode)
#define hi_log_print_func_err(Func, ErrCode)   HI_ERR_PRINT_FUNC_RES(Func, ErrCode)
#define hi_log_print_err_code(ErrCode)         HI_ERR_PRINT_ERR_CODE(ErrCode)

/* Used for displaying more detailed error information */
#define hi_log_print_s32(val)                     HI_INFO_PRINT_S32(val)
#define hi_log_print_u32(val)                     HI_INFO_PRINT_U32(val)
#define hi_log_print_s64(val)                     HI_INFO_PRINT_S64(val)
#define hi_log_print_u64(val)                     HI_INFO_PRINT_U64(val)
#define hi_log_print_h32(val)                     HI_INFO_PRINT_H32(val)
#define hi_log_print_h64(val)                     HI_INFO_PRINT_H64(val)
#define hi_log_print_str(val)                     HI_INFO_PRINT_STR(val)
#define hi_log_print_void(val)                    HI_INFO_PRINT_VOID(val)
#define hi_log_print_float(val)                   HI_INFO_PRINT_FLOAT(val)
#define hi_log_print_info(val)                    HI_INFO_PRINT_INFO(val)

#define hi_log_func_enter()                  HI_FUNC_ENTER()
#define hi_log_func_exit()                   HI_FUNC_EXIT()
#define hi_log_check(fnFunc)                HI_CHECK(fnFunc)

#define hi_log_check_param(bVal)                            \
    do                                                      \
    {                                                       \
        if (bVal)                                           \
        {                                                   \
            hi_log_print_err_code(HI_LOG_ERR_PARAM);          \
            return HI_LOG_ERR_PARAM;                        \
        }                                                   \
    } while (0)


#define hi_log_check_inited(init_count)                     \
    do                                                      \
    {                                                       \
        if (init_count == 0)                                \
        {                                                   \
            hi_log_print_err_code(HI_LOG_ERR_UNINITED);       \
            return HI_LOG_ERR_UNINITED;                     \
        }                                                   \
    } while (0)

#define hi_log_check_length(_check_length_val)                  \
    do                                                          \
    {                                                           \
        if (_check_length_val)                                  \
        {                                                       \
            hi_log_print_err_code(HI_ERR_CIPHER_INVALID_LENGTH);  \
            return HI_ERR_CIPHER_INVALID_LENGTH;                \
        }                                                       \
    } while (0)

#define hi_log_print_block(data, length)                HI_DBG_PrintBlock(data, length)

/*! \assert */
#define crypto_assert(expr) \
    do { \
        if (!(expr)) { \
            /* hi_log_error("assertion '%s' failed\n", #expr); */ \
            /* hi_log_error("at %s:%d (func '%s')\n", __FILE__, __LINE__, __func__); */ \
            return HI_ERR_CIPHER_INVALID_PARA; \
        } \
    } while (0)

#define check_exit(expr) \
    do { \
        if ((ret = expr != HI_SUCCESS)) { \
            hi_log_print_func_err(expr, ret); \
            goto exit__; \
        } \
    } while (0)

#define hi_print_hex(name, str, len)                   HI_DBG_PrintBlock(str, len)

#endif  /* End of #ifndef __HI_DRV_CIPHER_H__ */
