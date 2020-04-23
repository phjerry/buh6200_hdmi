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
	.file	"sonic.c"
	.text
	.global	__aeabi_uidiv
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	find_pitch_period_in_range, %function
find_pitch_period_in_range:
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
	mov	ip, r0
	str	r0, [fp, #-48]
	str	r3, [fp, #-52]
	bgt	.L11
	mov	r0, #0
	add	r7, ip, r1, lsl #1
	mov	r8, r0
	mov	r9, #1
	mov	r10, #255
.L10:
	cmp	r1, #0
	ble	.L3
	ldr	r4, [fp, #-48]
	mov	r6, r7
	mov	r5, #0
.L6:
	ldrsh	r3, [r4], #2
	ldrsh	ip, [r6], #2
	cmp	r3, ip
	uxth	r3, r3
	uxth	ip, ip
	sub	lr, ip, r3
	sub	r3, r3, ip
	uxthlt	r3, lr
	uxthge	r3, r3
	cmp	r7, r4
	add	r5, r5, r3
	bne	.L6
	cmp	r8, #0
	mul	r3, r10, r5
	beq	.L7
	mul	ip, r9, r1
	mul	lr, r5, r8
	cmp	ip, lr
	bls	.L8
.L7:
	mov	r9, r5
	mov	r8, r1
.L8:
	mul	ip, r0, r1
	cmp	ip, r3
	movcc	r3, r1
	movcs	r3, r10
	movcc	r0, r5
	movcc	r10, r1
.L9:
	add	r1, r1, #1
	add	r7, r7, #2
	cmp	r2, r1
	bge	.L10
	mov	r1, r3
	bl	__aeabi_uidiv
	mov	r1, r8
	mov	r4, r0
.L2:
	mov	r0, r9
	bl	__aeabi_uidiv
	ldr	r3, [fp, #-52]
	str	r0, [r3]
	mov	r0, r8
	ldr	r3, [fp, #4]
	str	r4, [r3]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L3:
	cmp	r8, #0
	beq	.L13
	mul	r3, r9, r1
	cmp	r3, #0
	moveq	r3, r10
	beq	.L9
	mov	r3, #0
	mov	r5, r3
	b	.L7
.L13:
	mov	r3, r10
	mov	r8, r1
	mov	r9, #0
	b	.L9
.L11:
	mov	r4, #0
	mov	r9, #1
	mov	r1, r4
	mov	r8, r4
	b	.L2
	UNWIND(.fnend)
	.size	find_pitch_period_in_range, .-find_pitch_period_in_range
	.global	__aeabi_idiv
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	find_pitch_period, %function
find_pitch_period:
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
	str	r2, [fp, #-68]
	mov	r5, r0
	ldr	r2, [r0, #76]
	mov	r4, r1
	ldr	r3, [r0, #92]
	ldr	r7, [r0, #48]
	str	r2, [fp, #-60]
	cmp	r3, #4000
	ldr	r2, [r0, #80]
	str	r2, [fp, #-64]
	ble	.L63
	ldr	r1, [r0, #44]
	cmp	r1, #0
	bne	.L63
	movw	r0, #19923
	movt	r0, 4194
	asr	r2, r3, #31
	smull	r0, r1, r3, r0
	rsb	r3, r2, r1, asr #8
	str	r3, [fp, #-56]
	cmp	r3, #1
	cmpeq	r7, #1
	mov	r6, r3
	beq	.L24
	mov	r1, r3
	ldr	r0, [fp, #-60]
	bl	__aeabi_idiv
	mov	r1, r6
	mul	r7, r6, r7
	str	r0, [fp, #-60]
	ldr	r0, [fp, #-64]
	bl	__aeabi_idiv
	str	r0, [fp, #-64]
	b	.L25
.L63:
	cmp	r7, #1
	beq	.L24
	mov	r3, #1
	str	r3, [fp, #-56]
.L25:
	ldr	r1, [fp, #-56]
	ldr	r0, [r5, #84]
	bl	__aeabi_idiv
	ldr	r10, [r5, #12]
	cmp	r0, #0
	ble	.L29
	add	r8, r10, r0, lsl #1
	lsl	r9, r7, #1
	mov	r6, r4
.L31:
	cmp	r7, #0
	movle	r0, #0
	ble	.L33
	add	r2, r6, r9
	mov	r0, #0
.L30:
	ldrsh	r3, [r6], #2
	cmp	r6, r2
	add	r0, r0, r3
	bne	.L30
	mov	r1, r7
	bl	__aeabi_idiv
	sxth	r0, r0
.L33:
	strh	r0, [r10], #2
	cmp	r8, r10
	bne	.L31
	ldr	r10, [r5, #12]
.L29:
	ldr	r2, [fp, #-64]
	sub	r3, fp, #52
	sub	ip, fp, #48
	ldr	r1, [fp, #-60]
	mov	r0, r10
	str	ip, [sp]
	bl	find_pitch_period_in_range
	ldr	r2, [fp, #-56]
	cmp	r2, #1
	mov	r3, r0
	beq	.L27
	mul	r3, r2, r0
	lsl	r7, r2, #2
	ldr	r1, [r5, #76]
	ldr	r2, [r5, #80]
	sub	r0, r3, r7
	add	r7, r7, r3
	cmp	r0, r1
	ldr	r10, [r5, #48]
	movge	r3, r0
	movlt	r3, r1
	cmp	r7, r2
	str	r3, [fp, #-56]
	movge	r7, r2
	cmp	r10, #1
	beq	.L64
	ldr	r8, [r5, #84]
	ldr	r6, [r5, #12]
	cmp	r8, #0
	addgt	r8, r6, r8, lsl #1
	lslgt	r9, r10, #1
	ble	.L36
.L38:
	cmp	r10, #0
	movle	r0, #0
	ble	.L40
	add	r2, r4, r9
	mov	r0, #0
.L37:
	ldrsh	r3, [r4], #2
	cmp	r4, r2
	add	r0, r0, r3
	bne	.L37
	mov	r1, r10
	bl	__aeabi_idiv
	sxth	r0, r0
.L40:
	strh	r0, [r6], #2
	cmp	r6, r8
	bne	.L38
	ldr	r6, [r5, #12]
.L36:
	sub	r3, fp, #48
	mov	r2, r7
	str	r3, [sp]
	mov	r0, r6
	sub	r3, fp, #52
	ldr	r1, [fp, #-56]
	bl	find_pitch_period_in_range
	mov	r3, r0
.L27:
	ldr	r2, [fp, #-52]
	cmp	r2, #0
	beq	.L45
	ldr	r0, [r5, #96]
	cmp	r0, #0
	beq	.L45
	ldr	r1, [fp, #-68]
	cmp	r1, #0
	beq	.L42
	lsl	ip, r2, #1
	ldr	lr, [fp, #-48]
	add	r1, ip, r2
	cmp	lr, r1
	bgt	.L45
	ldr	r1, [r5, #100]
	add	r1, r1, r1, lsl #1
	cmp	ip, r1
	movle	r0, r3
	b	.L41
.L45:
	mov	r0, r3
.L41:
	str	r2, [r5, #100]
	str	r3, [r5, #96]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L24:
	sub	r3, fp, #48
	ldr	r2, [fp, #-64]
	str	r3, [sp]
	mov	r0, r4
	sub	r3, fp, #52
	ldr	r1, [fp, #-60]
	bl	find_pitch_period_in_range
	mov	r3, r0
	b	.L27
.L42:
	ldr	r1, [r5, #100]
	str	r3, [r5, #96]
	cmp	r2, r1
	str	r2, [r5, #100]
	movle	r0, r3
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L64:
	sub	r3, fp, #48
	mov	r2, r7
	str	r3, [sp]
	mov	r0, r4
	sub	r3, fp, #52
	ldr	r1, [fp, #-56]
	bl	find_pitch_period_in_range
	mov	r3, r0
	b	.L27
	UNWIND(.fnend)
	.size	find_pitch_period, .-find_pitch_period
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	overlap_add, %function
overlap_add:
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
	cmp	r1, #0
	mov	r5, r0
	ble	.L65
	str	r3, [fp, #-48]
	lsl	r9, r1, #1
	ldr	r3, [fp, #4]
	mov	r8, r2
	add	r2, r2, r9
	str	r2, [fp, #-56]
	str	r3, [fp, #-52]
.L69:
	cmp	r5, #0
	mov	r6, r8
	ldr	r10, [fp, #-52]
	ldr	r7, [fp, #-48]
	ble	.L67
	mov	r4, #0
.L68:
	ldrsh	r3, [r10], r9
	sub	r0, r5, r4
	ldrsh	r2, [r7], r9
	mov	r1, r5
	mul	r3, r4, r3
	add	r4, r4, #1
	mla	r0, r0, r2, r3
	bl	__aeabi_idiv
	cmp	r5, r4
	strh	r0, [r6], r9
	bne	.L68
.L67:
	ldr	r3, [fp, #-52]
	add	r8, r8, #2
	add	r3, r3, #2
	str	r3, [fp, #-52]
	ldr	r3, [fp, #-56]
	cmp	r3, r8
	ldr	r3, [fp, #-48]
	add	r3, r3, #2
	str	r3, [fp, #-48]
	bne	.L69
.L65:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
	UNWIND(.fnend)
	.size	overlap_add, .-overlap_add
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	free_stream_buffers, %function
free_stream_buffers:
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
	ldr	r1, [r0]
	mov	r4, r0
	cmp	r1, #0
	beq	.L73
	mov	r0, #70
	bl	adsp_vfree
	mov	r3, #0
	str	r3, [r4]
.L73:
	ldr	r1, [r4, #4]
	cmp	r1, #0
	beq	.L74
	mov	r0, #70
	bl	adsp_vfree
	mov	r3, #0
	str	r3, [r4, #4]
.L74:
	ldr	r1, [r4, #8]
	cmp	r1, #0
	beq	.L75
	mov	r0, #70
	bl	adsp_vfree
	mov	r3, #0
	str	r3, [r4, #8]
.L75:
	ldr	r1, [r4, #12]
	cmp	r1, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	mov	r0, #70
	bl	adsp_vfree
	mov	r3, #0
	str	r3, [r4, #12]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	free_stream_buffers, .-free_stream_buffers
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	allocate_stream_buffers, %function
allocate_stream_buffers:
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
	movw	r3, #57471
	movt	r3, 32263
	mov	r7, r2
	asr	r8, r1, #31
	smull	r2, r3, r1, r3
	mov	r4, r0
	mov	r6, r1
	mov	r0, #70
	rsb	r5, r8, r3, asr #5
	lsl	r9, r5, #1
	lsl	r2, r5, #4
	str	r2, [r4, #52]
	mul	r10, r9, r7
	str	r2, [fp, #-52]
	lsl	r3, r10, #4
	str	r3, [fp, #-48]
	mov	r1, r3
	bl	adsp_vmalloc
	ldr	r3, [fp, #-48]
	ldr	r2, [fp, #-52]
	cmp	r0, #0
	str	r0, [r4]
	beq	.L92
	str	r2, [r4, #60]
	mov	r1, r3
	mov	r0, #70
	bl	adsp_vmalloc
	cmp	r0, #0
	str	r0, [r4, #4]
	beq	.L92
	str	r9, [r4, #56]
	lsl	r1, r10, #1
	mov	r0, #70
	bl	adsp_vmalloc
	cmp	r0, #0
	str	r0, [r4, #8]
	beq	.L92
	lsl	r1, r5, #2
	mov	r0, #70
	bl	adsp_vmalloc
	cmp	r0, #0
	str	r0, [r4, #12]
	beq	.L92
	movw	r2, #34079
	movt	r2, 20971
	mov	r1, #0
	mov	r0, #1
	smull	r2, r3, r6, r2
	str	r6, [r4, #92]
	str	r7, [r4, #48]
	str	r5, [r4, #80]
	str	r9, [r4, #84]
	str	r1, [r4, #32]
	rsb	r8, r8, r3, asr #7
	str	r1, [r4, #36]
	str	r1, [r4, #96]
	str	r8, [r4, #76]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L92:
	mov	r0, r4
	bl	free_stream_buffers
	mov	r1, r4
	mov	r0, #70
	bl	adsp_vfree
	mov	r0, #0
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
	UNWIND(.fnend)
	.size	allocate_stream_buffers, .-allocate_stream_buffers
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	move_new_samples_to_pitch_buffer, %function
move_new_samples_to_pitch_buffer:
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
	ldr	r5, [r0, #68]
	ldr	r3, [r0, #72]
	mov	r6, r1
	sub	r5, r5, r1
	ldr	r2, [r0, #56]
	add	r1, r3, r5
	mov	r4, r0
	cmp	r1, r2
	ble	.L105
	mov	r0, #0
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L105:
	ldr	ip, [r0, #48]
	ldr	r1, [r4, #4]
	ldr	r0, [r0, #8]
	mul	r3, ip, r3
	lsl	r2, ip, #1
	mul	ip, ip, r6
	mul	r2, r5, r2
	add	r0, r0, r3, lsl #1
	add	r1, r1, ip, lsl #1
	bl	adsp_memcpy
	ldr	r3, [r4, #72]
	str	r6, [r4, #68]
	mov	r0, #1
	add	r5, r3, r5
	str	r5, [r4, #72]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	UNWIND(.fnend)
	.size	move_new_samples_to_pitch_buffer, .-move_new_samples_to_pitch_buffer
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	remove_pitch_samples, %function
remove_pitch_samples:
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
	subs	r5, r1, #0
	mov	r4, r0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r2, [r0, #72]
	cmp	r2, r5
	sub	r2, r2, r5
	bne	.L112
	str	r2, [r4, #72]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L112:
	ldr	r3, [r0, #48]
	ldr	r1, [r0, #8]
	mul	ip, r5, r3
	lsl	r3, r3, #1
	mov	r0, r1
	mul	r2, r2, r3
	add	r1, r1, ip, lsl #1
	bl	adsp_memmove
	ldr	r2, [r4, #72]
	sub	r2, r2, r5
	str	r2, [r4, #72]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	remove_pitch_samples, .-remove_pitch_samples
	.align	2
	.global	sonic_get_speed
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_speed, %function
sonic_get_speed:
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
	ldr	r0, [r0, #16]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_speed, .-sonic_get_speed
	.align	2
	.global	sonic_set_speed
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_speed, %function
sonic_set_speed:
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
	str	r1, [r0, #16]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_speed, .-sonic_set_speed
	.align	2
	.global	sonic_get_pitch
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_pitch, %function
sonic_get_pitch:
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
	ldr	r0, [r0, #24]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_pitch, .-sonic_get_pitch
	.align	2
	.global	sonic_set_pitch
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_pitch, %function
sonic_set_pitch:
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
	str	r1, [r0, #24]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_pitch, .-sonic_set_pitch
	.align	2
	.global	sonic_get_rate
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_rate, %function
sonic_get_rate:
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
	ldr	r0, [r0, #28]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_rate, .-sonic_get_rate
	.align	2
	.global	sonic_set_rate
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_rate, %function
sonic_set_rate:
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
	mov	r3, #0
	str	r1, [r0, #28]
	str	r3, [r0, #32]
	str	r3, [r0, #36]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_rate, .-sonic_set_rate
	.align	2
	.global	sonic_get_chord_pitch
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_chord_pitch, %function
sonic_get_chord_pitch:
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
	ldr	r0, [r0, #40]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_chord_pitch, .-sonic_get_chord_pitch
	.align	2
	.global	sonic_set_chord_pitch
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_chord_pitch, %function
sonic_set_chord_pitch:
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
	str	r1, [r0, #40]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_chord_pitch, .-sonic_set_chord_pitch
	.align	2
	.global	sonic_get_quality
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_quality, %function
sonic_get_quality:
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
	ldr	r0, [r0, #44]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_quality, .-sonic_get_quality
	.align	2
	.global	sonic_set_quality
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_quality, %function
sonic_set_quality:
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
	str	r1, [r0, #44]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_quality, .-sonic_set_quality
	.align	2
	.global	sonic_get_volume
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_volume, %function
sonic_get_volume:
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
	ldr	r0, [r0, #20]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_volume, .-sonic_get_volume
	.align	2
	.global	sonic_set_volume
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_volume, %function
sonic_set_volume:
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
	str	r1, [r0, #20]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_volume, .-sonic_set_volume
	.align	2
	.global	sonic_destroy_stream
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_destroy_stream, %function
sonic_destroy_stream:
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
	mov	r4, r0
	bl	free_stream_buffers
	mov	r1, r4
	mov	r0, #70
	bl	adsp_vfree
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_destroy_stream, .-sonic_destroy_stream
	.align	2
	.global	sonic_create_stream
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_create_stream, %function
sonic_create_stream:
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
	mov	r5, r0
	mov	r6, r1
	mov	r0, #70
	mov	r1, #104
	bl	adsp_vmalloc
	subs	r4, r0, #0
	beq	.L129
	mov	r2, #104
	mov	r1, #0
	bl	adsp_memset
	mov	r2, r6
	mov	r1, r5
	mov	r0, r4
	bl	allocate_stream_buffers
	cmp	r0, #0
	beq	.L129
	mov	r2, #256
	mov	r3, #0
	str	r2, [r4, #16]
	mov	r0, r4
	str	r2, [r4, #24]
	str	r2, [r4, #20]
	str	r2, [r4, #28]
	str	r3, [r4, #32]
	str	r3, [r4, #36]
	str	r3, [r4, #40]
	str	r3, [r4, #44]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L129:
	mov	r4, #0
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_create_stream, .-sonic_create_stream
	.align	2
	.global	sonic_get_sample_rate
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_sample_rate, %function
sonic_get_sample_rate:
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
	ldr	r0, [r0, #92]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_sample_rate, .-sonic_get_sample_rate
	.align	2
	.global	sonic_set_sample_rate
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_sample_rate, %function
sonic_set_sample_rate:
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
	mov	r4, r0
	mov	r5, r1
	bl	free_stream_buffers
	mov	r1, r5
	ldr	r2, [r4, #48]
	mov	r0, r4
	bl	allocate_stream_buffers
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_sample_rate, .-sonic_set_sample_rate
	.align	2
	.global	sonic_get_num_channels
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_num_channels, %function
sonic_get_num_channels:
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
	ldr	r0, [r0, #48]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_num_channels, .-sonic_get_num_channels
	.align	2
	.global	sonic_set_num_channels
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_set_num_channels, %function
sonic_set_num_channels:
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
	mov	r5, r1
	mov	r4, r0
	bl	free_stream_buffers
	mov	r2, r5
	ldr	r1, [r4, #92]
	mov	r0, r4
	bl	allocate_stream_buffers
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_set_num_channels, .-sonic_set_num_channels
	.align	2
	.global	sonic_read_short_from_stream
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_read_short_from_stream, %function
sonic_read_short_from_stream:
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
	mov	r5, r0
	ldr	r4, [r5, #68]
	mov	r0, r1
	mov	r6, r2
	cmp	r4, #0
	beq	.L137
	cmp	r4, r2
	ldr	r1, [r5, #4]
	ldr	r3, [r5, #48]
	ble	.L139
	lsl	r2, r2, #1
	sub	r7, r4, r6
	mul	r2, r3, r2
	bl	adsp_memcpy
	cmp	r7, #0
	ble	.L141
	ldr	r2, [r5, #48]
	mov	r4, r6
	ldr	r3, [r5, #4]
	mul	r1, r6, r2
	lsl	r2, r2, #1
	mov	r0, r3
	mul	r2, r7, r2
	add	r1, r3, r1, lsl #1
	bl	adsp_memmove
	str	r7, [r5, #68]
.L137:
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L139:
	mul	r2, r3, r4
	mov	r7, #0
	lsl	r2, r2, #1
	bl	adsp_memcpy
.L140:
	str	r7, [r5, #68]
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L141:
	mov	r4, r6
	b	.L140
	UNWIND(.fnend)
	.size	sonic_read_short_from_stream, .-sonic_read_short_from_stream
	.align	2
	.global	sonic_samples_available
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_samples_available, %function
sonic_samples_available:
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
	ldr	r0, [r0, #68]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_samples_available, .-sonic_samples_available
	.align	2
	.global	sonic_write_short_to_stream
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_write_short_to_stream, %function
sonic_write_short_to_stream:
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
	subs	r4, r2, #0
	mov	r9, r0
	ldr	r6, [r0, #64]
	beq	.L147
	ldr	r2, [r0, #52]
	add	r0, r4, r6
	cmp	r0, r2
	bgt	.L168
	ldr	r2, [r9, #48]
	lsl	r3, r4, #1
	ldr	r0, [r9]
	cmp	r1, #0
	mul	r6, r2, r6
	mul	r2, r2, r3
	add	r0, r0, r6, lsl #1
	beq	.L223
	bl	adsp_memcpy
.L151:
	ldr	r6, [r9, #64]
	add	r6, r4, r6
	str	r6, [r9, #64]
.L147:
	ldr	r4, [r9, #24]
	ldr	r0, [r9, #16]
	ldr	r8, [r9, #68]
	mov	r1, r4
	lsl	r0, r0, #8
	bl	__aeabi_idiv
	ldr	r7, [r9, #40]
	ldr	r3, [r9, #28]
	cmp	r7, #0
	str	r8, [fp, #-56]
	muleq	r4, r3, r4
	str	r3, [fp, #-52]
	asreq	r3, r4, #8
	streq	r3, [fp, #-52]
	cmp	r0, #256
	mov	r2, r0
	beq	.L153
	ldr	r3, [r9, #84]
	cmp	r3, r6
	str	r3, [fp, #-48]
	bgt	.L154
	lsl	r3, r0, #1
	rsb	r1, r0, #256
	mov	r10, #0
	mov	r0, r8
	mov	r8, r2
	sub	r3, r3, #256
	str	r1, [fp, #-64]
	str	r3, [fp, #-72]
.L166:
	ldr	r2, [r9, #48]
	ldr	r4, [r9, #88]
	ldr	r5, [r9]
	mul	r3, r2, r10
	cmp	r4, #0
	add	r5, r5, r3, lsl #1
	ble	.L155
	ldr	r3, [r9, #84]
	ldr	r1, [r9, #60]
	cmp	r4, r3
	movge	r4, r3
	add	r3, r0, r4
	cmp	r3, r1
	ble	.L156
.L222:
	ldr	r7, [r9, #40]
.L154:
	cmp	r7, #0
	bne	.L224
	ldr	r1, [fp, #-52]
	cmp	r1, #256
	beq	.L198
	ldr	r8, [r9, #92]
	lsl	r0, r8, #8
	bl	__aeabi_idiv
	cmp	r8, #16384
	cmple	r0, #16384
	mov	r4, r0
	ble	.L180
.L181:
	asr	r4, r4, #1
	asr	r8, r8, #1
	cmp	r8, #16384
	cmple	r4, #16384
	bgt	.L181
.L180:
	ldr	r3, [r9, #68]
	ldr	r2, [fp, #-56]
	cmp	r2, r3
	beq	.L198
	mov	r1, r2
	mov	r0, r9
	ldr	r5, [r9, #48]
	bl	move_new_samples_to_pitch_buffer
	cmp	r0, #0
	beq	.L168
	ldr	r3, [r9, #72]
	sub	r3, r3, #1
	cmp	r3, #0
	mov	r3, #0
	str	r3, [fp, #-56]
	ble	.L182
	str	r7, [fp, #-60]
	mov	r7, r5
	lsl	r2, r5, #1
	str	r2, [fp, #-52]
.L183:
	ldr	r1, [r9, #32]
	ldr	lr, [r9, #36]
	add	r2, r1, #1
	mul	r0, r8, lr
	mul	r3, r4, r2
	cmp	r3, r0
	ble	.L192
	ldr	ip, [r9, #68]
	ldr	r5, [r9, #60]
	add	r3, ip, #1
	cmp	r5, r3
	blt	.L220
	ldr	r3, [fp, #-56]
	mov	r10, lr
	lsl	r3, r3, #1
	str	r3, [fp, #-48]
.L193:
	mul	r3, ip, r7
	ldr	r5, [r9, #8]
	ldr	lr, [fp, #-48]
	cmp	r7, #0
	ldr	r6, [r9, #4]
	add	r5, r5, lr
	lsl	r3, r3, #1
	ble	.L185
	sub	r3, r3, #2
	add	r10, r6, r3
	ldr	r3, [fp, #-52]
	add	r6, r5, r3
	b	.L187
.L186:
	ldr	r3, [r9, #36]
	ldr	r1, [r9, #32]
	mul	r0, r3, r8
.L187:
	ldr	r2, [r9, #48]
	mla	r3, r1, r4, r4
	ldrsh	ip, [r5]
	lsl	r2, r2, #1
	mov	lr, r5
	mov	r1, r4
	add	r5, r5, #2
	ldrsh	r2, [lr, r2]
	sub	r3, r3, r0
	sub	r0, r4, r3
	mul	r3, r3, ip
	mla	r0, r0, r2, r3
	bl	__aeabi_idiv
	cmp	r6, r5
	strh	r0, [r10, #2]!
	bne	.L186
	ldr	r1, [r9, #32]
	ldr	r3, [r9, #36]
	ldr	ip, [r9, #68]
	add	r2, r1, #1
	mov	r10, r3
.L185:
	add	r0, r10, #1
	str	r0, [r9, #36]
	mul	r3, r4, r2
	mov	r10, r0
	mul	r0, r8, r0
	mov	r5, ip
	add	ip, ip, #1
	str	ip, [r9, #68]
	cmp	r3, r0
	ble	.L225
	ldr	r3, [r9, #60]
	add	r5, r5, #2
	cmp	r5, r3
	ble	.L193
.L220:
	ldr	r7, [fp, #-60]
	b	.L146
.L153:
	ldr	r3, [r9, #60]
	add	r2, r8, r6
	cmp	r2, r3
	ble	.L226
.L168:
	mov	r7, #0
.L146:
	mov	r0, r7
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L224:
	ldr	r3, [r9, #24]
	cmp	r3, #256
	str	r3, [fp, #-72]
	beq	.L198
	ldr	r3, [r9, #68]
	ldr	r1, [fp, #-56]
	cmp	r1, r3
	beq	.L198
	ldr	r4, [r9, #48]
	mov	r0, r9
	str	r4, [fp, #-76]
	bl	move_new_samples_to_pitch_buffer
	cmp	r0, #0
	beq	.L168
	ldr	r2, [r9, #72]
	ldr	r3, [r9, #84]
	cmp	r2, r3
	blt	.L196
	mov	r3, #0
	lsl	r10, r4, #1
	str	r3, [fp, #-60]
	str	r9, [fp, #-68]
	b	.L179
.L228:
	sub	lr, lr, r7
	add	r3, ip, r4
	mov	r2, r1
	mov	r0, r7
	mul	lr, r8, lr
	mov	r1, r8
	add	ip, ip, lr, lsl #1
	str	ip, [sp]
	bl	overlap_add
	ldr	r3, [r6, #68]
	add	r3, r7, r3
.L172:
	ldr	r0, [fp, #-68]
	ldr	ip, [fp, #-60]
	ldr	r2, [r0, #72]
	ldr	r1, [r0, #84]
	sub	r2, r2, ip
	str	r3, [r0, #68]
	cmp	r2, r1
	blt	.L227
.L179:
	ldr	r3, [fp, #-60]
	mov	r2, #0
	ldr	r8, [fp, #-76]
	ldr	r6, [fp, #-68]
	mul	r4, r3, r8
	ldr	r1, [r6, #8]
	mov	r0, r6
	lsl	r4, r4, #1
	add	r1, r1, r4
	bl	find_pitch_period
	ldr	r9, [fp, #-72]
	mov	r1, r9
	mov	r5, r0
	lsl	r0, r0, #8
	bl	__aeabi_idiv
	ldr	r2, [r6, #68]
	ldr	r1, [r6, #60]
	asr	r7, r0, #8
	add	r3, r7, r2
	cmp	r3, r1
	bgt	.L168
	mul	r2, r2, r8
	ldr	ip, [fp, #-60]
	ldr	r1, [r6, #4]
	cmp	r9, #255
	add	lr, ip, r5
	ldr	ip, [r6, #8]
	str	lr, [fp, #-60]
	add	r1, r1, r2, lsl #1
	bgt	.L228
	ldr	r2, [fp, #-76]
	sub	r9, r7, r5
	cmp	r2, #0
	ble	.L172
	add	r3, ip, r4
	str	r3, [fp, #-56]
	add	r3, r1, r10
	str	r3, [fp, #-64]
	mov	r3, r10
	mov	r10, r9
	mov	r9, r3
	str	r1, [fp, #-52]
.L178:
	ldr	r3, [fp, #-56]
	cmp	r7, #0
	ldr	r6, [fp, #-52]
	mov	r8, r3
	ble	.L173
	str	r3, [fp, #-48]
	mov	r4, #0
	b	.L177
.L229:
	ldrsh	r0, [r8], r9
	add	r4, r4, #1
	mul	r0, r3, r0
	bl	__aeabi_idiv
	cmp	r7, r4
	strh	r0, [r6]
	add	r6, r6, r9
	beq	.L173
.L177:
	cmp	r10, r4
	sub	r3, r5, r4
	mov	r1, r5
	sub	ip, r4, r10
	bgt	.L229
	ldr	r2, [fp, #-48]
	cmp	r5, r4
	sub	lr, r5, r4
	add	r4, r4, #1
	movle	r1, r5
	ldrsh	r3, [r2], r9
	mul	r0, r3, ip
	ldrshgt	r3, [r8], r9
	str	r2, [fp, #-48]
	mlagt	r0, lr, r3, r0
	bl	__aeabi_idiv
	cmp	r7, r4
	strh	r0, [r6]
	add	r6, r6, r9
	bne	.L177
.L173:
	ldr	r2, [fp, #-56]
	ldr	r3, [fp, #-52]
	add	r2, r2, #2
	str	r2, [fp, #-56]
	ldr	r2, [fp, #-64]
	add	r3, r3, #2
	str	r3, [fp, #-52]
	cmp	r2, r3
	bne	.L178
	ldr	r3, [fp, #-68]
	mov	r10, r9
	ldr	r3, [r3, #68]
	add	r3, r7, r3
	b	.L172
.L198:
	mov	r7, #1
	mov	r0, r7
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L155:
	mov	r2, #1
	mov	r1, r5
	mov	r0, r9
	bl	find_pitch_period
	cmp	r8, #256
	mov	r7, r0
	ble	.L158
	ldr	r3, [r9, #48]
	cmp	r8, #512
	sub	r1, r8, #256
	str	r3, [fp, #-60]
	blt	.L159
	lsl	r0, r0, #8
	bl	__aeabi_idiv
	ldr	r3, [fp, #-60]
	mov	r4, r0
.L160:
	ldr	r2, [r9, #68]
	ldr	r0, [r9, #60]
	add	r1, r2, r4
	cmp	r1, r0
	bgt	.L222
	mul	ip, r3, r7
	ldr	lr, [r9, #4]
	mul	r2, r2, r3
	mov	r1, r3
	mov	r0, r4
	mov	r3, r5
	add	r7, r7, r4
	add	r5, r5, ip, lsl #1
	str	r5, [sp]
	add	r10, r10, r7
	add	r2, lr, r2, lsl #1
	bl	overlap_add
	ldr	r0, [r9, #68]
	add	r0, r0, r4
	str	r0, [r9, #68]
.L157:
	cmp	r4, #0
	beq	.L222
	ldr	r3, [fp, #-48]
	add	r3, r3, r10
	cmp	r3, r6
	ble	.L166
	ldr	r4, [r9, #64]
	sub	r4, r4, r10
	cmp	r4, #0
	ble	.L167
	ldr	r3, [r9, #48]
	ldr	r1, [r9]
	mul	r10, r3, r10
	lsl	r3, r3, #1
	mov	r0, r1
	mul	r2, r4, r3
	add	r1, r1, r10, lsl #1
	bl	adsp_memmove
.L167:
	str	r4, [r9, #64]
	b	.L222
.L156:
	mul	r0, r2, r0
	ldr	r3, [r9, #4]
	lsl	r2, r2, #1
	mov	r1, r5
	add	r10, r10, r4
	mul	r2, r4, r2
	add	r0, r3, r0, lsl #1
	bl	adsp_memcpy
	ldr	r0, [r9, #68]
	ldr	r3, [r9, #88]
	add	r0, r0, r4
	str	r0, [r9, #68]
	sub	r3, r3, r4
	str	r3, [r9, #88]
	b	.L157
.L158:
	ldr	r3, [r9, #48]
	cmp	r8, #127
	ldr	r1, [fp, #-64]
	str	r3, [fp, #-60]
	bgt	.L162
	mul	r0, r0, r8
	bl	__aeabi_idiv
	mov	r4, r0
.L163:
	add	r3, r7, r4
	ldr	r2, [r9, #68]
	str	r3, [fp, #-68]
	mov	r1, r3
	ldr	r3, [r9, #60]
	add	r1, r1, r2
	cmp	r1, r3
	bgt	.L222
	ldr	r1, [fp, #-60]
	add	r10, r10, r4
	ldr	r0, [r9, #4]
	lsl	r3, r1, #1
	mul	r2, r2, r1
	mov	r1, r5
	mul	r3, r7, r3
	add	r0, r0, r2, lsl #1
	mov	r2, r3
	str	r3, [fp, #-76]
	bl	adsp_memcpy
	ldr	r1, [r9, #68]
	ldr	ip, [fp, #-60]
	mov	r0, r4
	add	r7, r7, r1
	ldr	r2, [r9, #4]
	ldr	r3, [fp, #-76]
	mul	r7, ip, r7
	add	r3, r5, r3
	str	r5, [sp]
	mov	r1, ip
	add	r2, r2, r7, lsl #1
	bl	overlap_add
	ldr	r0, [r9, #68]
	ldr	r3, [fp, #-68]
	add	r0, r0, r3
	str	r0, [r9, #68]
	b	.L157
.L226:
	ldr	r2, [r9, #48]
	ldr	r0, [r9, #4]
	ldr	r1, [r9]
	mul	r8, r2, r8
	lsl	r2, r2, #1
	mul	r2, r6, r2
	add	r0, r0, r8, lsl #1
	bl	adsp_memcpy
	ldr	r3, [r9, #68]
	mov	r2, #0
	ldr	r7, [r9, #40]
	add	r6, r3, r6
	str	r2, [r9, #64]
	str	r6, [r9, #68]
	b	.L154
.L223:
	bl	adsp_memset
	b	.L151
.L159:
	rsb	r0, r8, #512
	mov	r4, r7
	mul	r0, r7, r0
	bl	__aeabi_idiv
	ldr	r3, [fp, #-60]
	str	r0, [r9, #88]
	b	.L160
.L162:
	ldr	r3, [fp, #-72]
	mov	r4, r0
	mul	r0, r3, r0
	bl	__aeabi_idiv
	str	r0, [r9, #88]
	b	.L163
.L225:
	mov	lr, r10
.L192:
	cmp	r2, r8
	strne	r2, [r9, #32]
	beq	.L230
.L190:
	ldr	r2, [fp, #-56]
	ldr	r3, [r9, #72]
	add	r2, r2, #1
	str	r2, [fp, #-56]
	sub	r3, r3, #1
	cmp	r2, r3
	blt	.L183
.L182:
	ldr	r1, [fp, #-56]
	mov	r0, r9
	bl	remove_pitch_samples
	mov	r7, #1
	b	.L146
.L227:
	mov	r9, r0
.L170:
	ldr	r1, [fp, #-60]
	mov	r0, r9
	bl	remove_pitch_samples
	mov	r7, #1
	b	.L146
.L230:
	cmp	lr, r4
	mov	r3, #0
	str	r3, [r9, #32]
	bne	.L198
	str	r3, [r9, #36]
	b	.L190
.L196:
	mov	r3, #0
	str	r3, [fp, #-60]
	b	.L170
	UNWIND(.fnend)
	.size	sonic_write_short_to_stream, .-sonic_write_short_to_stream
	.align	2
	.global	sonic_get_left_bytes
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_left_bytes, %function
sonic_get_left_bytes:
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
	ldr	r3, [r0, #64]
	ldr	r2, [r0, #68]
	ldr	r0, [r0, #48]
	add	r3, r3, r2
	mul	r0, r0, r3
	lsl	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_left_bytes, .-sonic_get_left_bytes
	.align	2
	.global	sonic_get_in_left_bytes
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_in_left_bytes, %function
sonic_get_in_left_bytes:
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
	ldr	r3, [r0, #64]
	ldr	r0, [r0, #48]
	mul	r0, r0, r3
	lsl	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_in_left_bytes, .-sonic_get_in_left_bytes
	.align	2
	.global	sonic_get_out_left_bytes
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	sonic_get_out_left_bytes, %function
sonic_get_out_left_bytes:
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
	ldr	r3, [r0, #68]
	ldr	r0, [r0, #48]
	mul	r0, r0, r3
	lsl	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	sonic_get_out_left_bytes, .-sonic_get_out_left_bytes
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
