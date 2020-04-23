/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: adsp audsp common header file
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#ifndef __HI_AUDSP_COMMON_H__
#define __HI_AUDSP_COMMON_H__

#include "hi_drv_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(__XTENSA__)
/* only use these in dsp firmware */
#ifndef typeof
#define typeof __typeof__
#endif

/*
 * @Macro: offsetof(type, member)
 *
 * @type: the type of the structure
 * @member: the member within the structure to get the offset of
 */
#ifndef offsetof
#define offsetof(type, member) ((unsigned int)&((type *)0)->member)
#endif

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#ifndef container_of
#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member) *__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)  (sizeof(array) / sizeof(array[0]))
#endif

#endif

/*************** HIFI DSP subsystem SOC definition ******************/
#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#ifdef HI_FPGA
#define DSP0_CLOCK_HZ (12 * 1000 * 1000) /* 12M FPGA */
#else
#define DSP0_CLOCK_HZ (710 * 1000 * 1000) /* 700M asic */
#endif
#else
#error "YOU MUST DEFINE CHIP_TYPE!"
#endif

#ifdef HI_FPGA
#define DSP_ACK_TIME 500
#else
#define DSP_ACK_TIME 100
#endif

#define DSP0_TIMER0_INTERVAL      (DSP0_CLOCK_HZ / 200) /* 5 ms */
#define DSP0_TIMER1_INTERVAL      (DSP0_CLOCK_HZ / 200) /* 5 ms */
#define DSP0_TIMER0_INTERVAL_CAST (DSP0_CLOCK_HZ / 200) /* 5 ms */
#define DSP0_TIMER1_INTERVAL_CAST (DSP0_CLOCK_HZ / 200) /* 5 ms */

#define DIV_COUNT    1000
#define GAIN_STEP 125
#define ADSP_PERCENTAGE_RATIO 100
#define ADSP_MS_PER_SECOND 1000

/* arm interrupt definition */
#define ADSP0TOA9_IRQ_NUM (51)

/* dsp interrupt definition */
typedef enum {
    HI_DSP_INTERRUPT_SWI0PERI = 0x0,  /* secue swi interrupt */
    HI_DSP_INTERRUPT_SWI1PERI = 0x1,  /* secue swi interrupt */
    HI_DSP_INTERRUPT_SWI2PERI = 0x2,  /* secue swi interrupt */
    HI_DSP_INTERRUPT_SWI3PERI = 0x3,  /* secue swi interrupt */

    HI_DSP_INTERRUPT_SWI_UNSEC = 0x4, /* unsecue swi interrupt start with SWI4PERI */
    HI_DSP_INTERRUPT_SWI4PERI = 0x4,  /* unsecue swi interrupt */
    HI_DSP_INTERRUPT_SWI5PERI = 0x5,  /* unsecue swi interrupt */
    HI_DSP_INTERRUPT_SWI6PERI = 0x6,  /* unsecue swi interrupt */
    HI_DSP_INTERRUPT_SWI7PERI = 0x7,  /* unsecue swi interrupt */

    HI_DSP_INTERRUPT_TIMER0 =   0x8,  /* dsp internal interrupt */
    HI_DSP_INTERRUPT_SOFT1  =   0x9,  /* dsp internal interrupt */
    HI_DSP_INTERRUPT_TIMER1 =   0xa,  /* dsp internal interrupt */
    HI_DSP_INTERRUPT_SOFT2  =   0xb,  /* dsp internal interrupt */
    HI_DSP_INTERRUPT_WRITE_ERR = 0xc, /* dsp internal interrupt */
    HI_DSP_INTERRUPT_TIMER2 =   0xd,  /* dsp internal interrupt */

    HI_DSP_INTERRUPT_MAD    =   0xe,  /* 15 */
    HI_DSP_INTERRUPT_MAD_RX =   0xf,  /* 16 */

    HI_DSP_INTERRUPT_UART0   =  0x10,
    HI_DSP_INTERRUPT_WDG_DSP =  0x11,

    HI_DSP_INTERRUPT_AIAO    =  0x12,
    HI_DSP_INTERRUPT_REV     =  0x13,
    HI_DSP_INTERRUPT_EDMA    =  0x14,

    HI_DSP_INTERRUPT_DSP2DSP =  0x15, /* dsp2dsp_int */
    HI_DSP_INTERRUPT_ARM2DSP =  0x16,

    HI_DSP_INT_ALL           = 0xffffffff,
} hi_dsp_int_type;

