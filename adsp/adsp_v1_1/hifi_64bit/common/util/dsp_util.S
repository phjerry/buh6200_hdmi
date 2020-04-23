#include "hi_asm_define.h"
	.arch armv8-a
	.file	"dsp_util.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	volume_db2_coef, %function
volume_db2_coef:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	and	w1, w19, 255
	cmp	w1, 138
	bhi	.L4
	mov	w2, 139
	sub	w2, w2, w1
	mov	w0, 0
	cmp	w2, 98
	bhi	.L1
	adrp	x3, .LANCHOR0
	add	x3, x3, :lo12:.LANCHOR0
	cmp	w20, 0
	ldr	w0, [x3, w2, uxtw 2]
	beq	.L1
	ble	.L3
	mov	w2, 138
	sub	w1, w2, w1
	ldr	w1, [x3, x1, lsl 2]
	sub	w1, w1, w0
	mul	w20, w1, w20
	add	w0, w0, w20, lsr 3
.L1:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L4:
	mov	w0, 48505
	movk	w0, 0x3f8, lsl 16
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L3:
	mov	w2, 140
	sub	w1, w2, w1
	ldr	w1, [x3, x1, lsl 2]
	sub	w1, w0, w1
	mneg	w20, w20, w1
	sub	w0, w0, w20, lsr 3
	b	.L1
	.size	volume_db2_coef, .-volume_db2_coef
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	AOE_FMUL_16_Q15.part.0, %function
AOE_FMUL_16_Q15.part.0:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	sxth	w0, w20
	cmp	w19, 8388608
	smull	x0, w19, w0
	asr	x0, x0, 23
	bls	.L14
	asr	w1, w0, 31
	asr	w3, w0, 15
	eor	w2, w1, 32767
	cmp	w1, w3
	csel	w0, w0, w2, eq
.L14:
	sxth	w0, w0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	AOE_FMUL_16_Q15.part.0, .-AOE_FMUL_16_Q15.part.0
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	volume16_bit_pro_coef, %function
volume16_bit_pro_coef:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	w20, w1
	mov	w21, w2
	mov	x0, x30
	bl	_mcount
	cbz	w20, .L15
	sub	w20, w20, #1
	add	x20, x20, 1
	add	x20, x19, x20, lsl 1
	.p2align 3
.L18:
	mov	w1, w21
	ldrsh	w0, [x19]
	mov	w3, 0
	cbz	w21, .L17
	bl	AOE_FMUL_16_Q15.part.0
	sxth	w3, w0
.L17:
	strh	w3, [x19], 2
	cmp	x19, x20
	bne	.L18
.L15:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	volume16_bit_pro_coef, .-volume16_bit_pro_coef
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	AOE_FMUL_24_Q15.part.1, %function
AOE_FMUL_24_Q15.part.1:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	lsr	w0, w19, 8
	smull	x0, w0, w20
	ldp	x19, x20, [sp, 16]
	asr	x0, x0, 23
	ldp	x29, x30, [sp], 32
	asr	w1, w0, 31
	eor	w2, w1, 8388607
	cmp	w1, w0, asr 23
	csel	w0, w0, w2, eq
	lsl	w0, w0, 8
	ret
	.size	AOE_FMUL_24_Q15.part.1, .-AOE_FMUL_24_Q15.part.1
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	volume24_bit_pro_coef, %function
volume24_bit_pro_coef:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	w21, w1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	cbz	w21, .L27
	sub	w21, w21, #1
	add	x21, x21, 1
	add	x21, x19, x21, lsl 2
	b	.L31
	.p2align 2
.L38:
	smull	x0, w20, w2
	ubfx	x0, x0, 23, 32
.L29:
	str	w0, [x19], 4
	cmp	x21, x19
	beq	.L27
.L31:
	mov	w0, 0
	ldr	w2, [x19]
	cbz	w20, .L29
	mov	w1, w20
	mov	w0, w2
	cmp	w20, 8388608
	bls	.L38
	bl	AOE_FMUL_24_Q15.part.1
	str	w0, [x19], 4
	cmp	x21, x19
	bne	.L31
.L27:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	volume24_bit_pro_coef, .-volume24_bit_pro_coef
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_calc_frame_size
	.type	util_calc_frame_size, %function
util_calc_frame_size:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	cmp	w20, 16
	lsl	w0, w19, 1
	lsl	w19, w19, 2
	csel	w0, w19, w0, ne
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	util_calc_frame_size, .-util_calc_frame_size
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_latency_ms2_byte_size
	.type	util_latency_ms2_byte_size, %function
util_latency_ms2_byte_size:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w19, w2
	mov	w21, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mul	w0, w19, w21
	mov	w2, 19923
	movk	w2, 0x1062, lsl 16
	ldr	x21, [sp, 32]
	umull	x0, w0, w2
	lsr	x0, x0, 38
	madd	w0, w0, w20, w20
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	util_latency_ms2_byte_size, .-util_latency_ms2_byte_size
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_latency_ms
	.type	util_latency_ms, %function
util_latency_ms:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w21, w2
	mov	w20, w0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	mov	w0, 1000
	mul	w19, w19, w21
	ldr	x21, [sp, 32]
	mul	w0, w20, w0
	udiv	w0, w0, w19
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	util_latency_ms, .-util_latency_ms
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_lr_linear_gain_process
	.type	util_lr_linear_gain_process, %function
util_lr_linear_gain_process:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	sxth	w20, w3
	sxth	w23, w4
	mov	x21, x0
	mov	x22, x1
	mov	w19, w2
	mov	x0, x30
	bl	_mcount
	cmp	w20, w23
	beq	.L45
	cmp	w19, 0
	ble	.L44
	lsl	w20, w20, 16
	lsl	w23, w23, 16
	mov	x3, 0
	b	.L51
	.p2align 2
.L54:
	smull	x0, w0, w23
	asr	x0, x0, 32
	lsl	w0, w0, 1
	strh	w0, [x22, x3, lsl 1]
	add	x3, x3, 1
	cmp	w19, w3
	ble	.L44
.L51:
	ldrsh	w0, [x21, x3, lsl 1]
	tbnz	x3, 0, .L54
	smull	x0, w0, w20
	asr	x0, x0, 32
	lsl	w0, w0, 1
	strh	w0, [x22, x3, lsl 1]
	add	x3, x3, 1
	cmp	w19, w3
	bgt	.L51
.L44:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L45:
	cmp	w19, 0
	ble	.L44
	lsl	w20, w20, 16
	mov	x3, 0
	.p2align 3
