/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of video module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_VIDEO_REG_H__
#define __HAL_HDMIRX_VIDEO_REG_H__

#define REG_CFG_CORE_ISO_EN 0x5000
#define CFG_CORE_ISO_EN 0x1 /* mask bit 0 */

#define REG_CFG_FIFO_R_STATUS 0x5004
#define CFG_TMDS_FIFO_RERROR  0x1 /* mask bit 0 */
#define CFG_TMDS_FIFO_RAEMPTY 0x2 /* mask bit 1 */
#define CFG_TMDS_FIFO_REMPTY  0x4 /* mask bit 2 */

#define REG_CFG_FIFO_W_STATUS 0x5008
#define CFG_TMDS_FIFO_WERROR 0x1 /* mask bit 0 */
#define CFG_TMDS_FIFO_WAFULL 0x2 /* mask bit 1 */
#define CFG_TMDS_FIFO_WFULL  0x4 /* mask bit 2 */

#define REG_CFG_TMDS_DE_ERR_CNT 0x500C
#define CFG_TMDS_DE_JITTER_CNT 0xFF  /* mask bit 7:0 */
#define CFG_TMDS_DE_ERR        0x100 /* mask bit 8 */

#define REG_CFG_PHASE_ADJ_CNT 0x5010
#define CFG_TMDS_PHASE_ADJ_CNT 0xFF /* mask bit 7:0 */

#define REG_CFG_DEFAULT_PHASE_CTRL 0x5014
#define CFG_DEFAULT_PHASE_EN 0x1  /* mask bit 0 */
#define CFG_PHASE_INC_VALUE  0x70 /* mask bit 6:4 */
#define CFG_SOFT_PHASE_EN    0x80 /* mask bit 7 */

#define REG_CFG_PHASE 0x5018
#define CFG_TMDS_PHASE 0xF /* mask bit 3:0 */

#define REG_CFG_PHASE_ERR 0x501C
#define CFG_PHASE_ERR 0x1 /* mask bit 0 */

#define REG_CFG_DAFAULT_PHASE_ERR 0x5020
#define CFG_DEFAULT_PHASE_ERR 0x1 /* mask bit 0 */

#define REG_CFG_SOFT_PHASE_VALID_PULSE 0x5024
#define CFG_SOFT_PHASE_PULSE 0x1 /* mask bit 0 */

#define REG_CFG_HSYNC_ERR 0x5030
#define CFG_HSYNC_ERR_CNT 0xFF  /* mask bit 7:0 */
#define CFG_HSYNC_ERR     0x100 /* mask bit 8 */

#define REG_CFG_VSYNC_ERR 0x5034
#define CFG_VSYNC_ERR_CNT 0xFF  /* mask bit 7:0 */
#define CFG_VSYNC_ERR     0x100 /* mask bit 8 */

#define REG_CFG_DE_ERR 0x5038
#define VIDEO_CFG_DE_ERR_CNT 0xFF  /* mask bit 7:0 */
#define CFG_DE_ERR           0x100 /* mask bit 8 */

#define REG_CFG_SYNC_PROCESS 0x503C
#define CFG_SYNC_POLARITY_FORCE  0x1  /* mask bit 0 */
#define CFG_VSYNC_POLARITY_VALUE 0x2  /* mask bit 1 */
#define CFG_HSYNC_POLARITY_VALUE 0x4  /* mask bit 2 */
#define CFG_VSYNC_INVERT_EN      0x8  /* mask bit 3 */
#define CFG_HSYNC_INVERT_EN      0x10 /* mask bit 4 */
#define CFG_SYNC_NORMALIZE_EN    0x20 /* mask bit 5 */

#define REG_CFG_INTERLACE_CTRL 0x5040
#define CFG_INTERLACE_OPT 0x1    /* mask bit 0 */
#define CFG_VSYNC_DLY     0x30   /* mask bit 5:4 */
#define CFG_FIELD_MODE    0x100  /* mask bit 8 */
#define CFG_ODD_VALUE     0x200  /* mask bit 9 */
#define CFG_EVEN_VALUE    0x400  /* mask bit 10 */
#define CFG_FIELD_FORCE0  0x1000 /* mask bit 12 */
#define CFG_FIELD_INVERT  0x2000 /* mask bit 13 */

