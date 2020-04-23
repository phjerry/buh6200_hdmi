#include "hi_asm_define.h"
	.arch armv8-a
	.file	"aoe_aop.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aop_create_chn, %function
aop_create_chn:
	stp	x29, x30, [sp, -160]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	uxtw	x21, w0
	mov	x0, x30
	bl	_mcount
	adrp	x20, .LANCHOR0
	add	x1, x20, :lo12:.LANCHOR0
	str	xzr, [x29, 72]
	mov	x22, x21
	add	x0, x29, 80
	ldr	x19, [x1, x21, lsl 3]
	ldr	w1, [x19]
	and	w1, w1, 3
	bl	aoe_reg_read_bit_depth
	cbnz	w0, .L4
	ldr	w1, [x19]
	add	x0, x29, 84
	ubfx	x1, x1, 2, 2
	bl	aoe_reg_read_channel
	cbnz	w0, .L4
	ldr	w1, [x19]
	add	x0, x29, 88
	ubfx	x1, x1, 4, 4
	bl	aoe_reg_read_sampel_rate
	cbnz	w0, .L4
	ldr	w1, [x19]
	add	x0, x29, 92
	ubfx	x1, x1, 8, 2
	bl	aoe_reg_read_data_format
	cbnz	w0, .L4
	ldr	w1, [x19]
	add	x0, x29, 68
	lsr	w1, w1, 20
	bl	aoe_reg_read_buf_latency_ms
	cbnz	w0, .L4
	ldr	w2, [x19]
	ldp	w1, w0, [x29, 80]
	ubfx	x2, x2, 10, 1
	str	w2, [x29, 96]
	bl	util_calc_frame_size
	mov	w3, w0
	mov	w0, 1
	str	w0, [x29, 104]
	mov	x2, 0
	ldr	w1, [x19, 24]
	ubfx	x1, x1, 24, 1
	str	w1, [x29, 144]
	ldr	w4, [x19, 16]
	ldr	w0, [x19, 20]
	bfi	x2, x4, 0, 32
	bfi	x2, x0, 32, 32
	str	x2, [x29, 112]
	ldr	w2, [x19, 24]
	ubfx	x2, x2, 0, 24
	str	w2, [x29, 120]
	cbnz	w1, .L19
	add	x0, x19, 28
	add	x19, x19, 36
	stp	x0, x19, [x29, 128]
.L6:
	ldr	w0, [x29, 68]
	mov	w1, w3
	ldr	w2, [x29, 88]
	str	w3, [x29, 148]
	str	w0, [x29, 156]
	bl	util_latency_ms2_byte_size
	str	w0, [x29, 152]
	add	x1, x29, 104
	add	x0, x29, 72
	bl	rbuf_create
	ldr	x19, [x29, 72]
	cbz	x19, .L1
	mov	w1, 168
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x19, x0
	cbz	x0, .L7
	mov	x2, 168
	mov	w1, 0
	str	x23, [x29, 48]
	add	x20, x20, :lo12:.LANCHOR0
	add	x23, x0, 48
	bl	adsp_memset
	mov	x2, 80
	mov	x0, x23
	add	x1, x29, x2
	bl	adsp_memcpy
	ldr	x0, [x29, 72]
	str	x0, [x19, 136]
	ldr	x0, [x20, x21, lsl 3]
	ldr	w1, [x0, 4]
	tbnz	x1, 19, .L20
.L8:
	str	wzr, [x19, 132]
.L10:
	mov	x0, 121
	mov	w1, 1
	movk	x0, 0x79, lsl 32
	str	wzr, [x19]
	str	x0, [x19, 4]
	str	xzr, [x19, 12]
	str	wzr, [x19, 20]
	stp	x0, xzr, [x19, 24]
	str	w1, [x19, 144]
	str	xzr, [x19, 148]
	ldr	x23, [x29, 48]
.L1:
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 160
	ret
	.p2align 2
.L19:
	ldr	w4, [x19, 28]
	mov	x1, 0
	ldr	w2, [x19, 32]
	mov	x0, 0
	bfi	x1, x4, 0, 32
	bfi	x1, x2, 32, 32
	str	x1, [x29, 128]
	ldr	w2, [x19, 36]
	ldr	w1, [x19, 40]
	bfi	x0, x2, 0, 32
	bfi	x0, x1, 32, 32
	str	x0, [x29, 136]
	b	.L6
	.p2align 2
.L20:
	ldr	w0, [x0, 4]
	ubfx	x0, x0, 18, 1
	cbz	w0, .L8
	str	w0, [x19, 132]
	add	x1, x19, 72
	mov	w0, w22
	ldp	w2, w3, [x23]
	ldr	w4, [x23, 8]
	bl	cast_create
	mov	x20, x0
	cbz	x0, .L11
	ldr	x1, [x19, 136]
	mov	w0, w22
	bl	cast_set_aop_buf
	str	x20, [x19, 136]
	b	.L10
	.p2align 2
.L4:
	mov	x19, 0
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 160
	ret
.L11:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
	ldr	x23, [x29, 48]
.L7:
	ldr	x0, [x29, 72]
	mov	x19, 0
	bl	rbuf_destroy
	b	.L1
	.size	aop_create_chn, .-aop_create_chn
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_flush_chn_buf
	.type	aop_flush_chn_buf, %function
aop_flush_chn_buf:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 136]
	cbz	x0, .L21
	bl	rbuf_flush
.L21:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aop_flush_chn_buf, .-aop_flush_chn_buf
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_proc_delay_compensation
	.type	aop_proc_delay_compensation, %function
aop_proc_delay_compensation:
	stp	x29, x30, [sp, -96]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	uxtw	x0, w19
	adrp	x1, .LANCHOR0
	add	x2, x0, 8
	add	x1, x1, :lo12:.LANCHOR0
	ldr	x19, [x1, x2, lsl 3]
	cbz	x19, .L26
	ldr	w2, [x19, 144]
	cbz	w2, .L26
	str	x20, [x29, 24]
	str	x22, [x29, 40]
	ldr	w2, [x19, 156]
	ldr	x22, [x1, x0, lsl 3]
	cmp	w2, 1
	beq	.L60
	ldr	w20, [x19, 148]
	cmp	w20, 9
	bhi	.L61
	ldr	x20, [x29, 24]
	ldr	x22, [x29, 40]
