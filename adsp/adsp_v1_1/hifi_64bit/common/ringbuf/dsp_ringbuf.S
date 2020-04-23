#include "hi_asm_define.h"
	.arch armv8-a
	.file	"dsp_ringbuf.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_create
	.type	rbuf_create, %function
rbuf_create:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x20, x1
	mov	x21, x0
	mov	x0, x30
	bl	_mcount
	mov	w1, 88
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x19, x0
	cbz	x0, .L7
	mov	x2, 88
	mov	w1, 0
	bl	adsp_memset
	mov	x2, 56
	mov	x1, x20
	mov	x0, x19
	bl	adsp_memcpy
	ldr	w0, [x20]
	cbz	w0, .L4
	ldr	w3, [x19, 16]
	ldr	x0, [x19, 8]
.L5:
	ldp	x2, x4, [x19, 24]
	stp	x2, x4, [x19, 56]
	str	x0, [x19, 72]
	str	w3, [x19, 80]
	str	wzr, [x2]
	ldr	x0, [x19, 64]
	str	wzr, [x0]
	str	x19, [x21]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L9:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L7:
	str	xzr, [x21]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L4:
	ldr	w1, [x20, 16]
	mov	w0, 70
	ldr	w2, [x20, 44]
	bl	hi_adsp_vmalloc_aligned
	cbz	x0, .L9
	str	x0, [x19, 8]
	ldr	w3, [x20, 16]
	str	w3, [x19, 16]
	b	.L5
	.size	rbuf_create, .-rbuf_create
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_create_ext
	.type	rbuf_create_ext, %function
rbuf_create_ext:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x20, x1
	mov	x21, x0
	mov	x0, x30
	bl	_mcount
	mov	w1, 88
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x19, x0
	cbz	x0, .L16
	mov	x2, 88
	mov	w1, 0
	bl	adsp_memset
	mov	x2, 56
	mov	x1, x20
	mov	x0, x19
	bl	adsp_memcpy
	ldr	w0, [x20]
	cbz	w0, .L13
	ldr	w2, [x19, 16]
	ldr	x0, [x19, 8]
.L14:
	ldp	x4, x3, [x19, 24]
	stp	x4, x3, [x19, 56]
	str	x0, [x19, 72]
	str	w2, [x19, 80]
	str	x19, [x21]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L17:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L16:
	str	xzr, [x21]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L13:
	ldr	w1, [x20, 16]
	mov	w0, 70
	ldr	w2, [x20, 44]
	bl	hi_adsp_vmalloc_aligned
	cbz	x0, .L17
	str	x0, [x19, 8]
	ldr	w2, [x20, 16]
	str	w2, [x19, 16]
	b	.L14
	.size	rbuf_create_ext, .-rbuf_create_ext
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_destroy
	.type	rbuf_destroy, %function
rbuf_destroy:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L18
	ldr	w0, [x19]
	cbz	w0, .L24
.L20:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L18:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L24:
	ldr	x1, [x19, 8]
	mov	w0, 70
	bl	hi_adsp_vfree_aligned
	b	.L20
	.size	rbuf_destroy, .-rbuf_destroy
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_read_data
	.type	rbuf_read_data, %function
rbuf_read_data:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	str	x20, [sp, 24]
	stp	x22, x23, [sp, 40]
	mov	x23, x0
	mov	x22, x1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	cbz	x23, .L31
	str	x19, [x29, 16]
	str	x24, [x29, 56]
	ldp	x1, x3, [x23, 56]
	ldr	w0, [x23, 80]
	ldr	w19, [x3]
	ldr	w1, [x1]
	add	w2, w1, w0
	cmp	w19, w1
	sub	w2, w2, w19
	sub	w1, w1, w19
	csel	w2, w2, w1, hi
	cmp	w20, w2
	csel	w20, w20, w2, ls
	cbz	x22, .L29
	str	x21, [x29, 32]
	sub	w21, w0, w19
	str	x25, [x29, 64]
	cmp	w21, w20
	csel	w21, w21, w20, ls
	mov	x0, x22
	ldr	x1, [x23, 72]
	uxtw	x25, w21
	mov	x2, x25
	add	x1, x1, x19, uxtw
	bl	adsp_memcpy
	ldr	x1, [x23, 72]
	sub	w2, w20, w21
	add	x0, x22, x25
	bl	adsp_memcpy
	ldr	x21, [x29, 32]
	ldr	w0, [x23, 80]
	ldr	x3, [x23, 64]
	ldr	x25, [x29, 64]
