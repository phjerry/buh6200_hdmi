#include "hi_asm_define.h"
	.arch armv8-a
	.file	"aoe_aip.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_calc_proc_block_size, %function
aip_calc_proc_block_size:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w19, w0
	mov	w21, w2
	mov	x0, x30
	mov	w20, w1
	bl	_mcount
	mov	w1, w21
	mov	w0, w20
	bl	util_calc_frame_size
	add	w1, w19, w19, lsl 2
	mov	w2, 19923
	movk	w2, 0x1062, lsl 16
	mov	w3, 25856
	movk	w3, 0x4, lsl 16
	cmp	w19, w3
	umull	x1, w1, w2
	mov	w3, 48000
	ccmp	w19, w3, 4, ne
	lsr	x1, x1, 38
	bne	.L2
	and	w3, w1, 255
	add	w2, w1, 256
	sub	w2, w2, w3
	cmp	w3, 0
	csel	w1, w2, w1, ne
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	mul	w0, w1, w0
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L2:
	and	w2, w1, -4
	tst	x1, 3
	add	w2, w2, 4
	ldr	x21, [sp, 32]
	csel	w1, w2, w1, ne
	ldp	x19, x20, [sp, 16]
	mul	w0, w1, w0
	ldp	x29, x30, [sp], 48
	ret
	.size	aip_calc_proc_block_size, .-aip_calc_proc_block_size
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_update_fifo_latency, %function
aip_update_fifo_latency:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 184]
	bl	rbuf_query_busy
	mov	w20, w0
	ldr	w0, [x19, 20]
	cmp	w0, 1
	bne	.L10
	ldr	w0, [x19, 444]
	add	w20, w20, w0
.L10:
	ldr	x0, [x19, 408]
	cbz	x0, .L11
	bl	sonic_get_left_bytes
	add	w20, w20, w0
.L11:
	ldr	w2, [x19, 112]
	mov	w0, w20
	ldr	w1, [x19, 320]
	bl	util_latency_ms
	ldr	w1, [x21, 4]
	bfi	w1, w0, 18, 12
	str	w1, [x21, 4]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	aip_update_fifo_latency, .-aip_update_fifo_latency
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_update_track_latency, %function
aip_update_track_latency:
	stp	x29, x30, [sp, -96]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	add	x1, x29, 60
	mov	w0, 0
	bl	aop_get_chn_status
	ldr	x0, [x19, 184]
	bl	rbuf_query_busy
	ldr	w2, [x19, 112]
	ldr	w1, [x19, 320]
	bl	util_latency_ms
	mov	w20, w0
	ldr	x0, [x19, 176]
	bl	rbuf_query_busy
	ldr	w1, [x19, 324]
	ldr	w2, [x19, 32]
	bl	util_latency_ms
	add	w0, w20, w0
	ldr	w1, [x29, 92]
	add	w0, w0, w1
	cmp	w0, 999
	bls	.L18
	ldr	w0, [x21, 56]
	orr	w0, w0, 67043328
	str	w0, [x21, 56]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L18:
	ldr	w1, [x21, 56]
	bfi	w1, w0, 16, 10
	str	w1, [x21, 56]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 96
	ret
	.size	aip_update_track_latency, .-aip_update_track_latency
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_update_chn_reg, %function
aip_update_chn_reg:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	uxtw	x0, w19
	adrp	x1, .LANCHOR0
	add	x2, x0, 8
	add	x1, x1, :lo12:.LANCHOR0
	ldr	x19, [x1, x2, lsl 3]
	ldr	x20, [x1, x0, lsl 3]
	mov	x0, x19
	mov	x1, x20
	bl	aip_update_fifo_latency
	mov	x1, x20
	mov	x0, x19
	bl	aip_update_track_latency
	str	xzr, [x19, 456]
	mov	w0, 6
	str	w0, [x19, 448]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_update_chn_reg, .-aip_update_chn_reg
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_destroy_chn.part.3, %function
aip_destroy_chn.part.3:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	mov	x0, x19
	str	wzr, [x19, 216]
	bl	aip_sonic_deinit
	ldr	x0, [x19, 176]
	cbz	x0, .L21
	bl	rbuf_destroy
.L21:
	ldr	x0, [x19, 184]
	cbz	x0, .L22
	bl	rbuf_destroy
.L22:
	ldr	x0, [x19, 208]
	cbz	x0, .L23
	bl	fade_destroy
.L23:
	ldr	x0, [x19, 192]
	cbz	x0, .L24
	bl	lsrc_destroy
.L24:
	ldr	x0, [x19, 200]
	cbz	x0, .L25
	bl	resampler_free
.L25:
	ldr	x1, [x19, 304]
	cbz	x1, .L26
	mov	w0, 70
	bl	HI_ADSP_FREE
.L26:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_destroy_chn.part.3, .-aip_destroy_chn.part.3
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_reclaim_fifo, %function
aip_reclaim_fifo:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 184]
	cbz	x0, .L45
	bl	rbuf_query_busy
	ldr	w1, [x19, 316]
	cmp	w0, w1
	bcc	.L60
	stp	x20, x21, [x29, 24]
	bl	aoe_get_wk_buf
	mov	x1, x0
	mov	x21, x0
	ldr	x0, [x19, 184]
	ldr	w20, [x19, 316]
	mov	w2, w20
	bl	rbuf_read_data
	cmp	w20, w0
	bne	.L50
	ldr	w0, [x19, 344]
	cbnz	w0, .L61
.L49:
	ldr	x3, [x19, 456]
	ldr	w2, [x19, 316]
	ldr	w0, [x19, 448]
	cbz	x3, .L50
	mov	x1, x21
	blr	x3
.L50:
	ldr	x0, [x19, 184]
	bl	rbuf_flush
	ldp	x20, x21, [x29, 24]
.L45:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L60:
	ldr	x0, [x19, 184]
	bl	rbuf_flush
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L61:
	ldr	x0, [x19, 208]
	mov	w2, 6
	mov	w1, 2
	bl	fade_active
	ldr	x0, [x19, 208]
	mov	w2, 256
	mov	x1, x21
	bl	fade_out_process
	b	.L49
	.size	aip_reclaim_fifo, .-aip_reclaim_fifo
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_pcm_type
	.type	aip_get_pcm_type, %function
aip_get_pcm_type:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	x20, x1
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	x0, [x0, w19, uxtw 3]
	cbz	x0, .L65
	ldr	w0, [x0]
	ubfx	x0, x0, 11, 3
	str	w0, [x20]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L65:
	mov	w0, 5
	str	w0, [x20]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_get_pcm_type, .-aip_get_pcm_type
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_rbf_fifo_attr
	.type	aip_get_rbf_fifo_attr, %function
aip_get_rbf_fifo_attr:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x23, [sp, 48]
	mov	x19, x1
	mov	w20, w0
	mov	x0, x30
	bl	_mcount
	adrp	x1, .LANCHOR0
	add	x1, x1, :lo12:.LANCHOR0
	add	x0, x19, 104
	ldr	x20, [x1, w20, uxtw 3]
	ldr	w1, [x20, 4]
	and	w1, w1, 3
	bl	aoe_reg_read_bit_depth
	cbnz	w0, .L69
	ldr	w1, [x20, 4]
	add	x0, x19, 108
	ubfx	x1, x1, 2, 2
	bl	aoe_reg_read_channel
	cbnz	w0, .L69
	ldr	w1, [x20, 4]
	add	x0, x19, 112
	ubfx	x1, x1, 4, 4
	bl	aoe_reg_read_sampel_rate
	cbnz	w0, .L69
	ldr	w1, [x20, 4]
	add	x0, x19, 116
	ubfx	x1, x1, 8, 2
	bl	aoe_reg_read_data_format
	cbnz	w0, .L69
	ldr	w1, [x20, 4]
	add	x0, x29, 76
	ubfx	x1, x1, 11, 7
	bl	aoe_reg_read_buf_latency_ms
	mov	w23, w0
	cbnz	w0, .L69
	ldr	w0, [x29, 76]
	cmp	w0, 80
	bhi	.L69
	stp	x21, x22, [x29, 32]
	ldp	w1, w0, [x19, 104]
	bl	util_calc_frame_size
	str	wzr, [x19, 120]
	ldr	w2, [x19, 112]
	mov	w22, w0
	str	wzr, [x19, 160]
	mov	w1, w0
	mov	w0, 80
	bl	util_latency_ms2_byte_size
	str	w0, [x19, 136]
	ldr	w2, [x29, 76]
	add	x0, x20, 68
	str	w2, [x19, 172]
	add	x20, x20, 72
	stp	x0, x20, [x19, 144]
	mov	w0, w2
	ldr	w2, [x19, 112]
	mov	w1, w22
	str	w22, [x19, 164]
	bl	util_latency_ms2_byte_size
	str	w0, [x19, 168]
	mov	w0, w23
	ldp	x21, x22, [x29, 32]
	ldp	x19, x20, [sp, 16]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L69:
	mov	w23, -1
	mov	w0, w23
	ldp	x19, x20, [sp, 16]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 80
	ret
	.size	aip_get_rbf_fifo_attr, .-aip_get_rbf_fifo_attr
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_rbf_in_attr
	.type	aip_get_rbf_in_attr, %function
