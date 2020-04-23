#include "hi_asm_define.h"
	.arch armv8-a
	.file	"aoe_cast.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	cast_timer_handle, %function
cast_timer_handle:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	adrp	x21, .LANCHOR0
	add	x21, x21, :lo12:.LANCHOR0
	mov	w20, 0
	mov	x0, x30
	bl	_mcount
.L10:
	ldr	x19, [x21, 56]
	cbz	x19, .L2
	mov	w0, w20
	bl	adsp_spin_lock
	ldr	x0, [x19, 40]
	ldr	w22, [x19, 32]
	bl	rbuf_query_busy
	mov	w23, w0
	ldr	x0, [x19, 48]
	bl	rbuf_query_free
	cmp	w22, w23
	bhi	.L3
	cmp	w22, w0
	ldr	x4, [x19, 40]
	bhi	.L4
	ldr	x1, [x19, 48]
	mov	w2, w22
	mov	w3, 1
.L15:
	mov	x0, x4
	bl	rbuf_copy_data
	mov	x0, x19
	ldr	w2, [x19, 56]
	add	w2, w2, 1
	str	w2, [x0, 56]!
.L5:
	ldr	w1, [x19, 68]
	add	w1, w1, 1
	str	w1, [x19, 68]
	ldr	x1, [x21, 24]
	str	w2, [x1, 28]
	ldr	w2, [x0, 4]
	str	w2, [x1, 32]
	ldr	w2, [x0, 8]
	str	w2, [x1, 36]
	ldr	w2, [x0, 12]
	str	w2, [x1, 40]
	ldr	w2, [x0, 16]
	str	w2, [x1, 24]
	ldr	w2, [x0, 20]
	str	w2, [x1, 16]
	ldr	w0, [x0, 24]
	str	w0, [x1, 20]
.L6:
	mov	w0, w20
	bl	adsp_spin_unlock
.L2:
	add	w20, w20, 1
	add	x21, x21, 8
	cmp	w20, 4
	bne	.L10
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L3:
	cbnz	w23, .L7
	cmp	w22, w0
	bls	.L19
	ldr	w0, [x19, 64]
	add	w0, w0, 1
	str	w0, [x19, 64]
	b	.L6
	.p2align 2
.L7:
	cmp	w23, w0
	ldr	x4, [x19, 40]
	bhi	.L9
	mov	w3, 1
	mov	w2, w23
	ldr	x1, [x19, 48]
	b	.L15
	.p2align 2
.L4:
	mov	w2, w22
.L16:
	mov	x1, 0
	mov	x0, x4
	bl	rbuf_read_data
	ldr	w0, [x19, 64]
	add	w0, w0, 1
	str	w0, [x19, 64]
	b	.L6
	.p2align 2
.L9:
	mov	w2, w23
	b	.L16
	.p2align 2
.L19:
	ldr	x0, [x19, 48]
	mov	w2, w22
	mov	x1, 0
	bl	rbuf_write_data_dma
	ldp	w2, w1, [x19, 56]
	add	x0, x19, 56
	add	w1, w1, 1
	str	w1, [x19, 60]
	b	.L5
	.size	cast_timer_handle, .-cast_timer_handle
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	cast_set_aop_buf
	.type	cast_set_aop_buf, %function
cast_set_aop_buf:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	x20, x1
	mov	x0, x30
	bl	_mcount
	cbz	x20, .L20
	adrp	x0, .LANCHOR0
	add	x2, x0, :lo12:.LANCHOR0
	ldr	w1, [x2, 4]
	cmp	w19, w1
	beq	.L24
	ldr	w1, [x2, 8]
	cmp	w19, w1
	beq	.L25
	ldr	w1, [x2, 12]
	cmp	w19, w1
	beq	.L26
	ldr	w1, [x2, 16]
	cmp	w19, w1
	beq	.L31
.L20:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L24:
	mov	w1, 0