.L48:
	ldrsh	w0, [x21, x3, lsl 1]
	smull	x0, w0, w20
	asr	x0, x0, 32
	lsl	w0, w0, 1
	strh	w0, [x22, x3, lsl 1]
	add	x3, x3, 1
	cmp	w19, w3
	bgt	.L48
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.size	util_lr_linear_gain_process, .-util_lr_linear_gain_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_upmix_pcm_data
	.type	util_upmix_pcm_data, %function
util_upmix_pcm_data:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x2
	mov	w20, w0
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 16]
	cmp	w0, 24
	beq	.L93
	cmp	w0, 16
	beq	.L94
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L94:
	ldr	w2, [x19, 12]
	ldr	x5, [x19]
	cbz	w2, .L95
	sub	w7, w20, #1
	mov	x4, x21
	add	x7, x7, 1
	mov	w3, 0
	mov	w6, 0
	lsl	x7, x7, 1
	.p2align 3
.L73:
	mov	x0, 0
	cbnz	w20, .L72
	b	.L76
	.p2align 2
.L97:
	ldrh	w3, [x5], 2
	strh	w3, [x4, x0, lsl 1]
	add	x0, x0, 1
	cmp	w20, w0
	bls	.L96
.L72:
	ldr	w2, [x19, 20]
	cmp	w2, w0
	bhi	.L97
	strh	w3, [x4, x0, lsl 1]
	add	x0, x0, 1
	cmp	w20, w0
	bhi	.L72
.L96:
	ldr	w2, [x19, 12]
	add	x4, x4, x7
.L76:
	add	w6, w6, 1
	cmp	w2, w6
	bhi	.L73
	ldr	w0, [x19, 16]
	str	x21, [x19]
	str	w20, [x19, 20]
	cmp	w0, 16
	beq	.L74
	lsl	w20, w20, 2
.L77:
	mul	w2, w2, w20
	str	w2, [x19, 8]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L93:
	ldr	w2, [x19, 12]
	ldr	x5, [x19]
	cbz	w2, .L98
	sub	w7, w20, #1
	mov	x4, x21
	add	x7, x7, 1
	mov	w3, 0
	mov	w6, 0
	lsl	x7, x7, 2
	.p2align 3
.L62:
	mov	x0, 0
	cbnz	w20, .L61
	b	.L65
	.p2align 2
.L100:
	ldr	w3, [x5], 4
	str	w3, [x4, x0, lsl 2]
	add	x0, x0, 1
	cmp	w20, w0
	bls	.L99
.L61:
	ldr	w2, [x19, 20]
	cmp	w2, w0
	bhi	.L100
	str	w3, [x4, x0, lsl 2]
	add	x0, x0, 1
	cmp	w20, w0
	bhi	.L61
.L99:
	ldr	w2, [x19, 12]
	add	x4, x4, x7
.L65:
	add	w6, w6, 1
	cmp	w2, w6
	bhi	.L62
	ldr	w1, [x19, 16]
	lsl	w0, w20, 1
	str	x21, [x19]
	str	w20, [x19, 20]
	cmp	w1, 16
	beq	.L66
.L64:
	lsl	w0, w20, 2
.L66:
	mul	w2, w2, w0
	str	w2, [x19, 8]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
.L95:
	str	x21, [x19]
	str	w20, [x19, 20]
.L74:
	lsl	w20, w20, 1
	b	.L77
.L98:
	str	x21, [x19]
	str	w20, [x19, 20]
	b	.L64
	.size	util_upmix_pcm_data, .-util_upmix_pcm_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_downmix_pcm_data
	.type	util_downmix_pcm_data, %function
util_downmix_pcm_data:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x20, x2
	mov	w19, w0
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x20, 16]
	cmp	w0, 24
	beq	.L135
	cmp	w0, 16
	beq	.L136
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L136:
	ldr	w2, [x20, 12]
	ldr	x4, [x20]
	cbz	w2, .L137
	sub	w7, w19, #1
	mov	x5, x21
	add	x7, x7, 1
	mov	w6, 0
	lsl	x7, x7, 1
	.p2align 3
.L115:
	mov	x0, 0
	cbz	w19, .L118
	.p2align 3
.L114:
	ldrh	w3, [x4, x0, lsl 1]
	strh	w3, [x5, x0, lsl 1]
	add	x0, x0, 1
	cmp	w19, w0
	bhi	.L114
	ldr	w2, [x20, 12]
	add	x4, x4, x7
	add	x5, x5, x7
.L118:
	add	w6, w6, 1
	cmp	w2, w6
	bhi	.L115
	ldr	w0, [x20, 16]
	str	x21, [x20]
	str	w19, [x20, 20]
	cmp	w0, 16
	beq	.L116
	lsl	w19, w19, 2
.L119:
	mul	w2, w19, w2
	str	w2, [x20, 8]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L135:
	ldr	w2, [x20, 12]
	ldr	x4, [x20]
	cbz	w2, .L138
	sub	w7, w19, #1
	mov	x5, x21
	add	x7, x7, 1
	mov	w6, 0
	lsl	x7, x7, 2
	.p2align 3
.L106:
	mov	x0, 0
	cbz	w19, .L109
	.p2align 3
.L105:
	ldr	w3, [x4, x0, lsl 2]
	str	w3, [x5, x0, lsl 2]
	add	x0, x0, 1
	cmp	w19, w0
	bhi	.L105
	ldr	w2, [x20, 12]
	add	x4, x4, x7
	add	x5, x5, x7
.L109:
	add	w6, w6, 1
	cmp	w2, w6
	bhi	.L106
	ldr	w1, [x20, 16]
	lsl	w0, w19, 1
	str	x21, [x20]
	str	w19, [x20, 20]
	cmp	w1, 16
	beq	.L110
.L108:
	lsl	w0, w19, 2
.L110:
	mul	w0, w0, w2
	str	w0, [x20, 8]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
.L137:
	str	x21, [x20]
	str	w19, [x20, 20]
.L116:
	lsl	w19, w19, 1
	b	.L119
.L138:
	str	x21, [x20]
	str	w19, [x20, 20]
	b	.L108
	.size	util_downmix_pcm_data, .-util_downmix_pcm_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_unify_bit_depth_pcm_data
	.type	util_unify_bit_depth_pcm_data, %function
util_unify_bit_depth_pcm_data:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x2
	mov	w20, w0
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 12]
	cmp	w20, 16
	ldr	w1, [x19, 20]
	mul	w0, w0, w1
	beq	.L154
	cmp	w20, 24
	bne	.L139
	ldr	w1, [x19, 16]
	cmp	w1, 16
	beq	.L155
.L139:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L154:
	ldr	w1, [x19, 16]
	cmp	w1, 24
	bne	.L139
	ldr	x1, [x19]
	cbz	w0, .L142
	mov	x2, 0
	.p2align 3