/* DSP on-chip sram memory addr definition */
#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define DSP0_SHARESRAM_BASEADDR 0x00E00000  /* 96cv300 adsp DRAM addr */
#else
#define DSP0_SHARESRAM_BASEADDR 0xffff5000
#endif

/* ADSP control register */
#define REG_ADSP_SMMU    0x00E81000
#define REG_ADSP_SECUE   0x00E82000
#define REG_ADSP_UNSECUE 0x00E83000

#define DSP0_SHARESRAM_MAXSIZE        0x01000
#define DSP0_SHARESRAM_AOE_OFFSET     0x0000
#define DSP0_SHARESRAM_AFLT_OFFSET    0x0b00
#define DSP0_SHARESRAM_SYS_OFFSET     0x0f00
#define DSP0_SHARESRAM_SYS_SIZE       0x0040
#define DSP0_SHARESRAM_CHAN0_BASEADDR (DSP0_SHARESRAM_BASEADDR + DSP0_SHARESRAM_SYS_OFFSET)
#define DSP0_SHARESRAM_CHAN1_BASEADDR (DSP0_SHARESRAM_BASEADDR + DSP0_SHARESRAM_SYS_OFFSET + DSP0_SHARESRAM_SYS_SIZE)

#define DSP0_SHARESRAM_RESERVESIZE (0x00100)
#define DSP0_SHARESRAM_ALIGN       (128)

/* DSP running code&data DDR memory addr definition */
#define DSP_DDR_CODE_BASEADDR 0x16f00000 /* 111 - 127M */
#define DSP_DDR_CODE_MAXSIZE  0x800000

#if defined(__XTENSA__) && !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
#define DSP_DDR_CACHE_DMA_SUPPORT /* default */
#endif

/* #define DSP_DDR_DMAREMAP_SUPPORT */
#if defined(DSP_DDR_CACHE_DMA_SUPPORT)
/* DSP DMA buffer is cacheable, dsp invalidate dcache before read dma & dsp writeback dcache after write dma */
#undef DSP_DDR_DMAREMAP_SUPPORT
#elif defined(DSP_DDR_DMAREMAP_SUPPORT)
/* DSP DMA buffer is uncacheable, remap DMA ddr memory to DSP_DDR_DMAREMAP_MAP_ADDR avoid conflict with
dsp running(code&data) memory DSP_DDR_DMAREMAP_BEG_ADDR~DSP_DDR_DMAREMAP_END_ADDR */
#undef DSP_DDR_CACHE_DMA_SUPPORT
#else
/* DSP DMA buffer & running(code&data) memory is uncache */
#undef DSP_DDR_CACHE_DMA_SUPPORT
#undef DSP_DDR_DMAREMAP_SUPPORT
#endif

#define DSP_DDR_DMAREMAP_BEG_ADDR 0x00000000 /* 512M,(0x00000000~0x1fffffff) */
#define DSP_DDR_DMAREMAP_END_ADDR 0x20000000 /* 512M, 0x20000000 */
#define DSP_DDR_DMAREMAP_MAP_ADDR 0xc0000000 /* 6*512M，must sure dsp never use this phy addr(0xc0000000~0xdfffffff) */

#define DSP_DEBUG_REG_NUM     4
#define DSP_PRIVDEBUG_REG_NUM 4

/* define the union U_ADSP_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int dis_lowpower : 1; /* [0] */
        unsigned int reserved_2 : 3;   /* [3..1] */
        unsigned int fsm : 4;          /* [7..4] */
        unsigned int reserved_1 : 24;  /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ADSP_ATTR;

/* define the union U_ADSP_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aoe_percent : 8;  /* [7..0] */
        unsigned int aflt_percent : 8; /* [15..8] */
        unsigned int idle_percent : 8; /* [23..16] */
        unsigned int reserved : 8;     /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ADSP_STATISTICS;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int sys_cmd : 2;              /* [1..0] */
        unsigned int reserved_0 : 1;           /* [2] */
        unsigned int sys_cmd_done : 1;         /* [3] */
        unsigned int sys_cmd_return_value : 4; /* [7..4] */

        unsigned int aoe_cmd : 2;              /* [9..8] */
        unsigned int reserved_1 : 1;           /* [10] */
        unsigned int aoe_cmd_done : 1;         /* [11] */
        unsigned int aoe_cmd_return_value : 4; /* [15..12] */

        unsigned int aflt_cmd : 2;              /* [17..16] */
        unsigned int reserved_2 : 1;            /* [18] */
        unsigned int aflt_cmd_done : 1;         /* [19] */
        unsigned int aflt_cmd_return_value : 4; /* [23..20] */

        unsigned int reserved_3 : 8; /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ADSP_CTRL;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int day : 8;      /* [7..0] */
        unsigned int month : 8;    /* [15..8] */
        unsigned int year : 8;     /* [23..16] */
        unsigned int reserved : 8; /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ADSP_DATE;