#define REG_CFG_CHANGED_THRESHOLD 0x5044
#define CFG_HACTIVE_CHANGED_THRESHOLD   0xF    /* mask bit 3:0 */
#define CFG_VACTIVE_CHANGED_THRESHOLD   0xF0   /* mask bit 7:4 */
#define CFG_INTERLACE_CHANGED_THRESHOLD 0xF00  /* mask bit 11:8 */
#define CFG_V_CHANGED_THRESHOLD         0xF000 /* mask bit 15:12 */

#define REG_CFG_H_THRESHOLD 0x5048
#define CFG_HACTIVE_DELTA_THRESHOLD 0xFF       /* mask bit 7:0 */
#define CFG_HSYNC_DELTA_THRESHOLD   0xFF00     /* mask bit 15:8 */
#define CFG_HFRONT_DELTA_THRESHOLD  0xFF0000   /* mask bit 23:16 */
#define CFG_HBACK_DELTA_THRESHOLD   0xFF000000 /* mask bit 31:24 */

#define REG_CFG_HTOTAL_THRESHOLD 0x504C
#define CFG_HTOTAL_DELTA_THRESHOLD 0xFF /* mask bit 7:0 */

#define REG_CFG_V_THRESHOLD 0x5050
#define CFG_VACTIVE_DELTA_THRESHOLD    0xFF     /* mask bit 7:0 */
#define CFG_VSYNC_EVEN_DELTA_THRESHOLD 0xFF00   /* mask bit 15:8 */
#define CFG_VSYNC_ODD_DELTA_THRESHOLD  0xFF0000 /* mask bit 23:16 */

#define REG_CFG_VFRONT_EVEN_THRESHOLD 0x5054
#define CFG_VFRONT_EVEN_DELTA_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VFRONT_ODD_THRESHOLD 0x5058
#define CFG_VFRONT_ODD_DELTA_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VBACK_THRESHOLD 0x505C
#define CFG_VBACK_EVEN_DELTA_THRESHOLD 0xFF   /* mask bit 7:0 */
#define CFG_VBACK_ODD_DELTA_THRESHOLD  0xFF00 /* mask bit 15:8 */

#define REG_CFG_VFRONT_MIN_THRESHOLD 0x5060
#define CFG_VFRONT_MIN_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VFRONT_MAX_THRESHOLD 0x5064
#define CFG_VFRONT_MAX_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_MIN_THRESHOLD 0x5068
#define CFG_VTOTAL_MIN_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_MAX_THRESHOLD 0x506C
#define CFG_VTOTAL_MAX_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_EVEN_THRESHOLD 0x5070
#define CFG_VTOTAL_EVEN_DELTA_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_ODD_THRESHOLD 0x5074
#define CFG_VTOTAL_ODD_DELTA_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_FRAME_RATE_THRESHOLD 0x5078
#define CFG_FRAME_RATE_DELTA_THRESHOLD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_HACTIVE_CHANGED_REF 0x507C
#define CFG_HACTIVE_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HSYNC_CHANGED_REF 0x5080
#define CFG_HSYNC_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HFRONT_CHANGED_REF 0x5084
#define CFG_HFRONT_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HBACK_CHANGED_REF 0x5088
#define CFG_HBACK_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HTOTAL_CHANGED_REF 0x508C
#define CFG_HTOTAL_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VACTIVE_CHANGED_REF 0x5090
#define CFG_VACTIVE_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VSYNC_EVEN_CHANGED_REF 0x5094
#define CFG_VSYNC_EVEN_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VSYNC_ODD_CHANGED_REF 0x5098
#define CFG_VSYNC_ODD_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VFRONT_EVEN_CHANGED_REF 0x509C
#define CFG_VFRONT_EVEN_CHANGED_REF 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VFRONT_ODD_CHANGED_REF 0x50A0
#define CFG_VFRONT_ODD_CHANGED_REF 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VBACK_EVEN_CHANGED_REF 0x50A4
#define CFG_VBACK_EVEN_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VBACK_ODD_CHANGED_REF 0x50A8
#define CFG_VBACK_ODD_CHANGED_REF 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VTOTAL_EVEN_CHANGED_REF 0x50AC
#define CFG_VTOTAL_EVEN_CHANGED_REF 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_ODD_CHANGED_REF 0x50B0
#define CFG_VTOTAL_ODD_CHANGED_REF 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_FRAME_RATE_CHANGED_REF 0x50B4
#define CFG_FRAME_RATE_CHANGED_REF 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_INTERLACE_SYNC_REF 0x50B8
#define CFG_HSYNC_POLARITY_CHANGED_REF 0x1 /* mask bit 0 */
#define CFG_VSYNC_POLARITY_CHANGED_REF 0x2 /* mask bit 1 */
#define CFG_INTERLACE_CHANGED_REF      0x4 /* mask bit 2 */