aip_get_rbf_in_attr:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x20, x1
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x1, .LANCHOR0
	add	x1, x1, :lo12:.LANCHOR0
	add	x0, x20, 24
	ldr	x19, [x1, w19, uxtw 3]
	ldr	w1, [x19]
	and	w1, w1, 3
	bl	aoe_reg_read_bit_depth
	cbnz	w0, .L73
	ldr	w1, [x19]
	add	x0, x20, 28
	ubfx	x1, x1, 2, 3
	bl	aoe_reg_read_channel
	cbnz	w0, .L73
	ldr	w1, [x19]
	add	x0, x20, 32
	ubfx	x1, x1, 5, 4
	bl	aoe_reg_read_sampel_rate
	cbz	w0, .L76
	ldr	w0, [x19]
	ubfx	x0, x0, 5, 4
	cmp	w0, 15
	beq	.L80
.L76:
	ldr	w1, [x19]
	add	x0, x20, 36
	ubfx	x1, x1, 9, 2
	bl	aoe_reg_read_data_format
	cbnz	w0, .L73
	ldr	w1, [x19]
	mov	x0, x20
	ubfx	x1, x1, 11, 3
	bl	aoe_reg_read_pcm_type
	mov	w21, w0
	cbnz	w0, .L73
	ldr	w2, [x19]
	ldp	w1, w0, [x20, 24]
	ubfx	x2, x2, 14, 1
	str	w2, [x20, 4]
	bl	util_calc_frame_size
	mov	w1, 1
	str	w1, [x20, 48]
	mov	x2, 0
	add	x1, x20, 48
	ldr	w4, [x19, 16]
	ldr	w3, [x19, 20]
	bfi	x2, x4, 0, 32
	bfi	x2, x3, 32, 32
	str	x2, [x1, 8]
	ldr	w2, [x19, 24]
	ubfx	x2, x2, 0, 24
	str	w2, [x1, 16]
	ldr	w3, [x19, 24]
	tbnz	x3, 24, .L81
	add	x4, x19, 28
	add	x3, x19, 36
	stp	x4, x3, [x1, 24]
.L78:
	ldr	w3, [x19, 24]
	mov	x4, 16
	stp	w0, w2, [x1, 44]
	movk	x4, 0x8, lsl 32
	str	wzr, [x1, 52]
	mov	w2, 2
	ubfx	x0, x3, 24, 1
	str	w0, [x1, 40]
	str	x4, [x20, 12]
	mov	w0, w21
	str	w2, [x20, 20]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L81:
	ldr	w6, [x19, 28]
	mov	x4, 0
	ldr	w5, [x19, 32]
	mov	x3, 0
	bfi	x4, x6, 0, 32
	bfi	x4, x5, 32, 32
	str	x4, [x1, 24]
	ldr	w5, [x19, 36]
	ldr	w4, [x19, 40]
	bfi	x3, x5, 0, 32
	bfi	x3, x4, 32, 32
	str	x3, [x1, 32]
	b	.L78
	.p2align 2
.L80:
	ldr	w0, [x19, 12]
	mov	w1, 56928
	movk	w1, 0x2, lsl 16
	ubfx	x0, x0, 0, 24
	str	w0, [x20, 32]
	sub	w0, w0, #4000
	cmp	w0, w1
	bhi	.L73
	ldr	w0, [x20, 28]
	cmp	w0, 2
	bhi	.L73
	ldr	w0, [x20, 24]
	cmp	w0, 16
	beq	.L76
.L73:
	mov	w21, -1
	mov	w0, w21
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	aip_get_rbf_in_attr, .-aip_get_rbf_in_attr
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_chn_create_attr
	.type	aip_get_chn_create_attr, %function
aip_get_chn_create_attr:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	x20, x1
	mov	x0, x30
	bl	_mcount
	mov	x1, x20
	mov	w0, w19
	bl	aip_get_rbf_in_attr
	cbnz	w0, .L82
	mov	x1, x20
	mov	w0, w19
	bl	aip_get_rbf_fifo_attr
.L82:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_get_chn_create_attr, .-aip_get_chn_create_attr
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_is_chn_attr_change
	.type	aip_is_chn_attr_change, %function
aip_is_chn_attr_change:
	stp	x29, x30, [sp, -208]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x2, .LANCHOR0
	add	x2, x2, :lo12:.LANCHOR0
	add	x2, x2, x19, uxtw 3
	mov	w0, w19
	add	x1, x29, 32
	ldr	x19, [x2, 64]
	bl	aip_get_chn_create_attr
	ldp	w0, w5, [x29, 56]
	ldp	w6, w7, [x19, 24]
	ldp	w4, w2, [x19, 32]
	cmp	w6, w0
	ldp	w3, w1, [x29, 64]
	cset	w0, ne
	cmp	w7, w5
	cinc	w0, w0, ne
	cmp	w4, w3
	ldp	w6, w4, [x19]
	cinc	w0, w0, ne
	ldp	w5, w3, [x29, 32]
	cmp	w2, w1
	cinc	w0, w0, ne
	ldr	x2, [x19, 56]
	ldr	x1, [x29, 88]
	cmp	w6, w5
	cinc	w0, w0, ne
	ldr	w6, [x19, 64]
	ldr	w5, [x29, 96]
	cmp	w4, w3
	cinc	w0, w0, ne
	cmp	x2, x1
	ldr	w4, [x19, 88]
	cinc	w0, w0, ne
	ldr	w3, [x29, 120]
	cmp	w6, w5
	ldp	w2, w6, [x19, 104]
	cinc	w0, w0, ne
	ldp	w1, w5, [x29, 136]
	cmp	w4, w3
	cinc	w0, w0, ne
	ldr	w4, [x19, 112]
	ldr	w3, [x29, 144]
	cmp	w2, w1
	cinc	w0, w0, ne
	ldr	w2, [x19, 116]
	ldr	w1, [x29, 148]
	cmp	w6, w5
	cinc	w0, w0, ne
	cmp	w4, w3
	cinc	w0, w0, ne
	cmp	w2, w1
	ldr	x19, [sp, 16]
	cinc	w0, w0, ne
	ldp	x29, x30, [sp], 208
	ret
	.size	aip_is_chn_attr_change, .-aip_is_chn_attr_change
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_create_chn
	.type	aip_create_chn, %function
aip_create_chn:
	stp	x29, x30, [sp, -288]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w20, w0
	mov	x0, x30
	bl	_mcount
	stp	xzr, xzr, [x29, 48]
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x1, x29, 112
	stp	xzr, xzr, [x29, 64]
	ldr	x21, [x0, w20, uxtw 3]
	mov	w0, w20
	bl	aip_get_chn_create_attr
	cbnz	w0, .L88
	mov	w1, 472
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x19, x0
	cbz	x0, .L88
	mov	x2, 472
	mov	w1, 0
	bl	adsp_memset
	mov	x2, 176
	add	x1, x29, 112
	mov	x0, x19
	bl	adsp_memcpy
	str	w20, [x19, 328]
	add	x0, x29, 64
	add	x1, x29, 160
	bl	rbuf_create_ext
	ldr	x0, [x29, 64]
	cbz	x0, .L89
	ldr	w1, [x29, 148]
	str	x0, [x19, 176]
	cbz	w1, .L126
	str	wzr, [x21, 60]
	ldr	w0, [x21, 64]
	and	w0, w0, -65536
	str	w0, [x21, 64]