.L143:
	ldr	w3, [x1, x2, lsl 2]
	lsr	w3, w3, 16
	strh	w3, [x21, x2, lsl 1]
	add	x2, x2, 1
	cmp	w0, w2
	bhi	.L143
.L142:
	ldr	w0, [x19, 8]
	mov	w1, 16
	str	x21, [x19]
	str	w1, [x19, 16]
	lsr	w0, w0, 1
	str	w0, [x19, 8]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L155:
	ldr	x1, [x19]
	cbz	w0, .L144
	mov	x2, 0
	.p2align 3
.L145:
	ldrh	w3, [x1, x2, lsl 1]
	lsl	w3, w3, 16
	str	w3, [x21, x2, lsl 2]
	add	x2, x2, 1
	cmp	w0, w2
	bhi	.L145
.L144:
	ldr	w0, [x19, 8]
	mov	w1, 24
	str	x21, [x19]
	str	w1, [x19, 16]
	lsl	w0, w0, 1
	str	w0, [x19, 8]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	util_unify_bit_depth_pcm_data, .-util_unify_bit_depth_pcm_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	utils_gain_balance_coef
	.type	utils_gain_balance_coef, %function
utils_gain_balance_coef:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w19, w2
	mov	x20, x0
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	cmp	w19, 0
	beq	.L156
	blt	.L162
	ldr	w2, [x20]
	mov	w0, 50
	sub	w0, w0, w19
	mov	w1, 34079
	movk	w1, 0x51eb, lsl 16
	mul	w0, w0, w2
	umull	x0, w0, w1
	lsr	x0, x0, 36
	str	w0, [x20]
.L156:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L162:
	ldr	w2, [x21]
	add	w0, w19, 50
	mov	w1, 34079
	movk	w1, 0x51eb, lsl 16
	mul	w0, w0, w2
	umull	x0, w0, w1
	lsr	x0, x0, 36
	str	w0, [x21]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	utils_gain_balance_coef, .-utils_gain_balance_coef
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_lr_gain_process
	.type	util_lr_gain_process, %function
util_lr_gain_process:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	str	x23, [sp, 48]
	mov	x21, x0
	mov	x20, x2
	mov	w23, w1
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x21, 20]
	ldr	w19, [x21, 12]
	ldr	w1, [x20]
	cmp	w1, 1
	mul	w19, w19, w0
	beq	.L220
	str	x22, [x29, 40]
	ldr	w1, [x20, 12]
	ldr	w0, [x20, 4]
	bl	volume_db2_coef
	mov	w22, w0
	ldr	w1, [x20, 16]
	ldr	w0, [x20, 8]
	bl	volume_db2_coef
	mov	w20, w0
	cmp	w23, 0
	bne	.L221
.L181:
	cmp	w22, 8388608
	mov	w0, 8388608
	ccmp	w20, w0, 0, eq
	beq	.L218
	ldr	w0, [x21, 16]
	cmp	w0, 16
	beq	.L183
	cmp	w0, 24
	bne	.L218
	cmp	w22, w20
	ldr	x21, [x21]
	beq	.L191
	sub	w23, w19, #1
	cbnz	w19, .L192
	b	.L218
	.p2align 2
.L222:
	cbz	w22, .L197
	cmp	w22, 8388608
	bhi	.L195
	smull	x0, w22, w0
	ubfx	x1, x0, 23, 32
	.p2align 3
.L197:
	str	w1, [x21]
	sub	w23, w23, #1
	cmn	w23, #1
	add	x21, x21, 4
	beq	.L218
.L192:
	mov	w1, 0
	ldr	w0, [x21]
	tbnz	x23, 0, .L222
	cbz	w20, .L197
	cmp	w20, 8388608
	bhi	.L198
	smull	x0, w20, w0
	ubfx	x1, x0, 23, 32
	b	.L197
	.p2align 2
.L218:
	ldr	x22, [x29, 40]
.L163:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L221:
	blt	.L223
	mov	w2, 50
	sub	w23, w2, w23
	mov	w2, 34079
	movk	w2, 0x51eb, lsl 16
	mul	w22, w23, w22
	umull	x22, w22, w2
	lsr	x22, x22, 36
	b	.L181
	.p2align 2
.L220:
	ldr	w0, [x21, 16]
	cmp	w0, 16
	beq	.L166
	cmp	w0, 24
	bne	.L163
	ldrsh	w0, [x20, 4]
	ldrsh	w1, [x20, 8]
	ldr	x2, [x21]
	cmp	w0, w1
	beq	.L175
	cmp	w19, 0
	ble	.L163
	lsl	w4, w1, 16
	lsl	w3, w0, 16
	mov	x1, 0
	b	.L180
	.p2align 2
.L224:
	smull	x0, w4, w0
	asr	x0, x0, 32
	lsl	w0, w0, 1
.L179:
	str	w0, [x2, x1, lsl 2]
	add	x1, x1, 1
	cmp	w19, w1
	ble	.L163
.L180:
	ldr	w0, [x2, x1, lsl 2]
	tbnz	x1, 0, .L224
	smull	x0, w3, w0
	asr	x0, x0, 32
	lsl	w0, w0, 1
	b	.L179
	.p2align 2
.L183:
	cmp	w22, w20
	ldr	x21, [x21]
	beq	.L185
	sub	w23, w19, #1
	cbnz	w19, .L186
	b	.L218
	.p2align 2
.L225:
	mov	w1, w22
	mov	w2, 0
	cbz	w22, .L190
.L219:
	bl	AOE_FMUL_16_Q15.part.0
	sxth	w2, w0
.L190:
	strh	w2, [x21]
	sub	w23, w23, #1
	add	x21, x21, 2
	cmn	w23, #1
	beq	.L218
.L186:
	ldrsh	w0, [x21]
	tbnz	x23, 0, .L225
	mov	w1, w20
	mov	w2, 0
	cbz	w20, .L190
	b	.L219
	.p2align 2
.L166:
	ldrsh	w0, [x20, 4]
	ldrsh	w1, [x20, 8]
	ldr	x2, [x21]
	cmp	w0, w1
	beq	.L168
	cmp	w19, 0
	ble	.L163
	lsl	w4, w1, 16
	lsl	w3, w0, 16
	mov	x1, 0
	b	.L174
	.p2align 2
.L226:
	smull	x0, w0, w4
	asr	x0, x0, 32
	lsl	w0, w0, 1
.L173:
	strh	w0, [x2, x1, lsl 1]
	add	x1, x1, 1
	cmp	w19, w1
	ble	.L163