.L26:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L61:
	str	x21, [x29, 32]
	add	x1, x29, 64
	mov	w21, 19923
	ldr	x0, [x19, 136]
	movk	w21, 0x1062, lsl 16
	bl	rbuf_query_info
	ldp	w1, w0, [x19, 48]
	bl	util_calc_frame_size
	ldr	w2, [x19, 56]
	ldr	w1, [x19, 48]
	mul	w2, w20, w2
	umull	x2, w2, w21
	lsr	x2, x2, 38
	and	w3, w2, -4
	tst	x2, 3
	add	w3, w3, 4
	csel	w2, w3, w2, ne
	mul	w20, w0, w2
	ldr	w0, [x19, 52]
	bl	util_calc_frame_size
	ldr	w3, [x19, 56]
	ldr	w2, [x29, 84]
	add	w1, w3, w3, lsl 2
	lsl	w1, w1, 1
	umull	x1, w1, w21
	lsr	x1, x1, 38
	and	w3, w1, -4
	tst	x1, 3
	add	w3, w3, 4
	csel	w1, w3, w1, ne
	cmp	w20, w2
	bcc	.L59
	sub	w2, w20, w2
	madd	w2, w0, w1, w2
	cbz	w2, .L59
	ldr	w3, [x29, 88]
	mov	x1, 0
	ldr	x0, [x19, 136]
	cmp	w3, w2
	csel	w2, w3, w2, ls
	bl	rbuf_write_data
	ldr	w0, [x22, 100]
	add	w0, w0, 1
	str	w0, [x22, 100]
	ldp	x20, x21, [x29, 24]
	ldr	x22, [x29, 40]
	b	.L26
	.p2align 2
.L59:
	ldp	x20, x21, [x29, 24]
	ldr	x22, [x29, 40]
	b	.L26
	.p2align 2
.L60:
	str	x21, [x29, 32]
	add	x1, x29, 64
	str	x23, [x29, 48]
	stp	xzr, xzr, [x29, 64]
	stp	xzr, xzr, [x29, 80]
	ldr	w0, [x22, 4]
	orr	w0, w0, 1048576
	str	w0, [x22, 4]
	ldr	x0, [x19, 136]
	bl	rbuf_query_info
	ldr	x0, [x19, 136]
	cbz	x0, .L29
	bl	rbuf_flush
.L29:
	ldp	w1, w0, [x19, 48]
	mov	w23, 19923
	ldr	w21, [x19, 152]
	movk	w23, 0x1062, lsl 16
	bl	util_calc_frame_size
	ldr	w20, [x19, 148]
	ldr	w2, [x19, 56]
	ldr	w1, [x19, 48]
	mul	w2, w21, w2
	umull	x2, w2, w23
	lsr	x2, x2, 38
	and	w3, w2, -4
	tst	x2, 3
	add	w3, w3, 4
	csel	w2, w3, w2, ne
	mul	w21, w0, w2
	ldr	w0, [x19, 52]
	bl	util_calc_frame_size
	ldr	w2, [x19, 56]
	mov	x1, 0
	ldp	w6, w5, [x29, 84]
	mul	w2, w20, w2
	sub	w3, w6, w21
	umull	x2, w2, w23
	lsr	x2, x2, 38
	and	w4, w2, -4
	add	w4, w4, 4
	tst	x2, 3
	csel	w2, w4, w2, ne
	cmp	w21, w6
	mul	w2, w0, w2
	ldr	x0, [x19, 136]
	add	w3, w2, w3
	csel	w2, w3, w2, ls
	cmp	w5, w2
	csel	w2, w5, w2, ls
	bl	rbuf_write_data
	ldr	w0, [x22, 100]
	add	w0, w0, 1
	str	w0, [x22, 100]
	ldr	w0, [x19, 148]
	stp	w0, wzr, [x19, 152]
	ldp	x20, x21, [x29, 24]
	ldp	x22, x23, [x29, 40]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 96
	ret
	.size	aop_proc_delay_compensation, .-aop_proc_delay_compensation
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_destroy_chn
	.type	aop_destroy_chn, %function
aop_destroy_chn:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x0, x0, x20, uxtw 3
	ldr	x19, [x0, 64]
	cbz	x19, .L62
	ldr	w0, [x19, 132]
	cmp	w0, 1
	beq	.L68
.L64:
	ldr	x0, [x19, 136]
	bl	rbuf_destroy
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L62:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L68:
	mov	w0, w20
	bl	cast_destroy
	b	.L64
	.size	aop_destroy_chn, .-aop_destroy_chn
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_init
	.type	aop_init, %function
aop_init:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x22, x0
	adrp	x20, .LANCHOR0
	mov	x19, x22
	add	x21, x22, 1024
	add	x20, x20, :lo12:.LANCHOR0
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L70:
	str	x19, [x20]
	mov	x0, x19
	mov	x2, 128
	mov	w1, 0
	bl	adsp_memset
	add	x19, x19, 128
	ldr	w0, [x19, -124]
	add	x20, x20, 8
	cmp	x19, x21
	orr	w0, w0, 4
	str	w0, [x19, -124]
	str	xzr, [x20, 56]
	bne	.L70
	mov	x0, x22
	bl	cast_init
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	aop_init, .-aop_init
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_deinit
	.type	aop_deinit, %function
aop_deinit:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	adrp	x20, .LANCHOR0
	add	x20, x20, :lo12:.LANCHOR0
	add	x20, x20, 64
	mov	w19, 0
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L73:
	mov	w0, w19
	bl	aop_destroy_chn
	str	xzr, [x20], 8
	add	w19, w19, 1
	cmp	w19, 8
	bne	.L73
	bl	cast_deinit
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aop_deinit, .-aop_deinit
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_process
	.type	aop_process, %function
aop_process:
	stp	x29, x30, [sp, -96]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	adrp	x20, .LANCHOR0
	mov	x0, x30
	add	x20, x20, :lo12:.LANCHOR0
	bl	_mcount
	mov	w21, 0
	mov	w23, 1
	mov	w22, 4
	b	.L93
	.p2align 2
.L105:
	cmp	w1, 1
	bne	.L102
	mov	w0, w21
	bl	aop_destroy_chn
	ldr	x0, [x20]
	str	xzr, [x20, 64]
.L83:
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L86
.L94:
	ldr	w1, [x0, 4]
	and	w1, w1, -121
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
.L77:
	ldr	x19, [x20, 64]
	cbz	x19, .L86
	ldr	w0, [x19, 144]
	str	wzr, [x19, 160]
	cmp	w0, 1
	beq	.L103
.L86:
	add	w21, w21, 1
	add	x20, x20, 8
	cmp	w21, 8
	beq	.L104
.L93:
	ldr	x0, [x20]
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L77
	ldr	w1, [x0, 4]
	ands	w1, w1, 3
	bne	.L105
	mov	w0, w21
	bl	aop_create_chn
	str	x0, [x20, 64]
	mov	x19, x0
	cbz	x0, .L106
	ldr	x0, [x20]
	ldr	w1, [x0, 4]
	tbz	x1, 2, .L94
	ldr	w0, [x19, 144]
	str	wzr, [x19, 160]
	cmp	w0, 1
	bne	.L86
	.p2align 3
