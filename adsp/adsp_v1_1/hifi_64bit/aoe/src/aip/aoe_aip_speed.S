#include "hi_asm_define.h"
	.arch armv8-a
	.file	"aoe_aip_speed.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_reset_speed
	.type	aip_reset_speed, %function
aip_reset_speed:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 408]
	cbz	x0, .L2
	mov	w1, 256
	bl	sonic_set_speed
.L2:
	str	wzr, [x19, 416]
	mov	w1, 5
	mov	x0, 1
	str	w1, [x19, 420]
	stp	x0, xzr, [x19, 424]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_reset_speed, .-aip_reset_speed
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_set_speed
	.type	aip_set_speed, %function
aip_set_speed:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	x19, x1
	mov	x0, x30
	bl	_mcount
	cmp	x20, 0
	ccmp	x19, 0, 4, ne
	beq	.L7
	ldr	x0, [x20, 408]
	cbz	x0, .L7
	ldr	w0, [x19, 4]
	tbnz	w0, #31, .L7
	ldr	w0, [x19, 8]
	cmp	w0, 999
	bls	.L29
.L7:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L29:
	mov	x2, 20
	mov	x1, x19
	add	x0, x20, 420
	bl	adsp_memcpy
	ldp	w0, w1, [x19, 4]
	add	w0, w0, w0, lsl 2
	lsl	w2, w0, 3
	add	w0, w2, w0, lsl 1
	lsl	w2, w0, 3
	add	w0, w2, w0, lsl 1
	add	w0, w1, w0, lsl 1
	sub	w1, w0, #1001
	cmp	w1, 999
	bls	.L30
	sub	w0, w0, #200
	cmp	w0, 799
	bhi	.L13
	ldr	w0, [x19, 12]
	cbnz	w0, .L14
	ldr	w0, [x19, 16]
	cbnz	w0, .L14
	ldr	x0, [x20, 408]
	mov	w1, 2
	str	w1, [x20, 416]
	cbz	x0, .L7
.L28:
	ldp	w2, w1, [x19, 4]
	mov	w3, 19923
	movk	w3, 0x1062, lsl 16
	lsl	w1, w1, 8
	umull	x1, w1, w3
	lsr	x1, x1, 38
	add	w1, w1, w2, lsl 8
	bl	sonic_set_speed
	b	.L7
	.p2align 2
.L30:
	ldr	w0, [x19, 12]
	cbnz	w0, .L12
	ldr	w0, [x19, 16]
	cbnz	w0, .L12
	ldr	x0, [x20, 408]
	mov	w1, 1
	str	w1, [x20, 416]
	cbnz	x0, .L28
	b	.L7
	.p2align 2
.L12:
	mov	w0, 3
	str	w0, [x20, 416]
	b	.L7
	.p2align 2
.L13:
	ldr	x0, [x20, 408]
	cbz	x0, .L15
	mov	w1, 256
	bl	sonic_set_speed
.L15:
	mov	w1, 5
	mov	x0, 1
	str	wzr, [x20, 416]
	str	w1, [x20, 420]
	stp	x0, xzr, [x20, 424]
	b	.L7
	.p2align 2
.L14:
	mov	w0, 5
	str	w0, [x20, 416]
	b	.L7
	.size	aip_set_speed, .-aip_set_speed
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_dma_sonic_left_frame
	.type	aip_dma_sonic_left_frame, %function
aip_dma_sonic_left_frame:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	str	x22, [sp, 40]
	mov	x22, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x22, 408]
	cbz	x0, .L31
	stp	x19, x20, [x29, 16]
	mov	w19, 256
	str	x21, [x29, 32]
	mov	w21, 0
	str	x23, [x29, 48]
	bl	aoe_get_wk_buf
	mov	x23, x0
	mov	x20, x0
	b	.L34
	.p2align 2
.L38:
	add	w21, w21, w0
	cmp	w19, 0
	ble	.L33
	add	x20, x20, x0, uxtw
.L34:
	ldr	x0, [x22, 408]
	mov	w2, w19
	mov	x1, x20
	bl	sonic_read_short_from_stream
	sub	w19, w19, w0
	cmp	w0, 0
	lsl	w0, w0, 2
	bgt	.L38
.L33:
	ldr	x0, [x22, 184]
	mov	w2, w21
	mov	x1, x23
	bl	rbuf_write_data
	ldr	x21, [x29, 32]
	ldp	x19, x20, [x29, 16]
	ldr	x23, [x29, 48]
.L31:
	ldr	x22, [sp, 40]
	ldp	x29, x30, [sp], 64
	ret
	.size	aip_dma_sonic_left_frame, .-aip_dma_sonic_left_frame
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_sonic_pcm_data
	.type	aip_sonic_pcm_data, %function
aip_sonic_pcm_data:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	str	x21, [sp, 32]
	str	x23, [sp, 48]
	mov	x21, x0
	mov	x23, x1
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x21, 408]
	cbz	x0, .L39
	str	x19, [x29, 16]
	str	x24, [x29, 56]
	ldr	w0, [x21, 416]
	cmp	w0, 2
	bhi	.L72