.L174:
	ldrsh	w0, [x2, x1, lsl 1]
	tbnz	x1, 0, .L226
	smull	x0, w0, w3
	asr	x0, x0, 32
	lsl	w0, w0, 1
	b	.L173
	.p2align 2
.L195:
	mov	w1, w22
	bl	AOE_FMUL_24_Q15.part.1
	mov	w1, w0
	b	.L197
	.p2align 2
.L198:
	mov	w1, w20
	bl	AOE_FMUL_24_Q15.part.1
	mov	w1, w0
	b	.L197
	.p2align 2
.L223:
	add	w23, w23, 50
	mov	w0, 34079
	movk	w0, 0x51eb, lsl 16
	mul	w20, w23, w20
	umull	x20, w20, w0
	lsr	x20, x20, 36
	b	.L181
	.p2align 2
.L185:
	mov	w2, w22
	mov	w1, w19
	mov	x0, x21
	bl	volume16_bit_pro_coef
	ldr	x22, [x29, 40]
	b	.L163
	.p2align 2
.L191:
	mov	w2, w22
	mov	w1, w19
	mov	x0, x21
	bl	volume24_bit_pro_coef
	ldr	x22, [x29, 40]
	b	.L163
	.p2align 2
.L175:
	cmp	w19, 0
	ble	.L163
	sub	w19, w19, #1
	add	x3, x2, 4
	lsl	w1, w0, 16
	add	x19, x3, x19, uxtw 2
	.p2align 3
.L177:
	ldr	w0, [x2]
	smull	x0, w1, w0
	asr	x0, x0, 32
	lsl	w0, w0, 1
	str	w0, [x2], 4
	cmp	x19, x2
	bne	.L177
	b	.L163
	.p2align 2
.L168:
	cmp	w19, 0
	ble	.L163
	sub	w19, w19, #1
	add	x3, x2, 2
	lsl	w1, w0, 16
	add	x19, x3, x19, uxtw 1
	.p2align 3
.L171:
	ldrsh	w0, [x2]
	smull	x0, w0, w1
	asr	x0, x0, 32
	lsl	w0, w0, 1
	strh	w0, [x2], 2
	cmp	x19, x2
	bne	.L171
	b	.L163
	.size	util_lr_gain_process, .-util_lr_gain_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_lr_fade_gain_process
	.type	util_lr_fade_gain_process, %function
util_lr_fade_gain_process:
	stp	x29, x30, [sp, -112]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	stp	x26, x27, [sp, 72]
	str	x28, [sp, 88]
	mov	x19, x2
	mov	x20, x3
	mov	x27, x0
	mov	w21, w1
	mov	x0, x30
	bl	_mcount
	ldr	w1, [x19, 12]
	ldr	w0, [x19, 4]
	bl	volume_db2_coef
	mov	w19, w0
	ldr	w1, [x20, 12]
	ldr	w0, [x20, 4]
	bl	volume_db2_coef
	sub	w0, w0, w19
	ldr	w2, [x27, 12]
	ldr	x26, [x27]
	sdiv	w28, w0, w2
	cbz	w2, .L227
	stp	x22, x23, [x29, 40]
	mov	w20, 50
	add	w23, w21, 50
	sub	w20, w20, w21
	stp	x24, x25, [x29, 56]
	mov	x3, x26
	mov	w22, 0
	mul	w4, w28, w23
	mul	w25, w28, w20
	mul	w23, w23, w19
	mul	w20, w19, w20
	b	.L239
	.p2align 2
.L257:
	mov	w0, 34079
	movk	w0, 0x51eb, lsl 16
	blt	.L253
	umull	x1, w20, w0
	mov	w24, w19
	lsr	x1, x1, 36
.L229:
	ldr	w0, [x27, 16]
	cmp	w0, 16
	beq	.L254
.L231:
	cmp	w0, 24
	beq	.L255
.L234:
	add	w22, w22, 1
	add	w19, w19, w28
	add	w23, w23, w4
	add	w20, w20, w25
	cmp	w2, w22
	bls	.L256
.L239:
	cmp	w21, 0
	bne	.L257
	ldr	w0, [x27, 16]
	mov	w24, w19
	mov	w1, w19
	cmp	w0, 16
	bne	.L231
.L254:
	ldrsh	w0, [x26]
	mov	w2, 0
	cbz	w1, .L232
	str	x3, [x29, 96]
	str	w4, [x29, 108]
	bl	AOE_FMUL_16_Q15.part.0
	ldr	w4, [x29, 108]
	sxth	w2, w0
	ldr	x3, [x29, 96]
.L232:
	strh	w2, [x26]
	mov	w1, 0
	ldrsh	w0, [x26, 2]
	cbz	w24, .L233
	mov	w1, w24
	str	x3, [x29, 96]
	str	w4, [x29, 108]
	bl	AOE_FMUL_16_Q15.part.0
	ldr	w4, [x29, 108]
	sxth	w1, w0
	ldr	x3, [x29, 96]
.L233:
	strh	w1, [x26, 2]
	add	w22, w22, 1
	add	x26, x26, 4
	add	w19, w19, w28
	ldr	w2, [x27, 12]
	add	w23, w23, w4
	add	w20, w20, w25
	cmp	w2, w22
	bhi	.L239
.L256:
	ldp	x22, x23, [x29, 40]
	ldp	x24, x25, [x29, 56]
.L227:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x26, x27, [sp, 72]
	ldr	x28, [sp, 88]
	ldp	x29, x30, [sp], 112
	ret
	.p2align 2
.L255:
	mov	w0, 0
	ldr	w2, [x3]
	cbz	w1, .L235
	cmp	w1, 8388608
	bhi	.L236
	smull	x1, w1, w2
	ubfx	x0, x1, 23, 32
.L235:
	str	w0, [x3]
	mov	w0, 0
	ldr	w2, [x3, 4]
	cbz	w24, .L237
	cmp	w24, 8388608
	bhi	.L238
	smull	x0, w24, w2
	ubfx	x0, x0, 23, 32
.L237:
	str	w0, [x3, 4]
	add	x3, x3, 8
	ldr	w2, [x27, 12]
	b	.L234
	.p2align 2
.L253:
	umull	x24, w23, w0
	mov	w1, w19
	lsr	x24, x24, 36
	b	.L229
	.p2align 2
.L238:
	mov	w0, w2
	str	x3, [x29, 96]
	str	w4, [x29, 108]
	mov	w1, w24
	bl	AOE_FMUL_24_Q15.part.1
	ldr	x3, [x29, 96]
	ldr	w4, [x29, 108]
	add	x3, x3, 8
	str	w0, [x3, -4]
	ldr	w2, [x27, 12]
	b	.L234
	.p2align 2