.L103:
	ldr	x24, [x20]
	ldr	w0, [x24, 4]
	str	w0, [x29, 88]
	ldr	w0, [x24, 12]
	str	w0, [x29, 92]
	str	wzr, [x19, 20]
	ldr	w1, [x29, 92]
	ldr	w0, [x29, 92]
	and	w1, w1, 255
	ldr	w2, [x29, 92]
	ubfx	x0, x0, 8, 8
	stp	w1, w0, [x19, 24]
	ldr	w0, [x29, 92]
	ubfx	x0, x0, 16, 3
	tbz	x2, 19, .L99
	neg	w0, w0
.L99:
	ldr	w1, [x29, 92]
	stp	w0, w0, [x19, 32]
	ldr	w0, [x29, 92]
	ubfx	x0, x0, 24, 6
	tbz	x1, 30, .L100
	neg	w0, w0
.L100:
	ldr	w1, [x29, 92]
	lsr	w1, w1, 31
	stp	w0, w1, [x19, 40]
	ldr	w0, [x29, 88]
	ldr	w1, [x19, 48]
	ubfx	x0, x0, 8, 1
	str	w0, [x19, 128]
	ldr	w0, [x19, 52]
	ldr	w26, [x24]
	lsr	w26, w26, 20
	str	w26, [x19, 124]
	bl	util_calc_frame_size
	mov	w1, w0
	ldr	w2, [x19, 56]
	mov	w0, w26
	bl	util_latency_ms2_byte_size
	str	w0, [x19, 120]
	ldr	x0, [x19, 136]
	add	x1, x19, 72
	bl	rbuf_update_attr
	ldr	w0, [x24, 4]
	ldr	w1, [x19, 152]
	ubfx	x0, x0, 21, 9
	str	w0, [x19, 148]
	cmp	w0, w1
	beq	.L92
	str	w23, [x19, 156]
.L92:
	ldr	x0, [x19, 136]
	add	w21, w21, 1
	add	x20, x20, 8
	bl	rbuf_query_busy
	mov	w24, w0
	ldp	w1, w0, [x19, 48]
	bl	util_calc_frame_size
	mov	w1, w0
	ldr	w2, [x19, 56]
	mov	w0, w24
	bl	util_latency_ms
	str	w0, [x19, 160]
	cmp	w21, 8
	bne	.L93
.L104:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L102:
	ldr	w1, [x0, 4]
	bfi	w1, w22, 3, 4
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
	b	.L77
	.p2align 2
.L106:
	ldr	x0, [x20]
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L83
	ldr	w1, [x0, 4]
	bfi	w1, w23, 3, 4
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
	ldr	x0, [x20]
	ldr	w1, [x0, 4]
	tbz	x1, 2, .L94
	b	.L77
	.size	aop_process, .-aop_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_get_chn_status
	.type	aop_get_chn_status, %function