.L91:
	mov	w1, 768
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	str	x0, [x19, 304]
	cbz	x0, .L89
	ldr	w0, [x29, 148]
	cbz	w0, .L92
	ldr	w1, [x19]
.L93:
	ldr	w0, [x21, 8]
	cmp	w1, 4
	ubfx	x0, x0, 15, 1
	str	w0, [x19, 344]
	beq	.L127
.L103:
	mov	w0, 6144
	str	w0, [x19, 468]
.L104:
	add	x20, x19, 512
	str	wzr, [x19, 220]
	str	xzr, [x19, 224]
	mov	x0, 256
	movk	x0, 0x11, lsl 32
	str	x0, [x20, -132]
	str	wzr, [x19, 272]
	ldp	w2, w1, [x19, 24]
	ldr	w0, [x19, 32]
	bl	aip_calc_proc_block_size
	str	w0, [x19, 312]
	ldp	w2, w1, [x19, 104]
	ldr	w0, [x19, 112]
	bl	aip_calc_proc_block_size
	str	w0, [x19, 316]
	ldr	w1, [x19, 104]
	ldr	w0, [x19, 108]
	bl	util_calc_frame_size
	str	w0, [x19, 320]
	ldr	w1, [x19, 24]
	ldr	w0, [x19, 28]
	bl	util_calc_frame_size
	str	w0, [x19, 324]
	mov	x0, 121
	mov	w1, 1
	str	w1, [x19, 216]
	movk	x0, 0x79, lsl 32
	str	wzr, [x19, 248]
	str	x0, [x19, 252]
	mov	x0, x19
	str	xzr, [x20, -252]
	str	xzr, [x19, 440]
	bl	aip_sonic_init
	ldr	x21, [sp, 32]
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 288
	ret
	.p2align 2
.L88:
	mov	x19, 0
	ldr	x21, [sp, 32]
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 288
	ret
	.p2align 2
.L92:
	ldr	w0, [x29, 144]
	ldr	w1, [x29, 224]
	cmp	w0, w1
	beq	.L94
	mov	w2, 32000
	cmp	w0, w2
	beq	.L95
	bhi	.L97
	mov	w2, 12000
	cmp	w0, w2
	beq	.L95
	bls	.L95
	.p2align 3
.L95:
	mov	w2, 1
	ldr	w3, [x29, 216]
	str	w2, [x19, 332]
	mov	w4, 0
	ldr	w2, [x29, 220]
	bl	resampler_create
	cbnz	x0, .L128
.L89:
	mov	x0, x19
	mov	x19, 0
	bl	aip_destroy_chn.part.3
	ldr	x21, [sp, 32]
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 288
	ret
	.p2align 2
.L126:
	add	x0, x29, 72
	add	x1, x29, 232
	bl	rbuf_create
	ldr	x0, [x29, 72]
	cbz	x0, .L89
	str	x0, [x19, 184]
	add	x1, x29, 80
	bl	rbuf_query_info
	ldrh	w1, [x29, 96]
	ldr	w0, [x21, 64]
	bfi	w0, w1, 0, 16
	str	w0, [x21, 64]
	b	.L91
	.p2align 2
.L128:
	ldr	w1, [x29, 224]
	str	x0, [x19, 200]
.L94:
	ldp	w3, w4, [x29, 216]
	add	x0, x29, 48
	mov	w2, w1
	bl	lsrc_create
	ldr	x0, [x29, 48]
	cbz	x0, .L89
	str	x0, [x19, 192]
	str	wzr, [x21, 80]
	ldr	w0, [x19]
	cbnz	w0, .L129
	ldr	w0, [x21, 8]
	ubfx	x0, x0, 15, 1
	str	w0, [x19, 344]
	b	.L103
	.p2align 2
.L97:
	mov	w2, 22664
	movk	w2, 0x1, lsl 16
	cmp	w0, w2
	beq	.L95
	bls	.L95
	b	.L95
	.p2align 2
.L127:
	mov	w0, 24576
	str	w0, [x19, 468]
	b	.L104
	.p2align 2
.L129:
	ldp	w2, w3, [x29, 136]
	add	x0, x29, 56
	ldr	w1, [x29, 144]
	bl	fade_create
	ldr	x0, [x29, 56]
	cbz	x0, .L89
	ldr	w1, [x19]
	str	x0, [x19, 208]
	str	wzr, [x19, 288]
	b	.L93
	.size	aip_create_chn, .-aip_create_chn
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_start_chn_cmd_proc, %function
aip_start_chn_cmd_proc:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	uxtw	x21, w0
	adrp	x19, .LANCHOR0
	add	x24, x21, 8
	add	x22, x19, :lo12:.LANCHOR0
	mov	x0, x30
	bl	_mcount
	mov	x23, x21
	ldr	x20, [x22, x24, lsl 3]
	cbz	x20, .L144
	ldr	w0, [x20, 216]
	cbz	w0, .L145
	cmp	w0, 2
	beq	.L146
	ldr	x1, [x22, x21, lsl 3]
.L137:
	ldr	w0, [x1, 8]
	tbnz	x0, 3, .L130
	ldr	w0, [x1, 8]
	and	w0, w0, -241
	str	w0, [x1, 8]
	ldr	w0, [x1, 8]
	orr	w0, w0, 8
	str	w0, [x1, 8]
.L130:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L145:
	mov	w0, w21
	bl	aip_is_chn_attr_change
	cbnz	w0, .L147
	ldr	x0, [x22, x24, lsl 3]
	mov	w1, 1
	str	w1, [x20, 216]
.L132:
	add	x19, x19, :lo12:.LANCHOR0
	ldr	x1, [x19, x21, lsl 3]
	cbnz	x0, .L137
	ldr	w0, [x1, 8]
	tbnz	x0, 3, .L130
	ldr	w0, [x1, 8]
	mov	w2, 1
	bfi	w0, w2, 4, 4
	str	w0, [x1, 8]
	ldr	w0, [x1, 8]
	orr	w0, w0, 8
	str	w0, [x1, 8]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L147:
	ldr	x0, [x22, x24, lsl 3]
	cbz	x0, .L135
	bl	aip_destroy_chn.part.3
.L135:
	mov	w0, w23
	bl	aip_create_chn
	add	x1, x19, :lo12:.LANCHOR0
	add	x2, x21, 8
	str	x0, [x1, x2, lsl 3]
	b	.L132
	.p2align 2
.L146:
	ldr	x0, [x20, 208]
	mov	w1, 1
	str	w1, [x20, 216]
	mov	w2, 8
	bl	fade_active
	ldr	x0, [x22, x24, lsl 3]
	b	.L132
	.p2align 2
.L144:
	mov	w0, w21
	bl	aip_create_chn
	str	x0, [x22, x24, lsl 3]
	b	.L132
	.size	aip_start_chn_cmd_proc, .-aip_start_chn_cmd_proc
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_flush_chn_ctx
	.type	aip_flush_chn_ctx, %function
aip_flush_chn_ctx:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 200]
	cbz	x0, .L149
	bl	resample_flush
.L149:
	ldr	x0, [x19, 192]
	cbz	x0, .L150
	bl	lsrc_flush
.L150:
	ldr	x0, [x19, 208]
	str	wzr, [x19, 296]
	cbz	x0, .L148
	mov	w2, 8
	mov	w1, 1
	bl	fade_active
.L148:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_flush_chn_ctx, .-aip_flush_chn_ctx
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_flush_chn, %function
aip_flush_chn:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	tbnz	x19, 0, .L179
.L162:
	tbz	x19, 1, .L163
	ldr	x0, [x20, 176]
	cbz	x0, .L163
	bl	rbuf_flush
.L163:
	tbnz	x19, 2, .L180
.L161:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L179:
	mov	x0, x20
	bl	aip_flush_chn_ctx
	b	.L162
	.p2align 2