.L236:
	str	x3, [x29, 96]
	mov	w0, w2
	str	w4, [x29, 108]
	bl	AOE_FMUL_24_Q15.part.1
	ldr	w4, [x29, 108]
	ldr	x3, [x29, 96]
	b	.L235
	.size	util_lr_fade_gain_process, .-util_lr_fade_gain_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_mixer_pro
	.type	util_mixer_pro, %function
util_mixer_pro:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x19, x0
	mov	x21, x1
	mov	w20, w2
	mov	w22, w3
	mov	x0, x30
	bl	_mcount
	cmp	x19, 0
	ccmp	x21, 0, 4, ne
	ccmp	w20, 0, 4, ne
	bne	.L274
.L258:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L274:
	cmp	w22, 16
	beq	.L275
	cmp	w22, 24
	bne	.L258
	cbz	w20, .L258
	mov	x0, 0
	mov	w4, 2147483647
	.p2align 3
.L264:
	ldr	w2, [x19, x0, lsl 2]
	ldr	w1, [x21, x0, lsl 2]
	add	w3, w2, w1
	eor	w1, w2, w1
	tbnz	w1, #31, .L263
	eor	w1, w2, w3
	tbz	w1, #31, .L263
	cmp	w2, 0
	csinv	w3, w4, w4, ge
.L263:
	str	w3, [x19, x0, lsl 2]
	add	x0, x0, 1
	cmp	w20, w0
	bhi	.L264
	b	.L258
	.p2align 2
.L275:
	cbz	w20, .L258
	mov	x3, 0
	.p2align 3
.L262:
	ldrsh	w0, [x19, x3, lsl 1]
	ldrsh	w1, [x21, x3, lsl 1]
	add	w0, w0, w1
	asr	w4, w0, 31
	cmp	w4, w0, asr 15
	eor	w4, w4, 32767
	csel	w0, w4, w0, ne
	strh	w0, [x19, x3, lsl 1]
	add	x3, x3, 1
	cmp	w20, w3
	bhi	.L262
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	util_mixer_pro, .-util_mixer_pro
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_channel_mode_process
	.type	util_channel_mode_process, %function
util_channel_mode_process:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	mov	w21, w2
	bl	_mcount
	ldr	w0, [x19, 12]
	cmp	w20, 4
	ldr	w1, [x19, 20]
	mul	w0, w0, w1
	beq	.L278
	bls	.L380
	cmp	w20, 6
	beq	.L283
	bcc	.L284
	cmp	w20, 7
	bne	.L276
	cbz	w0, .L276
	cmp	w21, 16
	ldr	x1, [x19]
	beq	.L381
	cmp	w21, 24
	bne	.L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 2
	.p2align 3
.L310:
	str	wzr, [x1], 4
	cmp	x1, x0
	bne	.L310
.L276:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L380:
	cmp	w20, 2
	beq	.L280
	bhi	.L281
	cmp	w20, 1
	bne	.L276
	cbz	w0, .L276
	cmp	w21, 16
	ldr	x1, [x19]
	beq	.L382
	cmp	w21, 24
	bne	.L276
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	mov	w5, 2147483647
	add	x0, x0, 1
	add	x0, x1, x0, lsl 3
	.p2align 3
.L292:
	ldp	w4, w2, [x1]
	add	w3, w4, w2
	eor	w2, w4, w2
	tbnz	w2, #31, .L291
	eor	w2, w4, w3
	tbz	w2, #31, .L291
	cmp	w4, 0
	csinv	w3, w5, w5, ge
.L291:
	stp	w3, w3, [x1]
	add	x1, x1, 8
	cmp	x1, x0
	bne	.L292
	b	.L276
	.p2align 2
.L284:
	cbz	w0, .L276
	cmp	w21, 16
	ldr	x1, [x19]
	beq	.L383
	cmp	w21, 24
	bne	.L276
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 3
	.p2align 3
.L304:
	str	wzr, [x1], 8
	cmp	x1, x0
	bne	.L304
	b	.L276
	.p2align 2
.L281:
	cbz	w0, .L276
	cmp	w21, 16
	ldr	x1, [x19]
	beq	.L384
	cmp	w21, 24
	bne	.L276
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 3
	.p2align 3
.L298:
	ldr	w2, [x1, 4]
	str	w2, [x1], 8
	cmp	x1, x0
	bne	.L298
	b	.L276
	.p2align 2
.L283:
	cbz	w0, .L276
	cmp	w21, 16
	ldr	x1, [x19]
	beq	.L385
	cmp	w21, 24
	bne	.L276
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x2, x1, 12
	add	x1, x1, 4
	add	x0, x2, x0, uxtw 3
	.p2align 3
.L307:
	str	wzr, [x1], 8
	cmp	x0, x1
	bne	.L307
	b	.L276
	.p2align 2
.L280:
	cbz	w0, .L276
	cmp	w21, 16
	ldr	x1, [x19]
	beq	.L386
	cmp	w21, 24
	bne	.L276
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 3
	.p2align 3
.L295:
	ldr	w2, [x1]
	add	x1, x1, 8
	str	w2, [x1, -4]
	cmp	x1, x0
	bne	.L295
	b	.L276
	.p2align 2
.L278:
	cbz	w0, .L276
	cmp	w21, 16
	ldr	x1, [x19]
	beq	.L387
	cmp	w21, 24
	bne	.L276
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 3
	.p2align 3
.L301:
	ldp	w2, w3, [x1]
	stp	w3, w2, [x1]
	add	x1, x1, 8
	cmp	x0, x1
	bne	.L301
	b	.L276
	.p2align 2
.L381:
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 1
	.p2align 3
.L309:
	strh	wzr, [x1], 2
	cmp	x1, x0
	bne	.L309
	b	.L276
	.p2align 2
.L383:
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 2
	.p2align 3
.L303:
	strh	wzr, [x1], 4
	cmp	x1, x0
	bne	.L303
	b	.L276
	.p2align 2
.L382:
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 2
	.p2align 3
.L290:
	ldrsh	w3, [x1, 2]
	add	x1, x1, 4
	ldrsh	w2, [x1, -4]
	add	w2, w2, w3
	asr	w3, w2, 31
	cmp	w3, w2, asr 15
	eor	w3, w3, 32767
	csel	w2, w3, w2, ne
	cmp	x1, x0
	sxth	w2, w2
	strh	w2, [x1, -4]
	strh	w2, [x1, -2]
	bne	.L290
	b	.L276
	.p2align 2
.L387:
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 2
	.p2align 3
.L300:
	ldrsh	w2, [x1]
	add	x1, x1, 4
	ldrh	w3, [x1, -2]
	strh	w3, [x1, -4]
	strh	w2, [x1, -2]
	cmp	x1, x0
	bne	.L300
	b	.L276
	.p2align 2