aop_get_chn_status:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x20, x0, x20, uxtw 3
	ldr	x0, [x20, 64]
	cbz	x0, .L110
	ldr	w1, [x0, 144]
	str	w1, [x19]
	ldr	w1, [x0, 64]
	str	w1, [x19, 4]
	ldr	w1, [x0, 116]
	str	w1, [x19, 16]
	ldr	w1, [x0, 48]
	str	w1, [x19, 8]
	ldr	w1, [x0, 52]
	str	w1, [x19, 12]
	ldr	w1, [x0, 128]
	str	w1, [x19, 20]
	ldr	w1, [x0, 132]
	str	w1, [x19, 24]
	ldr	w1, [x0, 148]
	str	w1, [x19, 28]
	ldr	w0, [x0, 160]
	str	w0, [x19, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L110:
	str	wzr, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aop_get_chn_status, .-aop_get_chn_status
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_query_rbf_free
	.type	aop_query_rbf_free, %function
aop_query_rbf_free:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	mov	w0, 0
	ldr	x1, [x19, 64]
	cbz	x1, .L111
	ldr	w2, [x1, 144]
	mov	w0, 0
	cbz	w2, .L111
	ldr	x2, [x1, 136]
	cbz	x2, .L111
	mov	w1, w20
	mov	x0, x2
	bl	rbuf_check_free
.L111:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aop_query_rbf_free, .-aop_query_rbf_free
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_write_buff_proc_try_cnt
	.type	aop_write_buff_proc_try_cnt, %function
aop_write_buff_proc_try_cnt:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x0, x0, x19, uxtw 3
	ldr	x0, [x0, 64]
	cbz	x0, .L118
	ldr	w0, [x0, 132]
	cmp	w0, 1
	beq	.L123
.L118:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L123:
	mov	w0, w19
	bl	cast_fifo_try_cnt
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aop_write_buff_proc_try_cnt, .-aop_write_buff_proc_try_cnt
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_write_buff_proc_ok_cnt
	.type	aop_write_buff_proc_ok_cnt, %function
aop_write_buff_proc_ok_cnt:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x0, x0, x19, uxtw 3
	ldr	x0, [x0, 64]
	cbz	x0, .L124
	ldr	w0, [x0, 132]
	cmp	w0, 1
	beq	.L129
.L124:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L129:
	mov	w0, w19
	bl	cast_fifo_free_cnt
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aop_write_buff_proc_ok_cnt, .-aop_write_buff_proc_ok_cnt
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_write_rbf_data
	.type	aop_write_rbf_data, %function
aop_write_rbf_data:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	x20, x1
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	mov	w0, 0
	ldr	x19, [x19, 64]
	cbz	x19, .L130
	ldr	w1, [x19, 144]
	mov	w0, 0
	cbz	w1, .L130
	ldr	w0, [x20, 28]
	cbz	w0, .L155
.L133:
	ldr	w3, [x19, 132]
	ldr	w2, [x20, 8]
	cmp	w3, 1
	ldr	x0, [x19, 136]
	ldr	x1, [x20]
	beq	.L156
	bl	rbuf_write_data_dma
.L130:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L155:
	str	x21, [x29, 32]
	ldr	x0, [x19, 48]
	ldr	x1, [x20, 16]
	cmp	x1, x0
	beq	.L135
	ldr	w0, [x19, 48]
	ldr	w1, [x20, 16]
	cmp	w1, w0
	bne	.L157
.L136:
	ldr	w0, [x19, 52]
	ldr	w1, [x20, 20]
	cmp	w1, w0
	beq	.L135
	bl	aoe_get_wk_buf
	mov	x1, x0
	ldr	w0, [x19, 52]
	mov	x2, x20
	bl	util_upmix_pcm_data
.L135:
	ldr	w0, [x19, 44]
	cbz	w0, .L138
	bl	aoe_get_wk_buf
	mov	x21, x0
	ldr	w2, [x20, 8]
	mov	w1, 0
	bl	adsp_memset
	str	x21, [x20]
.L138:
	ldr	w0, [x19, 4]
	ldr	w1, [x19, 24]
	cmp	w1, w0
	beq	.L158
.L139:
	ldr	w0, [x19, 132]
	cmp	w0, 1
	beq	.L140
	str	x22, [x29, 40]
	bl	aoe_get_wk_buf
	ldr	w2, [x20, 8]
	mov	x22, x0
	ldr	x1, [x20]
	add	x21, x19, 20
	bl	adsp_memcpy
	str	x22, [x20]
	mov	x3, x21
	mov	x2, x19
	ldr	w1, [x19, 40]
	mov	x0, x20
	bl	util_lr_fade_gain_process
	mov	x1, x21
	mov	x2, 20
	mov	x0, x19
	bl	adsp_memcpy
	ldp	x21, x22, [x29, 32]
	b	.L133
	.p2align 2
.L156:
	bl	rbuf_write_data
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L158:
	ldr	w0, [x19, 12]
	ldr	w1, [x19, 32]
	cmp	w1, w0
	bne	.L139
.L140:
	ldr	x1, [x19, 24]
	mov	x0, 121
	movk	x0, 0x79, lsl 32
	cmp	x1, x0
	bne	.L142
	ldr	x0, [x19, 32]
	cbnz	x0, .L142
	ldr	w0, [x19, 40]
	cbnz	w0, .L142
	ldr	x21, [x29, 32]
	b	.L133
	.p2align 2
.L142:
	bl	aoe_get_wk_buf
	mov	x21, x0
	ldr	w2, [x20, 8]
	ldr	x1, [x20]
	bl	adsp_memcpy
	str	x21, [x20]
	add	x2, x19, 20
	mov	x0, x20
	ldr	w1, [x19, 40]
	bl	util_lr_gain_process
	ldr	x21, [x29, 32]
	b	.L133
	.p2align 2
.L157:
	bl	aoe_get_wk_buf
	mov	x1, x0
	ldr	w0, [x19, 48]
	mov	x2, x20
	bl	util_unify_bit_depth_pcm_data
	b	.L136
	.size	aop_write_rbf_data, .-aop_write_rbf_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_get_ring_out
	.type	aop_get_ring_out, %function
aop_get_ring_out:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	ldr	x0, [x19, 64]
	cbz	x0, .L161
	ldr	x0, [x0, 136]
.L159:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L161:
	mov	x0, 0
	b	.L159
	.size	aop_get_ring_out, .-aop_get_ring_out
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_continue_output
	.type	aop_continue_output, %function
aop_continue_output:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	mov	w21, w2
	mov	w22, w3
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	ldr	x19, [x19, 64]
	cbz	x19, .L162
	cmp	w20, 4
	beq	.L170
	cmp	w20, 3
	beq	.L171
.L162:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L170:
	adrp	x1, .LANCHOR1
	cmp	w22, 1
	beq	.L166
	add	x1, x1, :lo12:.LANCHOR1
	mov	w20, 1544
.L165:
	ldr	x0, [x19, 136]
	mov	w2, w20
	bl	rbuf_write_data_dma
	ldr	x0, [x19, 136]
	sub	w2, w21, w20
	mov	x1, 0
	bl	rbuf_write_data_dma
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L171:
	ldr	x0, [x19, 136]
	adrp	x1, .LANCHOR2
	add	x1, x1, :lo12:.LANCHOR2
	mov	w2, 2568
	add	x1, x1, 456
	bl	rbuf_write_data_dma
	ldr	x0, [x19, 136]
	sub	w2, w21, #2568
	mov	x1, 0
	bl	rbuf_write_data_dma
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L166:
	add	x1, x1, :lo12:.LANCHOR1
	mov	w20, 3260
	add	x1, x1, 1544
	b	.L165
	.size	aop_continue_output, .-aop_continue_output
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aop_proc_cmd
	.type	aop_proc_cmd, %function
aop_proc_cmd:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	x0, x30
	bl	_mcount
	ldr	w19, [x20, 8]
	cmp	w19, 7
	bhi	.L172
	ldr	w0, [x20]
	cbz	w0, .L175
	cmp	w0, 1
	bne	.L172
	mov	w0, w19
	uxtw	x19, w19
	bl	aop_destroy_chn
	adrp	x1, .LANCHOR0
	add	x2, x19, 8
	add	x0, x1, :lo12:.LANCHOR0
	str	xzr, [x0, x2, lsl 3]
.L177:
	add	x1, x1, :lo12:.LANCHOR0
	ldr	x0, [x1, x19, lsl 3]
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L172
	ldr	w1, [x0, 4]
	and	w1, w1, -121
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
.L172:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L175:
	mov	w0, w19
	uxtw	x19, w19
	bl	aop_create_chn
	adrp	x1, .LANCHOR0
	add	x3, x19, 8
	add	x2, x1, :lo12:.LANCHOR0
	str	x0, [x2, x3, lsl 3]
	cbnz	x0, .L177
	ldr	x0, [x2, x19, lsl 3]
	ldr	w2, [x0, 4]
	tbnz	x2, 2, .L177
	ldr	w2, [x0, 4]
	mov	w3, 1
	bfi	w2, w3, 3, 4
	str	w2, [x0, 4]
	ldr	w2, [x0, 4]
	orr	w2, w2, 4
	str	w2, [x0, 4]
	b	.L177
	.size	aop_proc_cmd, .-aop_proc_cmd
	.section	.rodata
	.align	3
	.set	.LANCHOR1,. + 0
	.set	.LANCHOR2,. + 4352
	.type	g_ddp_heard, %object
	.size	g_ddp_heard, 1544
g_ddp_heard:
	.hword	-1934
	.hword	19999
	.hword	21
	.hword	1536
	.hword	2935
	.hword	767
	.hword	16263
	.hword	-8159
	.hword	1568
	.hword	0
	.hword	2
	.hword	0
	.hword	7
	.hword	-1727
	.hword	318
	.hword	-11256
	.hword	2056
	.hword	2050
	.hword	514
	.hword	512
	.hword	-32640
	.hword	-32640
	.hword	8224
	.hword	8224
	.hword	2056
	.hword	2056
	.hword	12787
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12353
	.hword	-6313
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12353
	.hword	-1579
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3089
	.hword	-395
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31995
	.hword	-99
	.hword	24382
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	-25
	.hword	22479
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-16391
	.hword	-10455
	.hword	-5998
	.hword	18887
	.hword	95
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-16
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	9
	.hword	5258
	.hword	17698
	.hword	-28344
	.hword	-23470
	.hword	10516
	.hword	-30139
	.hword	8895
	.hword	-1
	.hword	-1
	.hword	-64
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	51
	.hword	13107
	.hword	13107
	.hword	13107
	.hword	13107
	.hword	13107
	.hword	13103
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-8
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	4
	.hword	-30139
	.hword	8849
	.hword	18596
	.hword	21033
	.hword	5258
	.hword	17698
	.hword	-28321
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-16
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	9
	.hword	5258
	.hword	17698
	.hword	-28344
	.hword	-23470
	.hword	10516
	.hword	-30139
	.hword	8895
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-32
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	18
	.hword	10516
	.hword	-30139
	.hword	8849
	.hword	18596
	.hword	21033
	.hword	5258
	.hword	17791
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-1
	.hword	-64
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	36
	.hword	21033
	.hword	5258
	.hword	17698
	.hword	-28344
	.hword	-23470
	.hword	10516
	.hword	-29954
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	16448
	.hword	20772
	.hword	-28096
	.hword	16465
	.hword	9362
	.hword	16448
	.hword	20772
	.hword	-28096
	.hword	16465
	.hword	9362
	.hword	16448
	.hword	20772
	.hword	-28160
	.hword	0
	.hword	0
	.hword	0
	.hword	24528
	.hword	-1342
	.type	g_atmos_heard, %object
	.size	g_atmos_heard, 3260
g_atmos_heard:
	.byte	114
	.byte	-8
	.byte	31
	.byte	78
	.byte	21
	.byte	0
	.byte	-76
	.byte	12
	.byte	119
	.byte	11
	.byte	89
	.byte	6
	.byte	103
	.byte	63
	.byte	-38
	.byte	-31
	.byte	0
	.byte	73
	.byte	1
	.byte	65
	.byte	1
	.byte	7
	.byte	0
	.byte	32
	.byte	0
	.byte	0
	.byte	0
	.byte	16
	.byte	0
	.byte	0
	.byte	-124
	.byte	35
	.byte	-122
	.byte	121
	.byte	97
	.byte	24
	.byte	-4
	.byte	-121
	.byte	-7
	.byte	-22
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-13
	.byte	127
	.byte	-25
	.byte	-85
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	125
	.byte	62
	.byte	-50
	.byte	-1
	.byte	-97
	.byte	-81
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-9
	.byte	-7
	.byte	58
	.byte	-1
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-33
	.byte	-25
	.byte	-22
	.byte	-4
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	127
	.byte	-97
	.byte	-82
	.byte	-13
	.byte	-29
	.byte	83
	.byte	-39
	.byte	-128
	.byte	27
	.byte	-14
	.byte	111
	.byte	-121
	.byte	60
	.byte	30
	.byte	-18
	.byte	120
	.byte	-18
	.byte	-18
	.byte	-32
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	-35
	.byte	-35
	.byte	-74
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-33
	.byte	-74
	.byte	107
	.byte	62
	.byte	-42
	.byte	90
	.byte	-83
	.byte	-75
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	62
	.byte	27
	.byte	112
	.byte	67
	.byte	-29
	.byte	-19
	.byte	-113
	.byte	-57
	.byte	-35
	.byte	29
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-36
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	59
	.byte	0
	.byte	-69
	.byte	-69
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	-25
	.byte	-37
	.byte	107
	.byte	-51
	.byte	-42
	.byte	90
	.byte	-81
	.byte	-75
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-61
	.byte	-25
	.byte	-56
	.byte	103
	.byte	29
	.byte	110
	.byte	120
	.byte	-68
	.byte	-29
	.byte	-15
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	0
	.byte	-128
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	7
	.byte	118
	.byte	119
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-7
	.byte	124
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-20
	.byte	-5
	.byte	125
	.byte	1
	.byte	103
	.byte	-128
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-49
	.byte	0
	.byte	24
	.byte	-16
	.byte	-15
	.byte	14
	.byte	-57
	.byte	-29
	.byte	-18
	.byte	-114
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	29
	.byte	0
	.byte	-35
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-13
	.byte	109
	.byte	-75
	.byte	-26
	.byte	107
	.byte	-83
	.byte	-41
	.byte	90
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-17
	.byte	-13
	.byte	7
	.byte	-100
	.byte	-3
	.byte	-13
	.byte	71
	.byte	-98
	.byte	108
	.byte	-128
	.byte	13
	.byte	-7
	.byte	-73
	.byte	-61
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-97
	.byte	13
	.byte	-72
	.byte	33
	.byte	-15
	.byte	118
	.byte	-57
	.byte	-29
	.byte	-18
	.byte	-114
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	29
	.byte	0
	.byte	-35
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-13
	.byte	109
	.byte	-75
	.byte	-26
	.byte	107
	.byte	-83
	.byte	-41
	.byte	90
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-31
	.byte	-13
	.byte	-28
	.byte	-77
	.byte	14
	.byte	55
	.byte	60
	.byte	-34
	.byte	-15
	.byte	120
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	3
	.byte	-69
	.byte	-69
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	124
	.byte	-66
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-7
	.byte	90
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-10
	.byte	125
	.byte	-66
	.byte	0
	.byte	51
	.byte	-64
	.byte	-113
	.byte	-57
	.byte	59
	.byte	30
	.byte	-69
	.byte	-69
	.byte	-72
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	119
	.byte	109
	.byte	119
	.byte	-37
	.byte	-74
	.byte	-73
	.byte	109
	.byte	-102
	.byte	-49
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	62
	.byte	95
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	103
	.byte	-128
	.byte	12
	.byte	-8
	.byte	120
	.byte	7
	.byte	-29
	.byte	-15
	.byte	119
	.byte	-57
	.byte	119
	.byte	119
	.byte	0
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	14
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-74
	.byte	-19
	.byte	109
	.byte	-37
	.byte	-7
	.byte	-74
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-9
	.byte	-7
	.byte	3
	.byte	-50
	.byte	-2
	.byte	-7
	.byte	35
	.byte	-49
	.byte	-1
	.byte	-61
	.byte	56
	.byte	88
	.byte	73
	.byte	0
	.byte	-32
	.byte	10
	.byte	-128
	.byte	-4
	.byte	24
	.byte	1
	.byte	17
	.byte	62
	.byte	0
	.byte	15
	.byte	-100
	.byte	3
	.byte	112
	.byte	0
	.byte	-128
	.byte	0
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	-33
	.byte	1
	.byte	0
	.byte	2
	.byte	-127
	.byte	3
	.byte	0
	.byte	-12
	.byte	-5
	.byte	7
	.byte	0
	.byte	-24
	.byte	-5
	.byte	14
	.byte	8
	.byte	-16
	.byte	-9
	.byte	29
	.byte	16
	.byte	-32
	.byte	-48
	.byte	1
	.byte	64
	.byte	126
	.byte	84
	.byte	0
	.byte	16
	.byte	-58
	.byte	60
	.byte	10
	.byte	32
	.byte	16
	.byte	4
	.byte	-127
	.byte	32
	.byte	8
	.byte	0
	.byte	65
	.byte	2
	.byte	-82
	.byte	10
	.byte	-72
	.byte	43
	.byte	-32
	.byte	-82
	.byte	-128
	.byte	9
	.byte	0
	.byte	-121
	.byte	-103
	.byte	-82
	.byte	104
	.byte	-51
	.byte	52
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	50
	.byte	-51
	.byte	48
	.byte	30
	.byte	-51
	.byte	108
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	27
	.byte	0
	.byte	67
	.byte	62
	.byte	-19
	.byte	112
	.byte	-57
	.byte	-29
	.byte	29
	.byte	-113
	.byte	-35
	.byte	-35
	.byte	-36
	.byte	-35
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	59
	.byte	-74
	.byte	-69
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-51
	.byte	-25
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	-97
	.byte	-81
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	103
	.byte	-61
	.byte	110
	.byte	-56
	.byte	-68
	.byte	29
	.byte	-15
	.byte	120
	.byte	-69
	.byte	-29
	.byte	-69
	.byte	-69
	.byte	-128
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	7
	.byte	0
	.byte	119
	.byte	119
	.byte	-37
	.byte	118
	.byte	-74
	.byte	109
	.byte	124
	.byte	-37
	.byte	-83
	.byte	-7
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-8
	.byte	124
	.byte	-7
	.byte	108
	.byte	-61
	.byte	13
	.byte	-113
	.byte	-73
	.byte	60
	.byte	30
	.byte	119
	.byte	119
	.byte	119
	.byte	119
	.byte	0
	.byte	112
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	0
	.byte	-18
	.byte	-18
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-97
	.byte	111
	.byte	-83
	.byte	53
	.byte	90
	.byte	107
	.byte	-66
	.byte	-42
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	125
	.byte	-97
	.byte	47
	.byte	-128
	.byte	12
	.byte	-80
	.byte	-29
	.byte	-15
	.byte	-114
	.byte	-57
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-35
	.byte	29
	.byte	-37
	.byte	-35
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	-26
	.byte	-13
	.byte	-83
	.byte	-75
	.byte	90
	.byte	107
	.byte	-49
	.byte	-41
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	25
	.byte	-32
	.byte	3
	.byte	-2
	.byte	-34
	.byte	1
	.byte	120
	.byte	60
	.byte	-35
	.byte	-15
	.byte	-35
	.byte	-35
	.byte	-64
	.byte	-35
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	3
	.byte	0
	.byte	-69
	.byte	-69
	.byte	109
	.byte	-69
	.byte	-37
	.byte	-74
	.byte	-66
	.byte	109
	.byte	-42
	.byte	124
	.byte	-83
	.byte	-75
	.byte	90
	.byte	107
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	125
	.byte	62
	.byte	-128
	.byte	-13
	.byte	127
	.byte	-2
	.byte	-56
	.byte	-77
	.byte	-82
	.byte	-16
	.byte	0
	.byte	64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	6
	.byte	0
	.byte	-112
	.byte	-49
	.byte	59
	.byte	-36
	.byte	-15
	.byte	120
	.byte	-57
	.byte	-29
	.byte	119
	.byte	119
	.byte	119
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	14
	.byte	-19
	.byte	-18
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	-13
	.byte	-7
	.byte	-42
	.byte	90
	.byte	-83
	.byte	-75
	.byte	-25
	.byte	107
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-39
	.byte	-16
	.byte	27
	.byte	-14
	.byte	111
	.byte	-121
	.byte	60
	.byte	30
	.byte	-18
	.byte	120
	.byte	-18
	.byte	-18
	.byte	-32
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	-35
	.byte	-35
	.byte	-74
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-33
	.byte	-74
	.byte	107
	.byte	62
	.byte	-42
	.byte	90
	.byte	-83
	.byte	-75
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	62
	.byte	27
	.byte	112
	.byte	67
	.byte	-29
	.byte	-19
	.byte	-113
	.byte	-57
	.byte	-35
	.byte	29
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-36
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	59
	.byte	0
	.byte	-69
	.byte	-69
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	-25
	.byte	-37
	.byte	107
	.byte	-51
	.byte	-42
	.byte	90
	.byte	-81
	.byte	-75
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-33
	.byte	-25
	.byte	11
	.byte	96
	.byte	3
	.byte	-20
	.byte	120
	.byte	60
	.byte	-29
	.byte	-15
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	0
	.byte	-128
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	7
	.byte	118
	.byte	119
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-7
	.byte	124
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	6
	.byte	-8
	.byte	-128
	.byte	127
	.byte	119
	.byte	-64
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-32
	.byte	124
	.byte	-97
	.byte	63
	.byte	-14
	.byte	-20
	.byte	3
	.byte	60
	.byte	-56
	.byte	103
	.byte	29
	.byte	110
	.byte	120
	.byte	-68
	.byte	-29
	.byte	-15
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	0
	.byte	-128
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	7
	.byte	118
	.byte	119
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-7
	.byte	124
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	108
	.byte	-8
	.byte	13
	.byte	-7
	.byte	-73
	.byte	-61
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-97
	.byte	13
	.byte	-72
	.byte	33
	.byte	-15
	.byte	118
	.byte	-57
	.byte	-29
	.byte	-18
	.byte	-114
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	29
	.byte	0
	.byte	-35
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-13
	.byte	109
	.byte	-75
	.byte	-26
	.byte	107
	.byte	-83
	.byte	-41
	.byte	90
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-17
	.byte	-13
	.byte	5
	.byte	-80
	.byte	1
	.byte	-10
	.byte	60
	.byte	-98
	.byte	-15
	.byte	120
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	3
	.byte	-69
	.byte	-69
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	124
	.byte	-66
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-7
	.byte	90
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	3
	.byte	124
	.byte	-64
	.byte	63
	.byte	59
	.byte	96
	.byte	-113
	.byte	-57
	.byte	59
	.byte	30
	.byte	-69
	.byte	-69
	.byte	-72
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	119
	.byte	109
	.byte	119
	.byte	-37
	.byte	-74
	.byte	-73
	.byte	109
	.byte	-102
	.byte	-49
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	62
	.byte	95
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	112
	.byte	-66
	.byte	-49
	.byte	31
	.byte	121
	.byte	-10
	.byte	1
	.byte	30
	.byte	-28
	.byte	-77
	.byte	14
	.byte	55
	.byte	60
	.byte	-34
	.byte	-15
	.byte	120
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	3
	.byte	-69
	.byte	-69
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	124
	.byte	-66
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-7
	.byte	90
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	54
	.byte	124
	.byte	-122
	.byte	124
	.byte	-37
	.byte	-31
	.byte	-113
	.byte	-57
	.byte	59
	.byte	30
	.byte	-69
	.byte	-69
	.byte	-72
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	119
	.byte	109
	.byte	119
	.byte	-37
	.byte	-74
	.byte	-73
	.byte	109
	.byte	-102
	.byte	-49
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	62
	.byte	95
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	-49
	.byte	-122
	.byte	-36
	.byte	-112
	.byte	120
	.byte	59
	.byte	-29
	.byte	-15
	.byte	119
	.byte	-57
	.byte	119
	.byte	119
	.byte	0
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	14
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-74
	.byte	-19
	.byte	109
	.byte	-37
	.byte	-7
	.byte	-74
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-9
	.byte	-7
	.byte	2
	.byte	-40
	.byte	0
	.byte	-5
	.byte	30
	.byte	-49
	.byte	120
	.byte	60
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-32
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-35
	.byte	1
	.byte	-35
	.byte	-35
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	62
	.byte	-33
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	124
	.byte	-83
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	1
	.byte	62
	.byte	-32
	.byte	-97
	.byte	29
	.byte	48
	.byte	-57
	.byte	-29
	.byte	29
	.byte	-113
	.byte	-35
	.byte	-35
	.byte	-36
	.byte	-35
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	59
	.byte	-74
	.byte	-69
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-51
	.byte	-25
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	-97
	.byte	-81
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	56
	.byte	-33
	.byte	-25
	.byte	15
	.byte	60
	.byte	-5
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-112
	.byte	113
	.byte	21
	.byte	-8
	.zero	4
	.type	g_dd_heard, %object
	.size	g_dd_heard, 2568
g_dd_heard:
	.hword	-1934
	.hword	19999
	.hword	1
	.hword	20480
	.hword	2935
	.hword	3061
	.hword	9264
	.hword	-7684
	.hword	1216
	.hword	16129
	.hword	-27299
	.hword	5201
	.hword	17695
	.hword	-3157
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-13922
	.hword	-15501
	.hword	-50
	.hword	-20577
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24791
	.hword	-30999
	.hword	14335
	.hword	15038
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-23052
	.hword	14143
	.hword	-790
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3481
	.hword	19051
	.hword	32755
	.hword	-21529
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6198
	.hword	-27867
	.hword	32255
	.hword	-12615
	.hword	20366
	.hword	9288
	.hword	-28382
	.hword	17408
	.hword	0
	.hword	7
	.hword	30961
	.hword	-7225
	.hword	30583
	.hword	30583
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	3822
	.hword	-4371
	.hword	-18725
	.hword	28086
	.hword	-1549
	.hword	23254
	.hword	-19027
	.hword	27623
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-4096
	.hword	0
	.hword	239
	.hword	7740
	.hword	30958
	.hword	-4370
	.hword	-4384
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	1
	.hword	-8739
	.hword	-8778
	.hword	-9363
	.hword	-18721
	.hword	15979
	.hword	23254
	.hword	-19027
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	0
	.hword	0
	.hword	7651
	.hword	-14449
	.hword	7645
	.hword	-8739
	.hword	-9216
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	59
	.hword	-17477
	.hword	-18725
	.hword	28086
	.hword	-9241
	.hword	-12949
	.hword	23254
	.hword	-19025
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6208
	.hword	0
	.hword	3
	.hword	-17288
	.hword	-3613
	.hword	-17477
	.hword	-17477
	.hword	-32768
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	1911
	.hword	30582
	.hword	-9363
	.hword	-18725
	.hword	31993
	.hword	-21141
	.hword	23254
	.hword	-18957
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-2048
	.hword	0
	.hword	119
	.hword	-28898
	.hword	15479
	.hword	30583
	.hword	30576
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	-4370
	.hword	-4389
	.hword	28086
	.hword	-9361
	.hword	-24779
	.hword	-21141
	.hword	23254
	.hword	-16772
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	0
	.hword	0
	.hword	3824
	.hword	8064
	.hword	-32768
	.hword	0
	.hword	0
	.hword	3825
	.hword	-7225
	.hword	-28946
	.hword	-4370
	.hword	-4608
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	29
	.hword	-8739
	.hword	-9363
	.hword	-18725
	.hword	28147
	.hword	-6475
	.hword	-21141
	.hword	23255
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3104
	.hword	0
	.hword	1
	.hword	-8644
	.hword	30961
	.hword	-8739
	.hword	-8739
	.hword	-16384
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	955
	.hword	-17477
	.hword	28086
	.hword	-9363
	.hword	-16772
	.hword	-10571
	.hword	-21141
	.hword	23289
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31744
	.hword	0
	.hword	59
	.hword	-14449
	.hword	7739
	.hword	-17477
	.hword	-17480
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	30583
	.hword	30573
	.hword	-18725
	.hword	28087
	.hword	-12390
	.hword	-10571
	.hword	-21141
	.hword	24382
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-32768
	.hword	0
	.hword	1912
	.hword	-3613
	.hword	-14473
	.hword	30583
	.hword	30464
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	14
	.hword	-4370
	.hword	-4682
	.hword	-9363
	.hword	-18695
	.hword	-3238
	.hword	-10571
	.hword	-21141
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1552
	.hword	0
	.hword	0
	.hword	-4322
	.hword	15480
	.hword	-4370
	.hword	-4370
	.hword	-8192
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	477
	.hword	-8739
	.hword	-18725
	.hword	28086
	.hword	-8386
	.hword	27482
	.hword	-10571
	.hword	-21124
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15872
	.hword	0
	.hword	29
	.hword	-8129
	.hword	256
	.hword	0
	.hword	0
	.hword	29
	.hword	-7225
	.hword	-28899
	.hword	-8739
	.hword	-8740
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	15291
	.hword	-17482
	.hword	-9363
	.hword	-18725
	.hword	-6195
	.hword	27482
	.hword	-10571
	.hword	-20577
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-16384
	.hword	0
	.hword	956
	.hword	30961
	.hword	-7237
	.hword	-17477
	.hword	-17536
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	7
	.hword	30583
	.hword	30427
	.hword	28086
	.hword	-9348
	.hword	-1619
	.hword	27482
	.hword	-10571
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31992
	.hword	0
	.hword	0
	.hword	30607
	.hword	7740
	.hword	30583
	.hword	30583
	.hword	28672
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	238
	.hword	-4370
	.hword	-9363
	.hword	-18725
	.hword	28575
	.hword	13741
	.hword	27482
	.hword	-10562
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24832
	.hword	0
	.hword	14
	.hword	-3613
	.hword	-14450
	.hword	-4370
	.hword	-4370
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	7645
	.hword	-8741
	.hword	28086
	.hword	-9363
	.hword	-3098
	.hword	-19027
	.hword	27482
	.hword	-10289
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-8192
	.hword	0
	.hword	478
	.hword	15480
	.hword	-3619
	.hword	-8739
	.hword	-8768
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	3
	.hword	-17477
	.hword	-17555
	.hword	-18725
	.hword	28094
	.hword	31958
	.hword	-19027
	.hword	27482
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	0
	.hword	0
	.hword	15296
	.hword	32258
	.hword	2
	.hword	13056
	.hword	-17629
	.hword	-26289
	.hword	-991
	.hword	6727
	.hword	31487
	.hword	29080
	.hword	-23057
	.hword	-29454
	.hword	-4832
	.hword	23813
	.hword	-15679
	.hword	-11646
	.hword	-27644
	.hword	-9193
	.hword	-11496
	.hword	27765
	.hword	-4564
	.hword	28748
	.hword	22386
	.hword	-9652
	.hword	32025
	.hword	-26586
	.hword	-12207
	.hword	-25464
	.hword	-16311
	.hword	0
	.hword	0
	.hword	7651
	.hword	-14449
	.hword	7645
	.hword	-8739
	.hword	-9216
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	59
	.hword	-17477
	.hword	-18725
	.hword	28086
	.hword	-9241
	.hword	-12949
	.hword	23254
	.hword	-19025
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6208
	.hword	0
	.hword	3
	.hword	-17288
	.hword	-3613
	.hword	-17477
	.hword	-17477
	.hword	-32768
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	1911
	.hword	30582
	.hword	-9363
	.hword	-18725
	.hword	31993
	.hword	-21141
	.hword	23254
	.hword	-18957
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-2048
	.hword	0
	.hword	119
	.hword	-28898
	.hword	15479
	.hword	30583
	.hword	30576
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	-4370
	.hword	-4389
	.hword	28086
	.hword	-9361
	.hword	-24779
	.hword	-21141
	.hword	23254
	.hword	-16772
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	0
	.hword	0
	.hword	3825
	.hword	-7225
	.hword	-28946
	.hword	-4370
	.hword	-4608
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	29
	.hword	-8739
	.hword	-9363
	.hword	-18725
	.hword	28147
	.hword	-6475
	.hword	-21141
	.hword	23255
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3104
	.hword	0
	.hword	1
	.hword	-8644
	.hword	30961
	.hword	-8739
	.hword	-8739
	.hword	-16384
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	955
	.hword	-17477
	.hword	28086
	.hword	-9363
	.hword	-16772
	.hword	-10571
	.hword	-21141
	.hword	23289
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31744
	.hword	0
	.hword	59
	.hword	-16258
	.hword	512
	.hword	600
	.hword	212
	.hword	9149
	.hword	13105
	.hword	-22729
	.hword	16036
	.hword	-15202
	.hword	-26341
	.hword	11106
	.hword	30344
	.hword	-29965
	.hword	1241
	.hword	-27967
	.hword	29184
	.hword	-3268
	.hword	-3349
	.hword	24020
	.hword	-19572
	.hword	-32519
	.hword	-6959
	.hword	3355
	.hword	-18517
	.hword	-19798
	.hword	23307
	.hword	8213
	.hword	-33
	.hword	1998
	.hword	-10983
	.hword	5956
	.hword	3239
	.hword	487
	.hword	-11141
	.hword	29479
	.hword	23683
	.hword	7796
	.hword	-10576
	.hword	-19906
	.hword	29171
	.hword	-8203
	.hword	23610
	.hword	22025
	.hword	-13961
	.hword	-14870
	.hword	-6248
	.hword	-8952
	.hword	0
	.hword	0
	.hword	7651
	.hword	-14449
	.hword	7645
	.hword	-8739
	.hword	-9216
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	59
	.hword	-17477
	.hword	-18725
	.hword	28086
	.hword	-9241
	.hword	-12949
	.hword	23254
	.hword	-19025
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6208
	.hword	0
	.hword	3
	.hword	-17288
	.hword	-3613
	.hword	-17477
	.hword	-17477
	.hword	-32768
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	1911
	.hword	30582
	.hword	-9363
	.hword	-18725
	.hword	31993
	.hword	-21141
	.hword	23254
	.hword	-18957
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-2048
	.hword	0
	.hword	119
	.hword	-28898
	.hword	15479
	.hword	30583
	.hword	30576
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	-4370
	.hword	-4389
	.hword	28086
	.hword	-9361
	.hword	-24779
	.hword	-21141
	.hword	23254
	.hword	-16772
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	0
	.hword	0
	.hword	3825
	.hword	-7225
	.hword	-28946
	.hword	-4370
	.hword	-4608
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	29
	.hword	-8739
	.hword	-9363
	.hword	-18725
	.hword	28147
	.hword	-6475
	.hword	-21141
	.hword	23255
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3104
	.hword	0
	.hword	1
	.hword	-8644
	.hword	30961
	.hword	-8739
	.hword	-8739
	.hword	-16384
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	955
	.hword	-17477
	.hword	28086
	.hword	-9363
	.hword	-16772
	.hword	-10571
	.hword	-21141
	.hword	23289
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31744
	.hword	0
	.hword	59
	.hword	-16258
	.hword	512
	.hword	599
	.hword	229
	.hword	-28850
	.hword	-11275
	.hword	19144
	.hword	-17594
	.hword	-17413
	.hword	12295
	.hword	-12136
	.hword	-23958
	.hword	31309
	.hword	-32354
	.hword	-21813
	.hword	15570
	.hword	20162
	.hword	17720
	.hword	22754
	.hword	3071
	.hword	-18724
	.hword	-425
	.hword	23134
	.hword	-29073
	.hword	12825
	.hword	11129
	.hword	12221
	.hword	17828
	.hword	17146
	.hword	19487
	.hword	22913
	.hword	-20452
	.hword	26112
	.hword	-7734
	.hword	22312
	.hword	20312
	.hword	7849
	.hword	27384
	.hword	-22221
	.hword	-23847
	.hword	-5514
	.hword	26410
	.hword	-12509
	.hword	10525
	.hword	18920
	.hword	22753
	.hword	18688
	.hword	0
	.hword	29
	.hword	-7225
	.hword	-28899
	.hword	-8739
	.hword	-8740
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	15291
	.hword	-17482
	.hword	-9363
	.hword	-18725
	.hword	-6195
	.hword	27482
	.hword	-10571
	.hword	-20577
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-16384
	.hword	0
	.hword	956
	.hword	30961
	.hword	-7237
	.hword	-17477
	.hword	-17536
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	7
	.hword	30583
	.hword	30427
	.hword	28086
	.hword	-9348
	.hword	-1619
	.hword	27482
	.hword	-10571
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31992
	.hword	0
	.hword	0
	.hword	30607
	.hword	7740
	.hword	30583
	.hword	30583
	.hword	28672
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	238
	.hword	-4370
	.hword	-9363
	.hword	-18725
	.hword	28575
	.hword	13741
	.hword	27482
	.hword	-10562
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-6193
	.hword	-24832
	.hword	0
	.hword	14
	.hword	-3613
	.hword	-14450
	.hword	-4370
	.hword	-4370
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	7645
	.hword	-8741
	.hword	28086
	.hword	-9363
	.hword	-3098
	.hword	-19027
	.hword	27482
	.hword	-10289
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	-1549
	.hword	-8192
	.hword	0
	.hword	478
	.hword	15480
	.hword	-3619
	.hword	-8739
	.hword	-8768
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	0
	.hword	3
	.hword	-17477
	.hword	-17555
	.hword	-18725
	.hword	28094
	.hword	31958
	.hword	-19027
	.hword	27482
	.hword	-1549
	.hword	-6193
	.hword	-24770
	.hword	31993
	.hword	-3097
	.hword	-12385
	.hword	15996
	.hword	0
	.hword	0
	.hword	15296
	.hword	29987
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_aop_rm, %object
	.size	g_aop_rm, 128
g_aop_rm:
	.zero	128
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