#define REG_CFG_CHANGED_DET_EN 0x50BC
#define CFG_HSYNC_POLARITY_CHANGED_DET_EN 0x1     /* mask bit 0 */
#define CFG_VSYNC_POLARITY_CHANGED_DET_EN 0x2     /* mask bit 1 */
#define CFG_INTERLACE_CHANGED_DET_EN      0x4     /* mask bit 2 */
#define CFG_HACTIVE_CHANGED_DET_EN        0x8     /* mask bit 3 */
#define CFG_HSYNC_CHANGED_DET_EN          0x10    /* mask bit 4 */
#define CFG_HTOTAL_CHANGED_DET_EN         0x20    /* mask bit 5 */
#define CFG_HFRONT_CHANGED_DET_EN         0x40    /* mask bit 6 */
#define CFG_HBACK_CHANGED_DET_EN          0x80    /* mask bit 7 */
#define CFG_VACTIVE_CHANGED_DET_EN        0x100   /* mask bit 8 */
#define CFG_VSYNC_EVEN_CHANGED_DET_EN     0x200   /* mask bit 9 */
#define CFG_VSYNC_ODD_CHANGED_DET_EN      0x400   /* mask bit 10 */
#define CFG_VTOTAL_EVEN_CHANGED_DET_EN    0x800   /* mask bit 11 */
#define CFG_VTOTAL_ODD_CHANGED_DET_EN     0x1000  /* mask bit 12 */
#define CFG_VFRONT_EVEN_CHANGED_DET_EN    0x2000  /* mask bit 13 */
#define CFG_VFRONT_ODD_CHANGED_DET_EN     0x4000  /* mask bit 14 */
#define CFG_VBACK_EVEN_CHANGED_DET_EN     0x8000  /* mask bit 15 */
#define CFG_VBACK_ODD_CHANGED_DET_EN      0x10000 /* mask bit 16 */
#define CFG_FRAME_RATE_CHANGED_DET_EN     0x20000 /* mask bit 17 */

#define VIDEO_IRQ_MASK 0x50C0
#define CFG_MASK_HSYNC_POLARITY_CHANGED 0x1      /* mask bit 0 */
#define CFG_MASK_VSYNC_POLARITY_CHANGED 0x2      /* mask bit 1 */
#define CFG_MASK_INTERLACE_CHANGED      0x4      /* mask bit 2 */
#define CFG_MASK_HACTIVE_CHANGED        0x8      /* mask bit 3 */
#define CFG_MASK_HSYNC_CHANGED          0x10     /* mask bit 4 */
#define CFG_MASK_HTOTAL_CHANGED         0x20     /* mask bit 5 */
#define CFG_MASK_HFRONT_CHANGED         0x40     /* mask bit 6 */
#define CFG_MASK_HBACK_CHANGED          0x80     /* mask bit 7 */
#define CFG_MASK_VACTIVE_CHANGED        0x100    /* mask bit 8 */
#define CFG_MASK_VSYNC_EVEN_CHANGED     0x200    /* mask bit 9 */
#define CFG_MASK_VSYNC_ODD_CHANGED      0x400    /* mask bit 10 */
#define CFG_MASK_VTOTAL_EVEN_CHANGED    0x800    /* mask bit 11 */
#define CFG_MASK_VTOTAL_ODD_CHANGED     0x1000   /* mask bit 12 */
#define CFG_MASK_VFRONT_EVEN_CHANGED    0x2000   /* mask bit 13 */
#define CFG_MASK_VFRONT_ODD_CHANGED     0x4000   /* mask bit 14 */
#define CFG_MASK_VBACK_EVEN_CHANGED     0x8000   /* mask bit 15 */
#define CFG_MASK_VBACK_ODD_CHANGED      0x10000  /* mask bit 16 */
#define CFG_MASK_FRAME_RATE_CHANGED     0x20000  /* mask bit 17 */
#define CFG_MASK_VFRONT_ERR             0x40000  /* mask bit 18 */
#define CFG_MASK_VTOTAL_ERR             0x80000  /* mask bit 19 */
#define CFG_MASK_FIFO_FULL              0x100000 /* mask bit 20 */
#define CFG_MASK_FIFO_EMPTY             0x200000 /* mask bit 21 */
#define CFG_MASK_PHASE_ADJ              0x400000 /* mask bit 22 */