.L29:
	add	w19, w19, w20
	sub	w1, w19, w0
	cmp	w0, w19
	csel	w19, w1, w19, ls
	str	w19, [x3]
	mov	w0, w20
	ldr	x19, [x29, 16]
	ldr	x24, [x29, 56]
	ldr	x20, [sp, 24]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L31:
	mov	w20, 0
	mov	w0, w20
	ldr	x20, [sp, 24]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 80
	ret
	.size	rbuf_read_data, .-rbuf_read_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_write_data
	.type	rbuf_write_data, %function
rbuf_write_data:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x23, [sp, 48]
	mov	x19, x0
	mov	x23, x1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L36
	str	x21, [x29, 32]
	ldr	x0, [x19, 56]
	ldr	w21, [x0]
	cbnz	w20, .L47
	ldr	x21, [x29, 32]
.L36:
	mov	w0, w20
	ldr	x23, [sp, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L47:
	str	x22, [x29, 40]
	str	x24, [x29, 56]
	ldr	w2, [x19, 80]
	ldr	x0, [x19, 72]
	sub	w2, w2, w21
	cmp	w2, w20
	csel	w2, w2, w20, ls
	add	x0, x0, x21, uxtw
	sub	w24, w20, w2
	uxtw	x22, w2
	cbz	x23, .L48
	mov	x1, x23
	mov	x2, x22
	bl	adsp_memcpy
	ldr	x0, [x19, 72]
	mov	x2, x24
	add	x1, x23, x22
	bl	adsp_memcpy
.L38:
	ldr	w1, [x19, 80]
	add	w0, w20, w21
	ldr	x2, [x19, 56]
	sub	w3, w0, w1
	cmp	w1, w0
	csel	w0, w3, w0, ls
	str	w0, [x2]
	mov	w0, w20
	ldp	x21, x22, [x29, 32]
	ldr	x24, [x29, 56]
	ldp	x19, x20, [sp, 16]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L48:
	mov	x2, x22
	mov	w1, 0
	bl	adsp_memset
	ldr	x0, [x19, 72]
	mov	x2, x24
	mov	w1, 0
	bl	adsp_memset
	b	.L38
	.size	rbuf_write_data, .-rbuf_write_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_read_data_complete
	.type	rbuf_read_data_complete, %function
rbuf_read_data_complete:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	cbz	x20, .L49
	ldr	x0, [x20, 64]
	ldr	w2, [x20, 80]
	ldr	w1, [x0]
	add	w1, w19, w1
	sub	w3, w1, w2
	cmp	w2, w1
	csel	w1, w3, w1, ls
	str	w1, [x0]
.L49:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_read_data_complete, .-rbuf_read_data_complete
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_write_data_complete
	.type	rbuf_write_data_complete, %function
rbuf_write_data_complete:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	cbz	x20, .L55
	ldr	x0, [x20, 56]
	ldr	w2, [x20, 80]
	ldr	w1, [x0]
	add	w1, w19, w1
	sub	w3, w1, w2
	cmp	w2, w1
	csel	w1, w3, w1, ls
	str	w1, [x0]
.L55:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_write_data_complete, .-rbuf_write_data_complete
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_query_info
	.type	rbuf_query_info, %function
rbuf_query_info:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	cbz	x20, .L66
	ldr	w0, [x20, 48]
	str	w0, [x19, 28]
	ldr	x0, [x20, 72]
	str	x0, [x19]
	ldr	w1, [x20, 80]
	str	w1, [x19, 16]
	ldr	x0, [x20, 56]
	ldr	w3, [x0]
	str	w3, [x19, 12]
	add	w0, w1, w3
	ldr	x2, [x20, 64]
	ldr	w2, [x2]
	str	w2, [x19, 8]
	sub	w4, w3, w2
	sub	w0, w0, w2
	cmp	w3, w2
	csel	w0, w0, w4, cc
	str	w0, [x19, 20]
	sub	w0, w1, w0
	str	w0, [x19, 24]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L66:
	mov	x2, 32
	mov	w1, 0
	mov	x0, x19
	bl	adsp_memset
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_query_info, .-rbuf_query_info
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_query_busy_provide_rptr
	.type	rbuf_query_busy_provide_rptr, %function
rbuf_query_busy_provide_rptr:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	x20, x1
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	cbz	x19, .L67
	ldr	x1, [x19, 56]
	ldr	w2, [x20]
	ldr	w0, [x19, 80]
	ldr	w1, [x1]
	sub	w0, w0, w2
	sub	w3, w1, w2
	add	w0, w0, w1
	cmp	w2, w1
	csel	w0, w0, w3, hi
.L67:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_query_busy_provide_rptr, .-rbuf_query_busy_provide_rptr
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_query_busy
	.type	rbuf_query_busy, %function
rbuf_query_busy:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	cbz	x19, .L71
	ldp	x2, x1, [x19, 56]
	ldr	w0, [x19, 80]
	ldr	w2, [x2]
	ldr	w1, [x1]
	add	w0, w2, w0
	sub	w3, w2, w1
	sub	w0, w0, w1
	cmp	w2, w1
	csel	w0, w0, w3, cc
.L71:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_query_busy, .-rbuf_query_busy
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_query_free
	.type	rbuf_query_free, %function
rbuf_query_free:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	cbz	x19, .L75
	ldp	x2, x3, [x19, 56]
	ldr	w1, [x19, 80]
	ldr	w4, [x19, 44]
	ldr	w2, [x2]
	ldr	w3, [x3]
	add	w0, w2, w1
	cmp	w2, w3
	sub	w0, w0, w3
	sub	w2, w2, w3
	csel	w0, w0, w2, cc
	sub	w1, w1, w0
	sub	w0, w1, w4
	cmp	w4, w1
	csel	w0, w0, wzr, cc
.L75:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_query_free, .-rbuf_query_free
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_check_free
	.type	rbuf_check_free, %function
rbuf_check_free:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	cbz	x19, .L81
	ldp	x3, x2, [x19, 56]
	mov	w0, 0
	ldr	w4, [x19, 80]
	ldr	w1, [x19, 48]
	ldr	w3, [x3]
	ldr	w5, [x2]
	add	w2, w3, w4
	cmp	w3, w5
	sub	w2, w2, w5
	sub	w3, w3, w5
	csel	w2, w2, w3, cc
	add	w3, w2, w20
	sub	w2, w4, w2
	cmp	w3, w1
	bhi	.L81
	cmp	w20, w2
	cset	w0, cc
.L81:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_check_free, .-rbuf_check_free
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_flush
	.type	rbuf_flush, %function
rbuf_flush:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L87
	ldp	x1, x0, [x19, 56]
	ldr	w1, [x1]
	str	w1, [x0]
.L87:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_flush, .-rbuf_flush
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_read_data_dma
	.type	rbuf_read_data_dma, %function
rbuf_read_data_dma:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	stp	x20, x21, [sp, 24]
	stp	x22, x23, [sp, 40]
	mov	w22, 0
	mov	x23, x0
	mov	x21, x1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	cbz	x23, .L92
	str	x24, [x29, 56]
	add	x24, x23, 56
	str	x19, [x29, 16]
	mov	w22, 0
	ldr	x0, [x23, 56]
	ldr	x2, [x24, 8]
	ldr	w19, [x2]
	ldr	w0, [x0]
	cbz	w20, .L109
	ldr	w1, [x24, 24]
	cmp	w19, w0
	sub	w3, w0, w19
	add	w0, w0, w1
	sub	w0, w0, w19
	csel	w0, w0, w3, hi
	cmp	w20, w0
	csel	w22, w20, w0, ls
	cbz	x21, .L96
	sub	w20, w1, w19
	str	x25, [x29, 64]
	cmp	w20, w22
	csel	w20, w20, w22, ls
	ldr	x1, [x23, 72]
	uxtw	x25, w20
	cbnz	w20, .L111
.L97:
	subs	w20, w22, w20
	bne	.L98
.L110:
	ldr	w1, [x24, 24]
	ldr	x2, [x24, 8]
	ldr	x25, [x29, 64]
.L96:
	add	w19, w19, w22
	sub	w0, w19, w1
	cmp	w1, w19
	csel	w19, w0, w19, ls
	str	w19, [x2]
	ldr	x19, [x29, 16]
	ldr	x24, [x29, 56]
.L92:
	mov	w0, w22
	ldp	x20, x21, [sp, 24]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L109:
	mov	w0, w22
	ldr	x19, [x29, 16]
	ldr	x24, [x29, 56]
	ldp	x20, x21, [sp, 24]
	ldp	x22, x23, [sp, 40]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L98:
	uxtw	x2, w20
	add	x0, x21, x25
	bl	adsp_memcpy
	b	.L110
	.p2align 2
.L111:
	add	x1, x1, x19, uxtw
	mov	x2, x25
	mov	x0, x21
	bl	adsp_memcpy
	ldr	x1, [x23, 72]
	b	.L97
	.size	rbuf_read_data_dma, .-rbuf_read_data_dma
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_write_data_dma
	.type	rbuf_write_data_dma, %function
rbuf_write_data_dma:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x22, [sp, 40]
	mov	x19, x0
	mov	x22, x1
	uxtw	x20, w2
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L113
	str	x21, [x29, 32]
	ldr	x0, [x19, 56]
	ldr	w21, [x0]
	cbnz	w20, .L140
	ldr	x21, [x29, 32]
.L113:
	mov	w0, w20
	ldr	x22, [sp, 40]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L140:
	str	x23, [x29, 48]
	ldr	w2, [x19, 80]
	ldr	x0, [x19, 72]
	sub	w2, w2, w21
	cmp	w2, w20
	csel	x2, x2, x20, ls
	add	x3, x0, x21, uxtw
	sub	w23, w20, w2
	cbz	x22, .L141
	str	x24, [x29, 56]
	uxtw	x24, w2
	cbnz	w2, .L142
.L118:
	add	x1, x22, x24
	uxtw	x2, w23
	bl	adsp_memcpy
	ldr	x24, [x29, 56]
.L117:
	ldr	w1, [x19, 80]
	add	w0, w20, w21
	ldr	x2, [x19, 56]
	sub	w3, w0, w1
	cmp	w1, w0
	csel	w0, w3, w0, ls
	str	w0, [x2]
	mov	w0, w20
	ldr	x21, [x29, 32]
	ldr	x23, [x29, 48]
	ldp	x19, x20, [sp, 16]
	ldr	x22, [sp, 40]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L141:
	cbnz	w2, .L143
.L115:
	uxtw	x2, w23
	mov	w1, 0
	bl	adsp_memset
	b	.L117
	.p2align 2
.L143:
	mov	w1, 0
	mov	x0, x3
	bl	adsp_memset
	cbz	w23, .L117
	ldr	x0, [x19, 72]
	b	.L115
	.p2align 2
.L142:
	mov	x2, x24
	mov	x1, x22
	mov	x0, x3
	bl	adsp_memcpy
	cbnz	w23, .L144
	ldr	x24, [x29, 56]
	b	.L117
.L144:
	ldr	x0, [x19, 72]
	b	.L118
	.size	rbuf_write_data_dma, .-rbuf_write_data_dma
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_copy_data
	.type	rbuf_copy_data, %function
rbuf_copy_data:
	stp	x29, x30, [sp, -112]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x19, x0
	mov	x20, x1
	uxtw	x22, w2
	mov	x0, x30
	bl	_mcount
	mov	w21, 0
	cmp	x19, 0
	ccmp	x20, 0, 4, ne
	bne	.L203
.L145:
	mov	w0, w21
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 112
	ret
	.p2align 2
.L203:
	stp	x23, x24, [x29, 48]
	cmp	w22, 0
	add	x23, x19, 56
	add	x24, x20, 56
	ccmp	x23, 0, 4, ne
	mov	w21, 0
	ccmp	x24, 0, 4, ne
	cset	w0, eq
	cbz	w0, .L204
.L201:
	ldp	x23, x24, [x29, 48]
	mov	w0, w21
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 112
	ret
	.p2align 2
.L204:
	ldr	x1, [x19, 56]
	mov	w21, w0
	ldr	x5, [x23, 8]
	ldr	w3, [x23, 24]
	ldr	w2, [x1]
	ldr	w4, [x5]
	add	w1, w2, w3
	cmp	w2, w4
	sub	w1, w1, w4
	sub	w2, w2, w4
	csel	w1, w1, w2, cc
	cmp	w22, w1
	bhi	.L201
	ldr	x7, [x20, 56]
	ldr	x1, [x24, 8]
	ldr	w4, [x24, 24]
	ldr	w2, [x7]
	ldr	w6, [x1]
	add	w1, w2, w4
	cmp	w2, w6
	sub	w1, w1, w6
	sub	w2, w2, w6
	csel	w1, w1, w2, cc
	sub	w1, w4, w1
	cmp	w22, w1
	bcs	.L201
	stp	x25, x26, [x29, 64]
	str	x27, [x29, 80]
	ldr	w26, [x5]
	ldr	w25, [x7]
	sub	w3, w3, w26
	ldr	x27, [x23, 16]
	cmp	w3, w22
	csel	x19, x3, x22, ls
	cbnz	w19, .L205
.L151:
	ldr	w25, [x7]
	subs	w19, w22, w19
	bne	.L206
.L158:
	ldr	w0, [x23, 24]
	add	w22, w22, w26
	ldr	x1, [x23, 8]
	sub	w2, w22, w0
	cmp	w0, w22
	csel	w22, w2, w22, ls
	str	w22, [x1]
	ldp	x23, x24, [x29, 48]
	ldp	x25, x26, [x29, 64]
	ldr	x27, [x29, 80]
	b	.L145
	.p2align 2
.L205:
	sub	w2, w4, w25
	ldr	x0, [x20, 72]
	cmp	w2, w19
	csel	x2, x2, x19, ls
	adds	x27, x27, x26, uxtw
	sub	w21, w19, w2
	add	x3, x0, x25, uxtw
	beq	.L207
	str	x28, [x29, 88]
	uxtw	x28, w2
	cbnz	w2, .L208
.L156:
	add	x1, x27, x28
	uxtw	x2, w21
	bl	adsp_memcpy
	ldr	x28, [x29, 88]
.L155:
	ldr	w1, [x24, 24]
	add	w0, w19, w25
	ldr	x2, [x20, 56]
	mov	w21, w19
	sub	w3, w0, w1
	cmp	w1, w0
	csel	w0, w3, w0, ls
	str	w0, [x2]
	ldr	x7, [x20, 56]
	ldr	x27, [x23, 16]
	b	.L151
	.p2align 2
.L206:
	str	x28, [x29, 88]
	ldr	w2, [x20, 80]
	ldr	x0, [x20, 72]
	sub	w2, w2, w25
	cmp	w2, w19
	csel	w2, w2, w19, ls
	add	x3, x0, x25, uxtw
	sub	w28, w19, w2
	cbz	x27, .L209
	uxtw	x4, w2
	cbnz	w2, .L210
.L163:
	uxtw	x2, w28
	add	x1, x27, x4
	bl	adsp_memcpy
.L162:
	ldr	w1, [x24, 24]
	add	w0, w19, w25
	ldr	x2, [x20, 56]
	add	w21, w21, w19
	sub	w3, w0, w1
	cmp	w1, w0
	csel	w0, w3, w0, ls
	str	w0, [x2]
	ldr	x28, [x29, 88]
	b	.L158
.L207:
	cbnz	w2, .L211
.L153:
	uxtw	x2, w21
	mov	w1, 0
	bl	adsp_memset
	b	.L155
.L209:
	cbnz	w2, .L212
.L160:
	uxtw	x2, w28
	mov	w1, 0
	bl	adsp_memset
	b	.L162
.L208:
	mov	x2, x28
	mov	x1, x27
	mov	x0, x3
	bl	adsp_memcpy
	cbnz	w21, .L213
	ldr	x28, [x29, 88]
	b	.L155
.L210:
	mov	x2, x4
	str	x4, [x29, 104]
	mov	x1, x27
	mov	x0, x3
	bl	adsp_memcpy
	ldr	x4, [x29, 104]
	cbz	w28, .L162
	ldr	x0, [x20, 72]
	b	.L163
	.p2align 2
.L212:
	uxtw	x2, w2
	mov	w1, 0
	mov	x0, x3
	bl	adsp_memset
	cbz	w28, .L162
	ldr	x0, [x20, 72]
	b	.L160
	.p2align 2
.L211:
	mov	w1, 0
	mov	x0, x3
	bl	adsp_memset
	cbz	w21, .L155
	ldr	x0, [x20, 72]
	b	.L153
.L213:
	ldr	x0, [x20, 72]
	b	.L156
	.size	rbuf_copy_data, .-rbuf_copy_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_update_attr
	.type	rbuf_update_attr, %function
rbuf_update_attr:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	x20, x1
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L214
	ldr	w0, [x20, 52]
	str	w0, [x19, 52]
	ldr	w0, [x20, 48]
	str	w0, [x19, 48]
.L214:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	rbuf_update_attr, .-rbuf_update_attr
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	rbuf_read_data_with_action
	.type	rbuf_read_data_with_action, %function
rbuf_read_data_with_action:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x23, x24, [sp, 48]
	mov	x20, x0
	mov	x23, x1
	mov	x0, x30
	mov	x24, x2
	mov	w19, w3
	bl	_mcount
	mov	w0, 0
	cbz	x20, .L219
	stp	x21, x22, [x29, 32]
	add	x22, x20, 56
	ldp	x0, x1, [x20, 56]
	ldr	w20, [x20, 80]
	ldr	w21, [x1]
	ldr	w1, [x0]
	add	w0, w1, w20
	cmp	w21, w1
	sub	w0, w0, w21
	sub	w1, w1, w21
	csel	w0, w0, w1, hi
	cmp	w19, w0
	bhi	.L225
	str	x25, [x29, 64]
	sub	w20, w20, w21
	cmp	w20, w19
	mov	x0, x24
	ldr	x1, [x22, 16]
	csel	w20, w20, w19, ls
	mov	w2, w20
	add	x1, x1, x21, uxtw
	blr	x23
	ldr	x1, [x22, 16]
	mov	w25, w0
	sub	w2, w19, w20
	add	x0, x24, x20, uxtw
	add	w19, w19, w21
	blr	x23
	add	w0, w25, w0
	ldr	w1, [x22, 24]
	ldr	x2, [x22, 8]
	sub	w3, w19, w1
	cmp	w1, w19
	csel	w19, w3, w19, ls
	str	w19, [x2]
	ldp	x21, x22, [x29, 32]
	ldr	x25, [x29, 64]
.L219:
	ldp	x19, x20, [sp, 16]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L225:
	mov	w0, 0
	ldp	x21, x22, [x29, 32]
	b	.L219
	.size	rbuf_read_data_with_action, .-rbuf_read_data_with_action
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