.L180:
	mov	x0, x20
	bl	aip_sonic_flush
	ldr	x0, [x20, 184]
	cbz	x0, .L161
	bl	rbuf_flush
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_flush_chn, .-aip_flush_chn
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_pause_chn_cmd_proc, %function
aip_pause_chn_cmd_proc:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	uxtw	x19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x21, .LANCHOR0
	add	x1, x19, 8
	add	x0, x21, :lo12:.LANCHOR0
	ldr	x20, [x0, x1, lsl 3]
	cbz	x20, .L181
	str	x22, [x29, 40]
	ldr	w22, [x20, 216]
	cmp	w22, 1
	beq	.L187
.L184:
	add	x21, x21, :lo12:.LANCHOR0
	mov	w0, 2
	str	w0, [x20, 20]
	str	wzr, [x20, 464]
	ldr	x0, [x21, x19, lsl 3]
	ldr	w1, [x0, 8]
	tbnz	x1, 3, .L186
	ldr	w1, [x0, 8]
	and	w1, w1, -241
	str	w1, [x0, 8]
	ldr	w1, [x0, 8]
	orr	w1, w1, 8
	str	w1, [x0, 8]
	ldr	x22, [x29, 40]
.L181:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L186:
	ldr	x22, [x29, 40]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L187:
	mov	x0, x20
	bl	aip_reclaim_fifo
	mov	x0, x20
	mov	w1, w22
	bl	aip_flush_chn
	mov	w0, 2
	str	w0, [x20, 216]
	b	.L184
	.size	aip_pause_chn_cmd_proc, .-aip_pause_chn_cmd_proc
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_flush_chn_cmd_proc, %function
aip_flush_chn_cmd_proc:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	uxtw	x20, w0
	mov	x0, x30
	bl	_mcount
	adrp	x21, .LANCHOR0
	add	x1, x20, 8
	add	x0, x21, :lo12:.LANCHOR0
	ldr	x19, [x0, x1, lsl 3]
	cbz	x19, .L188
	str	x22, [x29, 40]
	ldr	w22, [x19, 216]
	cmp	w22, 1
	beq	.L195
	cmp	w22, 2
	beq	.L196
	add	x21, x21, :lo12:.LANCHOR0
	mov	w0, 2
	str	w0, [x19, 20]
	str	wzr, [x19, 464]
	ldr	x0, [x21, x20, lsl 3]
	ldr	w1, [x0, 8]
	tbnz	x1, 3, .L194
.L197:
	ldr	w1, [x0, 8]
	and	w1, w1, -241
	str	w1, [x0, 8]
	ldr	w1, [x0, 8]
	orr	w1, w1, 8
	str	w1, [x0, 8]
	ldr	x22, [x29, 40]
.L188:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L194:
	ldr	x22, [x29, 40]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L196:
	add	x21, x21, :lo12:.LANCHOR0
	mov	w1, 7
	mov	x0, x19
	bl	aip_flush_chn
	mov	w0, 2
	str	w0, [x19, 20]
	ldr	x0, [x21, x20, lsl 3]
	str	wzr, [x19, 464]
	ldr	w1, [x0, 8]
	tbz	x1, 3, .L197
	b	.L194
	.p2align 2
.L195:
	mov	x0, x19
	bl	aip_reclaim_fifo
	mov	x0, x19
	mov	w1, 3
	bl	aip_flush_chn
	add	x21, x21, :lo12:.LANCHOR0
	ldr	x0, [x19, 208]
	mov	w1, w22
	mov	w2, 8
	bl	fade_active
	str	wzr, [x19, 464]
	mov	w0, 2
	str	w0, [x19, 20]
	ldr	x0, [x21, x20, lsl 3]
	ldr	w1, [x0, 8]
	tbz	x1, 3, .L197
	b	.L194
	.size	aip_flush_chn_cmd_proc, .-aip_flush_chn_cmd_proc
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aip_stop_chn_cmd_proc, %function
aip_stop_chn_cmd_proc:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	uxtw	x19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x22, .LANCHOR0
	add	x1, x19, 8
	add	x0, x22, :lo12:.LANCHOR0
	ldr	x21, [x0, x1, lsl 3]
	ldr	x20, [x0, x19, lsl 3]
	cmp	x21, 0
	ccmp	x20, 0, 4, ne
	beq	.L198
	ldr	w0, [x21, 216]
	sub	w1, w0, #1
	cmp	w1, 1
	bls	.L204
	ldr	w0, [x20, 8]
	tbnz	x0, 3, .L198
.L206:
	ldr	w0, [x20, 8]
	and	w0, w0, -241
	str	w0, [x20, 8]
	ldr	w0, [x20, 8]
	orr	w0, w0, 8
	str	w0, [x20, 8]
.L198:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L204:
	cmp	w0, 1
	beq	.L205
.L202:
	add	x22, x22, :lo12:.LANCHOR0
	mov	x0, x21
	mov	w1, 3
	bl	aip_flush_chn
	mov	w0, 2
	str	w0, [x21, 20]
	str	wzr, [x21, 216]
	str	wzr, [x20, 80]
	ldr	x20, [x22, x19, lsl 3]
	str	xzr, [x21, 440]
	str	wzr, [x21, 464]
	ldr	w0, [x20, 8]
	tbz	x0, 3, .L206
	b	.L198
	.p2align 2
.L205:
	mov	x0, x21
	bl	aip_reclaim_fifo
	b	.L202
	.size	aip_stop_chn_cmd_proc, .-aip_stop_chn_cmd_proc
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_flush_chn_ruf_in
	.type	aip_flush_chn_ruf_in, %function
aip_flush_chn_ruf_in:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 176]
	cbz	x0, .L207
	bl	rbuf_flush
.L207:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_flush_chn_ruf_in, .-aip_flush_chn_ruf_in
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_flush_chn_fifo
	.type	aip_flush_chn_fifo, %function
aip_flush_chn_fifo:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 184]
	cbz	x0, .L212
	bl	rbuf_flush
.L212:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_flush_chn_fifo, .-aip_flush_chn_fifo
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_chn_cmd
	.type	aip_get_chn_cmd, %function
aip_get_chn_cmd:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	x3, [x0, w20, uxtw 3]
	ldr	w0, [x3, 8]
	str	w0, [x29, 44]
	ldr	w0, [x29, 44]
	tbnz	x0, 3, .L223
	ldr	w2, [x29, 44]
	and	w2, w2, 3
	cmp	w2, 1
	beq	.L220
	mov	w0, 0
	cbz	w2, .L218
	mov	w0, 2
	cmp	w2, w0
	beq	.L218
	mov	w0, 3
	cmp	w2, w0
	bne	.L229
.L218:
	str	w0, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L223:
	mov	w0, 4
	str	w0, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L229:
	ldr	w1, [x3, 8]
	mov	w2, 4
	mov	w0, w2
	bfi	w1, w2, 4, 4
	str	w1, [x3, 8]
	ldr	w1, [x3, 8]
	orr	w1, w1, 8
	str	w1, [x3, 8]
	str	w0, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L220:
	mov	w0, w2
	str	w0, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	aip_get_chn_cmd, .-aip_get_chn_cmd
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_ack_chn_cmd
	.type	aip_ack_chn_cmd, %function
aip_ack_chn_cmd:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	x0, [x0, w19, uxtw 3]
	ldr	w2, [x0, 8]
	tbnz	x2, 3, .L230
	ldr	w2, [x0, 8]
	bfi	w2, w20, 4, 4
	str	w2, [x0, 8]
	ldr	w1, [x0, 8]
	orr	w1, w1, 8
	str	w1, [x0, 8]
.L230:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_ack_chn_cmd, .-aip_ack_chn_cmd
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_init
	.type	aip_init, %function
aip_init:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	x19, x0
	adrp	x20, .LANCHOR0
	add	x22, x19, 768
	add	x20, x20, :lo12:.LANCHOR0
	mov	w23, 121
	mov	w21, 5
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L233:
	str	x19, [x20]
	mov	x0, x19
	mov	x2, 96
	mov	w1, 0
	bl	adsp_memset
	add	x19, x19, 96
	ldr	w0, [x19, -88]
	add	x20, x20, 8
	cmp	x19, x22
	orr	w0, w0, 8
	str	w0, [x19, -88]
	ldr	w0, [x19, -88]
	bfi	w0, w23, 8, 7
	str	w0, [x19, -88]
	ldr	w0, [x19, -96]
	bfi	w0, w21, 11, 3
	str	w0, [x19, -96]
	ldr	w0, [x19, -4]
	orr	w0, w0, 4
	str	w0, [x19, -4]
	str	xzr, [x20, 56]
	bne	.L233
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.size	aip_init, .-aip_init
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_deinit
	.type	aip_deinit, %function