#define VIDEO_ORIGINAL_IRQ_STATUS 0x50C4
#define CFG_ORIGINAL_STATUS_HSYNC_POLARITY_CHANGED 0x1      /* mask bit 0 */
#define CFG_ORIGINAL_STATUS_VSYNC_POLARITY_CHANGED 0x2      /* mask bit 1 */
#define CFG_ORIGINAL_STATUS_INTERLACE_CHANGED      0x4      /* mask bit 2 */
#define CFG_ORIGINAL_STATUS_HACTIVE_CHANGED        0x8      /* mask bit 3 */
#define CFG_ORIGINAL_STATUS_HSYNC_CHANGED          0x10     /* mask bit 4 */
#define CFG_ORIGINAL_STATUS_HTOTAL_CHANGED         0x20     /* mask bit 5 */
#define CFG_ORIGINAL_STATUS_HFRONT_CHANGED         0x40     /* mask bit 6 */
#define CFG_ORIGINAL_STATUS_HBACK_CHANGED          0x80     /* mask bit 7 */
#define CFG_ORIGINAL_STATUS_VACTIVE_CHANGED        0x100    /* mask bit 8 */
#define CFG_ORIGINAL_STATUS_VSYNC_EVEN_CHANGED     0x200    /* mask bit 9 */
#define CFG_ORIGINAL_STATUS_VSYNC_ODD_CHANGED      0x400    /* mask bit 10 */
#define CFG_ORIGINAL_STATUS_VTOTAL_EVEN_CHANGED    0x800    /* mask bit 11 */
#define CFG_ORIGINAL_STATUS_VTOTAL_ODD_CHANGED     0x1000   /* mask bit 12 */
#define CFG_ORIGINAL_STATUS_VFRONT_EVEN_CHANGED    0x2000   /* mask bit 13 */
#define CFG_ORIGINAL_STATUS_VFRONT_ODD_CHANGED     0x4000   /* mask bit 14 */
#define CFG_ORIGINAL_STATUS_VBACK_EVEN_CHANGED     0x8000   /* mask bit 15 */
#define CFG_ORIGINAL_STATUS_VBACK_ODD_CHANGED      0x10000  /* mask bit 16 */
#define CFG_ORIGINAL_STATUS_FRAME_RATE_CHANGED     0x20000  /* mask bit 17 */
#define CFG_ORIGINAL_STATUS_VFRONT_ERR             0x40000  /* mask bit 18 */
#define CFG_ORIGINAL_STATUS_VTOTAL_ERR             0x80000  /* mask bit 19 */
#define CFG_ORIGINAL_STATUS_FIFO_FULL              0x100000 /* mask bit 20 */
#define CFG_ORIGINAL_STATUS_FIFO_EMPTY             0x200000 /* mask bit 21 */
#define CFG_ORIGINAL_STATUS_PHASE_ADJ              0x400000 /* mask bit 22 */