.L43:
	bl	aoe_get_wk_buf
	mov	x24, x0
	ldr	w2, [x23, 12]
	ldr	x0, [x21, 408]
	ldr	x1, [x23]
	bl	sonic_write_short_to_stream
	cmp	w0, 0
	ble	.L68
	str	x20, [x29, 24]
	mov	w19, 256
	str	x22, [x29, 40]
	mov	x20, x24
	mov	w22, 0
	b	.L55
	.p2align 2
.L73:
	add	w22, w22, w0
	cmp	w19, 0
	ble	.L54
	add	x20, x20, x0, uxtw
.L55:
	ldr	x0, [x21, 408]
	mov	w2, w19
	mov	x1, x20
	bl	sonic_read_short_from_stream
	sub	w19, w19, w0
	cmp	w0, 0
	lsl	w0, w0, 2
	bgt	.L73
.L54:
	str	x24, [x23]
	lsr	w0, w22, 2
	stp	w22, w0, [x23, 8]
	ldp	x19, x20, [x29, 16]
	ldr	x22, [x29, 40]
	ldr	x24, [x29, 56]
.L39:
	ldr	x21, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L72:
	ldr	x0, [x21, 176]
	bl	rbuf_query_busy
	mov	w19, w0
	ldr	x0, [x21, 408]
	bl	sonic_get_in_left_bytes
	add	w0, w0, w19
	ldr	w1, [x21, 320]
	ldr	w2, [x21, 112]
	bl	util_latency_ms
	ldr	w1, [x21, 416]
	cmp	w1, 4
	beq	.L48
	bls	.L74
	cmp	w1, 5
	beq	.L47
	cmp	w1, 6
	bne	.L43
.L48:
	ldr	w1, [x21, 436]
	cmp	w0, w1
	bcs	.L43
	ldr	x0, [x21, 408]
	cbz	x0, .L52
	mov	w1, 256
	bl	sonic_set_speed
.L52:
	str	wzr, [x21, 416]
	b	.L43
	.p2align 2
.L68:
	ldr	x19, [x29, 16]
	ldr	x24, [x29, 56]
	ldr	x21, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L74:
	cmp	w1, 3
	bne	.L43
	ldr	w1, [x21, 432]
	cmp	w0, w1
	bcc	.L43
	ldr	x0, [x21, 408]
	cbz	x0, .L50
	ldr	w1, [x21, 428]
	mov	w3, 19923
	movk	w3, 0x1062, lsl 16
	ldr	w2, [x21, 424]
	lsl	w1, w1, 8
	umull	x1, w1, w3
	lsr	x1, x1, 38
	add	w1, w1, w2, lsl 8
	bl	sonic_set_speed
.L50:
	mov	w0, 4
	str	w0, [x21, 416]
	b	.L43
	.p2align 2
.L47:
	ldr	w1, [x21, 432]
	cmp	w0, w1
	bcc	.L43
	ldr	x0, [x21, 408]
	cbz	x0, .L51
	ldr	w1, [x21, 428]
	mov	w3, 19923
	movk	w3, 0x1062, lsl 16
	ldr	w2, [x21, 424]
	lsl	w1, w1, 8
	umull	x1, w1, w3
	lsr	x1, x1, 38
	add	w1, w1, w2, lsl 8
	bl	sonic_set_speed
.L51:
	mov	w0, 6
	str	w0, [x21, 416]
	b	.L43
	.size	aip_sonic_pcm_data, .-aip_sonic_pcm_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_sonic_flush
	.type	aip_sonic_flush, %function
aip_sonic_flush:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 408]
	cbz	x0, .L75
	bl	sonic_destroy_stream
	ldp	w1, w0, [x19, 108]
	bl	sonic_create_stream
	str	x0, [x19, 408]
.L75:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_sonic_flush, .-aip_sonic_flush
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_sonic_deinit
	.type	aip_sonic_deinit, %function
aip_sonic_deinit:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x0, [x19, 408]
	cbz	x0, .L80
	bl	sonic_destroy_stream
	str	xzr, [x19, 408]
.L80:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aip_sonic_deinit, .-aip_sonic_deinit
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aip_sonic_init
	.type	aip_sonic_init, %function
aip_sonic_init:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 36]
	cbnz	w0, .L85
	ldr	w0, [x19]
	cbz	w0, .L85
	ldr	w1, [x19, 108]
	cmp	w1, 2
	beq	.L95
.L85:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L95:
	ldr	w0, [x19, 112]
	mov	w2, 48000
	cmp	w0, w2
	bne	.L85
	ldr	w2, [x19, 104]
	cmp	w2, 16
	bne	.L85
	bl	sonic_create_stream
	str	x0, [x19, 408]
	cbz	x0, .L85
	mov	w1, 256
	bl	sonic_set_speed
	mov	w1, 5
	mov	x0, 1
	str	wzr, [x19, 416]
	str	w1, [x19, 420]
	stp	x0, xzr, [x19, 424]
	b	.L85
	.size	aip_sonic_init, .-aip_sonic_init
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
