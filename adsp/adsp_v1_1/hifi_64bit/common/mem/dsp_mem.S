#include "hi_asm_define.h"
	.arch armv8-a
	.file	"dsp_mem.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	HI_ADSP_MALLOC
	.type	HI_ADSP_MALLOC, %function
HI_ADSP_MALLOC:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	w1, w20
	mov	w0, w19
	bl	adsp_kmalloc
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	HI_ADSP_MALLOC, .-HI_ADSP_MALLOC
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	HI_ADSP_FREE
	.type	HI_ADSP_FREE, %function
HI_ADSP_FREE:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L3
	mov	x1, x19
	mov	w0, w20
	bl	adsp_kfree
.L3:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	HI_ADSP_FREE, .-HI_ADSP_FREE
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	HI_ADSP_V_MALLOC
	.type	HI_ADSP_V_MALLOC, %function
HI_ADSP_V_MALLOC:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	w1, w20
	mov	w0, w19
	bl	adsp_vmalloc
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	HI_ADSP_V_MALLOC, .-HI_ADSP_V_MALLOC
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	HI_ADSP_V_FREE
	.type	HI_ADSP_V_FREE, %function
HI_ADSP_V_FREE:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L9
	mov	x1, x19
	mov	w0, w20
	bl	adsp_vfree
.L9:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	HI_ADSP_V_FREE, .-HI_ADSP_V_FREE
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	hi_adsp_malloc_aligned
	.type	hi_adsp_malloc_aligned, %function
hi_adsp_malloc_aligned:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x22, x23, [sp, 40]
	str	x19, [sp, 16]
	mov	w19, w2
	mov	w23, w0
	mov	w22, w1
	mov	x0, x30
	bl	_mcount
	sub	w0, w19, #2
	cmp	w0, 254
	bhi	.L19
	stp	x20, x21, [x29, 24]
	cmp	w19, 1
	uxtw	x20, w19
	bls	.L21
	mov	w0, 0
	.p2align 3
.L18:
	add	w0, w0, 1
	lsr	w2, w19, w0
	cmp	w2, 1
	bhi	.L18
	mov	x21, -1
	lsl	x21, x21, x0
	bics	xzr, x20, x21
	bne	.L27
.L17:
	cmp	w19, w22
	bcs	.L20
	udiv	w0, w22, w19
	msub	w0, w0, w19, w22
	cbnz	w0, .L27
.L20:
	add	w1, w19, w22
	mov	w0, w23
	bl	adsp_kmalloc
	mov	x1, x0
	cbz	x0, .L27
	add	x20, x0, x20
	and	x0, x20, x21
	sub	x1, x0, x1
	strb	w1, [x0, -1]
	ldp	x20, x21, [x29, 24]
	ldr	x19, [sp, 16]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L27:
	ldp	x20, x21, [x29, 24]
.L19:
	mov	x0, 0
	ldr	x19, [sp, 16]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L21:
	mov	x21, -1
	b	.L17
	.size	hi_adsp_malloc_aligned, .-hi_adsp_malloc_aligned
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	hi_adsp_free_aligned
	.type	hi_adsp_free_aligned, %function
hi_adsp_free_aligned:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L28
	ldrb	w1, [x19, -1]
	subs	x1, x19, x1
	beq	.L28
	mov	w0, w20
	bl	adsp_kfree
.L28:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	hi_adsp_free_aligned, .-hi_adsp_free_aligned
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	hi_adsp_vmalloc_aligned
	.type	hi_adsp_vmalloc_aligned, %function
hi_adsp_vmalloc_aligned:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x22, x23, [sp, 40]
	str	x19, [sp, 16]
	mov	w19, w2
	mov	w23, w0
	mov	w22, w1
	mov	x0, x30
	bl	_mcount
	sub	w0, w19, #2
	cmp	w0, 254
	bhi	.L41
	stp	x20, x21, [x29, 24]
	cmp	w19, 1
	uxtw	x20, w19
	bls	.L43
	mov	w0, 0
	.p2align 3
.L40:
	add	w0, w0, 1
	lsr	w2, w19, w0
	cmp	w2, 1
	bhi	.L40
	mov	x21, -1
	lsl	x21, x21, x0
	bics	xzr, x20, x21
	bne	.L49
.L39:
	cmp	w19, w22
	bcs	.L42
	udiv	w0, w22, w19
	msub	w0, w0, w19, w22
	cbnz	w0, .L49
.L42:
	add	w1, w19, w22
	mov	w0, w23
	bl	adsp_vmalloc
	mov	x1, x0
	cbz	x0, .L49
	add	x20, x0, x20
	and	x0, x20, x21
	sub	x1, x0, x1
	strb	w1, [x0, -1]
	ldp	x20, x21, [x29, 24]
	ldr	x19, [sp, 16]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L49:
	ldp	x20, x21, [x29, 24]
.L41:
	mov	x0, 0
	ldr	x19, [sp, 16]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L43:
	mov	x21, -1
	b	.L39
	.size	hi_adsp_vmalloc_aligned, .-hi_adsp_vmalloc_aligned
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	hi_adsp_vfree_aligned
	.type	hi_adsp_vfree_aligned, %function
hi_adsp_vfree_aligned:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L50
	ldrb	w1, [x19, -1]
	subs	x1, x19, x1
	beq	.L50
	mov	w0, w20
	bl	adsp_vfree
.L50:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	hi_adsp_vfree_aligned, .-hi_adsp_vfree_aligned
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
