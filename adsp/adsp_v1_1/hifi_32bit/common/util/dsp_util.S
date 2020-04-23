#include "hi_asm_define.h"
	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 2
	.file	"dsp_util.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	volume_db2_coef, %function
volume_db2_coef:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	uxtb	r3, r0
	cmp	r3, #138
	bhi	.L4
	rsb	r2, r3, #139
	cmp	r2, #98
	bhi	.L5
	ldr	ip, .L10
	cmp	r1, #0
	ldr	r0, [ip, r2, lsl #2]
	ldmfdeq	sp, {fp, sp, pc}
	rsbgt	r3, r3, #138
	rsble	r3, r3, #140
	ldrgt	r3, [ip, r3, lsl #2]
	ldrle	r3, [ip, r3, lsl #2]
	subgt	r3, r3, r0
	suble	r3, r3, r0
	mulgt	r1, r1, r3
	mulle	r1, r3, r1
	addgt	r0, r0, r1, lsr #3
	suble	r0, r0, r1, lsr #3
	ldmfd	sp, {fp, sp, pc}
.L5:
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
.L4:
	movw	r0, #48505
	movt	r0, 1016
	ldmfd	sp, {fp, sp, pc}
.L11:
	.align	2
.L10:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	volume_db2_coef, .-volume_db2_coef
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	AOE_FMUL_16_Q15.part.0, %function
AOE_FMUL_16_Q15.part.0:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	sxth	r0, r0
	cmp	r1, #8388608
	smull	r2, r3, r1, r0
	lsr	r0, r2, #23
	orr	r0, r0, r3, lsl #9
	bls	.L15
	asr	r3, r0, #31
	cmp	r3, r0, asr #15
	eorne	r0, r3, #32512
	eorne	r0, r0, #255
.L15:
	sxth	r0, r0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	AOE_FMUL_16_Q15.part.0, .-AOE_FMUL_16_Q15.part.0
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	volume16_bit_pro_coef, %function
volume16_bit_pro_coef:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r1, #0
	sub	r4, r1, #1
	mov	r6, r2
	ldmfdeq	sp, {r4, r5, r6, r7, fp, sp, pc}
	sub	r5, r0, #2
.L19:
	cmp	r6, #0
	mov	r1, r6
	mov	r3, r6
	ldrsh	r0, [r5, #2]!
	beq	.L18
	bl	AOE_FMUL_16_Q15.part.0
	mov	r3, r0
.L18:
	sub	r4, r4, #1
	strh	r3, [r5]
	cmn	r4, #1
	bne	.L19
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	UNWIND(.fnend)
	.size	volume16_bit_pro_coef, .-volume16_bit_pro_coef
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	AOE_FMUL_24_Q15.part.1, %function
AOE_FMUL_24_Q15.part.1:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	lsr	r0, r0, #8
	smull	r2, r3, r0, r1
	lsr	r0, r2, #23
	orr	r0, r0, r3, lsl #9
	asr	r3, r0, #31
	cmp	r3, r0, asr #23
	movwne	r0, #65535
	movtne	r0, 127
	eorne	r0, r0, r3
	lsl	r0, r0, #8
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	AOE_FMUL_24_Q15.part.1, .-AOE_FMUL_24_Q15.part.1
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	volume24_bit_pro_coef, %function
volume24_bit_pro_coef:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r1, #0
	sub	r4, r1, #1
	mov	r5, r2
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	sub	r6, r0, #4
	b	.L31
.L37:
	lsr	r3, r8, #23
	orr	r3, r3, r9, lsl #9
.L29:
	sub	r4, r4, #1
	str	r3, [r6]
	cmn	r4, #1
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L31:
	cmp	r5, #0
	mov	r3, r5
	ldr	r0, [r6, #4]!
	beq	.L29
	cmp	r5, #8388608
	mov	r1, r5
	smull	r8, r9, r5, r0
	bls	.L37
	bl	AOE_FMUL_24_Q15.part.1
	mov	r3, r0
	b	.L29
	UNWIND(.fnend)
	.size	volume24_bit_pro_coef, .-volume24_bit_pro_coef
	.align	2
	.global	util_calc_frame_size
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_calc_frame_size, %function
util_calc_frame_size:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r1, #16
	lsleq	r0, r0, #1
	lslne	r0, r0, #2
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	util_calc_frame_size, .-util_calc_frame_size
	.align	2
	.global	util_latency_ms2_byte_size
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_latency_ms2_byte_size, %function
util_latency_ms2_byte_size:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	mul	r2, r0, r2
	movw	r0, #19923
	movt	r0, 4194
	umull	r2, r3, r2, r0
	lsr	r0, r3, #6
	mla	r0, r0, r1, r1
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	util_latency_ms2_byte_size, .-util_latency_ms2_byte_size
	.global	__aeabi_uidiv
	.align	2
	.global	util_latency_ms
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_latency_ms, %function
util_latency_ms:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r3, #1000
	mul	r1, r2, r1
	mul	r0, r3, r0
	bl	__aeabi_uidiv
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	util_latency_ms, .-util_latency_ms
	.align	2
	.global	util_lr_linear_gain_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_lr_linear_gain_process, %function
util_lr_linear_gain_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldrsh	r4, [fp, #4]
	cmp	r3, r4
	beq	.L44
	cmp	r2, #0
	ldmfdle	sp, {r4, r5, fp, sp, pc}
	sub	r0, r0, #2
	sub	r1, r1, #2
	mov	lr, #0
	b	.L50
.L55:
	.syntax divided
	smulwb  ip, ip, r4
	qadd    ip, ip, ip
	.arm
	.syntax unified
.L49:
	add	lr, lr, #1
	strh	ip, [r1, #2]!
	cmp	r2, lr
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
.L50:
	tst	lr, #1
	ldrsh	ip, [r0, #2]!
	bne	.L55
	.syntax divided
	smulwb  ip, ip, r3
	qadd    ip, ip, ip
	.arm
	.syntax unified
	b	.L49
.L44:
	cmp	r2, #0
	ldmfdle	sp, {r4, r5, fp, sp, pc}
	sub	r1, r1, #2
	add	r2, r0, r2, lsl #1
.L47:
	ldrsh	ip, [r0], #2
	.syntax divided
	smulwb  ip, ip, r3
	qadd    ip, ip, ip
	.arm
	.syntax unified
	cmp	r0, r2
	strh	ip, [r1, #2]!
	bne	.L47
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	util_lr_linear_gain_process, .-util_lr_linear_gain_process
	.align	2
	.global	util_upmix_pcm_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_upmix_pcm_data, %function
util_upmix_pcm_data:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, [r2, #12]
	cmp	r3, #24
	beq	.L91
	cmp	r3, #16
	ldmfdne	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	ldr	r3, [r2, #8]
	ldr	r5, [r2]
	cmp	r3, #0
	beq	.L92
	mov	r4, #0
	lsl	r8, r0, #1
	mov	r6, r4
	mov	r7, r1
.L74:
	cmp	r0, #0
	movne	ip, r7
	movne	r3, #0
	beq	.L77
.L73:
	ldr	lr, [r2, #16]
	add	ip, ip, #2
	cmp	lr, r3
	add	r3, r3, #1
	ldrhhi	r4, [r5], #2
	cmp	r0, r3
	strh	r4, [ip, #-2]
	bne	.L73
	ldr	r3, [r2, #8]
	add	r7, r7, r8
.L77:
	add	r6, r6, #1
	cmp	r3, r6
	bhi	.L74
	ldr	ip, [r2, #12]
	str	r1, [r2]
	cmp	ip, #16
	str	r0, [r2, #16]
	lslne	r8, r0, #2
.L78:
	mul	r3, r3, r8
	str	r3, [r2, #4]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L91:
	ldr	r3, [r2, #8]
	ldr	r5, [r2]
	cmp	r3, #0
	beq	.L93
	mov	lr, #0
	lsl	r7, r0, #2
	mov	r6, lr
	mov	r4, r1
.L63:
	cmp	r0, #0
	movne	r3, #0
	beq	.L66
.L62:
	ldr	ip, [r2, #16]
	cmp	ip, r3
	ldrhi	lr, [r5], #4
	str	lr, [r4, r3, lsl #2]
	add	r3, r3, #1
	cmp	r0, r3
	bne	.L62
	ldr	r3, [r2, #8]
	add	r4, r4, r7
.L66:
	add	r6, r6, #1
	cmp	r3, r6
	bhi	.L63
	ldr	ip, [r2, #12]
	str	r1, [r2]
	cmp	ip, #16
	str	r0, [r2, #16]
	beq	.L94
.L67:
	mul	r3, r3, r7
	str	r3, [r2, #4]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L94:
	lsl	r7, r0, #1
	b	.L67
.L93:
	str	r1, [r2]
	lsl	r7, r0, #2
	str	r0, [r2, #16]
	b	.L67
.L92:
	str	r1, [r2]
	lsl	r8, r0, #1
	str	r0, [r2, #16]
	b	.L78
	UNWIND(.fnend)
	.size	util_upmix_pcm_data, .-util_upmix_pcm_data
	.align	2
	.global	util_downmix_pcm_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_downmix_pcm_data, %function
util_downmix_pcm_data:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, [r2, #12]
	cmp	r3, #24
	beq	.L126
	cmp	r3, #16
	ldmfdne	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	ldr	r3, [r2, #8]
	ldr	r6, [r2]
	cmp	r3, #0
	lslne	r8, r0, #1
	movne	r7, r1
	movne	r5, #0
	beq	.L127
.L110:
	cmp	r0, #0
	subne	lr, r6, #2
	movne	ip, r7
	movne	r3, #0
	beq	.L114
.L109:
	add	r3, r3, #1
	ldrh	r4, [lr, #2]!
	cmp	r0, r3
	strh	r4, [ip], #2
	bne	.L109
	ldr	r3, [r2, #8]
	add	r6, r6, r8
	add	r7, r7, r8
.L114:
	add	r5, r5, #1
	cmp	r3, r5
	bhi	.L110
	ldr	ip, [r2, #12]
	str	r1, [r2]
	cmp	ip, #16
	str	r0, [r2, #16]
	lslne	r8, r0, #2
.L115:
	mul	r3, r3, r8
	str	r3, [r2, #4]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L126:
	ldr	r3, [r2, #8]
	ldr	r6, [r2]
	cmp	r3, #0
	lslne	r8, r0, #2
	movne	r7, r1
	movne	r5, #0
	beq	.L128
.L100:
	cmp	r0, #0
	subne	lr, r6, #4
	movne	ip, r7
	movne	r3, #0
	beq	.L104
.L99:
	add	r3, r3, #1
	ldr	r4, [lr, #4]!
	cmp	r0, r3
	str	r4, [ip], #4
	bne	.L99
	ldr	r3, [r2, #8]
	add	r6, r6, r8
	add	r7, r7, r8
.L104:
	add	r5, r5, #1
	cmp	r3, r5
	bhi	.L100
	ldr	ip, [r2, #12]
	str	r1, [r2]
	cmp	ip, #16
	str	r0, [r2, #16]
	lsleq	r8, r0, #1
	b	.L115
.L128:
	str	r1, [r2]
	lsl	r8, r0, #2
	str	r0, [r2, #16]
	b	.L115
.L127:
	str	r1, [r2]
	lsl	r8, r0, #1
	str	r0, [r2, #16]
	b	.L115
	UNWIND(.fnend)
	.size	util_downmix_pcm_data, .-util_downmix_pcm_data
	.align	2
	.global	util_unify_bit_depth_pcm_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_unify_bit_depth_pcm_data, %function
util_unify_bit_depth_pcm_data:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, [r2, #8]
	ldr	ip, [r2, #16]
	cmp	r0, #16
	mul	r3, ip, r3
	beq	.L144
	cmp	r0, #24
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	ldr	r0, [r2, #12]
	cmp	r0, #16
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	cmp	r3, #0
	ldr	lr, [r2]
	beq	.L134
	sub	lr, lr, #2
	mov	r4, r1
	mov	ip, #0
.L135:
	ldrh	r0, [lr, #2]!
	add	ip, ip, #1
	cmp	r3, ip
	lsl	r0, r0, #16
	str	r0, [r4], #4
	bne	.L135
.L134:
	ldr	r3, [r2, #4]
	mov	r0, #24
	str	r1, [r2]
	str	r0, [r2, #12]
	lsl	r3, r3, #1
	str	r3, [r2, #4]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L144:
	ldr	r0, [r2, #12]
	cmp	r0, #24
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	cmp	r3, #0
	ldr	lr, [r2]
	beq	.L132
	sub	lr, lr, #4
	mov	r4, r1
	mov	ip, #0
.L133:
	ldr	r0, [lr, #4]!
	add	ip, ip, #1
	cmp	r3, ip
	lsr	r0, r0, #16
	strh	r0, [r4], #2
	bne	.L133
.L132:
	ldr	r3, [r2, #4]
	mov	r0, #16
	str	r1, [r2]
	str	r0, [r2, #12]
	lsr	r3, r3, #1
	str	r3, [r2, #4]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	util_unify_bit_depth_pcm_data, .-util_unify_bit_depth_pcm_data
	.align	2
	.global	utils_gain_balance_coef
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	utils_gain_balance_coef, %function
utils_gain_balance_coef:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r2, #0
	ldmfdeq	sp, {fp, sp, pc}
	blt	.L151
	ldr	r3, [r0]
	rsb	r2, r2, #50
	movw	r1, #34079
	movt	r1, 20971
	mul	r2, r3, r2
	umull	r2, r3, r2, r1
	lsr	r3, r3, #4
	str	r3, [r0]
	ldmfd	sp, {fp, sp, pc}
.L151:
	ldr	r3, [r1]
	add	r2, r2, #50
	movw	r0, #34079
	movt	r0, 20971
	mul	r2, r3, r2
	umull	r2, r3, r2, r0
	lsr	r3, r3, #4
	str	r3, [r1]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	utils_gain_balance_coef, .-utils_gain_balance_coef
	.align	2
	.global	util_lr_gain_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_lr_gain_process, %function
util_lr_gain_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r5, r2
	ldr	r2, [r2]
	mov	r6, r0
	ldr	r3, [r0, #8]
	mov	r8, r1
	ldr	r4, [r0, #16]
	cmp	r2, #1
	mul	r4, r4, r3
	beq	.L215
	ldr	r1, [r5, #12]
	ldr	r0, [r5, #4]
	bl	volume_db2_coef
	ldr	r1, [r5, #16]
	mov	r7, r0
	ldr	r0, [r5, #8]
	bl	volume_db2_coef
	cmp	r8, #0
	mov	r5, r0
	bne	.L216
.L170:
	cmp	r7, #8388608
	cmpeq	r5, #8388608
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	ldr	r3, [r6, #12]
	cmp	r3, #16
	beq	.L172
	cmp	r3, #24
	ldmfdne	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	cmp	r7, r5
	ldr	r0, [r6]
	beq	.L181
	cmp	r4, #0
	sub	r4, r4, #1
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	sub	r6, r0, #4
	b	.L189
.L217:
	cmp	r7, #0
	moveq	r0, r7
	beq	.L187
	cmp	r7, #8388608
	bhi	.L185
	smull	r2, r3, r7, r0
	lsr	r0, r2, #23
	orr	r0, r0, r3, lsl #9
.L187:
	subs	r4, r4, #1
	str	r0, [r6]
	ldmfdcc	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L189:
	tst	r4, #1
	ldr	r0, [r6, #4]!
	bne	.L217
	cmp	r5, #0
	moveq	r0, r5
	beq	.L187
	cmp	r5, #8388608
	bhi	.L188
	smull	r2, r3, r5, r0
	lsr	r0, r2, #23
	orr	r0, r0, r3, lsl #9
	b	.L187
.L216:
	blt	.L218
	rsb	r2, r8, #50
	movw	r3, #34079
	movt	r3, 20971
	mul	r7, r7, r2
	umull	r2, r3, r7, r3
	lsr	r7, r3, #4
	b	.L170
.L215:
	ldr	r3, [r0, #12]
	cmp	r3, #16
	beq	.L155
	cmp	r3, #24
	ldmfdne	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	ldrsh	r0, [r5, #4]
	ldrsh	ip, [r5, #8]
	ldr	r2, [r6]
	cmp	r0, ip
	beq	.L164
	cmp	r4, #0
	ldmfdle	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	sub	r2, r2, #4
	mov	r1, #0
	b	.L169
.L219:
	.syntax divided
	smulwb  r3, r3, ip
	qadd    r3, r3, r3
	.arm
	.syntax unified
.L168:
	add	r1, r1, #1
	str	r3, [r2]
	cmp	r4, r1
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L169:
	tst	r1, #1
	ldr	r3, [r2, #4]!
	bne	.L219
	.syntax divided
	smulwb  r3, r3, r0
	qadd    r3, r3, r3
	.arm
	.syntax unified
	b	.L168
.L172:
	cmp	r7, r5
	ldr	r0, [r6]
	beq	.L174
	cmp	r4, #0
	sub	r4, r4, #1
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	sub	r6, r0, #2
	b	.L180
.L220:
	cmp	r7, #0
	mov	r1, r7
	mov	r3, r7
	beq	.L179
.L214:
	bl	AOE_FMUL_16_Q15.part.0
	mov	r3, r0
.L179:
	subs	r4, r4, #1
	strh	r3, [r6]
	ldmfdcc	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L180:
	tst	r4, #1
	ldrsh	r0, [r6, #2]!
	bne	.L220
	cmp	r5, #0
	mov	r1, r5
	mov	r3, r5
	beq	.L179
	b	.L214
.L155:
	ldrsh	r0, [r5, #4]
	ldrsh	ip, [r5, #8]
	ldr	r2, [r6]
	cmp	r0, ip
	beq	.L157
	cmp	r4, #0
	ldmfdle	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	sub	r2, r2, #2
	mov	r1, #0
	b	.L163
.L221:
	.syntax divided
	smulwb  r3, r3, ip
	qadd    r3, r3, r3
	.arm
	.syntax unified
.L162:
	add	r1, r1, #1
	strh	r3, [r2]
	cmp	r4, r1
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L163:
	tst	r1, #1
	ldrsh	r3, [r2, #2]!
	bne	.L221
	.syntax divided
	smulwb  r3, r3, r0
	qadd    r3, r3, r3
	.arm
	.syntax unified
	b	.L162
.L185:
	mov	r1, r7
	bl	AOE_FMUL_24_Q15.part.1
	b	.L187
.L188:
	mov	r1, r5
	bl	AOE_FMUL_24_Q15.part.1
	b	.L187
.L218:
	add	r2, r8, #50
	movw	r3, #34079
	movt	r3, 20971
	mul	r2, r0, r2
	umull	r2, r3, r2, r3
	lsr	r5, r3, #4
	b	.L170
.L174:
	mov	r2, r7
	mov	r1, r4
	bl	volume16_bit_pro_coef
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L181:
	mov	r2, r7
	mov	r1, r4
	bl	volume24_bit_pro_coef
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L164:
	cmp	r4, #0
	ldmfdle	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	sub	r2, r2, #4
	mov	r1, #0
.L166:
	ldr	r3, [r2, #4]!
	.syntax divided
	smulwb  r3, r3, r0
	qadd    r3, r3, r3
	.arm
	.syntax unified
	add	r1, r1, #1
	str	r3, [r2]
	cmp	r4, r1
	bne	.L166
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L157:
	cmp	r4, #0
	ldmfdle	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	add	r4, r2, r4, lsl #1
.L160:
	ldrsh	r3, [r2]
	.syntax divided
	smulwb  r3, r3, r0
	qadd    r3, r3, r3
	.arm
	.syntax unified
	strh	r3, [r2], #2
	cmp	r4, r2
	bne	.L160
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	UNWIND(.fnend)
	.size	util_lr_gain_process, .-util_lr_gain_process
	.global	__aeabi_idiv
	.align	2
	.global	util_lr_fade_gain_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_lr_fade_gain_process, %function
util_lr_fade_gain_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #28)
	sub	sp, sp, #28
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r10, r0
	str	r1, [fp, #-48]
	ldr	r0, [r2, #4]
	mov	r6, r1
	ldr	r1, [r2, #12]
	mov	r5, r3
	bl	volume_db2_coef
	ldr	r1, [r5, #12]
	mov	r4, r0
	ldr	r0, [r5, #4]
	bl	volume_db2_coef
	ldr	r3, [r10, #8]
	ldr	r9, [r10]
	mov	r1, r3
	str	r3, [fp, #-56]
	sub	r0, r0, r4
	bl	__aeabi_idiv
	ldr	r3, [fp, #-56]
	cmp	r3, #0
	str	r0, [fp, #-52]
	beq	.L222
	add	r8, r6, #50
	rsb	r6, r6, #50
	movw	r2, #34079
	movt	r2, 20971
	mul	r1, r6, r0
	str	r2, [fp, #-68]
	mul	r2, r8, r0
	mov	r7, #0
	mul	r8, r4, r8
	str	r9, [fp, #-64]
	str	r1, [fp, #-60]
	mov	ip, r3
	mul	r6, r6, r4
	str	r2, [fp, #-56]
	mov	r2, r9
	b	.L234
.L226:
	cmp	r3, #24
	beq	.L244
.L229:
	ldr	r3, [fp, #-52]
	add	r7, r7, #1
	cmp	ip, r7
	add	r4, r4, r3
	ldr	r3, [fp, #-56]
	add	r8, r8, r3
	ldr	r3, [fp, #-60]
	add	r6, r6, r3
	bls	.L222
.L234:
	ldr	r3, [fp, #-48]
	cmp	r3, #0
	moveq	r5, r4
	moveq	r1, r4
	beq	.L224
	ldr	r3, [fp, #-68]
	movlt	r1, r4
	movge	r5, r4
	umulllt	r2, r3, r8, r3
	umullge	r2, r3, r6, r3
	lsrlt	r5, r3, #4
	lsrge	r1, r3, #4
.L224:
	ldr	r3, [r10, #12]
	cmp	r3, #16
	bne	.L226
	cmp	r1, #0
	ldrsh	r0, [r9]
	moveq	r0, r1
	beq	.L227
	bl	AOE_FMUL_16_Q15.part.0
.L227:
	cmp	r5, #0
	strh	r0, [r9]
	ldrsh	r0, [r9, #2]
	moveq	r0, r5
	beq	.L228
	mov	r1, r5
	bl	AOE_FMUL_16_Q15.part.0
.L228:
	strh	r0, [r9, #2]
	add	r9, r9, #4
	ldr	ip, [r10, #8]
	b	.L229
.L244:
	ldr	r3, [fp, #-64]
	cmp	r1, #0
	ldr	r0, [r3]
	beq	.L230
	cmp	r1, #8388608
	bhi	.L231
	smull	r2, r3, r1, r0
	lsr	r1, r2, #23
	orr	r1, r1, r3, lsl #9
.L230:
	ldr	r3, [fp, #-64]
	cmp	r5, #0
	ldr	r0, [r3, #4]
	moveq	r0, r5
	str	r1, [r3]
	beq	.L232
	cmp	r5, #8388608
	bhi	.L233
	smull	r2, r3, r5, r0
	lsr	r0, r2, #23
	orr	r0, r0, r3, lsl #9
.L232:
	ldr	r3, [fp, #-64]
	add	r3, r3, #8
	str	r0, [r3, #-4]
	str	r3, [fp, #-64]
	ldr	ip, [r10, #8]
	b	.L229
.L222:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L233:
	mov	r1, r5
	bl	AOE_FMUL_24_Q15.part.1
	b	.L232
.L231:
	bl	AOE_FMUL_24_Q15.part.1
	mov	r1, r0
	b	.L230
	UNWIND(.fnend)
	.size	util_lr_fade_gain_process, .-util_lr_fade_gain_process
	.align	2
	.global	util_mixer_pro
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_mixer_pro, %function
util_mixer_pro:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r0, #0
	clz	ip, r1
	lsr	ip, ip, #5
	moveq	ip, #1
	cmp	r2, #0
	moveq	ip, #1
	cmp	ip, #0
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	cmp	r3, #16
	beq	.L260
	cmp	r3, #24
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	cmp	r2, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	sub	r0, r0, #4
.L250:
	ldr	r3, [r0, #4]!
	ldr	lr, [r1], #4
	.syntax divided
	qadd r3, r3, lr
	.arm
	.syntax unified
	add	ip, ip, #1
	str	r3, [r0]
	cmp	r2, ip
	bne	.L250
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L260:
	cmp	r2, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	sub	r1, r1, #2
	sub	r0, r0, #2
	movw	r4, #32767
.L249:
	ldrsh	lr, [r1, #2]!
	add	ip, ip, #1
	ldrsh	r3, [r0, #2]!
	add	r3, r3, lr
	asr	lr, r3, #31
	cmp	lr, r3, asr #15
	eorne	r3, lr, r4
	cmp	r2, ip
	strh	r3, [r0]
	bne	.L249
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	util_mixer_pro, .-util_mixer_pro
	.align	2
	.global	util_channel_mode_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_channel_mode_process, %function
util_channel_mode_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, [r0, #8]
	ldr	ip, [r0, #16]
	sub	r1, r1, #1
	mul	r3, ip, r3
	cmp	r1, #6
	ldrls	pc, [pc, r1, asl #2]
	b	.L261
.L264:
	.word	.L263
	.word	.L265
	.word	.L266
	.word	.L267
	.word	.L268
	.word	.L269
	.word	.L270
.L261:
	ldmfd	sp, {fp, sp, pc}
.L269:
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #16
	ldr	r1, [r0]
	beq	.L376
	cmp	r2, #24
	ldmfdne	sp, {fp, sp, pc}
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	mov	r2, #0
	mov	r0, r2
.L291:
	add	r2, r2, #1
	str	r0, [r1, #4]
	cmp	r2, r3
	add	r1, r1, #8
	bne	.L291
	ldmfd	sp, {fp, sp, pc}
.L263:
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #16
	ldr	r1, [r0]
	beq	.L377
	cmp	r2, #24
	ldmfdne	sp, {fp, sp, pc}
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #8
	mov	r0, #0
.L276:
	ldmdb	r2, {r1, ip}
	.syntax divided
	qadd r1, r1, ip
	.arm
	.syntax unified
	add	r0, r0, #1
	str	r1, [r2, #-8]
	cmp	r0, r3
	str	r1, [r2, #-4]
	add	r2, r2, #8
	bne	.L276
	ldmfd	sp, {fp, sp, pc}
.L265:
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #16
	ldr	r1, [r0]
	beq	.L378
	cmp	r2, #24
	ldmfdne	sp, {fp, sp, pc}
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #8
	mov	r1, #0
.L279:
	ldr	r0, [r2, #-8]
	add	r1, r1, #1
	cmp	r1, r3
	add	r2, r2, #8
	str	r0, [r2, #-12]
	bne	.L279
	ldmfd	sp, {fp, sp, pc}
.L266:
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #16
	ldr	r1, [r0]
	beq	.L379
	cmp	r2, #24
	ldmfdne	sp, {fp, sp, pc}
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #8
	mov	r1, #0
.L282:
	ldr	r0, [r2, #-4]
	add	r1, r1, #1
	cmp	r1, r3
	add	r2, r2, #8
	str	r0, [r2, #-16]
	bne	.L282
	ldmfd	sp, {fp, sp, pc}
.L267:
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #16
	ldr	r1, [r0]
	beq	.L380
	cmp	r2, #24
	ldmfdne	sp, {fp, sp, pc}
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #8
	mov	r1, #0
.L285:
	ldr	ip, [r2, #-8]
	add	r1, r1, #1
	ldr	r0, [r2, #-4]
	cmp	r1, r3
	stmdb	r2, {r0, ip}
	add	r2, r2, #8
	bne	.L285
	ldmfd	sp, {fp, sp, pc}
.L268:
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #16
	ldr	r1, [r0]
	beq	.L381
	cmp	r2, #24
	ldmfdne	sp, {fp, sp, pc}
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	mov	r2, #0
	mov	r0, r2
.L288:
	str	r0, [r1, r2, lsl #3]
	add	r2, r2, #1
	cmp	r2, r3
	bne	.L288
	ldmfd	sp, {fp, sp, pc}
.L270:
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #16
	ldr	r1, [r0]
	beq	.L382
	cmp	r2, #24
	ldmfdne	sp, {fp, sp, pc}
	mov	r2, #0
	mov	r0, r2
.L294:
	add	r2, r2, #1
	str	r0, [r1], #4
	cmp	r3, r2
	bne	.L294
	ldmfd	sp, {fp, sp, pc}
.L376:
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	mov	r2, #0
	mov	r0, r2
.L290:
	add	r2, r2, #1
	strh	r0, [r1, #2]
	cmp	r2, r3
	add	r1, r1, #4
	bne	.L290
	ldmfd	sp, {fp, sp, pc}
.L378:
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #4
	mov	r1, #0
.L278:
	ldrh	r0, [r2, #-4]
	add	r1, r1, #1
	cmp	r1, r3
	add	r2, r2, #4
	strh	r0, [r2, #-6]
	bne	.L278
	ldmfd	sp, {fp, sp, pc}
.L377:
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #4
	mov	r0, #0
	movw	lr, #32767
.L275:
	ldrsh	ip, [r2, #-2]
	add	r0, r0, #1
	ldrsh	r1, [r2, #-4]
	add	r2, r2, #4
	add	r1, r1, ip
	asr	ip, r1, #31
	cmp	ip, r1, asr #15
	eorne	r1, ip, lr
	cmp	r0, r3
	sxth	r1, r1
	strh	r1, [r2, #-8]
	strh	r1, [r2, #-6]
	bne	.L275
	ldmfd	sp, {fp, sp, pc}
.L382:
	mov	r2, #0
	mov	r0, r2
.L293:
	add	r2, r2, #1
	strh	r0, [r1], #2
	cmp	r3, r2
	bne	.L293
	ldmfd	sp, {fp, sp, pc}
.L381:
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #4
	mov	r1, #0
	mov	r0, r1
.L287:
	add	r1, r1, #1
	strh	r0, [r2, #-4]
	cmp	r1, r3
	add	r2, r2, #4
	bne	.L287
	ldmfd	sp, {fp, sp, pc}
.L380:
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #4
	mov	r1, #0
.L284:
	ldrsh	ip, [r2, #-4]
	add	r1, r1, #1
	ldrh	r0, [r2, #-2]
	cmp	r1, r3
	add	r2, r2, #4
	strh	ip, [r2, #-6]
	strh	r0, [r2, #-8]
	bne	.L284
	ldmfd	sp, {fp, sp, pc}
.L379:
	lsrs	r3, r3, #1
	ldmfdeq	sp, {fp, sp, pc}
	add	r2, r1, #4
	mov	r1, #0
.L281:
	ldrh	r0, [r2, #-2]
	add	r1, r1, #1
	cmp	r1, r3
	add	r2, r2, #4
	strh	r0, [r2, #-8]
	bne	.L281
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	util_channel_mode_process, .-util_channel_mode_process
	.align	2
	.global	fade_create
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	fade_create, %function
fade_create:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #12)
	sub	sp, sp, #12
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r5, r1
	movw	r1, #60927
	sub	ip, r5, #1
	movt	r1, 2
	cmp	ip, r1
	mov	r6, r2
	mov	r4, r0
	mov	r2, #0
	str	r2, [r0]
	bhi	.L383
	bic	r2, r6, #8
	cmp	r2, #16
	bne	.L383
	sub	r1, r3, #1
	cmp	r3, #8
	cmpne	r1, #1
	movhi	r7, #1
	movls	r7, #0
	bls	.L390
.L383:
	sub	sp, fp, #32
	ldmfd	sp, {r4, r5, r6, r7, r8, fp, sp, pc}
.L390:
	mov	r1, #36
	mov	r0, #70
	str	r3, [fp, #-40]
	bl	HI_ADSP_MALLOC
	subs	r8, r0, #0
	beq	.L383
	mov	r2, #36
	mov	r1, r7
	bl	adsp_memset
	ldr	r3, [fp, #-40]
	str	r6, [r8]
	str	r7, [r8, #32]
	stmib	r8, {r3, r5}
	str	r8, [r4]
	b	.L383
	UNWIND(.fnend)
	.size	fade_create, .-fade_create
	.align	2
	.global	fade_flush
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	fade_flush, %function
fade_flush:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	movw	ip, #60927
	sub	lr, r1, #1
	movt	ip, 2
	cmp	lr, ip
	ldmfdhi	sp, {fp, sp, pc}
	bic	ip, r2, #8
	cmp	ip, #16
	ldmfdne	sp, {fp, sp, pc}
	sub	ip, r3, #1
	cmp	r3, #8
	cmpne	ip, #1
	movhi	ip, #1
	movls	ip, #0
	cmp	r0, #0
	eorne	ip, ip, #1
	moveq	ip, #0
	cmp	ip, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	ip, [r0]
	cmp	ip, r2
	beq	.L402
.L395:
	ldr	ip, [r0, #12]
	mov	lr, #0
	str	r1, [r0, #8]
	cmp	ip, lr
	str	r2, [r0]
	str	r3, [r0, #4]
	mulne	r1, r1, ip
	movwne	r2, #18849
	movtne	r2, 53024
	str	lr, [r0, #20]
	ldr	lr, [r0, #32]
	streq	ip, [r0, #24]
	umullne	r2, r3, r1, r2
	lsrne	r3, r3, #16
	addne	r3, r3, #1
	strne	r3, [r0, #24]
	cmp	lr, #1
	beq	.L403
	cmp	lr, #2
	moveq	r3, #18
	streq	r3, [r0, #16]
	ldmfd	sp, {fp, sp, pc}
.L402:
	ldr	ip, [r0, #4]
	cmp	r3, ip
	bne	.L395
	ldr	ip, [r0, #8]
	cmp	r1, ip
	bne	.L395
	ldmfd	sp, {fp, sp, pc}
.L403:
	mov	r2, #99
	mov	r3, #0
	str	r2, [r0, #16]
	str	r3, [r0, #28]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	fade_flush, .-fade_flush
	.align	2
	.global	fade_destroy
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	fade_destroy, %function
fade_destroy:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	subs	r1, r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	mov	r0, #70
	bl	HI_ADSP_FREE
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	fade_destroy, .-fade_destroy
	.align	2
	.global	fade_active
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	fade_active, %function
fade_active:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r2, #0
	mov	r3, #0
	str	r3, [r0, #20]
	ldrne	r3, [r0, #8]
	movwne	ip, #18849
	str	r2, [r0, #12]
	movtne	ip, 53024
	streq	r2, [r0, #24]
	mulne	r2, r3, r2
	str	r1, [r0, #32]
	umullne	r2, r3, r2, ip
	lsrne	r3, r3, #16
	addne	r3, r3, #1
	strne	r3, [r0, #24]
	cmp	r1, #1
	beq	.L416
	cmp	r1, #2
	moveq	r3, #18
	streq	r3, [r0, #16]
	ldmfd	sp, {fp, sp, pc}
.L416:
	mov	r2, #99
	mov	r3, #0
	str	r2, [r0, #16]
	str	r3, [r0, #28]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	fade_active, .-fade_active
	.align	2
	.global	fade_in_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	fade_in_process, %function
fade_in_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	subs	r4, r0, #0
	mov	r6, r1
	ldmfdeq	sp, {r3, r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
	ldr	r3, [r4, #32]
	cmp	r3, #0
	ldmfdeq	sp, {r3, r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
	cmp	r2, #0
	ldr	r10, [r4, #4]
	ldmfdeq	sp, {r3, r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
	sub	r5, r2, #1
	lsl	r9, r10, #2
	lsl	r8, r10, #1
	ldr	r7, .L453
	b	.L432
.L419:
	ldr	r2, [r4, #16]
	cmp	r1, #16
	ldr	r2, [r7, r2, lsl #2]
	beq	.L429
.L452:
	cmp	r1, #24
	bne	.L430
	mov	r0, r6
	mov	r1, r10
	add	r6, r6, r9
	bl	volume24_bit_pro_coef
.L430:
	ldr	r2, [r4, #20]
	ldr	r1, [r4, #24]
	add	r2, r2, #1
	str	r2, [r4, #20]
	cmp	r2, r1
	beq	.L451
.L428:
	cmp	r5, #0
	sub	r5, r5, #1
	ldmfdeq	sp, {r3, r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L432:
	ldr	r2, [r4, #28]
	ldr	r1, [r4]
	cmp	r2, #0
	bne	.L419
	ldr	r0, [r4, #4]
	cmp	r0, #0
	bne	.L424
	b	.L420
.L421:
	cmp	r1, #24
	bne	.L423
	ldr	ip, [r6]
	cmp	ip, #0
	bne	.L422
.L423:
	cmp	r2, r0
	beq	.L420
.L424:
	cmp	r1, #16
	add	r2, r2, #1
	bne	.L421
	ldrsh	ip, [r6]
	cmp	ip, #0
	beq	.L423
.L422:
	mov	r2, #1
	str	r2, [r4, #28]
	ldr	r2, [r4, #16]
	cmp	r1, #16
	ldr	r2, [r7, r2, lsl #2]
	bne	.L452
.L429:
	mov	r0, r6
	mov	r1, r10
	add	r6, r6, r8
	bl	volume16_bit_pro_coef
	b	.L430
.L420:
	cmp	r1, #16
	addeq	r6, r6, r8
	beq	.L428
	cmp	r1, #24
	addeq	r6, r6, r9
	b	.L428
.L451:
	ldr	r2, [r4, #32]
	mov	r3, #0
	str	r3, [r4, #20]
	cmp	r2, #1
	bne	.L428
	ldr	r2, [r4, #16]
	sub	r2, r2, #1
	str	r2, [r4, #16]
	cmp	r2, #18
	streq	r3, [r4, #32]
	b	.L428
.L454:
	.align	2
.L453:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	fade_in_process, .-fade_in_process
	.align	2
	.global	fade_out_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	fade_out_process, %function
fade_out_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #12)
	sub	sp, sp, #12
	push	{lr}
	bl	__gnu_mcount_nc
	subs	r4, r0, #0
	mov	r6, r1
	mov	r5, r2
	beq	.L455
	cmp	r2, #0
	ldr	r9, [r4, #4]
	beq	.L455
	lsl	r3, r9, #1
	ldr	r7, .L471
	lsl	r10, r9, #2
	str	r3, [fp, #-48]
	mov	r8, #0
	b	.L463
.L461:
	cmp	r5, #0
	beq	.L455
.L463:
	ldr	ip, [r4]
	ldr	r2, [r4, #16]
	cmp	ip, #16
	ldr	r2, [r7, r2, lsl #2]
	beq	.L458
	cmp	ip, #24
	mov	r0, r6
	mov	r1, r9
	bne	.L457
	bl	volume24_bit_pro_coef
	add	r6, r6, r10
.L457:
	ldr	r2, [r4, #20]
	sub	r5, r5, #1
	ldr	r1, [r4, #24]
	add	r2, r2, #1
	str	r2, [r4, #20]
	cmp	r2, r1
	bne	.L461
	ldr	r2, [r4, #32]
	str	r8, [r4, #20]
	cmp	r2, #2
	bne	.L461
	ldr	r2, [r4, #16]
	add	r2, r2, #1
	str	r2, [r4, #16]
	cmp	r2, #99
	streq	r8, [r4, #32]
	cmp	r5, #0
	bne	.L463
.L455:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L458:
	ldr	r3, [fp, #-48]
	mov	r0, r6
	mov	r1, r9
	add	r6, r6, r3
	bl	volume16_bit_pro_coef
	b	.L457
.L472:
	.align	2
.L471:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	fade_out_process, .-fade_out_process
	.align	2
	.global	util_sqrt
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_sqrt, %function
util_sqrt:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	subs	r5, r0, #0
	moveq	r4, r5
	beq	.L473
	tst	r5, #1
	bne	.L483
	mov	r3, #1
	b	.L477
.L476:
	add	r3, r3, #1
	cmp	r3, #16
	beq	.L485
.L477:
	lsr	r4, r5, r3
	tst	r4, #1
	beq	.L476
	lsr	r3, r3, #1
	mov	r0, r5
	lsl	r4, r4, r3
	mov	r1, r4
	bl	__aeabi_uidiv
	b	.L478
.L486:
	cmp	r3, #1
	add	r3, r0, r4
	mov	r0, r5
	movls	r2, #0
	movhi	r2, #1
	cmp	r2, #0
	beq	.L473
.L481:
	lsr	r4, r3, #1
	mov	r1, r4
	bl	__aeabi_uidiv
.L478:
	cmp	r0, r4
	sub	r2, r4, r0
	sub	r3, r0, r4
	bhi	.L486
	cmp	r2, #1
	add	r3, r0, r4
	mov	r0, r5
	movls	r2, #0
	movhi	r2, #1
	cmp	r2, #0
	bne	.L481
.L473:
	mov	r0, r4
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L485:
	lsl	r4, r4, #8
	mov	r0, r5
	mov	r1, r4
	bl	__aeabi_uidiv
	b	.L478
.L483:
	mov	r4, r5
	mov	r0, #1
	b	.L478
	UNWIND(.fnend)
	.size	util_sqrt, .-util_sqrt
	.align	2
	.global	util_dre_16_bit_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	util_dre_16_bit_process, %function
util_dre_16_bit_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #20)
	sub	sp, sp, #20
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r9, r0
	ldr	r0, [r0, #20]
	mov	r3, #1000
	movw	r2, #19923
	movt	r2, 4194
	ldr	r8, [r9, #8]
	mul	r3, r3, r0
	mov	r4, r1
	ldr	r1, [r9]
	subs	ip, r8, #1
	ldr	r10, [r4, #16]
	str	ip, [fp, #-56]
	str	r1, [fp, #-60]
	umull	r2, r3, r3, r2
	ldr	r6, [r4]
	ldr	r5, [r4, #4]
	lsr	r3, r3, #6
	str	r3, [fp, #-48]
	bmi	.L508
	mov	r7, #0
	mov	lr, #1
	add	r1, r1, #4
	movt	lr, 16383
	mov	r0, r7
	b	.L495
.L518:
	cmn	r3, #32768
	blt	.L510
	cmp	r3, #0
	mul	r2, r3, r3
	blt	.L490
.L489:
	add	r6, r6, #512
.L491:
	ldrsh	r3, [r1, #-2]
	add	r0, r0, r2
	sub	r3, r3, r5, lsr #16
	cmp	r3, #32768
	bge	.L511
	cmn	r3, #32768
	blt	.L512
	cmp	r3, #0
	mul	r3, r3, r3
	blt	.L493
.L492:
	subs	ip, ip, #1
	add	r5, r5, #512
	add	r7, r7, r3
	add	r1, r1, #4
	bmi	.L517
.L495:
	ldrsh	r3, [r1, #-4]
	sub	r3, r3, r6, lsr #16
	cmp	r3, #32768
	blt	.L518
	mov	r2, lr
	b	.L489
.L511:
	mov	r3, lr
	b	.L492
.L512:
	mov	r3, #1073741824
.L493:
	subs	ip, ip, #1
	sub	r5, r5, #512
	add	r7, r7, r3
	add	r1, r1, #4
	bpl	.L495
.L517:
	mov	r1, r8
	bl	__aeabi_uidiv
	mov	r1, r8
	str	r0, [fp, #-52]
	mov	r0, r7
	bl	__aeabi_uidiv
	ldr	r3, [fp, #-52]
.L488:
	cmp	r10, r3
	str	r6, [r4]
	str	r5, [r4, #4]
	movls	r3, #0
	strls	r3, [r4, #8]
	bls	.L497
	ldr	r3, [r4, #8]
	ldr	r2, [fp, #-48]
	cmp	r3, r2
	addcc	r3, r3, r8
	strcc	r3, [r4, #8]
.L497:
	cmp	r10, r0
	movls	r3, #0
	strls	r3, [r4, #12]
	bls	.L499
	ldr	r3, [r4, #12]
	ldr	r2, [fp, #-48]
	cmp	r3, r2
	addcc	r8, r3, r8
	strcc	r8, [r4, #12]
.L499:
	ldr	r2, [r9, #12]
	ldr	r3, [r4, #20]
	cmp	r3, r2
	bcs	.L487
	ldr	r2, [r4, #8]
	ldr	r1, [fp, #-48]
	cmp	r2, r1
	bcc	.L501
	ldr	r9, [fp, #-56]
	cmp	r9, #0
	blt	.L487
	movw	r2, #62303
	movt	r2, 15470
	str	r2, [fp, #-52]
	movw	r10, #26125
	ldr	r2, [fp, #-60]
	movt	r10, 25
	mov	r8, #1
	add	r5, r2, #4
	b	.L504
.L519:
	ldr	r3, [r4, #20]
.L504:
	cmp	r3, #32
	ldr	ip, [r4, #36]
	ldr	r1, [r4, #24]
	add	r5, r5, #4
	movcc	r0, r3
	ldr	r3, [fp, #-52]
	ldr	lr, [r4, #28]
	movcs	r0, #32
	rsb	r7, r0, #32
	subs	r9, r9, #1
	mla	r6, r10, ip, r3
	ldr	r3, [r4, #32]
	sub	r2, r1, lr
	lsr	ip, ip, r7
	add	r2, r2, r3
	ldrsh	r3, [r5, #-8]
	add	r1, r1, r1, lsr #31
	str	lr, [r4, #32]
	add	r2, r2, r3
	sub	r3, r0, #1
	rsb	ip, ip, r6, lsr r7
	add	r3, r2, r8, lsl r3
	lsl	r7, r8, r0
	add	r3, ip, r3
	rsb	r7, r7, #0
	asr	r1, r1, #1
	and	r3, r3, r7
	str	r1, [r4, #28]
	sub	r2, r2, r3
	str	r6, [r4, #36]
	lsr	r3, r3, r0
	str	r2, [r4, #24]
	strh	r3, [r5, #-8]
	bpl	.L519
	ldr	r3, [r4, #12]
	ldr	r2, [fp, #-48]
	cmp	r3, r2
	bcc	.L487
	ldr	r3, [fp, #-60]
	add	r5, r3, #2
	ldr	r3, [r4, #20]
	b	.L506
.L510:
	mov	r2, #1073741824
.L490:
	sub	r6, r6, #512
	b	.L491
.L501:
	ldr	r2, [r4, #12]
	ldr	r1, [fp, #-48]
	cmp	r2, r1
	bcs	.L520
.L487:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L520:
	ldr	r2, [fp, #-56]
	cmp	r2, #0
	ldr	r2, [fp, #-60]
	add	r5, r2, #2
	blt	.L487
.L506:
	movw	r9, #26125
	movw	r8, #62303
	movt	r9, 25
	add	r5, r5, #4
	movt	r8, 15470
	mov	r7, #1
	ldr	lr, [fp, #-56]
	str	r9, [fp, #-48]
	b	.L505
.L521:
	ldr	r3, [r4, #20]
.L505:
	cmp	r3, #32
	ldr	ip, [r4, #36]
	ldr	r1, [r4, #24]
	add	r5, r5, #4
	movcc	r0, r3
	ldr	r3, [fp, #-48]
	ldr	r6, [r4, #28]
	movcs	r0, #32
	rsb	r10, r0, #32
	subs	lr, lr, #1
	mla	r9, r3, ip, r8
	ldr	r3, [r4, #32]
	sub	r2, r1, r6
	lsr	ip, ip, r10
	add	r2, r2, r3
	ldrsh	r3, [r5, #-8]
	add	r1, r1, r1, lsr #31
	str	r6, [r4, #32]
	add	r2, r2, r3
	sub	r3, r0, #1
	rsb	ip, ip, r9, lsr r10
	add	r3, r2, r7, lsl r3
	lsl	r10, r7, r0
	add	r3, ip, r3
	rsb	r10, r10, #0
	asr	r1, r1, #1
	and	r3, r3, r10
	str	r1, [r4, #28]
	sub	r2, r2, r3
	str	r9, [r4, #36]
	lsr	r3, r3, r0
	str	r2, [r4, #24]
	strh	r3, [r5, #-8]
	bpl	.L521
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L508:
	mov	r0, #0
	mov	r3, r0
	b	.L488
	UNWIND(.fnend)
	.size	util_dre_16_bit_process, .-util_dre_16_bit_process
	.section	.rodata
	.align	2
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
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