aip_deinit:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	adrp	x20, .LANCHOR0
	add	x20, x20, :lo12:.LANCHOR0
	add	x19, x20, 64
	add	x20, x20, 128
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L237:
	ldr	x1, [x19]
	mov	x0, x1
	cbz	x1, .L236
	bl	aip_destroy_chn.part.3
.L236:
	str	xzr, [x19], 8
	cmp	x19, x20
	bne	.L237
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_deinit, .-aip_deinit
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_process
	.type	aip_process, %function
aip_process:
	stp	x29, x30, [sp, -176]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	mov	w25, 19923
	adrp	x24, .LANCHOR0
	mov	w19, 0
	add	x21, x24, :lo12:.LANCHOR0
	mov	w26, 2
	movk	w25, 0x1062, lsl 16
	mov	x0, x30
	bl	_mcount
	b	.L312
	.p2align 2
.L274:
	mov	w0, w19
	bl	aip_update_chn_reg
.L249:
	add	w19, w19, 1
	add	x21, x21, 8
	cmp	w19, 8
	beq	.L372
.L312:
	add	x1, x29, 144
	mov	w0, w19
	bl	aip_get_chn_cmd
	cmp	w0, 4
	beq	.L243
	ldr	w0, [x29, 144]
	cmp	w0, 1
	beq	.L244
	cbz	w0, .L245
	cmp	w0, 2
	beq	.L246
	cmp	w0, 3
	bne	.L243
	mov	w0, w19
	bl	aip_flush_chn_cmd_proc
	.p2align 3
.L243:
	ldr	x20, [x21, 64]
	mov	x1, x20
	cbz	x20, .L249
	ldr	w22, [x20, 216]
	cmp	w22, 1
	bne	.L274
	ldr	x23, [x21]
	mov	x0, x23
	cbz	x23, .L314
	ldr	w0, [x20, 20]
	cmp	w0, 1
	bls	.L252
	ldr	x0, [x20, 176]
	bl	rbuf_query_busy
	cbz	w0, .L253
	ldr	w5, [x20, 32]
	ldr	w3, [x20, 92]
	ldr	w1, [x23, 84]
	ldr	w2, [x23, 84]
	and	w1, w1, 1023
	ldr	w4, [x20, 20]
	mul	w3, w3, w5
	ubfx	x2, x2, 10, 10
	mul	w1, w1, w3
	mul	w2, w2, w3
	umull	x1, w1, w25
	umull	x2, w2, w25
	lsr	x1, x1, 38
	cmp	w0, w1
	lsr	x2, x2, 38
	ccmp	w4, 2, 0, cc
	beq	.L253
	cmp	w4, 3
	ccmp	w0, w2, 2, eq
	bcc	.L253
	ldr	x0, [x20, 208]
	mov	w2, 8
	str	w22, [x20, 20]
	mov	w1, w22
	bl	fade_active
.L253:
	ldr	x1, [x21, 64]
	ldr	x23, [x21]
	cmp	x1, 0
	mov	x22, x1
	mov	x0, x23
	ccmp	x23, 0, 4, ne
	beq	.L254
.L252:
	ldr	w0, [x23, 92]
	str	w0, [x29, 120]
	ldr	w0, [x29, 120]
	tbnz	x0, 2, .L371
	ldr	w0, [x29, 120]
	ands	w0, w0, 3
	beq	.L257
	cmp	w0, 1
	bne	.L368
.L371:
	mov	x0, x23
	mov	x22, x1
.L254:
	cbz	x22, .L262
.L251:
	ldr	w1, [x0, 84]
	ubfx	x1, x1, 20, 1
	cbz	w1, .L263
	str	w1, [x22, 20]
	ldr	w1, [x0, 84]
	and	w1, w1, -1048577
	str	w1, [x0, 84]
.L263:
	ldr	w1, [x22, 36]
	cbnz	w1, .L373
	ldr	w1, [x0, 8]
	str	w1, [x29, 108]
	ldr	w1, [x0, 48]
	str	w1, [x29, 112]
	ldr	w1, [x0, 76]
	str	w1, [x29, 116]
	str	wzr, [x22, 220]
	ldr	w3, [x29, 112]
	ldr	w2, [x29, 112]
	ldr	w1, [x29, 116]
	and	w3, w3, 255
	ldr	w4, [x29, 116]
	and	w1, w1, 255
	str	w1, [x22, 240]
	ldr	w1, [x29, 116]
	ubfx	x2, x2, 8, 8
	stp	w3, w2, [x22, 224]
	tbnz	x4, 11, .L265
	ubfx	x1, x1, 8, 3
	str	w1, [x22, 244]
.L266:
	ldr	w3, [x29, 112]
	ldr	w2, [x29, 112]
	ldr	w1, [x29, 112]
	ubfx	x3, x3, 16, 1
	str	w3, [x22, 268]
	ubfx	x2, x2, 17, 3
	str	w2, [x22, 272]
	ubfx	x1, x1, 23, 2
	str	w1, [x22, 276]
	cmp	w1, 1
	beq	.L374
	ldr	w1, [x29, 112]
	neg	w1, w1, lsr 25
	str	w1, [x22, 380]
.L268:
	ldr	w1, [x29, 112]
	ldr	w2, [x29, 108]
	ubfx	x1, x1, 20, 3
	str	w1, [x22, 384]
	ldr	w1, [x29, 108]
	tst	x2, 1610612736
	bne	.L269
	ubfx	x1, x1, 24, 5
	str	w1, [x22, 292]
.L270:
	ldr	w3, [x0]
	add	x8, x22, 256
	ldr	w1, [x22, 320]
	ldr	w2, [x22, 112]
	ubfx	x3, x3, 14, 1
	str	w3, [x22, 4]
	ldr	w7, [x0, 4]
	ldr	w6, [x29, 116]
	ldr	w5, [x29, 116]
	ldr	w4, [x29, 116]
	ubfx	x7, x7, 10, 1
	ldr	w3, [x29, 108]
	ubfx	x6, x6, 24, 1
	ubfx	x5, x5, 12, 4
	stp	w7, w6, [x8, 80]
	ubfx	x4, x4, 16, 8
	ubfx	x3, x3, 15, 1
	stp	w3, w5, [x8, 88]
	str	w4, [x22, 352]
	ldr	w0, [x0, 4]
	ubfx	x0, x0, 11, 7
	str	w0, [x22, 172]
	bl	util_latency_ms2_byte_size
	str	w0, [x22, 168]
	ldr	x0, [x22, 184]
	add	x1, x22, 120
	bl	rbuf_update_attr
.L262:
	ldr	w22, [x20, 36]
	cbnz	w22, .L249
	ldr	w1, [x20, 440]
	ldr	w0, [x20, 20]
	cmp	w1, 2
	beq	.L375
	cmp	w0, 1
	bne	.L274
	ldr	w0, [x20, 336]
	cmp	w0, 1
	beq	.L274
	ldr	w5, [x20, 296]
	ldr	w1, [x20, 320]
	ldr	w23, [x20, 316]
	mul	w0, w5, w1
	cmp	w0, w23
	bcs	.L376
	ldr	x0, [x20, 408]
	cbz	x0, .L281
	bl	sonic_get_out_left_bytes
	ldr	w22, [x20, 316]
	cmp	w0, w22
	bcs	.L377
	ldr	w0, [x20, 36]
	mov	w23, w22
	cbz	w0, .L378