.L22:
	ubfiz	x1, x1, 3, 2
	add	x0, x0, :lo12:.LANCHOR0
	add	x0, x0, x1
	ldr	x0, [x0, 56]
	str	x20, [x0, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L25:
	mov	w1, 1
	b	.L22
	.p2align 2
.L31:
	mov	w1, 3
	b	.L22
	.p2align 2
.L26:
	mov	w1, 2
	b	.L22
	.size	cast_set_aop_buf, .-cast_set_aop_buf
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	cast_destroy
	.type	cast_destroy, %function
cast_destroy:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x20, .LANCHOR0
	add	x0, x20, :lo12:.LANCHOR0
	ldr	w1, [x0, 4]
	cmp	w19, w1
	beq	.L36
	ldr	w1, [x0, 8]
	cmp	w19, w1
	beq	.L37
	ldr	w1, [x0, 12]
	cmp	w19, w1
	beq	.L38
	ldr	w0, [x0, 16]
	cmp	w19, w0
	beq	.L44
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L36:
	stp	x21, x22, [x29, 32]
	mov	w22, 0
.L33:
	ubfiz	x0, x22, 3, 2
	add	x19, x20, :lo12:.LANCHOR0
	add	x0, x0, 48
	add	x19, x19, x0
	ldr	x21, [x19, 8]
	cbz	x21, .L43
	str	x23, [x29, 48]
	mov	w0, w22
	ldr	x23, [x21, 48]
	bl	adsp_spin_lock
	mov	x1, x21
	mov	w0, 70
	bl	HI_ADSP_FREE
	str	xzr, [x19, 8]
	mov	w0, w22
	bl	adsp_spin_unlock
	mov	w0, w22
	bl	adsp_spin_deinit
	mov	x0, x23
	bl	rbuf_destroy
	ldr	w0, [x20, #:lo12:.LANCHOR0]
	ldp	x21, x22, [x29, 32]
	sub	w0, w0, #1
	ldr	x23, [x29, 48]
	str	w0, [x20, #:lo12:.LANCHOR0]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L43:
	ldp	x21, x22, [x29, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L37:
	stp	x21, x22, [x29, 32]
	mov	w22, 1
	b	.L33
	.p2align 2
.L38:
	stp	x21, x22, [x29, 32]
	mov	w22, 2
	b	.L33
	.p2align 2
.L44:
	stp	x21, x22, [x29, 32]
	mov	w22, 3
	b	.L33
	.size	cast_destroy, .-cast_destroy
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	cast_create
	.type	cast_create, %function
cast_create:
	stp	x29, x30, [sp, -112]!
	add	x29, sp, 0
	stp	x24, x25, [sp, 56]
	str	x20, [sp, 24]
	str	x22, [sp, 40]
	stp	x26, x27, [sp, 72]
	str	x28, [sp, 88]
	mov	x20, x1
	mov	w24, w0
	mov	w25, w2
	mov	x0, x30
	mov	w27, w3
	mov	w28, w4
	bl	_mcount
	adrp	x22, .LANCHOR0
	add	x0, x22, :lo12:.LANCHOR0
	str	xzr, [x29, 104]
	ldr	x1, [x0, 56]
	cbz	x1, .L53
	ldr	x1, [x0, 64]
	cbz	x1, .L54
	ldr	x1, [x0, 72]
	cbz	x1, .L55
	ldr	x0, [x0, 80]
	cbz	x0, .L56
.L49:
	mov	x20, 0
.L45:
	mov	x0, x20
	ldr	x20, [sp, 24]
	ldr	x22, [sp, 40]
	ldp	x24, x25, [sp, 56]
	ldp	x26, x27, [sp, 72]
	ldr	x28, [sp, 88]
	ldp	x29, x30, [sp], 112
	ret
	.p2align 2
.L53:
	mov	w26, 0
	str	x19, [x29, 16]
.L46:
	mov	w1, 88
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x19, x0
	cbz	x0, .L64
	str	x21, [x29, 32]
	uxtw	x21, w26
	mov	x2, 88
	mov	w1, 0
	str	x23, [x29, 48]
	bl	adsp_memset
	add	x23, x22, :lo12:.LANCHOR0
	stp	w25, w27, [x19, 12]
	add	x0, x23, x21, lsl 3
	str	w28, [x19, 20]
	mov	w25, 19923
	movk	w25, 0x1062, lsl 16
	ldr	x27, [x0, 24]
	str	wzr, [x20]
	ldp	w1, w0, [x19, 12]
	bl	util_calc_frame_size
	ldr	w1, [x19, 20]
	add	x2, x27, 12
	add	x3, x27, 8
	stp	x3, x2, [x20, 24]
	add	w1, w1, w1, lsl 6
	umull	x1, w1, w25
	lsr	x1, x1, 38
	and	w2, w1, -4
	tst	x1, 3
	add	w2, w2, 4
	csel	w1, w2, w1, ne
	mul	w0, w0, w1
	str	w0, [x20, 16]
	ldp	w1, w0, [x19, 12]
	bl	util_calc_frame_size
	str	w0, [x20, 44]
	ldr	w2, [x20, 16]
	mov	x1, x20
	str	w2, [x27, 4]
	add	x0, x29, 104
	str	w24, [x27]
	bl	rbuf_create
	ldr	x20, [x29, 104]
	cbz	x20, .L51
	add	x1, x23, x21, lsl 2
	ldr	w0, [x22, #:lo12:.LANCHOR0]
	str	x20, [x19, 40]
	add	x21, x23, x21, lsl 3
	add	w0, w0, 1
	str	w0, [x22, #:lo12:.LANCHOR0]
	str	w24, [x1, 4]
	ldp	w1, w0, [x19, 12]
	bl	util_calc_frame_size
	ldr	w1, [x19, 20]
	add	w1, w1, w1, lsl 2
	umull	x25, w1, w25
	lsr	x25, x25, 38
	and	w1, w25, -4
	tst	x25, 3
	add	w1, w1, 4
	csel	w25, w1, w25, ne
	mul	w25, w0, w25
	mov	w0, w26
	str	w25, [x19, 32]
	bl	adsp_spin_init
	mov	x2, 28
	mov	w1, 0
	add	x0, x19, 56
	bl	adsp_memset
	str	x19, [x21, 56]
	ldr	x20, [x29, 104]
	ldr	x19, [x29, 16]
	ldr	x21, [x29, 32]
	mov	x0, x20
	ldr	x23, [x29, 48]
	ldr	x20, [sp, 24]
	ldr	x22, [sp, 40]
	ldp	x24, x25, [sp, 56]
	ldp	x26, x27, [sp, 72]
	ldr	x28, [sp, 88]
	ldp	x29, x30, [sp], 112
	ret
	.p2align 2
.L64:
	ldr	x19, [x29, 16]
	b	.L49
	.p2align 2
.L54:
	mov	w26, 1
	str	x19, [x29, 16]
	b	.L46
	.p2align 2
.L55:
	mov	w26, 2
	str	x19, [x29, 16]
	b	.L46
	.p2align 2
.L56:
	mov	w26, 3
	str	x19, [x29, 16]
	b	.L46
	.p2align 2
.L51:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
	ldr	x19, [x29, 16]
	ldr	x21, [x29, 32]
	ldr	x23, [x29, 48]
	b	.L45
	.size	cast_create, .-cast_create
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	cast_fifo_try_cnt
	.type	cast_fifo_try_cnt, %function
cast_fifo_try_cnt:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x1, x0, :lo12:.LANCHOR0
	ldr	w2, [x1, 4]
	cmp	w19, w2
	beq	.L69
	ldr	w2, [x1, 8]
	cmp	w19, w2
	beq	.L70
	ldr	w2, [x1, 12]
	cmp	w19, w2
	beq	.L71
	ldr	w1, [x1, 16]
	cmp	w19, w1
	beq	.L76
.L65:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L69:
	mov	w1, 0
.L66:
	add	x0, x0, :lo12:.LANCHOR0
	ubfiz	x1, x1, 3, 2
	add	x0, x0, x1
	ldr	x0, [x0, 56]
	cbz	x0, .L65
	ldr	w1, [x0, 76]
	add	w1, w1, 1
	str	w1, [x0, 76]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L70:
	mov	w1, 1
	b	.L66
	.p2align 2
.L71:
	mov	w1, 2
	b	.L66
	.p2align 2
.L76:
	mov	w1, 3
	b	.L66
	.size	cast_fifo_try_cnt, .-cast_fifo_try_cnt
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	cast_fifo_free_cnt
	.type	cast_fifo_free_cnt, %function
cast_fifo_free_cnt:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x1, x0, :lo12:.LANCHOR0
	ldr	w2, [x1, 4]
	cmp	w19, w2
	beq	.L81
	ldr	w2, [x1, 8]
	cmp	w19, w2
	beq	.L82
	ldr	w2, [x1, 12]
	cmp	w19, w2
	beq	.L83
	ldr	w1, [x1, 16]
	cmp	w19, w1
	beq	.L88
.L77:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L81:
	mov	w1, 0
.L78:
	add	x0, x0, :lo12:.LANCHOR0
	ubfiz	x1, x1, 3, 2
	add	x0, x0, x1
	ldr	x0, [x0, 56]
	cbz	x0, .L77
	ldr	w1, [x0, 80]
	add	w1, w1, 1
	str	w1, [x0, 80]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L82:
	mov	w1, 1
	b	.L78
	.p2align 2
.L83:
	mov	w1, 2
	b	.L78
	.p2align 2
.L88:
	mov	w1, 3
	b	.L78
	.size	cast_fifo_free_cnt, .-cast_fifo_free_cnt
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	cast_init
	.type	cast_init, %function
cast_init:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	add	x19, x19, 1024
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	mov	x20, x0
	mov	x21, 1
	mov	x23, x0
	mov	w22, 8
	mov	x2, 104
	mov	w1, 0
	bl	adsp_memset
.L90:
	str	x19, [x20, 24]
	mov	x0, x19
	mov	x2, 48
	mov	w1, 0
	bl	adsp_memset
	str	w22, [x19], 48
	str	xzr, [x20, 56]
	add	x20, x20, 8
	str	w22, [x23, x21, lsl 2]
	add	x21, x21, 1
	cmp	x21, 5
	bne	.L90
	adrp	x0, cast_timer_handle
	add	x0, x0, :lo12:cast_timer_handle
	bl	timer_reg_isr_func
	ldr	x23, [sp, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.size	cast_init, .-cast_init
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	cast_deinit
	.type	cast_deinit, %function
cast_deinit:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	bl	timer_del_isr_func
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	stp	xzr, xzr, [x0, 56]
	stp	xzr, xzr, [x0, 72]
	ldp	x29, x30, [sp], 16
	ret
	.size	cast_deinit, .-cast_deinit
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_cast_rm, %object
	.size	g_cast_rm, 104
g_cast_rm:
	.zero	104
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