.L386:
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 2
	.p2align 3
.L294:
	ldrh	w2, [x1]
	add	x1, x1, 4
	strh	w2, [x1, -2]
	cmp	x1, x0
	bne	.L294
	b	.L276
	.p2align 2
.L385:
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x2, x1, 6
	add	x1, x1, 2
	add	x0, x2, x0, uxtw 2
	.p2align 3
.L306:
	strh	wzr, [x1], 4
	cmp	x0, x1
	bne	.L306
	b	.L276
	.p2align 2
.L384:
	lsr	w0, w0, 1
	cbz	w0, .L276
	sub	w0, w0, #1
	add	x0, x0, 1
	add	x0, x1, x0, lsl 2
	.p2align 3
.L297:
	ldrh	w2, [x1, 2]
	strh	w2, [x1], 4
	cmp	x1, x0
	bne	.L297
	b	.L276
	.size	util_channel_mode_process, .-util_channel_mode_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	fade_create
	.type	fade_create, %function
fade_create:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	mov	w22, w2
	mov	w21, w3
	bl	_mcount
	str	xzr, [x19]
	sub	w1, w20, #1
	mov	w0, 60927
	movk	w0, 0x2, lsl 16
	cmp	w1, w0
	bhi	.L388
	and	w0, w22, -9
	cmp	w0, 16
	bne	.L388
	sub	w0, w21, #1
	cmp	w0, 1
	ccmp	w21, 8, 4, hi
	beq	.L396
.L388:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L396:
	str	x23, [x29, 48]
	mov	w1, 36
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x23, x0
	cbz	x0, .L395
	mov	x2, 36
	mov	w1, 0
	bl	adsp_memset
	stp	w22, w21, [x23]
	str	w20, [x23, 8]
	str	wzr, [x23, 32]
	str	x23, [x19]
	ldr	x23, [x29, 48]
	b	.L388
	.p2align 2
.L395:
	ldr	x23, [x29, 48]
	b	.L388
	.size	fade_create, .-fade_create
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	fade_flush
	.type	fade_flush, %function
fade_flush:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	w20, w1
	mov	x19, x0
	mov	w21, w2
	mov	x0, x30
	mov	w22, w3
	bl	_mcount
	sub	w1, w20, #1
	mov	w0, 60927
	movk	w0, 0x2, lsl 16
	cmp	w1, w0
	bhi	.L397
	and	w0, w21, -9
	cmp	w0, 16
	bne	.L397
	sub	w0, w22, #1
	cmp	w0, 1
	ccmp	w22, 8, 4, hi
	cset	w0, eq
	cmp	x19, 0
	csel	w0, w0, wzr, ne
	cbnz	w0, .L408
.L397:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L408:
	ldr	w0, [x19]
	cmp	w0, w21
	beq	.L409
.L401:
	ldr	w0, [x19, 12]
	stp	w21, w22, [x19]
	str	w20, [x19, 8]
	str	wzr, [x19, 20]
	ldr	w1, [x19, 32]
	cbz	w0, .L410
	mul	w0, w0, w20
	mov	w20, 18849
	movk	w20, 0xcf20, lsl 16
	umull	x0, w0, w20
	lsr	x0, x0, 48
	add	w0, w0, 1
	str	w0, [x19, 24]
.L403:
	cmp	w1, 1
	beq	.L411
	cmp	w1, 2
	bne	.L397
	mov	w0, 18
	str	w0, [x19, 16]
	b	.L397
	.p2align 2
.L410:
	str	wzr, [x19, 24]
	b	.L403
	.p2align 2
.L409:
	ldr	w0, [x19, 4]
	cmp	w22, w0
	bne	.L401
	ldr	w0, [x19, 8]
	cmp	w20, w0
	bne	.L401
	b	.L397
	.p2align 2
.L411:
	mov	w0, 99
	str	w0, [x19, 16]
	str	wzr, [x19, 28]
	b	.L397
	.size	fade_flush, .-fade_flush
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	fade_destroy
	.type	fade_destroy, %function
fade_destroy:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L412
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L412:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	fade_destroy, .-fade_destroy
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	fade_active
	.type	fade_active, %function
fade_active:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	w21, w1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L417
	str	w20, [x19, 12]
	str	wzr, [x19, 20]
	str	w21, [x19, 32]
	cbz	w20, .L424
	ldr	w0, [x19, 8]
	mov	w1, 18849
	movk	w1, 0xcf20, lsl 16
	cmp	w21, 1
	mul	w0, w20, w0
	umull	x0, w0, w1
	lsr	x0, x0, 48
	add	w0, w0, 1
	str	w0, [x19, 24]
	beq	.L425
.L422:
	cmp	w21, 2
	beq	.L426
.L417:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L424:
	str	wzr, [x19, 24]
	cmp	w21, 1
	bne	.L422
.L425:
	str	wzr, [x19, 28]
	mov	w0, 99
	str	w0, [x19, 16]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L426:
	mov	w0, 18
	str	w0, [x19, 16]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	fade_active, .-fade_active
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	fade_in_process
	.type	fade_in_process, %function
fade_in_process:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x26, [sp, 72]
	mov	x19, x0
	mov	x20, x1
	mov	w26, w2
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L427
	ldr	w0, [x19, 32]
	cbz	w0, .L427
	str	x23, [x29, 48]
	ldr	w3, [x19, 4]
	mov	w23, w3
	cbz	w26, .L462
	stp	x21, x22, [x29, 32]
	sxtw	x21, w3
	stp	x24, x25, [x29, 56]
	adrp	x22, .LANCHOR0
	lsl	x24, x21, 2
	sub	w26, w26, #1
	add	x22, x22, :lo12:.LANCHOR0
	lsl	x21, x21, 1
	mov	w25, 1
	b	.L442
	.p2align 2
.L429:
	ldr	w0, [x19, 16]
	cmp	w1, 16
	ldr	w2, [x22, x0, lsl 2]
	beq	.L440
.L465:
	cmp	w1, 24
	bne	.L439
	mov	x0, x20
	mov	w1, w23
	add	x20, x20, x24
	bl	volume24_bit_pro_coef
.L439:
	ldp	w0, w1, [x19, 20]
	add	w0, w0, 1
	str	w0, [x19, 20]
	cmp	w0, w1
	beq	.L464
.L438:
	sub	w0, w26, #1
	cbz	w26, .L463
	ldr	w3, [x19, 4]
	mov	w26, w0
.L442:
	ldr	w0, [x19, 28]
	ldr	w1, [x19]
	cbnz	w0, .L429
	cbnz	w3, .L434
	b	.L430
	.p2align 2