.L283:
	ldr	x0, [x20, 176]
	bl	rbuf_query_busy
	cmp	w0, w22
	bcc	.L274
	cbz	w22, .L274
	ldr	x0, [x20, 184]
	cbz	x0, .L274
	mov	w1, w23
	bl	rbuf_check_free
	cmp	w23, 0
	ccmp	w0, 0, 4, ne
	beq	.L274
	bl	aoe_get_wk_buf
	str	x0, [x29, 144]
	mov	x1, x0
	mov	w2, w22
	ldr	w0, [x20, 324]
	udiv	w22, w22, w0
	str	w22, [x29, 156]
	ldr	w0, [x20, 28]
	str	w0, [x29, 164]
	ldr	w0, [x20, 24]
	str	w0, [x29, 160]
	ldr	w0, [x20, 32]
	str	w0, [x29, 168]
	ldr	w0, [x20, 36]
	str	w0, [x29, 172]
	ldr	x0, [x20, 176]
	bl	rbuf_read_data_dma
	str	w0, [x29, 152]
	ldr	w22, [x20, 36]
	cbz	w22, .L379
.L288:
	ldr	w0, [x20, 268]
	cmp	w0, 1
	beq	.L380
.L308:
	ldr	w2, [x20, 296]
	ldr	w0, [x20, 320]
	ldr	w23, [x20, 316]
	mul	w2, w2, w0
	cbnz	w2, .L381
.L309:
	ldr	w0, [x29, 152]
	cbz	w23, .L310
	cmp	w23, w0
	csel	w23, w23, w0, ls
	cbz	w23, .L310
	ldr	x0, [x20, 184]
	mov	w2, w23
	ldr	x1, [x29, 144]
	bl	rbuf_write_data
	ldr	w0, [x29, 152]
.L310:
	subs	w22, w0, w23
	beq	.L311
	ldr	x0, [x20, 304]
	uxtw	x2, w22
	ldr	x1, [x29, 144]
	add	x1, x1, x23, uxtw
	bl	adsp_memcpy
.L311:
	ldr	w1, [x20, 320]
	ldr	w0, [x20, 464]
	add	w0, w0, 1
	str	w0, [x20, 464]
	udiv	w0, w22, w1
	str	w0, [x20, 296]
	b	.L274
	.p2align 2
.L245:
	mov	w0, w19
	bl	aip_start_chn_cmd_proc
	b	.L243
	.p2align 2
.L244:
	mov	w0, w19
	bl	aip_stop_chn_cmd_proc
	b	.L243
	.p2align 2
.L246:
	mov	w0, w19
	bl	aip_pause_chn_cmd_proc
	b	.L243
	.p2align 2
.L314:
	mov	x22, x20
	b	.L251
	.p2align 2
.L372:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x29, x30, [sp], 176
	ret
	.p2align 2
.L373:
	ldr	w0, [x29, 112]
	ubfx	x0, x0, 16, 1
	str	w0, [x22, 268]
	b	.L262
	.p2align 2
.L265:
	tbz	x1, 11, .L266
	ldr	w1, [x29, 116]
	ubfx	x1, x1, 8, 3
	neg	w1, w1
	str	w1, [x22, 244]
	b	.L266
	.p2align 2
.L368:
	ldr	w0, [x23, 92]
	mov	w1, 4
	bfi	w0, w1, 3, 4
	str	w0, [x23, 92]
	ldr	w0, [x23, 92]
	orr	w0, w0, w1
	str	w0, [x23, 92]
	ldr	x0, [x21]
	ldr	x22, [x21, 64]
	b	.L254
	.p2align 2
.L269:
	ubfx	x1, x1, 29, 2
	cmp	w1, 1
	bne	.L270
	ldr	w1, [x29, 108]
	ubfx	x1, x1, 24, 5
	neg	w1, w1
	str	w1, [x22, 292]
	b	.L270
	.p2align 2
.L375:
	cmp	w0, 1
	bne	.L274
	ldr	x0, [x20, 184]
	ldr	w23, [x20, 316]
	cbz	x0, .L275
	mov	w1, w23
	bl	rbuf_check_free
	cbz	w0, .L276
	mov	w22, w23
	ldr	w23, [x20, 316]
.L275:
	cmp	w22, w23
	bcc	.L274
	ldr	w2, [x20, 444]
	mov	x1, 0
	ldr	x0, [x20, 184]
	cmp	w2, w23
	csel	w23, w2, w23, ls
	mov	w2, w23
	bl	rbuf_write_data
	ldr	w0, [x20, 444]
	sub	w23, w0, w23
	str	w23, [x20, 444]
	cbnz	w23, .L274
	ldr	x0, [x20, 208]
	mov	w1, 3
	str	w1, [x20, 440]
	mov	w2, 8
	mov	w1, 1
	bl	fade_active
	b	.L274
	.p2align 2
.L374:
	mov	w1, 256
	str	w1, [x22, 380]
	b	.L268
	.p2align 2
.L257:
	ldr	w0, [x1, 216]
	cbz	w0, .L261
	ldr	w0, [x1, 112]
	ldr	w5, [x1, 320]
	ldr	w3, [x23, 92]
	ldr	w2, [x1, 444]
	ldr	w4, [x1, 36]
	mul	w0, w0, w5
	lsr	w3, w3, 7
	mul	w0, w0, w3
	umull	x0, w0, w25
	lsr	x0, x0, 38
	add	w0, w2, w0
	str	w0, [x1, 444]
	cbz	w4, .L382
	str	w26, [x1, 440]
.L261:
	ldr	w0, [x23, 92]
	and	w0, w0, 127
	str	w0, [x23, 92]
	ldr	w0, [x23, 92]
	and	w0, w0, -121
	str	w0, [x23, 92]
	ldr	w0, [x23, 92]
	orr	w0, w0, 4
	str	w0, [x23, 92]
	ldr	x0, [x21]
	ldr	x22, [x21, 64]
	b	.L254
.L376:
	ldr	x0, [x20, 184]
	cbz	x0, .L274
	mov	w1, w23
	bl	rbuf_check_free
	cmp	w0, 0
	ccmp	w23, 0, 4, ne
	beq	.L274
	ldr	w2, [x20, 316]
	ldr	x0, [x20, 184]
	ldr	x1, [x20, 304]
	bl	rbuf_write_data
	ldr	w1, [x20, 316]
	ldr	w2, [x20, 320]
	ldr	w0, [x20, 296]
	udiv	w1, w1, w2
	sub	w0, w0, w1
	str	w0, [x20, 296]
	b	.L274
.L382:
	ldr	x0, [x1, 208]
	mov	w2, 1
	str	w2, [x1, 440]
	mov	w2, 8
	mov	w1, 2
	bl	fade_active
	b	.L261
.L281:
	ldp	w3, w4, [x20, 104]
	ldr	w2, [x20, 112]
.L313:
	udiv	w22, w23, w1
	ldr	x0, [x20, 192]
	mov	w1, w2
	sub	w22, w22, w5
	bl	lsrc_update_change
	ldr	w1, [x20, 292]
	ldr	x0, [x20, 192]
	bl	lsrc_set_speed_adjust
	ldr	x0, [x20, 192]
	mov	w1, w22
	bl	lsrc_get_min_input_num
	ldr	x2, [x20, 200]
	cbz	x2, .L285
	mov	w1, w0
	mov	x0, x2
	bl	resampler_get_min_input_num
.L285:
	and	w1, w0, -4
	tst	x0, 3
	add	w1, w1, 4
	ldr	w22, [x20, 324]
	csel	w0, w1, w0, ne
	mul	w22, w0, w22
	b	.L283
.L377:
	mov	x0, x20
	bl	aip_dma_sonic_left_frame
	b	.L274
.L276:
	ldr	w23, [x20, 316]
	b	.L275
.L381:
	ldr	x0, [x20, 184]
	sub	w23, w23, w2
	ldr	x1, [x20, 304]
	bl	rbuf_write_data
	b	.L309
.L379:
	str	x27, [x29, 80]
	ldr	w0, [x29, 164]
	ldp	w27, w23, [x20, 104]
	cmp	w23, w0
	beq	.L289
	bls	.L290
	bl	aoe_get_wk_buf
	mov	x1, x0
	add	x2, x29, 144
	mov	w0, w23
	bl	util_upmix_pcm_data
.L289:
	ldr	w0, [x29, 160]
	cmp	w27, w0
	beq	.L291
	bl	aoe_get_wk_buf
	mov	x1, x0
	add	x2, x29, 144
	mov	w0, w27
	bl	util_unify_bit_depth_pcm_data