#define VIDEO_MASKED_IRQ_STATUS 0x50C8
#define CFG_MASKED_STATUS_HSYNC_POLARITY_CHANGED 0x1      /* mask bit 0 */
#define CFG_MASKED_STATUS_VSYNC_POLARITY_CHANGED 0x2      /* mask bit 1 */
#define CFG_MASKED_STATUS_INTERLACE_CHANGED      0x4      /* mask bit 2 */
#define CFG_MASKED_STATUS_HACTIVE_CHANGED        0x8      /* mask bit 3 */
#define CFG_MASKED_STATUS_HSYNC_CHANGED          0x10     /* mask bit 4 */
#define CFG_MASKED_STATUS_HTOTAL_CHANGED         0x20     /* mask bit 5 */
#define CFG_MASKED_STATUS_HFRONT_CHANGED         0x40     /* mask bit 6 */
#define CFG_MASKED_STATUS_HBACK_CHANGED          0x80     /* mask bit 7 */
#define CFG_MASKED_STATUS_VACTIVE_CHANGED        0x100    /* mask bit 8 */
#define CFG_MASKED_STATUS_VSYNC_EVEN_CHANGED     0x200    /* mask bit 9 */
#define CFG_MASKED_STATUS_VSYNC_ODD_CHANGED      0x400    /* mask bit 10 */
#define CFG_MASKED_STATUS_VTOTAL_EVEN_CHANGED    0x800    /* mask bit 11 */
#define CFG_MASKED_STATUS_VTOTAL_ODD_CHANGED     0x1000   /* mask bit 12 */
#define CFG_MASKED_STATUS_VFRONT_EVEN_CHANGED    0x2000   /* mask bit 13 */
#define CFG_MASKED_STATUS_VFRONT_ODD_CHANGED     0x4000   /* mask bit 14 */
#define CFG_MASKED_STATUS_VBACK_EVEN_CHANGED     0x8000   /* mask bit 15 */
#define CFG_MASKED_STATUS_VBACK_ODD_CHANGED      0x10000  /* mask bit 16 */
#define CFG_MASKED_STATUS_FRAME_RATE_CHANGED     0x20000  /* mask bit 17 */
#define CFG_MASKED_STATUS_VFRONT_ERR             0x40000  /* mask bit 18 */
#define CFG_MASKED_STATUS_VTOTAL_ERR             0x80000  /* mask bit 19 */
#define CFG_MASKED_STATUS_FIFO_FULL              0x100000 /* mask bit 20 */
#define CFG_MASKED_STATUS_FIFO_EMPTY             0x200000 /* mask bit 21 */
#define CFG_MASKED_STATUS_PHASE_ADJ              0x400000 /* mask bit 22 */

#define REG_CFG_VFRONT_EVEN_CHANGED_VALUE 0x50CC
#define CFG_VFRONT_EVEN_CHANGED_VALUE 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_EVEN_CHANGED_VALUE 0x50D0
#define CFG_VTOTAL_EVEN_CHANGED_VALUE 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VFRONT_ODD_CHANGED_VALUE 0x50D4
#define CFG_VFRONT_ODD_CHANGED_VALUE 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_ODD_CHANGED_VALUE 0x50D8
#define CFG_VTOTAL_ODD_CHANGED_VALUE 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_FDET_STATUS 0x50DC
#define CFG_STATUS_HSYNC     0x1 /* mask bit 0 */
#define CFG_STATUS_VSYNC     0x2 /* mask bit 1 */
#define CFG_STATUS_INTERLACE 0x4 /* mask bit 2 */

#define REG_CFG_HACTIVE_COUNT 0x50E0
#define CFG_HACTIVE_COUNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HSYNC_COUNT 0x50E4
#define CFG_HSYNC_COUNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HTOTAL_COUNT 0x50E8
#define CFG_HTOTAL_COUNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HFRONT_COUNT 0x50EC
#define CFG_HFRONT_COUNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_HBACK_COUNT 0x50F0
#define CFG_HBACK_COUNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VACTIVE_COUNT 0x50F4
#define CFG_VACTIVE_COUNT 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VSYNC_EVEN_COUNT 0x50F8
#define CFG_VSYNC_COUNT_EVEN 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VSYNC_ODD_COUNT 0x50FC
#define CFG_VSYNC_COUNT_ODD 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VTOTAL_EVEN_COUNT 0x5100
#define CFG_VTOTAL_COUNT_EVEN 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VTOTAL_ODD_COUNT 0x5104
#define CFG_VTOTAL_COUNT_ODD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VFRONT_EVEN_COUNT 0x5108
#define CFG_VFRONT_COUNT_EVEN 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VFRONT_ODD_COUNT 0x510C
#define CFG_VFRONT_COUNT_ODD 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_VBACK_EVEN_COUNT 0x5110
#define CFG_VBACK_COUNT_EVEN 0xFFFF /* mask bit 15:0 */

