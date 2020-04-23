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
	.file	"resample.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cvt_short_short, %function
cvt_short_short:
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
	ldr	ip, [fp, #4]
	mov	lr, r0
	cmp	r3, #0
	cmpeq	ip, #2
	bne	.L2
	lsl	r3, r2, #1
	add	r2, r1, r3
	cmp	r1, r2
	beq	.L8
	sub	r3, r3, #2
	add	lr, r2, #2
	add	r0, r0, #4
	sub	r1, r1, #2
	lsr	r3, r3, #1
	add	lr, lr, r3, lsl #1
.L4:
	ldrsh	ip, [r1, #2]!
	add	r0, r0, #4
	strh	ip, [r0, #-8]
	ldrsh	ip, [r2], #2
	cmp	r2, lr
	strh	ip, [r0, #-6]
	bne	.L4
	add	r0, r3, #1
	lsl	r0, r0, #2
	asr	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
.L2:
	cmp	ip, #2
	lsleq	r2, r2, #1
	lsl	r2, r2, #1
	add	r3, r1, r2
	cmp	r1, r3
	beq	.L8
	sub	r1, r1, #2
	add	r2, r0, r2
.L6:
	ldrsh	r3, [r1, #2]!
	strh	r3, [r0], #2
	cmp	r0, r2
	bne	.L6
	sub	r0, r0, lr
	asr	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
.L8:
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	cvt_short_short, .-cvt_short_short
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cvt2int, %function
cvt2int:
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
	ldr	lr, [fp, #4]
	ldr	ip, [fp, #8]
	cmp	lr, #0
	cmpeq	ip, #2
	mov	lr, r0
	bne	.L13
	cmp	r3, #16
	beq	.L31
	lsl	r2, r2, #2
	add	ip, r1, r2
	cmp	r1, ip
	beq	.L25
	sub	r2, r2, #4
	add	lr, ip, #4
	add	r0, r0, #8
	sub	r1, r1, #4
	lsr	r3, r2, #2
	add	lr, lr, r3, lsl #2
.L17:
	ldr	r2, [r1, #4]!
	add	r0, r0, #8
	str	r2, [r0, #-16]
	ldr	r2, [ip], #4
	cmp	ip, lr
	str	r2, [r0, #-12]
	bne	.L17
.L30:
	add	r0, r3, #1
	lsl	r0, r0, #3
	asr	r0, r0, #2
	ldmfd	sp, {fp, sp, pc}
.L13:
	cmp	ip, #2
	lsleq	r2, r2, #1
	cmp	r3, #16
	beq	.L32
	lsl	r2, r2, #2
	add	r3, r1, r2
	cmp	r1, r3
	beq	.L25
	sub	r1, r1, #4
	add	r2, r0, r2
.L21:
	ldr	r3, [r1, #4]!
	str	r3, [r0], #4
	cmp	r0, r2
	bne	.L21
	sub	r0, r0, lr
	asr	r0, r0, #2
	ldmfd	sp, {fp, sp, pc}
.L32:
	lsl	r2, r2, #1
	add	r3, r1, r2
	cmp	r1, r3
	beq	.L25
	sub	r2, r2, #2
	sub	ip, r1, #2
	lsr	r2, r2, #1
	add	r1, r1, r2, lsl #1
.L20:
	ldrsh	r3, [ip, #2]!
	cmp	ip, r1
	lsl	r3, r3, #16
	str	r3, [r0], #4
	bne	.L20
	add	r0, r2, #1
	sbfx	r0, r0, #0, #30
	ldmfd	sp, {fp, sp, pc}
.L31:
	lsl	r2, r2, #1
	add	ip, r1, r2
	cmp	r1, ip
	beq	.L25
	sub	r2, r2, #2
	add	lr, ip, #2
	add	r0, r0, #8
	sub	r1, r1, #2
	lsr	r3, r2, #1
	add	lr, lr, r3, lsl #1
.L16:
	ldrsh	r2, [r1, #2]!
	add	r0, r0, #8
	lsl	r2, r2, #16
	str	r2, [r0, #-16]
	ldrsh	r2, [ip], #2
	cmp	ip, lr
	lsl	r2, r2, #16
	str	r2, [r0, #-12]
	bne	.L16
	b	.L30
.L25:
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	cvt2int, .-cvt2int
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resampler_mono_process24, %function
resampler_mono_process24:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #52)
	sub	sp, sp, #52
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r5, r0
	mov	ip, #1
	str	r3, [fp, #-76]
	mov	r6, r3
	ldr	r3, [r0, #40]
	ldr	r0, [r0, #56]
	str	ip, [sp, #4]
	str	ip, [sp]
	ldr	ip, [r5, #28]
	ldr	r4, [r5, #20]
	str	ip, [fp, #-52]
	ldr	ip, [r5, #72]
	str	ip, [fp, #-64]
	bl	cvt2int
	lsl	r2, r4, #2
	ldr	r3, [r5, #56]
	ldr	r1, [r5, #32]
	rsb	r2, r2, #4
	add	r3, r3, r2
	ldr	ip, [r5, #64]
	ldr	r7, [r5, #60]
	add	r10, r3, r1, lsl #2
	lsl	r2, r0, #2
	str	r2, [fp, #-68]
	add	r2, r3, r2
	str	r2, [fp, #-60]
	cmp	r2, r10
	movls	r4, #0
	bls	.L34
	add	r9, r6, #4
	movw	r3, #65535
	mov	r8, r5
	mov	r2, r3
	movt	r3, 127
	movt	r2, 7
	str	r3, [fp, #-72]
	movw	r3, #65535
	str	r2, [fp, #-84]
	movt	r3, 1
	str	r3, [fp, #-80]
.L49:
	ldr	r3, [r8, #40]
	cmp	r4, #0
	str	r3, [fp, #-48]
	ble	.L35
	mov	lr, #0
	sub	r0, r10, #12
	add	r1, r10, #8
	mov	r3, lr
	mov	r2, lr
	str	r10, [fp, #-56]
.L36:
	ldr	r6, [ip], #4
	ldr	r5, [r7], #4
	ldr	r10, [r0, #8]
	.syntax divided
	smlawb r10, r10, r6, r2
	.arm
	.syntax unified
	ldr	r2, [r1, #-8]
	.syntax divided
	smlawb r3, r2, r5, r3
	.arm
	.syntax unified
	ldr	r2, [r0, #4]
	.syntax divided
	smlawt r2, r2, r6, r10
	.arm
	.syntax unified
	ldr	r6, [r1, #-4]
	.syntax divided
	smlawt r3, r6, r5, r3
	.arm
	.syntax unified
	add	lr, lr, #2
	sub	r0, r0, #8
	cmp	r4, lr
	add	r1, r1, #8
	bgt	.L36
	ldr	r1, [fp, #-48]
	add	r3, r3, r2
	ldr	r10, [fp, #-56]
	cmp	r1, #16
	beq	.L56
	ldr	r2, [fp, #-48]
	cmp	r2, #18
	beq	.L57
.L41:
	ldr	r2, [fp, #-48]
	cmp	r2, #20
	beq	.L58
	add	r3, r3, #64
	asr	r2, r3, #31
	cmp	r2, r3, asr #30
	ubfxeq	r3, r3, #7, #24
	ldrne	r3, [fp, #-72]
	eorne	r3, r2, r3
	mov	r2, r9
	str	r3, [r9, #-4]
.L40:
	ldr	r1, [fp, #-52]
	add	r9, r9, #4
	ldr	r3, [fp, #-64]
	ldr	ip, [r8, #68]
	ldr	r0, [r3, r1, lsl #2]
	ubfx	r7, r0, #8, #12
	ubfx	r1, r0, #1, #7
	lsr	r3, r0, #20
	asr	r0, r0, #20
	mul	r7, r4, r7
	add	r10, r10, r1, lsl #2
	mul	r3, r4, r3
	ldr	r1, [fp, #-60]
	str	r0, [fp, #-52]
	cmp	r1, r10
	add	r7, ip, r7, lsl #1
	add	ip, ip, r3, lsl #1
	bhi	.L49
	ldr	r3, [fp, #-76]
	mov	r5, r8
	sub	r2, r2, r3
	asr	r4, r2, #2
.L34:
	ldr	r3, [fp, #-60]
	ldr	r1, [fp, #-52]
	sub	r3, r10, r3
	ldr	r2, [r5, #24]
	ldr	r0, [r5, #52]
	asr	r3, r3, #2
	str	r3, [r5, #32]
	ldr	r3, [fp, #-68]
	lsl	r2, r2, #2
	str	r1, [r5, #28]
	str	ip, [r5, #64]
	add	r1, r0, r3
	str	r7, [r5, #60]
	bl	adsp_memmove
	mov	r0, r4
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L56:
	add	r3, r3, #16384
	asr	r2, r3, #31
	cmp	r2, r3, asr #30
	eorne	r2, r2, #32512
	sbfxeq	r2, r3, #15, #16
	eorne	r2, r2, #255
.L39:
	str	r2, [r9, #-4]
	mov	r2, r9
	b	.L40
.L57:
	add	r3, r3, #4096
	asr	r2, r3, #31
	cmp	r2, r3, asr #30
	beq	.L42
	ldr	r3, [fp, #-80]
	eor	r3, r2, r3
.L46:
	str	r3, [r9, #-4]
	mov	r2, r9
	b	.L40
.L35:
	ldr	r3, [fp, #-48]
	cmp	r3, #16
	beq	.L51
	cmp	r3, #18
	movne	r3, #0
	bne	.L41
	mov	r3, #4096
.L42:
	ubfx	r3, r3, #13, #18
	b	.L46
.L58:
	add	r3, r3, #1024
	asr	r2, r3, #31
	cmp	r2, r3, asr #30
	ubfxeq	r3, r3, #11, #20
	ldrne	r3, [fp, #-84]
	eorne	r3, r2, r3
	b	.L46
.L51:
	mov	r2, #0
	b	.L39
	UNWIND(.fnend)
	.size	resampler_mono_process24, .-resampler_mono_process24
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resampler_stereo_process24, %function
resampler_stereo_process24:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #52)
	sub	sp, sp, #52
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r4, [fp, #4]
	mov	r10, r0
	ldr	lr, [r0, #40]
	mov	ip, #2
	ldr	r0, [r0, #56]
	mov	r8, r3
	str	r4, [sp]
	str	ip, [sp, #4]
	ldr	ip, [r10, #28]
	str	r3, [fp, #-76]
	mov	r3, lr
	ldr	r4, [r10, #20]
	str	ip, [fp, #-48]
	ldr	ip, [r10, #72]
	str	ip, [fp, #-56]
	bl	cvt2int
	sub	r2, r4, #1
	ldr	r3, [r10, #56]
	ldr	r7, [r10, #32]
	sub	r3, r3, r2, lsl #3
	ldr	r6, [r10, #64]
	ldr	r5, [r10, #60]
	add	r7, r3, r7, lsl #2
	ldr	r1, [r10, #68]
	lsl	r2, r0, #2
	str	r2, [fp, #-72]
	add	r2, r3, r2
	str	r2, [fp, #-60]
	cmp	r2, r7
	movls	r4, #0
	bls	.L60
	add	r8, r8, #8
	str	r10, [fp, #-80]
	mov	r10, r1
	movw	r3, #65535
	mov	r2, #65280
	movt	r3, 127
	movt	r2, 32767
	str	r3, [fp, #-64]
	str	r2, [fp, #-68]
.L67:
	cmp	r4, #0
	ble	.L61
	mov	lr, #0
	sub	r1, r7, #20
	add	r2, r7, #16
	mov	r9, lr
	mov	r3, lr
	str	r7, [fp, #-52]
.L62:
	ldr	ip, [r6], #4
	ldr	r0, [r5], #4
	ldr	r7, [r1, #16]
	.syntax divided
	smlawb r3, r7, ip, r3
	.arm
	.syntax unified
	ldr	r7, [r2, #-16]
	.syntax divided
	smlawb r9, r7, r0, r9
	.arm
	.syntax unified
	ldr	r7, [r1, #12]
	.syntax divided
	smlawb r9, r7, ip, r9
	.arm
	.syntax unified
	ldr	r7, [r2, #-12]
	.syntax divided
	smlawb r3, r7, r0, r3
	.arm
	.syntax unified
	ldr	r7, [r1, #8]
	.syntax divided
	smlawt r3, r7, ip, r3
	.arm
	.syntax unified
	ldr	r7, [r2, #-8]
	.syntax divided
	smlawt r9, r7, r0, r9
	.arm
	.syntax unified
	ldr	r7, [r1, #4]
	.syntax divided
	smlawt r9, r7, ip, r9
	.arm
	.syntax unified
	ldr	ip, [r2, #-4]
	.syntax divided
	smlawt r3, ip, r0, r3
	.arm
	.syntax unified
	add	lr, lr, #2
	sub	r1, r1, #16
	cmp	r4, lr
	add	r2, r2, #16
	bgt	.L62
	add	r9, r9, #64
	add	r3, r3, #64
	ldr	r7, [fp, #-52]
	asr	r1, r9, #31
	asr	r2, r3, #31
	cmp	r1, r9, asr #30
	asr	r0, r3, #7
	asreq	r9, r9, #7
	asr	r3, r3, #30
	ldrne	ip, [fp, #-68]
	lsleq	r1, r9, #8
	eorne	r1, ip, r1, lsl #8
	cmp	r2, r3
	str	r1, [r8, #-8]
	mov	r3, r8
	beq	.L65
	ldr	r1, [fp, #-64]
	eor	r2, r2, r1
.L66:
	ldr	r1, [fp, #-48]
	lsl	r2, r2, #8
	str	r2, [r8, #-4]
	add	r8, r8, #8
	ldr	r2, [fp, #-56]
	ldr	r2, [r2, r1, lsl #2]
	ubfx	r5, r2, #8, #12
	lsr	r6, r2, #20
	uxtb	r1, r2
	mul	r5, r4, r5
	add	r7, r7, r1, lsl #2
	mul	r6, r4, r6
	ldr	r1, [fp, #-60]
	asr	r2, r2, #20
	str	r2, [fp, #-48]
	cmp	r1, r7
	add	r5, r10, r5, lsl #1
	add	r6, r10, r6, lsl #1
	bhi	.L67
	ldr	r2, [fp, #-76]
	ldr	r10, [fp, #-80]
	sub	r3, r3, r2
	asr	r3, r3, #2
	add	r3, r3, r3, lsr #31
	asr	r4, r3, #1
.L60:
	ldr	r3, [fp, #-48]
	ldr	r2, [r10, #24]
	ldr	r0, [r10, #52]
	str	r3, [r10, #28]
	ldr	r3, [fp, #-72]
	lsl	r2, r2, #2
	str	r6, [r10, #64]
	add	r1, r0, r3
	str	r5, [r10, #60]
	mov	r3, #0
	str	r3, [r10, #32]
	bl	adsp_memmove
	mov	r0, r4
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L61:
	mov	r3, #0
	mov	r0, #0
	str	r3, [r8, #-8]
	mov	r3, r8
.L65:
	bic	r2, r0, #-16777216
	b	.L66
	UNWIND(.fnend)
	.size	resampler_stereo_process24, .-resampler_stereo_process24
	.global	__aeabi_idiv
	.align	2
	.global	resample_config
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resample_config, %function
resample_config:
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
	cmp	r1, r2
	mov	r4, r1
	mov	r6, r2
	mov	r5, r0
	beq	.L73
	mov	r8, r2
	mov	r3, r1
.L76:
	cmp	r3, r8
	subgt	r3, r3, r8
	suble	r8, r8, r3
	cmp	r8, r3
	bne	.L76
	cmp	r8, #0
	beq	.L90
	mov	r1, r8
	mov	r0, r6
	bl	__aeabi_idiv
	mov	r1, r8
	mov	r7, r0
	mov	r0, r4
	bl	__aeabi_idiv
	cmp	r7, #1280
	mov	r8, r0
	bgt	.L90
.L85:
	add	r0, r7, r8
	mov	r1, r7
	sub	r0, r0, #1
	ldr	r9, [r5, #16]
	bl	__aeabi_idiv
	mul	r0, r9, r0
	cmp	r0, #255
	bgt	.L90
	cmp	r4, #0
	add	ip, r4, #3
	add	lr, r6, #3
	add	r2, r4, r4, lsr #31
	movge	ip, r4
	cmp	r6, #0
	add	r10, r6, r6, lsr #31
	ldr	r3, .L99
	movge	lr, r6
	asr	ip, ip, #2
	asr	lr, lr, #2
	asr	r2, r2, #1
	asr	r10, r10, #1
	mov	r0, #0
	str	r5, [fp, #-48]
	b	.L82
.L78:
	cmp	r1, r2
	beq	.L97
	cmp	r1, ip
	beq	.L98
.L81:
	add	r0, r0, #1
	add	r3, r3, #24
	cmp	r0, #40
	beq	.L90
.L82:
	ldr	r1, [r3]
	cmp	r1, r4
	bne	.L78
	ldr	r5, [r3, #4]
	cmp	r5, r6
	bne	.L78
.L79:
	cmp	r0, #40
	ldr	r5, [fp, #-48]
	beq	.L90
	ldr	r2, [r3, #20]
	mov	ip, #0
	ldr	lr, [r3, #12]
	ldr	r1, [r5, #40]
	str	r2, [r5, #68]
	ldr	r0, [r3, #16]
	cmp	r1, #16
	lsl	r3, lr, #1
	str	ip, [r5, #36]
	str	r0, [r5, #72]
	mul	r3, r9, r3
	beq	.L96
	sub	r0, r1, #18
	bic	r0, r0, #2
	cmp	r0, #0
	cmpne	r1, #24
	bne	.L84
.L96:
	ldr	r1, [r5, #52]
	add	r1, r1, r3, lsl #2
	str	r1, [r5, #56]
.L84:
	sub	ip, r7, #1
	mov	r1, #0
	mov	r0, r1
	str	r2, [r5, #64]
	mul	ip, lr, ip
	str	r4, [r5, #8]
	str	r6, [r5, #12]
	stm	r5, {r7, r8}
	str	lr, [r5, #20]
	str	r3, [r5, #24]
	add	r2, r2, ip, lsl #1
	str	r1, [r5, #28]
	str	r2, [r5, #60]
	str	r1, [r5, #32]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L97:
	ldr	r5, [r3, #4]
	cmp	r5, r10
	beq	.L79
	cmp	r1, ip
	bne	.L81
.L98:
	ldr	r1, [r3, #4]
	cmp	r1, lr
	beq	.L79
	add	r0, r0, #1
	add	r3, r3, #24
	cmp	r0, #40
	bne	.L82
.L90:
	mvn	r0, #0
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L73:
	cmp	r1, #0
	beq	.L90
	mov	r8, #1
	mov	r7, r8
	b	.L85
.L100:
	.align	2
.L99:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	resample_config, .-resample_config
	.align	2
	.global	resampler_create
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resampler_create, %function
resampler_create:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #36)
	sub	sp, sp, #36
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r0, #32000
	mov	r5, r0
	mov	r6, r1
	mov	r8, r2
	beq	.L103
	ble	.L178
	movw	r2, #22664
	movt	r2, 1
	cmp	r0, r2
	beq	.L103
	ble	.L179
	movw	r2, #45328
	movt	r2, 2
	cmp	r0, r2
	beq	.L103
	mov	r2, #60928
	movt	r2, 2
	cmp	r0, r2
	beq	.L103
	mov	r2, #30464
	movt	r2, 1
	cmp	r0, r2
	beq	.L103
.L102:
	sub	r0, fp, #44
	str	r8, [sp]
	mov	ip, #0
	mov	r2, r6
	mov	r1, r5
	str	ip, [r0, #-4]!
	bl	lsrc_create
	ldr	r7, [fp, #-48]
	cmp	r7, #0
	beq	.L101
	mov	r1, #88
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	subs	r7, r0, #0
	beq	.L180
	ldr	r3, [fp, #-48]
	mov	r2, #1
	str	r2, [r7, #80]
	str	r3, [r7, #84]
.L101:
	mov	r0, r7
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L186:
	cmp	r0, #8000
	beq	.L103
	movw	r2, #11025
	cmp	r0, r2
	bne	.L102
.L103:
	cmp	r6, #32000
	beq	.L107
	ble	.L181
	movw	r2, #22664
	movt	r2, 1
	cmp	r6, r2
	beq	.L107
	ble	.L182
	movw	r2, #45328
	movt	r2, 2
	cmp	r6, r2
	beq	.L107
	mov	r2, #60928
	movt	r2, 2
	cmp	r6, r2
	beq	.L107
	mov	r2, #30464
	movt	r2, 1
	cmp	r6, r2
	bne	.L102
.L107:
	cmp	r5, r6
	bge	.L102
	sub	r2, r8, #1
	cmp	r2, #1
	bhi	.L102
	bic	r2, r3, #2
	cmp	r2, #16
	beq	.L132
	sub	r2, r3, #20
	bics	r2, r2, #4
	bne	.L102
.L132:
	mov	r2, r6
	mov	r4, r5
.L116:
	cmp	r2, r4
	sublt	r4, r4, r2
	subge	r2, r2, r4
	cmp	r4, r2
	bne	.L116
	cmp	r4, #0
	beq	.L102
	mov	r1, r4
	mov	r0, r6
	str	r3, [fp, #-56]
	bl	__aeabi_idiv
	mov	r1, r4
	mov	r9, r0
	mov	r0, r5
	bl	__aeabi_idiv
	cmp	r9, #1280
	ldr	r3, [fp, #-56]
	str	r0, [fp, #-60]
	bgt	.L102
	add	r0, r9, r0
	mov	r1, r9
	sub	r0, r0, #1
	bl	__aeabi_idiv
	ldr	r3, [fp, #-56]
	mul	r0, r8, r0
	cmp	r0, #255
	bgt	.L102
	cmp	r5, #0
	add	r0, r5, #3
	add	r7, r6, #3
	add	ip, r5, r5, lsr #31
	movge	r0, r5
	cmp	r6, #0
	add	r10, r6, r6, lsr #31
	ldr	r4, .L188
	movge	r7, r6
	asr	r0, r0, #2
	asr	r7, r7, #2
	asr	ip, ip, #1
	asr	r10, r10, #1
	mov	r1, #0
	b	.L121
.L117:
	cmp	r2, ip
	beq	.L183
.L119:
	cmp	r2, r0
	beq	.L184
.L120:
	add	r1, r1, #1
	add	r4, r4, #24
	cmp	r1, #40
	beq	.L102
.L121:
	ldr	r2, [r4]
	cmp	r5, r2
	bne	.L117
	ldr	lr, [r4, #4]
	cmp	r6, lr
	bne	.L117
	ldr	lr, [r4, #8]
	cmp	lr, #2
	bne	.L117
.L118:
	cmp	r1, #40
	beq	.L102
	ldr	r10, [r4, #12]
	mov	r1, #88
	str	r3, [fp, #-64]
	mov	r0, #70
	lsl	r2, r10, #1
	mul	r3, r8, r2
	str	r3, [fp, #-56]
	bl	HI_ADSP_MALLOC
	ldr	r3, [fp, #-64]
	subs	r7, r0, #0
	beq	.L102
	ldr	r1, [r4, #20]
	cmp	r3, #16
	mov	r2, #0
	str	r1, [r7, #68]
	ldr	r0, [r4, #16]
	str	r3, [r7, #40]
	str	r2, [r7, #36]
	str	r0, [r7, #72]
	beq	.L185
	sub	r0, r3, #18
	str	r3, [fp, #-64]
	bic	r0, r0, #2
	cmp	r0, #0
	cmpne	r3, #24
	bne	.L125
	mov	r1, #16640
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	ldr	r3, [fp, #-64]
	mov	r2, #0
	cmp	r0, #0
	str	r0, [r7, #52]
	beq	.L177
	ldr	r3, [fp, #-56]
	mov	r1, r2
	lsl	r4, r3, #2
	mov	r2, r4
	bl	adsp_memset
	ldr	r1, [r7, #68]
	ldr	r3, [r7, #52]
	add	r4, r3, r4
	str	r4, [r7, #56]
.L125:
	sub	r3, r9, #1
	ldr	r0, [fp, #-60]
	mov	r2, #0
	str	r5, [r7, #8]
	mul	r3, r10, r3
	str	r0, [r7, #4]
	ldr	r0, [fp, #-56]
	str	r6, [r7, #12]
	str	r9, [r7]
	str	r0, [r7, #24]
	mov	r0, r7
	add	r3, r1, r3, lsl #1
	str	r3, [r7, #60]
	ldr	r3, [fp, #4]
	str	r8, [r7, #16]
	str	r10, [r7, #20]
	str	r1, [r7, #64]
	str	r3, [r7, #76]
	str	r2, [r7, #28]
	str	r2, [r7, #32]
	str	r2, [r7, #80]
	str	r2, [r7, #84]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L178:
	movw	r2, #12000
	cmp	r0, r2
	beq	.L103
	ble	.L186
	movw	r2, #22050
	cmp	r0, r2
	beq	.L103
	movw	r2, #24000
	cmp	r0, r2
	beq	.L103
	cmp	r0, #16000
	bne	.L102
	b	.L103
.L181:
	movw	r2, #12000
	cmp	r6, r2
	beq	.L107
	ble	.L187
	movw	r2, #22050
	cmp	r6, r2
	beq	.L107
	movw	r2, #24000
	cmp	r6, r2
	beq	.L107
	cmp	r6, #16000
	bne	.L102
	b	.L107
.L183:
	ldr	lr, [r4, #4]
	cmp	lr, r10
	bne	.L119
	ldr	lr, [r4, #8]
	cmp	lr, #2
	beq	.L118
	cmp	r2, r0
	bne	.L120
.L184:
	ldr	r2, [r4, #4]
	cmp	r2, r7
	bne	.L120
	ldr	r2, [r4, #8]
	cmp	r2, #2
	bne	.L120
	b	.L118
.L179:
	movw	r2, #44100
	cmp	r0, r2
	beq	.L103
	movw	r2, #48000
	cmp	r0, r2
	beq	.L103
	b	.L102
.L187:
	cmp	r6, #8000
	beq	.L107
	movw	r2, #11025
	cmp	r6, r2
	beq	.L107
	b	.L102
.L182:
	movw	r2, #44100
	cmp	r6, r2
	beq	.L107
	movw	r2, #48000
	cmp	r6, r2
	beq	.L107
	b	.L102
.L185:
	ldr	r2, [fp, #4]
	cmp	r2, #0
	beq	.L123
	mov	r1, #16640
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	ldr	r3, [fp, #-64]
	mov	r2, #0
	cmp	r0, #0
	str	r0, [r7, #52]
	beq	.L177
	ldr	r3, [fp, #-56]
	mov	r1, r2
	lsl	r4, r3, #2
	mov	r2, r4
	bl	adsp_memset
	ldr	r3, [r7, #52]
	ldr	r1, [r7, #68]
	add	r4, r3, r4
	str	r4, [r7, #56]
	b	.L125
.L123:
	str	r3, [fp, #-64]
	mov	r0, #70
	ldr	r3, [fp, #-56]
	add	r1, r3, #4096
	lsl	r1, r1, #1
	bl	HI_ADSP_MALLOC
	ldr	r3, [fp, #-64]
	cmp	r0, #0
	str	r0, [r7, #44]
	beq	.L177
	ldr	r3, [fp, #-56]
	ldr	r1, [fp, #4]
	lsl	r4, r3, #1
	mov	r2, r4
	bl	adsp_memset
	ldr	r3, [r7, #44]
	ldr	r1, [r7, #68]
	add	r4, r3, r4
	str	r4, [r7, #48]
	b	.L125
.L180:
	ldr	r0, [fp, #-48]
	bl	lsrc_destroy
	b	.L101
.L177:
	mov	r1, r7
	mov	r0, #70
	str	r3, [fp, #-56]
	bl	HI_ADSP_FREE
	ldr	r3, [fp, #-56]
	b	.L102
.L189:
	.align	2
.L188:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	resampler_create, .-resampler_create
	.align	2
	.global	resampler_free
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resampler_free, %function
resampler_free:
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
	subs	r4, r0, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r4, #80]
	cmp	r3, #0
	beq	.L215
	ldr	r0, [r4, #84]
	bl	lsrc_destroy
.L212:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L215:
	ldr	r3, [r4, #40]
	cmp	r3, #16
	beq	.L216
	sub	r2, r3, #18
	bic	r2, r2, #2
	cmp	r2, #0
	cmpne	r3, #24
	bne	.L212
.L214:
	ldr	r1, [r4, #52]
	cmp	r1, #0
	beq	.L212
.L211:
	mov	r0, #70
	bl	HI_ADSP_FREE
	b	.L212
.L216:
	ldr	r3, [r4, #76]
	cmp	r3, #0
	bne	.L214
	ldr	r1, [r4, #44]
	cmp	r1, #0
	bne	.L211
	b	.L212
	UNWIND(.fnend)
	.size	resampler_free, .-resampler_free
	.align	2
	.global	resample_flush
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resample_flush, %function
resample_flush:
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
	subs	r4, r0, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r1, [r4, #80]
	cmp	r1, #0
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	ldr	r0, [r4]
	ldr	r2, [r4, #20]
	sub	r0, r0, #1
	ldr	ip, [r4, #40]
	ldr	r3, [r4, #68]
	mul	r2, r2, r0
	cmp	ip, #16
	str	r3, [r4, #64]
	str	r1, [r4, #28]
	str	r1, [r4, #32]
	str	r1, [r4, #36]
	add	r3, r3, r2, lsl #1
	str	r3, [r4, #60]
	beq	.L227
	sub	r3, ip, #18
	bic	r3, r3, #2
	cmp	r3, #0
	cmpne	ip, #24
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	ldr	r2, [r4, #24]
	ldr	r0, [r4, #52]
	add	r2, r2, #4096
	lsl	r2, r2, #2
.L226:
	bl	adsp_memset
	ldr	r2, [r4, #24]
	ldr	r3, [r4, #52]
	add	r3, r3, r2, lsl #2
	str	r3, [r4, #56]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L227:
	ldr	r3, [r4, #76]
	ldr	r2, [r4, #24]
	cmp	r3, #0
	add	r2, r2, #4096
	lslne	r2, r2, #2
	ldrne	r0, [r4, #52]
	bne	.L226
.L222:
	lsl	r2, r2, #1
	mov	r1, r3
	ldr	r0, [r4, #44]
	bl	adsp_memset
	ldr	r2, [r4, #24]
	ldr	r3, [r4, #44]
	add	r3, r3, r2, lsl #1
	str	r3, [r4, #48]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	resample_flush, .-resample_flush
	.align	2
	.global	new_resampler_check
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	new_resampler_check, %function
new_resampler_check:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #12)
	sub	sp, sp, #12
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r0, #0
	mov	lr, r3
	beq	.L231
	ldr	r3, [r0, #80]
	cmp	r3, #0
	bne	.L230
	ldr	r3, [r0, #8]
	cmp	r1, r3
	beq	.L235
.L234:
	mov	r0, #1
.L228:
	sub	sp, fp, #16
	ldmfd	sp, {r4, fp, sp, pc}
.L235:
	ldr	r3, [r0, #12]
	cmp	r2, r3
	bne	.L234
	ldr	r3, [r0, #16]
	cmp	lr, r3
	bne	.L234
	ldr	r0, [r0, #40]
	ldr	r3, [fp, #4]
	subs	r0, r0, r3
	movne	r0, #1
	b	.L228
.L230:
	ldr	r0, [r0, #84]
	ldr	r3, [fp, #4]
	str	lr, [sp]
	bl	lsrc_check_update
	sub	sp, fp, #16
	ldmfd	sp, {r4, fp, sp, pc}
.L231:
	mov	r0, #-2147483647
	b	.L228
	UNWIND(.fnend)
	.size	new_resampler_check, .-new_resampler_check
	.align	2
	.global	resampler_get_max_output_num
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resampler_get_max_output_num, %function
resampler_get_max_output_num:
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
	mov	r2, r1
	beq	.L239
	ldr	r3, [r0, #80]
	cmp	r3, #0
	bne	.L238
	cmp	r1, #2048
	bgt	.L241
	tst	r1, #1
	bne	.L241
	ldr	r3, [r0, #16]
	ldr	ip, [r0, #12]
	sub	r3, r3, #1
	ldr	r1, [r0, #8]
	add	r3, r3, r2
	mul	r0, ip, r3
	bl	__aeabi_idiv
	add	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
.L238:
	ldr	r0, [r0, #84]
	bl	lsrc_get_max_output_num
	ldmfd	sp, {fp, sp, pc}
.L241:
	mov	r0, #-2147483645
	ldmfd	sp, {fp, sp, pc}
.L239:
	mov	r0, #-2147483647
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	resampler_get_max_output_num, .-resampler_get_max_output_num
	.global	__aeabi_idivmod
	.align	2
	.global	resampler_get_min_input_num
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resampler_get_min_input_num, %function
resampler_get_min_input_num:
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
	subs	r4, r0, #0
	mov	r0, r1
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r4, #80]
	cmp	r3, #0
	bne	.L244
	ldr	r3, [r4, #8]
	ldr	r1, [r4, #12]
	mul	r0, r3, r0
	bl	__aeabi_idivmod
	ldr	r3, [r4, #16]
	cmp	r1, #0
	addne	r0, r0, #1
	mul	r3, r3, r0
	tst	r3, #1
	addne	r0, r0, #1
	cmp	r0, #2048
	movgt	r0, #-2147483645
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L244:
	ldr	r0, [r4, #84]
	bl	lsrc_get_min_input_num
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	resampler_get_min_input_num, .-resampler_get_min_input_num
	.align	2
	.global	resampler_process_frame
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	resampler_process_frame, %function
resampler_process_frame:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #44)
	sub	sp, sp, #44
	push	{lr}
	bl	__gnu_mcount_nc
	subs	r4, r0, #0
	mov	r7, r1
	str	r2, [fp, #-48]
	mov	r5, r3
	beq	.L264
	ldr	r6, [r4, #80]
	cmp	r6, #0
	beq	.L297
	ldr	r0, [r4, #84]
	bl	lsrc_process_frame
	mov	r6, r0
.L255:
	mov	r0, r6
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L297:
	cmp	r1, #0
	beq	.L283
	cmp	r3, #2048
	bgt	.L260
	tst	r3, #1
	bne	.L260
	ldr	r3, [fp, #-48]
	cmp	r3, #0
	beq	.L283
	ldr	r3, [r4, #8]
	ldr	r2, [fp, #4]
	cmp	r2, r3
	bne	.L261
	ldr	r3, [r4, #12]
	ldr	r2, [fp, #8]
	cmp	r2, r3
	beq	.L262
.L261:
	ldmib	fp, {r1, r2}
	mov	r0, r4
	bl	resample_config
.L262:
	mov	r1, r5
	mov	r0, r4
	bl	resampler_get_max_output_num
	cmp	r0, #0
	blt	.L298
	ldr	ip, [r4, #16]
	cmp	ip, #2
	beq	.L299
	cmp	ip, #1
	bne	.L284
	ldr	r3, [r4, #40]
	cmp	r3, #16
	beq	.L300
	sub	r2, r3, #18
	bic	r2, r2, #2
	cmp	r2, #0
	cmpne	r3, #24
	bne	.L296
.L276:
	ldr	r3, [fp, #-48]
	mov	r2, r5
	mov	r1, r7
	mov	r0, r4
	bl	resampler_mono_process24
	mov	r6, r0
	mov	r0, r6
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L299:
	ldr	r3, [r4, #40]
	cmp	r3, #16
	beq	.L301
	sub	r2, r3, #18
	bic	r2, r2, #2
	cmp	r2, #0
	cmpne	r3, #24
	bne	.L296
.L267:
	ldr	r3, [fp, #12]
	mov	r2, r5
	mov	r1, r7
	mov	r0, r4
	str	r3, [sp]
	ldr	r3, [fp, #-48]
	bl	resampler_stereo_process24
	mov	r6, r0
	b	.L255
.L298:
	cmp	r0, #-2147483647
	beq	.L264
	cmp	r0, #-2147483645
	movne	r6, #-2147483640
	bne	.L255
.L260:
	mov	r6, #-2147483645
	b	.L255
.L300:
	ldr	r3, [r4, #76]
	cmp	r3, #0
	bne	.L276
	ldr	r3, [r4, #28]
	mov	r2, r5
	ldr	lr, [r4, #72]
	mov	r1, r7
	ldr	r5, [r4, #20]
	ldr	r0, [r4, #48]
	str	r3, [fp, #-52]
	str	ip, [sp]
	ldr	r3, [fp, #12]
	str	lr, [fp, #-60]
	bl	cvt_short_short
	ldr	r2, [r4, #48]
	lsl	r3, r5, #1
	ldr	r8, [r4, #32]
	rsb	r3, r3, #2
	add	r3, r2, r3
	ldr	r1, [fp, #-48]
	ldr	ip, [r4, #64]
	add	r8, r3, r8, lsl #1
	ldr	r7, [r4, #60]
	str	r1, [fp, #-68]
	lsl	r2, r0, #1
	str	r2, [fp, #-64]
	add	r3, r3, r2
	str	r3, [fp, #-48]
	cmp	r3, r8
	bls	.L277
	str	r4, [fp, #-56]
	mov	r4, r1
.L281:
	cmp	r5, #0
	add	r4, r4, #2
	movle	r3, #0
	ble	.L278
	mov	lr, #0
	sub	r0, r8, #6
	add	r1, r8, #4
	mov	r2, lr
	mov	r3, lr
.L279:
	ldr	r10, [ip], #4
	ldr	r9, [r7], #4
	ldrsh	r6, [r0, #4]
	.syntax divided
	smlabb r3, r6, r10, r3
	.arm
	.syntax unified
	ldrsh	r6, [r1, #-4]
	.syntax divided
	smlabb r6, r6, r9, r2
	.arm
	.syntax unified
	ldrsh	r2, [r0, #2]
	.syntax divided
	smlabt r3, r2, r10, r3
	.arm
	.syntax unified
	ldrsh	r2, [r1, #-2]
	.syntax divided
	smlabt r2, r2, r9, r6
	.arm
	.syntax unified
	add	lr, lr, #2
	sub	r0, r0, #4
	cmp	r5, lr
	add	r1, r1, #4
	bgt	.L279
	add	r2, r3, r2
	add	r2, r2, #16384
	asr	r3, r2, #31
	cmp	r3, r2, asr #30
	eorne	r3, r3, #32512
	sbfxeq	r3, r2, #15, #16
	eorne	r3, r3, #255
.L278:
	strh	r3, [r4, #-2]
	ldr	r2, [fp, #-52]
	ldr	r3, [fp, #-60]
	ldr	r3, [r3, r2, lsl #2]
	ldr	r2, [fp, #-56]
	ubfx	r7, r3, #8, #12
	and	r1, r3, #254
	add	r8, r8, r1
	ldr	r1, [fp, #-48]
	ldr	ip, [r2, #68]
	lsr	r2, r3, #20
	mul	r7, r5, r7
	cmp	r1, r8
	mul	r2, r5, r2
	asr	r3, r3, #20
	str	r3, [fp, #-52]
	add	r7, ip, r7, lsl #1
	add	ip, ip, r2, lsl #1
	bhi	.L281
	ldr	r2, [fp, #-68]
	mov	r3, r4
	ldr	r4, [fp, #-56]
	sub	r6, r3, r2
	asr	r6, r6, #1
.L277:
	ldr	r3, [fp, #-48]
	ldr	r2, [r4, #24]
	sub	r8, r8, r3
	ldr	r3, [fp, #-52]
	ldr	r0, [r4, #44]
	asr	r8, r8, #1
	str	ip, [r4, #64]
	str	r3, [r4, #28]
	lsl	r2, r2, #1
	ldr	r3, [fp, #-64]
	str	r8, [r4, #32]
	add	r1, r0, r3
	str	r7, [r4, #60]
	bl	adsp_memmove
	b	.L255
.L301:
	ldr	r8, [r4, #76]
	cmp	r8, #0
	bne	.L267
	ldr	r3, [r4, #28]
	mov	r2, r5
	ldr	lr, [r4, #72]
	mov	r1, r7
	ldr	r9, [r4, #20]
	ldr	r0, [r4, #48]
	str	r3, [fp, #-56]
	str	ip, [sp]
	ldr	r3, [fp, #12]
	str	lr, [fp, #-68]
	bl	cvt_short_short
	ldr	r1, [r4, #48]
	sub	r3, r9, #1
	ldr	r2, [r4, #32]
	sub	r3, r1, r3, lsl #2
	ldr	r5, [r4, #64]
	ldr	r7, [r4, #60]
	add	r2, r3, r2, lsl #1
	str	r2, [fp, #-52]
	lsl	r1, r0, #1
	str	r1, [fp, #-72]
	add	r3, r3, r1
	str	r3, [fp, #-60]
	cmp	r3, r2
	bls	.L268
	ldr	r3, [fp, #-48]
	str	r8, [fp, #-76]
	add	r10, r3, #4
	str	r4, [fp, #-64]
.L274:
	cmp	r9, #0
	ble	.L269
	ldr	r3, [fp, #-52]
	mov	r8, #0
	mov	r2, r8
	mov	r1, r8
	sub	r6, r3, #12
	add	r4, r3, #8
.L270:
	ldr	ip, [r5], #4
	ldr	r0, [r7], #4
	ldr	r3, [r6, #8]
	ldr	lr, [r4, #-8]
	.syntax divided
	smlabb r1, r3, ip, r1
	smlatb r3, r3, ip, r2
	smlabb r1, lr, r0, r1
	smlatb r3, lr, r0, r3
	.arm
	.syntax unified
	ldr	lr, [r6, #4]
	ldr	r2, [r4, #-4]
	.syntax divided
	smlabt r1, lr, ip, r1
	smlatt r3, lr, ip, r3
	smlabt r1, r2, r0, r1
	smlatt r2, r2, r0, r3
	.arm
	.syntax unified
	add	r8, r8, #2
	sub	r6, r6, #8
	cmp	r9, r8
	add	r4, r4, #8
	bgt	.L270
	add	r1, r1, #16384
	add	r2, r2, #16384
	asr	r3, r1, #31
	asr	r0, r2, #15
	cmp	r3, r1, asr #30
	asr	ip, r2, #31
	eorne	r3, r3, #32512
	asr	r2, r2, #30
	sbfxeq	r3, r1, #15, #16
	eorne	r3, r3, #255
	cmp	ip, r2
	strh	r3, [r10, #-4]
	eorne	r0, ip, #32512
	mov	r3, r10
	eorne	r0, r0, #255
.L273:
	ldr	r1, [fp, #-56]
	add	r10, r10, #4
	ldr	r2, [fp, #-68]
	strh	r0, [r10, #-6]
	ldr	ip, [fp, #-52]
	ldr	r2, [r2, r1, lsl #2]
	ldr	r1, [fp, #-64]
	ubfx	r7, r2, #8, #12
	uxtb	r0, r2
	ldr	r5, [r1, #68]
	lsr	r1, r2, #20
	mul	r7, r9, r7
	add	r0, ip, r0, lsl #1
	mul	r1, r9, r1
	ldr	ip, [fp, #-60]
	asr	r2, r2, #20
	str	r0, [fp, #-52]
	cmp	ip, r0
	str	r2, [fp, #-56]
	add	r7, r5, r7, lsl #1
	add	r5, r5, r1, lsl #1
	bhi	.L274
	ldr	r2, [fp, #-48]
	ldr	r4, [fp, #-64]
	sub	r3, r3, r2
	asr	r6, r3, #1
	add	r6, r6, r6, lsr #31
	asr	r6, r6, #1
.L268:
	ldr	r2, [fp, #-60]
	ldr	r3, [fp, #-52]
	ldr	r1, [fp, #-56]
	sub	r3, r3, r2
	ldr	r2, [r4, #24]
	ldr	r0, [r4, #44]
	asr	r3, r3, #1
	str	r3, [r4, #32]
	ldr	r3, [fp, #-72]
	lsl	r2, r2, #1
	str	r1, [r4, #28]
	str	r5, [r4, #64]
	add	r1, r0, r3
	str	r7, [r4, #60]
	bl	adsp_memmove
	b	.L255
.L269:
	ldr	r3, [fp, #-76]
	mov	r0, #0
	strh	r3, [r10, #-4]
	mov	r3, r10
	b	.L273
.L264:
	mov	r6, #-2147483647
	b	.L255
.L283:
	mov	r6, #-2147483646
	b	.L255
.L284:
	mov	r6, #-2147483643
	b	.L255
.L296:
	mov	r6, #-2147483642
	b	.L255
	UNWIND(.fnend)
	.size	resampler_process_frame, .-resampler_process_frame
	.data
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_resampler_info, %object
	.size	g_resampler_info, 960
g_resampler_info:
	.word	32000
	.word	48000
	.word	1
	.word	8
	.word	SRC_res_32_48_mid_nextphasetab
	.word	SRC_res_32_48_mid_filttab
	.word	44100
	.word	48000
	.word	1
	.word	8
	.word	SRC_res_44_48_mid_nextphasetab
	.word	SRC_res_44_48_mid_filttab
	.word	48000
	.word	48000
	.word	1
	.word	8
	.word	res_48_48_nextphasetab
	.word	res_48_48_good_filttab
	.word	8000
	.word	11025
	.word	2
	.word	8
	.word	SRC_res_8_11_good_nextphasetab
	.word	SRC_res_8_11_mid_filttab
	.word	16000
	.word	22050
	.word	2
	.word	8
	.word	SRC_res_8_11_good_nextphasetab
	.word	SRC_res_8_11_mid_filttab
	.word	8000
	.word	22050
	.word	2
	.word	8
	.word	SRC_res_8_22_good_nextphasetab
	.word	SRC_res_8_22_mid_filttab
	.word	16000
	.word	44100
	.word	2
	.word	8
	.word	SRC_res_8_22_good_nextphasetab
	.word	SRC_res_8_22_mid_filttab
	.word	8000
	.word	44100
	.word	2
	.word	8
	.word	SRC_res_8_44_good_nextphasetab
	.word	SRC_res_8_44_mid_filttab
	.word	11025
	.word	12000
	.word	2
	.word	8
	.word	SRC_res_11_12_good_nextphasetab
	.word	SRC_res_11_12_mid_filttab
	.word	22050
	.word	24000
	.word	2
	.word	8
	.word	SRC_res_11_12_good_nextphasetab
	.word	SRC_res_11_12_mid_filttab
	.word	11025
	.word	16000
	.word	2
	.word	8
	.word	SRC_res_11_16_good_nextphasetab
	.word	SRC_res_11_16_mid_filttab
	.word	22050
	.word	32000
	.word	2
	.word	8
	.word	SRC_res_11_16_good_nextphasetab
	.word	SRC_res_11_16_mid_filttab
	.word	11025
	.word	24000
	.word	2
	.word	8
	.word	SRC_res_11_24_good_nextphasetab
	.word	SRC_res_11_24_mid_filttab
	.word	22050
	.word	48000
	.word	2
	.word	8
	.word	SRC_res_11_24_good_nextphasetab
	.word	SRC_res_11_24_mid_filttab
	.word	11025
	.word	32000
	.word	2
	.word	8
	.word	SRC_res_11_32_good_nextphasetab
	.word	SRC_res_11_32_mid_filttab
	.word	11025
	.word	48000
	.word	2
	.word	8
	.word	SRC_res_11_48_good_nextphasetab
	.word	SRC_res_11_48_mid_filttab
	.word	12000
	.word	22050
	.word	2
	.word	8
	.word	SRC_res_12_22_good_nextphasetab
	.word	SRC_res_12_22_mid_filttab
	.word	24000
	.word	44100
	.word	2
	.word	8
	.word	SRC_res_12_22_good_nextphasetab
	.word	SRC_res_12_22_mid_filttab
	.word	12000
	.word	44100
	.word	2
	.word	8
	.word	SRC_res_12_44_good_nextphasetab
	.word	SRC_res_12_44_mid_filttab
	.word	8000
	.word	12000
	.word	2
	.word	16
	.word	SRC_res_8_12_good_nextphasetab
	.word	SRC_res_8_12_good_filttab
	.word	8000
	.word	16000
	.word	2
	.word	16
	.word	SRC_res_8_16_good_nextphasetab
	.word	SRC_res_8_16_good_filttab
	.word	8000
	.word	24000
	.word	2
	.word	16
	.word	SRC_res_8_24_good_nextphasetab
	.word	SRC_res_8_24_good_filttab
	.word	8000
	.word	32000
	.word	2
	.word	16
	.word	SRC_res_8_32_good_nextphasetab
	.word	SRC_res_8_32_good_filttab
	.word	8000
	.word	48000
	.word	2
	.word	16
	.word	SRC_res_8_48_good_nextphasetab
	.word	SRC_res_8_48_good_filttab
	.word	11025
	.word	22050
	.word	2
	.word	16
	.word	SRC_res_8_16_good_nextphasetab
	.word	SRC_res_8_16_good_filttab
	.word	11025
	.word	44100
	.word	2
	.word	16
	.word	SRC_res_8_32_good_nextphasetab
	.word	SRC_res_8_32_good_filttab
	.word	12000
	.word	16000
	.word	2
	.word	16
	.word	SRC_res_12_16_good_nextphasetab
	.word	SRC_res_12_16_good_filttab
	.word	12000
	.word	24000
	.word	2
	.word	16
	.word	SRC_res_8_16_good_nextphasetab
	.word	SRC_res_8_16_good_filttab
	.word	12000
	.word	32000
	.word	2
	.word	16
	.word	SRC_res_12_32_good_nextphasetab
	.word	SRC_res_12_32_good_filttab
	.word	12000
	.word	48000
	.word	2
	.word	16
	.word	SRC_res_8_32_good_nextphasetab
	.word	SRC_res_8_32_good_filttab
	.word	16000
	.word	24000
	.word	2
	.word	16
	.word	SRC_res_8_12_good_nextphasetab
	.word	SRC_res_8_12_good_filttab
	.word	16000
	.word	32000
	.word	2
	.word	16
	.word	SRC_res_8_16_good_nextphasetab
	.word	SRC_res_8_16_good_filttab
	.word	16000
	.word	48000
	.word	2
	.word	16
	.word	SRC_res_8_24_good_nextphasetab
	.word	SRC_res_8_24_good_filttab
	.word	22050
	.word	44100
	.word	2
	.word	16
	.word	SRC_res_8_16_good_nextphasetab
	.word	SRC_res_8_16_good_filttab
	.word	24000
	.word	32000
	.word	2
	.word	16
	.word	SRC_res_12_16_good_nextphasetab
	.word	SRC_res_12_16_good_filttab
	.word	24000
	.word	48000
	.word	2
	.word	16
	.word	SRC_res_8_16_good_nextphasetab
	.word	SRC_res_8_16_good_filttab
	.word	32000
	.word	44100
	.word	2
	.word	16
	.word	SRC_res_8_11_good_nextphasetab
	.word	SRC_res_8_11_good_filttab
	.word	32000
	.word	48000
	.word	2
	.word	16
	.word	SRC_res_8_12_good_nextphasetab
	.word	SRC_res_8_12_good_filttab
	.word	44100
	.word	48000
	.word	2
	.word	36
	.word	SRC_res_11_12_good_nextphasetab
	.word	SRC_res_11_12_good_filttab
	.word	48000
	.word	48000
	.word	2
	.word	8
	.word	res_48_48_nextphasetab
	.word	res_48_48_good_filttab
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