.L431:
	cmp	w1, 24
	bne	.L433
	ldr	w2, [x20]
	cbnz	w2, .L432
.L433:
	cmp	w3, w0
	beq	.L430
.L434:
	add	w0, w0, 1
	cmp	w1, 16
	bne	.L431
	ldrsh	w2, [x20]
	cbz	w2, .L433
.L432:
	ldr	w0, [x19, 16]
	cmp	w1, 16
	str	w25, [x19, 28]
	ldr	w2, [x22, x0, lsl 2]
	bne	.L465
	.p2align 3
.L440:
	mov	x0, x20
	mov	w1, w23
	bl	volume16_bit_pro_coef
	add	x20, x20, x21
	ldp	w0, w1, [x19, 20]
	add	w0, w0, 1
	str	w0, [x19, 20]
	cmp	w0, w1
	bne	.L438
.L464:
	ldr	w0, [x19, 32]
	str	wzr, [x19, 20]
	cmp	w0, 1
	bne	.L438
	ldr	w0, [x19, 16]
	sub	w0, w0, #1
	str	w0, [x19, 16]
	cmp	w0, 18
	bne	.L438
	str	wzr, [x19, 32]
	b	.L438
	.p2align 2
.L463:
	ldp	x21, x22, [x29, 32]
	ldp	x23, x24, [x29, 48]
	ldr	x25, [x29, 64]
.L427:
	ldp	x19, x20, [sp, 16]
	ldr	x26, [sp, 72]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L430:
	cmp	w1, 16
	beq	.L435
	add	x0, x20, x24
	cmp	w1, 24
	csel	x20, x0, x20, eq
	b	.L438
	.p2align 2
.L435:
	add	x20, x20, x21
	b	.L438
.L462:
	ldr	x23, [x29, 48]
	b	.L427
	.size	fade_in_process, .-fade_in_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	fade_out_process
	.type	fade_out_process, %function
fade_out_process:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	x21, x1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L466
	str	x24, [x29, 56]
	ldr	w24, [x19, 4]
	cbz	w20, .L482
	stp	x22, x23, [x29, 40]
	sxtw	x23, w24
	adrp	x22, .LANCHOR0
	str	x25, [x29, 64]
	add	x22, x22, :lo12:.LANCHOR0
	lsl	x25, x23, 2
	lsl	x23, x23, 1
	.p2align 3
.L474:
	ldr	w0, [x19, 16]
	ldr	w1, [x19]
	cmp	w1, 16
	ldr	w2, [x22, x0, lsl 2]
	mov	x0, x21
	beq	.L469
	cmp	w1, 24
	bne	.L468
	add	x21, x21, x25
	mov	w1, w24
	bl	volume24_bit_pro_coef
.L468:
	ldp	w0, w1, [x19, 20]
	sub	w20, w20, #1
	add	w0, w0, 1
	str	w0, [x19, 20]
	cmp	w0, w1
	beq	.L483
.L472:
	cbnz	w20, .L474
.L484:
	ldp	x22, x23, [x29, 40]
	ldp	x24, x25, [x29, 56]
.L466:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L483:
	ldr	w0, [x19, 32]
	str	wzr, [x19, 20]
	cmp	w0, 2
	bne	.L472
	ldr	w0, [x19, 16]
	add	w0, w0, 1
	str	w0, [x19, 16]
	cmp	w0, 99
	bne	.L472
	str	wzr, [x19, 32]
	cbnz	w20, .L474
	b	.L484
	.p2align 2
.L469:
	mov	w1, w24
	add	x21, x21, x23
	bl	volume16_bit_pro_coef
	b	.L468
	.p2align 2
.L482:
	ldr	x24, [x29, 56]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 80
	ret
	.size	fade_out_process, .-fade_out_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_sqrt
	.type	util_sqrt, %function
util_sqrt:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	cbz	w19, .L485
	tbnz	x19, 0, .L495
	mov	w1, 1
	b	.L489
	.p2align 2
.L488:
	add	w1, w1, 1
	cmp	w1, 16
	beq	.L497
.L489:
	lsr	w0, w19, w1
	tbz	x0, 0, .L488
	lsr	w1, w1, 1
	lsl	w0, w0, w1
	udiv	w1, w19, w0
	b	.L490
	.p2align 2
.L498:
	cmp	w3, 1
	add	w1, w1, w0
	cset	w2, hi
	cbz	w2, .L485
.L493:
	lsr	w0, w1, 1
	udiv	w1, w19, w0
.L490:
	sub	w2, w0, w1
	subs	w3, w1, w0
	bhi	.L498
	cmp	w2, 1
	add	w1, w1, w0
	cset	w2, hi
	cbnz	w2, .L493
.L485:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L497:
	lsl	w0, w0, 8
	udiv	w1, w19, w0
	b	.L490
	.p2align 2
.L495:
	mov	w0, w19
	mov	w1, 1
	b	.L490
	.size	util_sqrt, .-util_sqrt
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	util_dre_16_bit_process
	.type	util_dre_16_bit_process, %function
util_dre_16_bit_process:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	mov	w1, 1000
	mov	w0, 19923
	ldr	w9, [x20, 24]
	movk	w0, 0x1062, lsl 16
	ldr	w13, [x20, 12]
	ldp	w4, w3, [x19]
	subs	w8, w13, #1
	mul	w9, w9, w1
	ldr	w14, [x19, 16]
	ldr	x10, [x20]
	umull	x9, w9, w0
	lsr	x9, x9, 38
	bmi	.L520
	mov	w12, 1
	mov	x2, x10
	mov	w5, w8
	mov	w6, 0
	mov	w7, 0
	mov	w1, 32767
	movk	w12, 0x3fff, lsl 16
	.p2align 3
.L507:
	ldrsh	w0, [x2]
	sub	w0, w0, w4, lsr 16
	cmp	w0, w1
	bgt	.L521
	cmn	w0, #32768
	blt	.L522
	mul	w11, w0, w0
	tbnz	w0, #31, .L502
.L501:
	ldrsh	w0, [x2, 2]
	add	w4, w4, 512
	add	w7, w7, w11
	add	x2, x2, 4
	sub	w0, w0, w3, lsr 16
	cmp	w0, w1
	bgt	.L523
.L530:
	cmn	w0, #32768
	blt	.L524
	mul	w11, w0, w0
	tbnz	w0, #31, .L505
.L504:
	add	w3, w3, 512
.L506:
	add	w6, w6, w11
	subs	w5, w5, #1
	bpl	.L507
	udiv	w7, w7, w13
	udiv	w6, w6, w13
.L500:
	stp	w4, w3, [x19]
	cmp	w14, w7
	bls	.L508
	ldr	w0, [x19, 8]
	cmp	w0, w9
	bcc	.L527