typedef union {
    /* define the struct bits */
    struct {
        unsigned int second : 8;   /* [7..0] */
        unsigned int minute : 8;   /* [15..8] */
        unsigned int hour : 8;     /* [23..16] */
        unsigned int reserved : 8; /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ADSP_TIME;

/* define the global struct */
typedef struct {
    volatile U_ADSP_ATTR ADSP_ATTR;
    volatile U_ADSP_STATISTICS ADSP_STATISTICS;
    volatile U_ADSP_CTRL ADSP_SYSCTRL;
    volatile unsigned int ADSP_TIMER1_CNT;
    volatile unsigned int ADSP_TIMER0_CNT; /* reserved */
    volatile U_ADSP_DATE ADSP_DATE;
    volatile U_ADSP_TIME ADSP_TIME;
    volatile unsigned int ADSP_DEBUG[DSP_DEBUG_REG_NUM];
    volatile unsigned int ADSP_PRIVDEBUG[DSP_PRIVDEBUG_REG_NUM];
    volatile unsigned int ADSP_RESERVED[5 - DSP_PRIVDEBUG_REG_NUM];
} adsp_chn_regs_type;

/* audio DSP code definition */
/* CNcomment:音频处理器模块代码标识定义 */
typedef enum {
    /* dsp manage module */
    ADSP_CODE_SYSTEM = 0x0000, /* dsp system */
    ADSP_CODE_AOE,             /* audio output engine */
    ADSP_CODE_AFLT,            /* audio aflt engine */

    /* dsp decoder module */
    ADSP_CODE_DEC_MP2 = 0x0100, /* MPEG audio layer 1, 2 */
    ADSP_CODE_DEC_MP3,          /* MPEG audio layer 1, 2 or 3 */
    ADSP_CODE_DEC_AAC,
    ADSP_CODE_DEC_DDP,
    ADSP_CODE_DEC_DTS,
    ADSP_CODE_DEC_TRUEHD,
    ADSP_CODE_DEC_WMASTD,
    ADSP_CODE_DEC_WMAPRO,
    ADSP_CODE_DEC_DRA,

    /* dsp encoder module */
    ADSP_CODE_ENC_AAC = 0x200,

    /* dsp codec module */
    ADSP_CODE_CODEC_AMRNB = 0x400,
    ADSP_CODE_CODEC_AMRWB,

    /* dsp SRS advance effect */
    ADSP_CODE_ADV_SRS_STUDIOSOUND_3D = 0x800,
    ADSP_CODE_ADV_SRS_STUDIOSOUND_HD = 0x800,

    /* dsp dolby advance effect */
    ADSP_CODE_ADV_DOLBYDV258 = 0x810,
} adsp_code_id;

typedef struct {
    hi_bool aoe_sw_flag;
    hi_void *aoe_reg_vir_addr;
    hi_u64  aoe_reg_phy_addr;
} aoe_fw_info;

typedef struct {
    hi_u32 cmd;       /* start, stop, flush, add mute, set speed, etc */
    hi_u32 cmd_type;  /* this cmd is assumed to sent to engine or aip or aop */
    hi_u32 id;        /* engine id, aip id, aop id, etc */
    hi_u32 size;      /* argv size */
} adsp_cmd_argc;

typedef struct {
    adsp_cmd_argc argc;
    hi_char *argv;
} adsp_cmd_arg;

typedef enum {
    ADSP_MEMCPY_DSP2DSP = 0x0,
    ADSP_MEMCPY_DSP2ARM,
    ADSP_MEMCPY_ARM2DSP,
    ADSP_MEMCPY_ARM2ARM
} adsp_memcpy_dir;

typedef struct {
    volatile unsigned int ADDR_LOW;
    volatile unsigned int ADDR_HIGH;
} aoe_addr_type;

/*
 * 作用: adsp_write_addr将地址写入寄存器，adsp_read_addr从寄存器中取出地址
 * addr支持类型: 1、hi_void* 2、32位物理地址 3、内核虚拟地址(32位系统中为32bit，64位系统中为64bit)
 * reg必须为aoe_addr_type结构体类型
 */
/* 使用中间变量传递64位虚拟地址，避免出现强制使用时的对齐问题 */
#define adsp_write_addr(addr, reg)            \
    do {                                      \
        aoe_addr_type reg_tmp = { 0x0, 0x0 }; \
        *((typeof(addr) *)(&reg_tmp)) = addr; \
        (reg).ADDR_LOW = reg_tmp.ADDR_LOW;      \
        (reg).ADDR_HIGH = reg_tmp.ADDR_HIGH;    \
    } while (0)

#define adsp_read_addr(reg, addr)                   \
    do {                                            \
        aoe_addr_type reg_tmp = { 0x0, 0x0 };       \
        reg_tmp.ADDR_LOW = (reg).ADDR_LOW;            \
        reg_tmp.ADDR_HIGH = (reg).ADDR_HIGH;          \
        (addr) = *((typeof(addr) *)(&reg_tmp)); \
    } while (0)

/* define the union U_AIAO_INT_SWI_CPU2DSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_swi_cpu2dsp : 1; /* [0] */
        unsigned int reserved_0 : 31;     /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_SWI_CPU2DSP;

/* define the union U_AIAO_INT_SWI_DSP2CPU */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_swi_dsp2cpu : 1; /* [0] */
        unsigned int reserved_0 : 31;     /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_SWI_DSP2CPU;

/* define the union U_AIAO_INT_SWI0_DSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_swi0_dsp : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_SWI0_DSP;

/* define the union U_AIAO_INT_SWI1_DSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_swi1_dsp : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_SWI1_DSP;

/* define the union U_AIAO_INT_SWI2_DSP */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int int_swi2_dsp : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_INT_SWI2_DSP;

/* define the union U_AIAO_DSP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 8;          /* [7..0] */
        unsigned int runstall_dsp0 : 1;       /* [8] */
        unsigned int statvectorsel_dsp0 : 1;  /* [9] */
        unsigned int ocdhaltonreset_dsp0 : 1; /* [10] */
        unsigned int reserved_1 : 1;          /* [11] */
        unsigned int wdg1_dsp0 : 1;           /* [12] */
        unsigned int syn_dbg_sel_dsp0 : 1;    /* [13] */
        unsigned int reserved_2 : 18;         /* [31..14] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_DSP_CTRL;

/* define the union U_AIAO_DSP_STATUS */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int reserved_0 : 2;          /* [1..0] */
        unsigned int peri_pwaitmode_dsp0 : 1; /* [2] */
        unsigned int peri_xocdmode_dsp0 : 1;  /* [3] */
        unsigned int reserved_1 : 28;         /* [31..4] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIAO_DSP_STATUS;

/* define the global struct */
typedef struct {
    volatile unsigned int AIAO_DSP_START_00;  /* 0xf8cdf000 */
    volatile unsigned int AIAO_DSP_START_01;
    volatile unsigned int AIAO_DSP_START_02;
    volatile unsigned int AIAO_DSP_START_03;
    volatile unsigned int AIAO_DSP_START_04;  /* 0xf8cdf010 */
    volatile unsigned int AIAO_DSP_START_05;
    volatile unsigned int AIAO_DSP_START_06;
    volatile unsigned int AIAO_DSP_START_07;
    volatile U_AIAO_INT_SWI_CPU2DSP AIAO_INT_SWI_CPU2DSP;  /* 0xf8cdf020 */
    volatile U_AIAO_INT_SWI_DSP2CPU AIAO_INT_SWI_DSP2CPU;
    volatile U_AIAO_INT_SWI0_DSP AIAO_INT_SWI0_DSP;
    volatile unsigned int reserved;                  /* 0xf8cdf02c */
    volatile U_AIAO_INT_SWI1_DSP AIAO_INT_SWI1_DSP;  /* 0xf8cdf020 */
    volatile U_AIAO_INT_SWI2_DSP AIAO_INT_SWI2_DSP;
    volatile U_AIAO_DSP_CTRL AIAO_DSP_CTRL;
    volatile U_AIAO_DSP_STATUS AIAO_DSP_STATUS;
} aiao_dsp_regs_type;

#define DSP0_CTRL_REG_ADDR 0xf8cdf000

/* define the union hi_drv_adsp_sys_reg */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int dsp_ready : 1; /* [0] */
        unsigned int arm_ready : 1; /* [1] */
        unsigned int test : 30;     /* [31..2] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} adsp_status_reg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