.L291:
	ldr	x0, [x20, 200]
	cbz	x0, .L292
	ldr	w23, [x20, 112]
	bl	aoe_get_wk_buf
	ldr	w3, [x29, 156]
	mov	x2, x0
	mov	x27, x0
	ldr	w4, [x29, 168]
	ldr	x1, [x29, 144]
	mov	w6, 1
	ldr	x0, [x20, 200]
	mov	w5, w23
	bl	resampler_process_frame
	str	w0, [x29, 156]
	ldr	w1, [x29, 160]
	ldr	w0, [x29, 164]
	str	x27, [x29, 144]
	str	w23, [x29, 168]
	bl	util_calc_frame_size
	ldr	w1, [x29, 156]
	mul	w0, w1, w0
	str	w0, [x29, 152]
.L292:
	ldr	x0, [x20, 192]
	cbz	x0, .L294
	ldr	w2, [x20, 328]
	add	x1, x24, :lo12:.LANCHOR0
	ldr	x23, [x1, x2, lsl 3]
	bl	lsrc_check_do_src
	cbz	w0, .L294
	str	x28, [x29, 88]
	ldr	w28, [x29, 156]
	bl	aoe_get_wk_buf
	ldr	x1, [x29, 144]
	mov	x2, x0
	mov	x27, x0
	ldr	x0, [x20, 192]
	mov	w3, w28
	bl	lsrc_process_frame
	str	w0, [x29, 156]
	ldr	w1, [x29, 160]
	ldr	w0, [x29, 164]
	str	x27, [x29, 144]
	bl	util_calc_frame_size
	ldr	w1, [x29, 156]
	mul	w0, w1, w0
	str	w0, [x29, 152]
	ldr	w0, [x23, 80]
	add	w0, w0, 1
	str	w0, [x23, 80]
	ldr	x28, [x29, 88]
.L294:
	ldr	w0, [x20, 276]
	cmp	w0, 1
	beq	.L383
.L296:
	add	x0, x29, 124
	add	x1, x20, 220
	mov	x2, 20
	str	wzr, [x29, 140]
	stp	xzr, xzr, [x0]
	bl	adsp_memcpy
	ldr	w0, [x20, 240]
	ldr	w1, [x29, 128]
	add	w1, w0, w1
	cmp	w1, 120
	bhi	.L297
	str	wzr, [x29, 128]
.L298:
	ldp	w4, w1, [x29, 132]
	ldr	w3, [x20, 244]
	add	w0, w0, w4
	ldr	w2, [x29, 140]
	cmp	w0, 121
	sub	w0, w0, #121
	csel	w0, wzr, w0, cc
	add	w1, w3, w1
	add	w2, w2, w3
	ldr	w4, [x20, 252]
	stp	w0, w1, [x29, 132]
	str	w2, [x29, 140]
	cmp	w4, w22
	bne	.L301
	ldr	w0, [x20, 260]
	cmp	w0, w1
	beq	.L384
.L301:
	add	x3, x29, 124
	add	x22, x20, 248
	mov	x2, x22
	mov	w1, 0
	add	x0, x29, 144
	bl	util_lr_fade_gain_process
	mov	x2, 20
	add	x1, x29, 124
	mov	x0, x22
	bl	adsp_memcpy
.L302:
	ldr	w1, [x20, 272]
	sub	w0, w1, #1
	cmp	w0, 6
	bls	.L385
.L303:
	mov	x0, x20
	add	x1, x29, 144
	bl	aip_sonic_pcm_data
	ldr	x0, [x20, 208]
	cbz	x0, .L370
	ldr	w1, [x20, 440]
	cmp	w1, 1
	beq	.L386
	cmp	w1, 3
	beq	.L387
	ldr	w1, [x20, 344]
	cbnz	w1, .L388
	mov	w2, 8
	mov	w1, 0
	bl	fade_active
	ldr	x27, [x29, 80]
	b	.L288
.L380:
	ldr	w2, [x29, 152]
	mov	w1, 0
	ldr	x0, [x29, 144]
	bl	adsp_memset
	b	.L308
.L297:
	sub	w22, w1, #121
	str	w22, [x29, 128]
	b	.L298
.L290:
	bl	aoe_get_wk_buf
	mov	x1, x0
	add	x2, x29, 144
	mov	w0, w23
	bl	util_downmix_pcm_data
	b	.L289
.L388:
	ldp	w2, w3, [x29, 160]
	ldr	w1, [x29, 168]
	bl	fade_flush
	ldr	w2, [x29, 156]
	ldr	x0, [x20, 208]
	ldr	x1, [x29, 144]
	bl	fade_in_process
	ldr	x27, [x29, 80]
	b	.L288
.L370:
	ldr	x27, [x29, 80]
	b	.L288
.L383:
	add	x1, x20, 364
	add	x0, x29, 144
	bl	util_dre_16_bit_process
	b	.L296
.L385:
	ldr	w2, [x20, 104]
	add	x0, x29, 144
	bl	util_channel_mode_process
	b	.L303
.L386:
	ldp	w2, w3, [x29, 160]
	ldr	w1, [x29, 168]
	bl	fade_flush
	ldr	w2, [x29, 156]
	ldr	x0, [x20, 208]
	ldr	x1, [x29, 144]
	bl	fade_out_process
	str	w26, [x20, 440]
	ldr	x27, [x29, 80]
	b	.L288
.L387:
	ldp	w2, w3, [x29, 160]
	ldr	w1, [x29, 168]
	bl	fade_flush
	ldr	w2, [x29, 156]
	ldr	x0, [x20, 208]
	ldr	x1, [x29, 144]
	bl	fade_in_process
	str	wzr, [x20, 440]
	ldr	x27, [x29, 80]
	b	.L288
.L384:
	add	x2, x29, 124
	mov	w1, 0
	add	x0, x29, 144
	bl	util_lr_gain_process
	b	.L302
.L378:
	ldp	w3, w4, [x20, 104]
	ldr	w2, [x20, 112]
	ldr	w5, [x20, 296]
	ldr	w1, [x20, 320]
	b	.L313
	.size	aip_process, .-aip_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_chn_status
	.type	aip_get_chn_status, %function
aip_get_chn_status:
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
	cbz	x0, .L392
	ldr	w1, [x0, 216]
	str	w1, [x19]
	ldr	w1, [x0, 4]
	str	w1, [x19, 4]
	ldr	w1, [x0]
	str	w1, [x19, 8]
	ldr	w1, [x0, 468]
	str	w1, [x19, 12]
	ldr	w0, [x0, 336]
	str	w0, [x19, 16]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L392:
	str	wzr, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_get_chn_status, .-aip_get_chn_status
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_check_data_status_stop
	.type	aip_check_data_status_stop, %function
aip_check_data_status_stop:
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
	cbz	x0, .L395
	ldr	w0, [x0, 20]
	ldr	x19, [sp, 16]
	cmp	w0, 2
	cset	w0, eq
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L395:
	mov	w0, 1
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_check_data_status_stop, .-aip_check_data_status_stop
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_query_fifo_info
	.type	aip_query_fifo_info, %function
aip_query_fifo_info:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	x20, x1
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	ldr	x0, [x19, 64]
	cbz	x0, .L400
	ldr	w1, [x0, 336]
	cbz	w1, .L401
	ldr	w1, [x0, 216]
	cmp	w1, 1
	beq	.L402
.L400:
	mov	x2, 32
	mov	w1, 0
	mov	x0, x20
	bl	adsp_memset
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L401:
	ldr	x0, [x0, 184]
	mov	x1, x20
	bl	rbuf_query_info
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L402:
	ldr	x0, [x0, 176]
	mov	x1, x20
	bl	rbuf_query_info
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_query_fifo_info, .-aip_query_fifo_info
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_query_fifo_busy
	.type	aip_query_fifo_busy, %function
aip_query_fifo_busy:
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
	cbz	x0, .L406
	ldr	w1, [x0, 336]
	cbz	w1, .L407
	ldr	x0, [x0, 176]
	bl	rbuf_query_busy
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L407:
	ldr	x0, [x0, 184]
	bl	rbuf_query_busy
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L406:
	mov	w0, 0
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_query_fifo_busy, .-aip_query_fifo_busy
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_query_busy
	.type	aip_query_busy, %function
