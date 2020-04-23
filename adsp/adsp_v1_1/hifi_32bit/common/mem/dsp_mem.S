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
	.file	"dsp_mem.c"
	.text
	.align	2
	.global	HI_ADSP_MALLOC
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	HI_ADSP_MALLOC, %function
HI_ADSP_MALLOC:
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
	bl	adsp_kmalloc
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	HI_ADSP_MALLOC, .-HI_ADSP_MALLOC
	.align	2
	.global	HI_ADSP_FREE
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	HI_ADSP_FREE, %function
HI_ADSP_FREE:
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
	cmp	r1, #0
	ldmfdeq	sp, {fp, sp, pc}
	bl	adsp_kfree
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	HI_ADSP_FREE, .-HI_ADSP_FREE
	.align	2
	.global	HI_ADSP_V_MALLOC
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	HI_ADSP_V_MALLOC, %function
HI_ADSP_V_MALLOC:
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
	bl	adsp_vmalloc
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	HI_ADSP_V_MALLOC, .-HI_ADSP_V_MALLOC
	.align	2
	.global	HI_ADSP_V_FREE
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	HI_ADSP_V_FREE, %function
HI_ADSP_V_FREE:
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
	cmp	r1, #0
	ldmfdeq	sp, {fp, sp, pc}
	bl	adsp_vfree
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	HI_ADSP_V_FREE, .-HI_ADSP_V_FREE
	.global	__aeabi_uidivmod
	.align	2
	.global	hi_adsp_malloc_aligned
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	hi_adsp_malloc_aligned, %function
hi_adsp_malloc_aligned:
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
	sub	r3, r2, #2
	cmp	r3, #254
	mov	r4, r2
	mov	r7, r0
	mov	r6, r1
	bhi	.L19
	cmp	r2, #1
	bls	.L21
	mov	r0, #0
.L18:
	add	r0, r0, #1
	lsr	r3, r4, r0
	cmp	r3, #1
	bhi	.L18
	mvn	r5, #0
	lsl	r5, r5, r0
	bics	r3, r4, r5
	bne	.L19
.L17:
	cmp	r4, r6
	bcs	.L20
	mov	r1, r4
	mov	r0, r6
	bl	__aeabi_uidivmod
	cmp	r1, #0
	bne	.L19
.L20:
	add	r1, r4, r6
	mov	r0, r7
	bl	adsp_kmalloc
	subs	r3, r0, #0
	beq	.L19
	add	r4, r3, r4
	and	r0, r4, r5
	sub	r3, r0, r3
	strb	r3, [r0, #-1]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L19:
	mov	r0, #0
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L21:
	mvn	r5, #0
	b	.L17
	UNWIND(.fnend)
	.size	hi_adsp_malloc_aligned, .-hi_adsp_malloc_aligned
	.align	2
	.global	hi_adsp_free_aligned
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	hi_adsp_free_aligned, %function
hi_adsp_free_aligned:
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
	cmp	r1, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldrb	r3, [r1, #-1]
	subs	r1, r1, r3
	ldmfdeq	sp, {fp, sp, pc}
	bl	adsp_kfree
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	hi_adsp_free_aligned, .-hi_adsp_free_aligned
	.align	2
	.global	hi_adsp_vmalloc_aligned
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	hi_adsp_vmalloc_aligned, %function
hi_adsp_vmalloc_aligned:
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
	sub	r3, r2, #2
	cmp	r3, #254
	mov	r4, r2
	mov	r7, r0
	mov	r6, r1
	bhi	.L39
	cmp	r2, #1
	bls	.L41
	mov	r0, #0
.L38:
	add	r0, r0, #1
	lsr	r3, r4, r0
	cmp	r3, #1
	bhi	.L38
	mvn	r5, #0
	lsl	r5, r5, r0
	bics	r3, r4, r5
	bne	.L39
.L37:
	cmp	r4, r6
	bcs	.L40
	mov	r1, r4
	mov	r0, r6
	bl	__aeabi_uidivmod
	cmp	r1, #0
	bne	.L39
.L40:
	add	r1, r4, r6
	mov	r0, r7
	bl	adsp_vmalloc
	subs	r3, r0, #0
	beq	.L39
	add	r4, r3, r4
	and	r0, r4, r5
	sub	r3, r0, r3
	strb	r3, [r0, #-1]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L39:
	mov	r0, #0
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L41:
	mvn	r5, #0
	b	.L37
	UNWIND(.fnend)
	.size	hi_adsp_vmalloc_aligned, .-hi_adsp_vmalloc_aligned
	.align	2
	.global	hi_adsp_vfree_aligned
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	hi_adsp_vfree_aligned, %function
hi_adsp_vfree_aligned:
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
	cmp	r1, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldrb	r3, [r1, #-1]
	subs	r1, r1, r3
	ldmfdeq	sp, {fp, sp, pc}
	bl	adsp_vfree
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	hi_adsp_vfree_aligned, .-hi_adsp_vfree_aligned
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