#define REG_CFG_VBACK_ODD_COUNT 0x5114
#define CFG_VBACK_COUNT_ODD 0xFFFF /* mask bit 15:0 */

#define REG_CFG_FRAME_COUNT 0x5118
#define CFG_FRAME_COUNT 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_FRAME_RATE_COUNT 0x511C
#define CFG_FRAME_RATE 0xFFFFFF /* mask bit 23:0 */

#define REG_CFG_BIST_CTRL 0x5120
#define CFG_BIST_EN            0x1       /* mask bit 0 */
#define CFG_BIST_DURATION_MODE 0x2       /* mask bit 1 */
#define CFG_BIST_DURATION      0xFFFFFF0 /* mask bit 27:4 */

#define REG_CFG_BIST_CLR 0x5124
#define CFG_BIST_CLR 0x1 /* mask bit 0 */

#define REG_CFG_PATTERN_SEL 0x5128
#define CFG_TPG_PATTERN_SEL      0x70   /* mask bit 6:4 */
#define CFG_CBAR_PATTERN_EXTMODE 0x100  /* mask bit 8 */
#define CFG_CBAR_PATTERN_SEL     0x3000 /* mask bit 13:12 */

#define REG_CFG_TIMING_SEL 0x512C
#define CFG_TIMING_SEL 0x3F  /* mask bit 5:0 */
#define CFG_EXTMODE    0x100 /* mask bit 8 */