aip_query_busy:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	mov	w0, 0
	ldr	x1, [x19, 64]
	cbz	x1, .L408
	ldr	x2, [x1, 176]
	mov	w0, 0
	cbz	x2, .L408
	ldr	w1, [x1, 20]
	cmp	w1, 1
	beq	.L414
.L408:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L414:
	mov	x0, x2
	bl	rbuf_query_busy
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_query_busy, .-aip_query_busy
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_read_data_complete
	.type	aip_read_data_complete, %function
aip_read_data_complete:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	uxtw	x0, w19
	adrp	x2, .LANCHOR0
	add	x3, x0, 8
	add	x2, x2, :lo12:.LANCHOR0
	ldr	x19, [x2, x3, lsl 3]
	cbz	x19, .L415
	ldr	w1, [x19, 336]
	cbz	w1, .L421
	ldr	x0, [x19, 176]
	mov	w1, w20
	bl	rbuf_read_data_complete
.L415:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L421:
	str	x21, [x29, 32]
	mov	w1, w20
	ldr	x21, [x2, x0, lsl 3]
	ldr	x0, [x19, 184]
	bl	rbuf_read_data_complete
	mov	x1, x21
	mov	x0, x19
	bl	aip_update_fifo_latency
	mov	x1, x21
	mov	x0, x19
	bl	aip_update_track_latency
	ldr	x21, [x29, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	aip_read_data_complete, .-aip_read_data_complete
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_under_flow_calc
	.type	aip_under_flow_calc, %function
aip_under_flow_calc:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	x1, [x0, w19, uxtw 3]
	cbz	x1, .L422
	ldr	w0, [x1, 64]
	ldr	w2, [x1, 64]
	lsr	w0, w0, 16
	add	w0, w0, 1
	bfi	w2, w0, 16, 16
	str	w2, [x1, 64]
.L422:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_under_flow_calc, .-aip_under_flow_calc
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_ring_in
	.type	aip_get_ring_in, %function
aip_get_ring_in:
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
	cbz	x0, .L429
	ldr	x0, [x0, 176]
.L427:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L429:
	mov	x0, 0
	b	.L427
	.size	aip_get_ring_in, .-aip_get_ring_in
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_ring_out
	.type	aip_get_ring_out, %function
aip_get_ring_out:
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
	cbz	x0, .L432
	ldr	x0, [x0, 184]
.L430:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L432:
	mov	x0, 0
	b	.L430
	.size	aip_get_ring_out, .-aip_get_ring_out
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_get_mute_status
	.type	aip_get_mute_status, %function
aip_get_mute_status:
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
	cbz	x0, .L435
	ldr	w0, [x0, 268]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L435:
	mov	w0, 0
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_get_mute_status, .-aip_get_mute_status
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_continue_output_cnt
	.type	aip_continue_output_cnt, %function
aip_continue_output_cnt:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	x0, [x0, w19, uxtw 3]
	cbz	x0, .L436
	ldr	w1, [x0, 88]
	add	w1, w1, 1
	str	w1, [x0, 88]
.L436:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_continue_output_cnt, .-aip_continue_output_cnt
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_continue_output
	.type	aip_continue_output, %function
aip_continue_output:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	uxtw	x20, w0
	mov	w22, w1
	mov	x0, x30
	bl	_mcount
	adrp	x21, .LANCHOR0
	add	x2, x20, 8
	add	x0, x21, :lo12:.LANCHOR0
	ldr	x19, [x0, x2, lsl 3]
	cbz	x19, .L441
	ldr	w1, [x19, 36]
	cbz	w1, .L444
	cmp	w22, 1
	beq	.L466
	ldr	w0, [x19, 20]
	cmp	w0, 1
	beq	.L467
.L441:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L444:
	ldr	w0, [x19, 464]
	cmp	w0, 256
	bhi	.L449
	ldr	w0, [x19, 216]
	cmp	w0, 1
	beq	.L441
.L449:
	ldr	w0, [x19, 20]
	and	w1, w0, -3
	cmp	w1, 1
	bne	.L441
	cmp	w0, 1
	beq	.L468
.L450:
	cmp	w22, 1
	bne	.L441
	ldr	w2, [x19, 468]
	add	x21, x21, :lo12:.LANCHOR0
	ldr	x0, [x19, 184]
	mov	x1, 0
	bl	rbuf_write_data
	ldr	x0, [x21, x20, lsl 3]
	cbz	x0, .L441
	ldr	w1, [x0, 88]
	add	w1, w1, 1
	str	w1, [x0, 88]
	b	.L441
	.p2align 2
.L467:
	mov	w0, 2
	cbnz	w22, .L451
	str	w0, [x19, 20]
.L469:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L466:
	ldr	x0, [x0, x20, lsl 3]
	cbz	x0, .L446
	ldr	w1, [x0, 88]
	add	w1, w1, 1
	str	w1, [x0, 88]
.L446:
	ldr	w0, [x19, 20]
	cmp	w0, 1
	bne	.L441
.L451:
	mov	w0, 3
	str	w0, [x19, 20]
	b	.L469
	.p2align 2
.L468:
	mov	w0, 3
	str	w0, [x19, 20]
	mov	x0, x19
	bl	aip_reclaim_fifo
	mov	x0, x19
	bl	aip_flush_chn_ctx
	cbnz	w22, .L450
	str	wzr, [x19, 464]
	b	.L441
	.size	aip_continue_output, .-aip_continue_output
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_reg_reclaim_fun
	.type	aip_reg_reclaim_fun, %function
aip_reg_reclaim_fun:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w19, w0
	mov	w21, w1
	mov	x20, x2
	mov	x0, x30
	bl	_mcount
	cmp	w19, 7
	bhi	.L473
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	ldr	x0, [x19, 64]
	cbz	x0, .L473
	str	w21, [x0, 448]
	str	x20, [x0, 456]
	mov	w0, 0
.L470:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L473:
	mov	w0, -1
	b	.L470
	.size	aip_reg_reclaim_fun, .-aip_reg_reclaim_fun
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_peek_add_mute_size
	.type	aip_peek_add_mute_size, %function
aip_peek_add_mute_size:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	mov	w0, 0
	ldr	x1, [x19, 64]
	cbz	x1, .L474
	ldr	w2, [x1, 440]
	mov	w0, 0
	cmp	w2, 2
	beq	.L478
.L474:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L478:
	ldr	w0, [x1, 444]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_peek_add_mute_size, .-aip_peek_add_mute_size
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_poke_add_mute_size
	.type	aip_poke_add_mute_size, %function
aip_poke_add_mute_size:
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
	ldr	x0, [x19, 64]
	cbz	x0, .L479
	ldr	w2, [x0, 444]
	cmp	w2, w20
	bcs	.L482
	str	wzr, [x0, 444]
.L483:
	str	wzr, [x0, 440]
.L479:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L482:
	sub	w2, w2, w20
	str	w2, [x0, 444]
	cbz	w2, .L483
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_poke_add_mute_size, .-aip_poke_add_mute_size
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_proc_cmd
	.type	aip_proc_cmd, %function
aip_proc_cmd:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 8]
	cmp	w0, 7
	bhi	.L486
	ldr	w1, [x19]
	cmp	w1, 2
	beq	.L488
	bhi	.L489
	cbz	w1, .L490
	cmp	w1, 1
	bne	.L486
	bl	aip_stop_chn_cmd_proc
.L486:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L489:
	cmp	w1, 3
	beq	.L492
	cmp	w1, 5
	bne	.L486
	ldr	w1, [x19, 12]
	cmp	w1, 20
	bne	.L486
	adrp	x1, .LANCHOR0
	add	x1, x1, :lo12:.LANCHOR0
	add	x0, x1, x0, uxtw 3
	ldr	x0, [x0, 64]
	cbz	x0, .L486
	ldr	x1, [x19, 16]
	bl	aip_set_speed
	b	.L486
	.p2align 2
.L490:
	bl	aip_start_chn_cmd_proc
	b	.L486
	.p2align 2
.L492:
	bl	aip_flush_chn_cmd_proc
	b	.L486
	.p2align 2
.L488:
	bl	aip_pause_chn_cmd_proc
	b	.L486
	.size	aip_proc_cmd, .-aip_proc_cmd
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_aip_rm, %object
	.size	g_aip_rm, 128
g_aip_rm:
	.zero	128
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
