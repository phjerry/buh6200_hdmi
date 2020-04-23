/*
 *
 * (C) COPYRIGHT 2008-2017 ARM Limited. All rights reserved.
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





#ifndef _KBASE_UKU_H_
#define _KBASE_UKU_H_



/******************************************************************************
******************|low----------------------->-----------------------high|
******************|--2bit----|---1bit-----|-2bit-|--4bit--|-1bit--|----22bit---|
******************|-bitwidth-|-compressed-|-tile-|-format-|loss---|-reserved---|
*******************************************************************************/
#define HIICE_FMT_8BIT (0)
#define HIICE_FMT_10BIT (1)
#define HIICE_FMT_12BIT (2)
#define HIICE_FMT_16BIT (3)
#define HIICE_FMT_COMPRESSED (0 << 2)
#define HIICE_FMT_NONCOMPRESSED (1 << 2)
#define HIICE_FMT_TILE (1 << 3)
#define HIICE_FMT_420SP (0 << 5)
#define HIICE_FMT_422SP (1 << 5)
#define HIICE_FMT_LOSS  (1 << 9)

#define HIICE_FMT_ENABLE_COMPRESS(fmt) ((fmt) & (~(HIICE_FMT_NONCOMPRESSED)))
#define HIICE_FMT_ENABLE_LOSS(fmt) ((fmt) | (HIICE_FMT_LOSS))

typedef enum
{
	ICEFMT_420SP_8BIT_TILE                  = (HIICE_FMT_420SP | HIICE_FMT_8BIT | HIICE_FMT_TILE | HIICE_FMT_NONCOMPRESSED),
	ICEFMT_420SP_8BIT_TILE_COMPRESSED       = HIICE_FMT_ENABLE_COMPRESS(ICEFMT_420SP_8BIT_TILE),
	ICEFMT_420SP_8BIT_TILE_COMPRESSED_LOSS  = HIICE_FMT_ENABLE_LOSS(ICEFMT_420SP_8BIT_TILE_COMPRESSED),

	ICEFMT_420SP_10BIT_TILE                 = (HIICE_FMT_420SP | HIICE_FMT_10BIT | HIICE_FMT_TILE | HIICE_FMT_NONCOMPRESSED),
	ICEFMT_420SP_10BIT_TILE_COMPRESSED      = HIICE_FMT_ENABLE_COMPRESS(ICEFMT_420SP_10BIT_TILE),
	ICEFMT_420SP_10BIT_TILE_COMPRESSED_LOSS = HIICE_FMT_ENABLE_LOSS(ICEFMT_420SP_10BIT_TILE_COMPRESSED),

	ICEFMT_422SP_8BIT_TILE                  = (HIICE_FMT_422SP | HIICE_FMT_8BIT | HIICE_FMT_TILE | HIICE_FMT_NONCOMPRESSED),
	ICEFMT_422SP_8BIT_TILE_COMPRESSED       = HIICE_FMT_ENABLE_COMPRESS(ICEFMT_422SP_8BIT_TILE),
	ICEFMT_422SP_8BIT_TILE_COMPRESSED_LOSS  = HIICE_FMT_ENABLE_LOSS(ICEFMT_422SP_8BIT_TILE_COMPRESSED),

	ICEFMT_422SP_10BIT_TILE                 = (HIICE_FMT_422SP | HIICE_FMT_10BIT | HIICE_FMT_TILE | HIICE_FMT_NONCOMPRESSED),
	ICEFMT_422SP_10BIT_TILE_COMPRESSED      = HIICE_FMT_ENABLE_COMPRESS(ICEFMT_422SP_10BIT_TILE),
	ICEFMT_422SP_10BIT_TILE_COMPRESSED_LOSS = HIICE_FMT_ENABLE_LOSS(ICEFMT_422SP_10BIT_TILE_COMPRESSED)
}hiice_fmt;

typedef union
{
	struct
	{
		u32 asid    : 3;
		u32 secure  : 1;
		u32 reserved: 28;
	}bits;

	u32 value;
}hiice_tag;


struct eglp_hiice_desc
{
	u16 align_width;         /* align width in pixel, it has a minimum size requirement of [64,16384] */
	u16 align_height;        /* align height in pixel, it has a minimum size requirement of [64,16384] */

	hiice_fmt format;       /* only support yuv420sp currently */

	u32 luma_stride;
	hiice_tag asid_s;       /* asid and secure flag */

	u64 luma_header_addr;   /* luma header address */
	u64 luma_data_addr;     /* luma data address */

	u64 chroma_header_addr; /* chroma header address */
	u64 chroma_data_addr;   /* chroma data address */

	u64 luma_data2_addr;    /* luma data address for part2 if exist */
	u64 chroma_data2_addr;  /* chroma data address for part2 if exist */

	u64 chroma_judge_base;  /* chroma header address for calculating the coordinate not real address */

	u64 gpu_address;        /* gpu virtual address for this texture, only used by driver to match job extres */
};

#endif /* _KBASE_UKU_H_ */