.L509:
	cmp	w14, w6
	bls	.L510
	ldr	w0, [x19, 12]
	cmp	w0, w9
	bcc	.L528
.L511:
	ldr	w3, [x19, 20]
	ldr	w0, [x20, 16]
	cmp	w3, w0
	bcs	.L499
	ldr	w0, [x19, 8]
	cmp	w0, w9
	bcc	.L513
	tbnz	w8, #31, .L499
	mov	w16, 26125
	mov	w15, 62303
	mov	x12, x10
	mov	w11, w8
	add	x0, x19, 24
	mov	w14, 32
	movk	w16, 0x19, lsl 16
	movk	w15, 0x3c6e, lsl 16
	mov	x13, 1
	b	.L516
	.p2align 2
.L529:
	ldr	w3, [x19, 20]
.L516:
	ldp	w4, w7, [x0, 8]
	cmp	w3, 32
	ldp	w5, w17, [x0]
	csel	w2, w3, w14, ls
	ldrsh	w20, [x12]
	madd	w18, w7, w16, w15
	sub	w1, w5, w17
	sub	w6, w14, w2
	add	w1, w1, w4
	sub	w4, w2, #1
	add	w1, w1, w20
	lsr	w7, w7, w6
	lsl	x4, x13, x4
	sub	w4, w4, w7
	lsr	w3, w18, w6
	add	w3, w3, w1
	add	w4, w4, w3
	add	w5, w5, w5, lsr 31
	lsl	x3, x13, x2
	neg	w3, w3
	and	w3, w4, w3
	asr	w5, w5, 1
	sub	w1, w1, w3
	stp	w1, w5, [x0]
	stp	w17, w18, [x0, 8]
	lsr	w3, w3, w2
	strh	w3, [x12], 4
	subs	w11, w11, #1
	bpl	.L529
	ldr	w1, [x19, 12]
	cmp	w1, w9
	bcc	.L499
	add	x10, x10, 2
	ldr	w3, [x19, 20]
	b	.L518
	.p2align 2
.L524:
	mov	w11, 1073741824
.L505:
	sub	w3, w3, #512
	b	.L506
	.p2align 2
.L522:
	mov	w11, 1073741824
.L502:
	ldrsh	w0, [x2, 2]
	sub	w4, w4, #512
	add	w7, w7, w11
	add	x2, x2, 4
	sub	w0, w0, w3, lsr 16
	cmp	w0, w1
	ble	.L530
.L523:
	mov	w11, w12
	b	.L504
	.p2align 2
.L521:
	mov	w11, w12
	b	.L501
	.p2align 2
.L513:
	ldr	w0, [x19, 12]
	cmp	w0, w9
	bcs	.L531
.L499:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L510:
	str	wzr, [x19, 12]
	b	.L511
	.p2align 2
.L508:
	str	wzr, [x19, 8]
	b	.L509
	.p2align 2
.L528:
	add	w13, w0, w13
	str	w13, [x19, 12]
	b	.L511
	.p2align 2
.L527:
	add	w0, w0, w13
	str	w0, [x19, 8]
	b	.L509
	.p2align 2
.L531:
	add	x10, x10, 2
	tbnz	w8, #31, .L499
	add	x0, x19, 24
.L518:
	mov	w12, 26125
	mov	w11, 62303
	mov	w9, 32
	movk	w12, 0x19, lsl 16
	movk	w11, 0x3c6e, lsl 16
	mov	x7, 1
	b	.L517
	.p2align 2
.L532:
	ldr	w3, [x19, 20]
.L517:
	ldp	w2, w6, [x0, 8]
	cmp	w3, 32
	ldp	w4, w14, [x0]
	csel	w3, w3, w9, ls
	ldrsh	w15, [x10]
	madd	w13, w6, w12, w11
	sub	w1, w4, w14
	sub	w5, w9, w3
	add	w1, w1, w2
	sub	w2, w3, #1
	add	w1, w1, w15
	lsr	w6, w6, w5
	lsl	x2, x7, x2
	sub	w2, w2, w6
	lsr	w5, w13, w5
	add	w5, w5, w1
	add	w2, w2, w5
	add	w4, w4, w4, lsr 31
	lsl	x5, x7, x3
	neg	w5, w5
	and	w2, w2, w5
	asr	w4, w4, 1
	sub	w1, w1, w2
	stp	w1, w4, [x0]
	stp	w14, w13, [x0, 8]
	lsr	w2, w2, w3
	strh	w2, [x10], 4
	subs	w8, w8, #1
	bpl	.L532
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L520:
	mov	w6, 0
	mov	w7, 0
	b	.L500
	.size	util_dre_16_bit_process, .-util_dre_16_bit_process
	.section	.rodata
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_vol_coef, %object
	.size	g_vol_coef, 400
g_vol_coef:
	.word	66633081
	.word	59386796
	.word	52928538
	.word	47172609
	.word	42042632
	.word	37470535
	.word	33395649
	.word	29763904
	.word	26527107
	.word	23642309
	.word	21071230
	.word	18779754
	.word	16737473
	.word	14917288
	.word	13295047
	.word	11849223
	.word	10560631
	.word	9412172
	.word	8388608
	.word	7476354
	.word	6663308
	.word	5938679
	.word	5292853
	.word	4717260
	.word	4204263
	.word	3747053
	.word	3339564
	.word	2976390
	.word	2652710
	.word	2364230
	.word	2107123
	.word	1877975
	.word	1673747
	.word	1491728
	.word	1329504
	.word	1184922
	.word	1056063
	.word	941217
	.word	838860
	.word	747635
	.word	666330
	.word	593867
	.word	529285
	.word	471726
	.word	420426
	.word	374705
	.word	333956
	.word	297639
	.word	265271
	.word	236423
	.word	210712
	.word	187797
	.word	167374
	.word	149172
	.word	132950
	.word	118492
	.word	105606
	.word	94121
	.word	83886
	.word	74763
	.word	66633
	.word	59386
	.word	52928
	.word	47172
	.word	42042
	.word	37470
	.word	33395
	.word	29763
	.word	26527
	.word	23642
	.word	21071
	.word	18779
	.word	16737
	.word	14917
	.word	13295
	.word	11849
	.word	10560
	.word	9412
	.word	8388
	.word	7476
	.word	6663
	.word	5938
	.word	5292
	.word	4717
	.word	4204
	.word	3747
	.word	3339
	.word	2976
	.word	2652
	.word	2364
	.word	2107
	.word	1877
	.word	1673
	.word	1491
	.word	1329
	.word	1184
	.word	1056
	.word	941
	.word	838
	.word	0
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
