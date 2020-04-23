#include "hi_asm_define.h"
	.arch armv8-a
	.file	"aoe_reg.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reg_read_bit_depth
	.type	aoe_reg_read_bit_depth, %function
aoe_reg_read_bit_depth:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w1
	mov	x20, x0
	mov	x0, x30
	bl	_mcount
	cmp	w19, 1
	beq	.L3
	cbz	w19, .L4
	cmp	w19, 2
	beq	.L5
	str	wzr, [x20]
	mov	w0, -1
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L5:
	mov	w1, 24
	str	w1, [x20]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L4:
	mov	w1, 8
	str	w1, [x20]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L3:
	mov	w1, 16
	str	w1, [x20]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_reg_read_bit_depth, .-aoe_reg_read_bit_depth
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reg_read_channel
	.type	aoe_reg_read_channel, %function
aoe_reg_read_channel:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w1
	mov	x20, x0
	mov	x0, x30
	bl	_mcount
	cmp	w19, 1
	beq	.L11
	cbz	w19, .L12
	cmp	w19, 3
	beq	.L13
	str	wzr, [x20]
	mov	w0, -1
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L13:
	mov	w1, 8
	str	w1, [x20]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L12:
	mov	w1, 1
	str	w1, [x20]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L11:
	mov	w1, 2
	str	w1, [x20]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_reg_read_channel, .-aoe_reg_read_channel
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reg_read_pcm_type
	.type	aoe_reg_read_pcm_type, %function
aoe_reg_read_pcm_type:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w1
	mov	x20, x0
	mov	x0, x30
	bl	_mcount
	cmp	w19, 4
	bhi	.L18
	mov	w0, 0
	str	w19, [x20]
.L16:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L18:
	mov	w0, -1
	b	.L16
	.size	aoe_reg_read_pcm_type, .-aoe_reg_read_pcm_type
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reg_read_sampel_rate
	.type	aoe_reg_read_sampel_rate, %function
aoe_reg_read_sampel_rate:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w1
	mov	x20, x0
	mov	x0, x30
	bl	_mcount
	cmp	w19, 13
	bhi	.L21
	adrp	x2, .LANCHOR0
	add	x2, x2, :lo12:.LANCHOR0
	mov	w0, 0
	ldr	w1, [x2, w19, uxtw 2]
	str	w1, [x20]
.L19:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L21:
	mov	w0, -1
	b	.L19
	.size	aoe_reg_read_sampel_rate, .-aoe_reg_read_sampel_rate
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reg_read_data_format
	.type	aoe_reg_read_data_format, %function
aoe_reg_read_data_format:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	str	w20, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_reg_read_data_format, .-aoe_reg_read_data_format
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reg_read_buf_latency_ms
	.type	aoe_reg_read_buf_latency_ms, %function
aoe_reg_read_buf_latency_ms:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	str	w20, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_reg_read_buf_latency_ms, .-aoe_reg_read_buf_latency_ms
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reg_read_buf_delay_ms
	.type	aoe_reg_read_buf_delay_ms, %function
aoe_reg_read_buf_delay_ms:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	w0, 0
	str	w20, [x19]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_reg_read_buf_delay_ms, .-aoe_reg_read_buf_delay_ms
	.section	.rodata
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_aoe_sample_rate_tab, %object
	.size	g_aoe_sample_rate_tab, 56
g_aoe_sample_rate_tab:
	.word	8000
	.word	11025
	.word	12000
	.word	16000
	.word	22050
	.word	24000
	.word	32000
	.word	44100
	.word	48000
	.word	88200
	.word	96000
	.word	176400
	.word	192000
	.word	288000
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