#define REG_CFG_SOLID_PATTERN_Y 0x5130
#define CFG_SOLID_PATTERN_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_SOLID_PATTERN_CB 0x5134
#define CFG_SOLID_PATTERN_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_SOLID_PATTERN_CR 0x5138
#define CFG_SOLID_PATTERN_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_CBAR_CTRL 0x513C
#define CFG_COLORBAR_WIDTH 0xFFFF     /* mask bit 15:0 */
#define CFG_SQUARE_HEIGHT  0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_PATTERN_A0_Y 0x5140
#define CFG_BAR_PATTERN_A0_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A0_CB 0x5144
#define CFG_BAR_PATTERN_A0_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A0_CR 0x5148
#define CFG_BAR_PATTERN_A0_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A1_Y 0x514C
#define CFG_BAR_PATTERN_A1_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A1_CB 0x5150
#define CFG_BAR_PATTERN_A1_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A1_CR 0x5154
#define CFG_BAR_PATTERN_A1_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A2_Y 0x5158
#define CFG_BAR_PATTERN_A2_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A2_CB 0x515C
#define CFG_BAR_PATTERN_A2_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A2_CR 0x5160
#define CFG_BAR_PATTERN_A2_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A3_Y 0x5164
#define CFG_BAR_PATTERN_A3_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A3_CB 0x5168
#define CFG_BAR_PATTERN_A3_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A3_CR 0x516C
#define CFG_BAR_PATTERN_A3_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A4_Y 0x5170
#define CFG_BAR_PATTERN_A4_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A4_CB 0x5174
#define CFG_BAR_PATTERN_A4_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A4_CR 0x5178
#define CFG_BAR_PATTERN_A4_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A5_Y 0x517C
#define CFG_BAR_PATTERN_A5_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A5_CB 0x5180
#define CFG_BAR_PATTERN_A5_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A5_CR 0x5184
#define CFG_BAR_PATTERN_A5_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A6_Y 0x5188
#define CFG_BAR_PATTERN_A6_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A6_CB 0x518C
#define CFG_BAR_PATTERN_A6_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A6_CR 0x5190
#define CFG_BAR_PATTERN_A6_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A7_Y 0x5194
#define CFG_BAR_PATTERN_A7_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A7_CB 0x5198
#define CFG_BAR_PATTERN_A7_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_A7_CR 0x519C
#define CFG_BAR_PATTERN_A7_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B0_Y 0x51A0
#define CFG_BAR_PATTERN_B0_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B0_CB 0x51A4
#define CFG_BAR_PATTERN_B0_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B0_CR 0x51A8
#define CFG_BAR_PATTERN_B0_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B1_Y 0x51AC
#define CFG_BAR_PATTERN_B1_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B1_CB 0x51B0
#define CFG_BAR_PATTERN_B1_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B1_CR 0x51B4
#define CFG_BAR_PATTERN_B1_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B2_Y 0x51B8
#define CFG_BAR_PATTERN_B2_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B2_CB 0x51BC
#define CFG_BAR_PATTERN_B2_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B2_CR 0x51C0
#define CFG_BAR_PATTERN_B2_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B3_Y 0x51C4
#define CFG_BAR_PATTERN_B3_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B3_CB 0x51C8
#define CFG_BAR_PATTERN_B3_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B3_CR 0x51CC
#define CFG_BAR_PATTERN_B3_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B4_Y 0x51D0
#define CFG_BAR_PATTERN_B4_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B4_CB 0x51D4
#define CFG_BAR_PATTERN_B4_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B4_CR 0x51D8
#define CFG_BAR_PATTERN_B4_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B5_Y 0x51DC
#define CFG_BAR_PATTERN_B5_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B5_CB 0x51E0
#define CFG_BAR_PATTERN_B5_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B5_CR 0x51E4
#define CFG_BAR_PATTERN_B5_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B6_Y 0x51E8
#define CFG_BAR_PATTERN_B6_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B6_CB 0x51EC
#define CFG_BAR_PATTERN_B6_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B6_CR 0x51F0
#define CFG_BAR_PATTERN_B6_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B7_Y 0x51F4
#define CFG_BAR_PATTERN_B7_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B7_CB 0x51F8
#define CFG_BAR_PATTERN_B7_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_PATTERN_B7_CR 0x51FC
#define CFG_BAR_PATTERN_B7_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_H_CFG0 0x5200
#define CFG_H_SYNC  0xFFFF     /* mask bit 15:0 */
#define CFG_H_FRONT 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_H_CFG1 0x5204
#define CFG_H_ACTIVE 0xFFFF     /* mask bit 15:0 */
#define CFG_H_BACK   0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_V_CFG0 0x5208
#define CFG_V_SYNC  0xFFFF     /* mask bit 15:0 */
#define CFG_V_FRONT 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_V_CFG1 0x520C
#define CFG_V_ACTIVE 0xFFFF     /* mask bit 15:0 */
#define CFG_V_BACK   0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_BIST_ERR 0x5210
#define CFG_BIST_ERR           0x1     /* mask bit 0 */
#define CFG_BIST_HSYNC_ERR     0x2     /* mask bit 1 */
#define CFG_BIST_VSYNC_ERR     0x4     /* mask bit 2 */
#define CFG_BIST_DE_ERR        0x8     /* mask bit 3 */
#define VIDEO_CFG_BIST_DONE    0x10    /* mask bit 4 */
#define CFG_BIST_H_ERR         0x20    /* mask bit 5 */
#define CFG_BIST_V_ERR         0x40    /* mask bit 6 */
#define CFG_BIST_CTRL_STATE    0x700   /* mask bit 10:8 */
#define CFG_TIMING_GEN_H_STATE 0x7000  /* mask bit 14:12 */
#define CFG_TIMING_GEN_V_STATE 0x70000 /* mask bit 18:16 */

#define REG_CFG_PIXEL_ERR_COUNT0 0x5214
#define CFG_PIXEL_ERR_COUNT 0xFFFF     /* mask bit 15:0 */
#define CFG_CR_ERR_COUNT    0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_PIXEL_ERR_COUNT1 0x5218
#define CFG_Y_ERR_COUNT  0xFFFF     /* mask bit 15:0 */
#define CFG_CB_ERR_COUNT 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CATCH_POS 0x521C
#define CFG_CATCH_H 0xFFFF     /* mask bit 15:0 */
#define CFG_CATCH_V 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CATCH_EN 0x5220
#define CFG_SOFT_TRIGGER_EN 0x1 /* mask bit 0 */
#define CFG_AUTO_TRIGGER_EN 0x2 /* mask bit 1 */
#define CFG_SHOW_POINT_EN   0x4 /* mask bit 2 */

#define REG_CFG_CATCHED_Y 0x5224
#define CFG_CATCHED_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_CATCHED_CB 0x5228
#define CFG_CATCHED_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_CATCHED_CR 0x522C
#define CFG_CATCHED_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_CATCHED_STATE 0x5230
#define CFG_CATCHED_DONE 0x1 /* mask bit 0 */
#define CFG_CATCHED_BUSY 0x2 /* mask bit 1 */
#define CFG_CATCHED_ERR  0x4 /* mask bit 2 */

#define REG_CFG_CRC_EN 0x5234
#define CFG_CRC_EN 0x1 /* mask bit 0 */

#define REG_CFG_CRC_RESULT_Y01 0x5238
#define CFG_CRC_RESULT_Y0 0xFFFF     /* mask bit 15:0 */
#define CFG_CRC_RESULT_Y1 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CRC_RESULT_Y23 0x523C
#define CFG_CRC_RESULT_Y2 0xFFFF     /* mask bit 15:0 */
#define CFG_CRC_RESULT_Y3 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CRC_RESULT_CB01 0x5240
#define CFG_CRC_RESULT_CB0 0xFFFF     /* mask bit 15:0 */
#define CFG_CRC_RESULT_CB1 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CRC_RESULT_CB23 0x5244
#define CFG_CRC_RESULT_CB2 0xFFFF     /* mask bit 15:0 */
#define CFG_CRC_RESULT_CB3 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CRC_RESULT_CR01 0x5248
#define CFG_CRC_RESULT_CR0 0xFFFF     /* mask bit 15:0 */
#define CFG_CRC_RESULT_CR1 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CRC_RESULT_CR23 0x524C
#define CFG_CRC_RESULT_CR2 0xFFFF     /* mask bit 15:0 */
#define CFG_CRC_RESULT_CR3 0xFFFF0000 /* mask bit 31:16 */

#define REG_CFG_CRC_CHANGED_STATE 0x5250
#define CFG_CRC_CHANGED_Y      0x1        /* mask bit 0 */
#define CFG_CRC_CHANGED_CB     0x2        /* mask bit 1 */
#define CFG_CRC_CHANGED_CR     0x4        /* mask bit 2 */
#define CFG_CRC_CHANGED_CNT_Y  0xFF00     /* mask bit 15:8 */
#define CFG_CRC_CHANGED_CNT_CB 0xFF0000   /* mask bit 23:16 */
#define CFG_CRC_CHANGED_CNT_CR 0xFF000000 /* mask bit 31:24 */

#define REG_CFG_MUTE_CTRL 0x5254
#define CFG_MUTE_EN   0x1  /* mask bit 0 */
#define CFG_MUTE_MODE 0x2  /* mask bit 1 */
#define CFG_MUTE_DE   0x4  /* mask bit 2 */
#define CFG_MUTE_HS   0x8  /* mask bit 3 */
#define CFG_MUTE_VS   0x10 /* mask bit 4 */

#define REG_CFG_MUTE_Y 0x5258
#define CFG_MUTE_Y 0xFFF /* mask bit 11:0 */

#define REG_CFG_MUTE_CB 0x525C
#define CFG_MUTE_CB 0xFFF /* mask bit 11:0 */

#define REG_CFG_MUTE_CR 0x5260
#define CFG_MUTE_CR 0xFFF /* mask bit 11:0 */

#define REG_CFG_OUTPUT_MUXING0 0x5264
#define CFG_OUTPUT_Y_SEL  0x3      /* mask bit 1:0 */
#define CFG_OUTPUT_CB_SEL 0x30     /* mask bit 5:4 */
#define CFG_OUTPUT_CR_SEL 0x300    /* mask bit 9:8 */
#define CFG_OUTPUT_Y_MASK 0xFFF000 /* mask bit 23:12 */

#define REG_CFG_OUTPUT_MUXING1 0x5268
#define CFG_OUTPUT_CB_MASK 0xFFF     /* mask bit 11:0 */
#define CFG_OUTPUT_CR_MASK 0xFFF0000 /* mask bit 27:16 */

#define REG_CFG_422_EN 0x526C
#define CFG_422_EN 0x1 /* mask bit 0 */

#endif /* __HAL_HDMIRX_VIDEO_REG_H__ */
