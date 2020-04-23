#include "hi_asm_define.h"
	.arch armv8-a
	.file	"aoe_engine.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_check_aip_data, %function
engine_check_aip_data:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldp	w0, w3, [x19, 32]
	ldr	w4, [x19, 40]
	cmp	w0, 0
	ccmp	w3, 0, 4, ne
	bne	.L18
	mov	w0, 1
	cbnz	w4, .L1
	ldr	w0, [x19, 176]
	cmp	w0, 0
	cset	w0, ne
.L1:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L18:
	uxtw	x6, w4
	uxtw	x1, w0
	mov	w2, 0
	mov	w0, 0
.L4:
	asr	x5, x1, x2
	tbz	x5, 0, .L6
	mov	w0, 1
	asr	x5, x6, x2
	tbz	x5, 0, .L19
.L6:
	add	w2, w2, 1
	cmp	w2, 8
	bne	.L4
.L5:
	uxtw	x3, w3
	uxtw	x4, w4
	mov	w1, 0
	.p2align 3
.L8:
	asr	x2, x3, x1
	tbz	x2, 0, .L11
	asr	x2, x4, x1
	tbz	x2, 0, .L20
	orr	w0, w0, 2
.L11:
	add	w1, w1, 1
	cmp	w1, 8
	bne	.L8
	and	w0, w0, 1
.L21:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L20:
	and	w0, w0, -3
	and	w0, w0, 1
	b	.L21
	.p2align 2
.L19:
	mov	w0, 0
	b	.L5
	.size	engine_check_aip_data, .-engine_check_aip_data
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_destroy_chn, %function
engine_destroy_chn:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L22
	ldr	w0, [x19, 12]
	cbz	w0, .L31
.L24:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L22:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L31:
	ldr	x1, [x19, 112]
	cbz	x1, .L24
	mov	w0, 70
	bl	HI_ADSP_FREE
	str	xzr, [x19, 112]
	b	.L24
	.size	engine_destroy_chn, .-engine_destroy_chn
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_out_raw_frame, %function
engine_out_raw_frame:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	str	x23, [sp, 48]
	mov	w19, w0
	uxtw	x20, w1
	mov	x0, x30
	mov	w21, w2
	bl	_mcount
	mov	w0, w19
	bl	aip_get_ring_in
	mov	x23, x0
	cbz	x0, .L32
	str	x22, [x29, 40]
	mov	w0, w19
	mov	x22, 0
	bl	aip_get_mute_status
	cbz	w0, .L47
.L34:
	mov	w19, 0
	mov	w2, w21
	mov	x1, x22
	mov	x0, x23
	bl	rbuf_read_data_dma
	.p2align 3
.L38:
	asr	x0, x20, x19
	tbz	x0, 0, .L36
	mov	w0, w19
	bl	aop_get_ring_out
	mov	w2, w21
	mov	x1, x22
	cbz	x0, .L36
	bl	rbuf_write_data_dma
	mov	w0, w19
	bl	aop_proc_delay_compensation
.L36:
	add	w19, w19, 1
	cmp	w19, 8
	bne	.L38
	ldr	x22, [x29, 40]
.L32:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L47:
	bl	aoe_get_wk_buf
	mov	x22, x0
	b	.L34
	.size	engine_out_raw_frame, .-engine_out_raw_frame
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_check_aop_space.isra.0, %function
engine_check_aop_space.isra.0:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w21, w0
	uxtw	x19, w2
	mov	x0, x30
	uxtw	x20, w1
	bl	_mcount
	mov	w0, 0
	cbz	w19, .L48
	cbz	w21, .L50
	uxtw	x2, w21
	uxtw	x4, w19
	mov	w0, 0
	mov	w3, 0
.L52:
	asr	x1, x2, x0
	tbz	x1, 0, .L54
	mov	w3, 1
	asr	x1, x4, x0
	tbz	x1, 0, .L73
.L54:
	add	w0, w0, 1
	cmp	w0, 8
	bne	.L52
	cbnz	w20, .L62
.L60:
	and	w0, w3, 1
.L48:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L50:
	mov	w0, 0
	mov	w3, 0
	cbz	w20, .L48
.L62:
	mov	w0, 0
.L56:
	asr	x1, x20, x0
	tbz	x1, 0, .L59
	asr	x1, x19, x0
	tbz	x1, 0, .L74
	orr	w3, w3, 2
.L59:
	add	w0, w0, 1
	cmp	w0, 8
	bne	.L56
.L57:
	mov	w0, 0
	cbnz	w21, .L60
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L73:
	mov	w3, 0
	cbz	w20, .L60
	b	.L62
	.p2align 2
.L74:
	and	w3, w3, -3
	b	.L57
	.size	engine_check_aop_space.isra.0, .-engine_check_aop_space.isra.0
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_create_chn, %function
engine_create_chn:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w20, w0
	mov	x0, x30
	bl	_mcount
	adrp	x3, .LANCHOR0
	add	x3, x3, :lo12:.LANCHOR0
	mov	w1, 0
	mov	x2, 28
	add	x0, x29, 52
	ldr	x19, [x3, w20, uxtw 3]
	bl	adsp_memset
	add	x0, x29, 52
	ldr	w1, [x19]
	and	w1, w1, 3
	bl	aoe_reg_read_bit_depth
	cbnz	w0, .L78
	ldr	w1, [x19]
	add	x0, x29, 56
	ubfx	x1, x1, 2, 2
	bl	aoe_reg_read_channel
	cbnz	w0, .L78
	ldr	w1, [x19]
	add	x0, x29, 60
	ubfx	x1, x1, 4, 4
	bl	aoe_reg_read_sampel_rate
	cbnz	w0, .L78
	ldr	w1, [x19]
	add	x0, x29, 64
	ubfx	x1, x1, 8, 2
	bl	aoe_reg_read_data_format
	cbnz	w0, .L78
	mov	w1, 216
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x19, x0
	cbz	x0, .L78
	mov	x2, 216
	mov	w1, 0
	bl	adsp_memset
	mov	x2, 28
	add	x1, x29, 52
	mov	x0, x19
	bl	adsp_memcpy
	ldp	w1, w0, [x19]
	bl	util_calc_frame_size
	mov	w3, w0
	ldr	w0, [x19, 8]
	mov	w2, 48000
	mov	w1, 19923
	mov	w4, 60928
	cmp	w0, w2
	movk	w1, 0x1062, lsl 16
	add	w2, w0, w0, lsl 2
	movk	w4, 0x2, lsl 16
	ccmp	w0, w4, 4, ne
	umull	x2, w2, w1
	lsr	x2, x2, 38
	beq	.L90
	and	w0, w2, -4
	tst	x2, 3
	add	w0, w0, 4
	csel	w2, w0, w2, ne
.L80:
	mul	w2, w3, w2
	ldp	w1, w0, [x19]
	str	w2, [x19, 84]
	bl	util_calc_frame_size
	str	w0, [x19, 88]
	ldr	w1, [x19, 84]
	ldr	w2, [x19, 12]
	udiv	w0, w1, w0
	str	w0, [x19, 16]
	cbz	w2, .L91
.L81:
	mov	w0, 1
	str	w20, [x19, 80]
	str	w0, [x19, 92]
.L75:
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L90:
	and	w1, w2, 255
	and	w0, w2, -256
	add	w0, w0, 256
	cmp	w1, 0
	csel	w2, w0, w2, ne
	b	.L80
	.p2align 2
.L91:
	stp	x21, x22, [x29, 32]
	add	x21, x19, 112
	str	xzr, [x19, 112]
	ldp	w4, w3, [x19]
	stp	w1, w0, [x19, 120]
	ldr	w2, [x19, 8]
	mov	x1, x21
	stp	w4, w3, [x19, 128]
	add	x0, x19, 144
	stp	w2, wzr, [x19, 136]
	mov	x2, 32
	bl	adsp_memcpy
	mov	x1, x21
	mov	x2, 32
	add	x0, x19, 184
	bl	adsp_memcpy
	ldr	w2, [x19, 152]
	mov	w0, 70
	ldr	w22, [x19, 120]
	add	w22, w2, w22, lsl 1
	mov	w1, w22
	bl	HI_ADSP_MALLOC
	mov	x21, x0
	cbz	x0, .L82
	uxtw	x2, w22
	mov	w1, 0
	bl	adsp_memset
	str	x21, [x19, 112]
	ldr	w0, [x19, 120]
	lsl	w1, w0, 1
	add	x0, x21, x0, uxtw
	add	x21, x21, x1
	str	x21, [x19, 144]
	str	x0, [x19, 184]
	ldp	x21, x22, [x29, 32]
	b	.L81
	.p2align 2
.L78:
	mov	x19, 0
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 80
	ret
.L82:
	mov	x0, x19
	mov	x19, 0
	bl	engine_destroy_chn
	ldp	x21, x22, [x29, 32]
	b	.L75
	.size	engine_create_chn, .-engine_create_chn
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_proc_chn_start_cmd, %function
engine_proc_chn_start_cmd:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	uxtw	x20, w0
	adrp	x19, .LANCHOR0
	add	x22, x20, 6
	add	x21, x19, :lo12:.LANCHOR0
	mov	x0, x30
	bl	_mcount
	mov	x23, x20
	ldr	x0, [x21, x22, lsl 3]
	cbz	x0, .L104
	ldr	w1, [x0, 92]
	cbz	w1, .L105
.L98:
	add	x19, x19, :lo12:.LANCHOR0
	mov	w1, 1
	str	w1, [x0, 92]
	ldr	x0, [x19, x20, lsl 3]
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L92
	ldr	w1, [x0, 4]
	and	w1, w1, -121
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
.L92:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L105:
	bl	engine_destroy_chn
.L104:
	mov	w0, w23
	bl	engine_create_chn
	str	x0, [x21, x22, lsl 3]
	cbnz	x0, .L98
	ldr	x0, [x21, x20, lsl 3]
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L92
	ldr	w1, [x0, 4]
	mov	w2, 3
	bfi	w1, w2, 3, 4
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
	b	.L92
	.size	engine_proc_chn_start_cmd, .-engine_proc_chn_start_cmd
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_reclaim_in, %function
engine_reclaim_in:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w19, w0
	mov	x20, x1
	mov	x0, x30
	mov	w21, w2
	bl	_mcount
	mov	w0, 0
	cmp	w19, 5
	bhi	.L106
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	add	x19, x0, x19, uxtw 3
	mov	w0, 0
	ldr	x19, [x19, 48]
	cmp	x19, 0
	ccmp	x20, 0, 4, ne
	beq	.L106
	ldr	w1, [x19, 84]
	cmp	w21, w1
	beq	.L111
.L106:
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L111:
	ldr	w2, [x19, 88]
	mov	x1, x20
	ldp	w3, w4, [x19]
	ldr	x0, [x19, 184]
	udiv	w2, w21, w2
	mul	w2, w2, w4
	bl	util_mixer_pro
	mov	w1, 7
	mov	w0, w21
	str	w1, [x19, 176]
	b	.L106
	.size	engine_reclaim_in, .-engine_reclaim_in
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	action_mixer_24bit, %function
action_mixer_24bit:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x20, x0
	mov	w19, w2
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	cbz	w19, .L113
	mov	w3, 24
	lsr	w2, w19, 2
	mov	x1, x21
	mov	x0, x20
	bl	util_mixer_pro
.L113:
	mov	w0, w19
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	action_mixer_24bit, .-action_mixer_24bit
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	action_mixer_16bit, %function
action_mixer_16bit:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x20, x0
	mov	w19, w2
	mov	x21, x1
	mov	x0, x30
	bl	_mcount
	cbz	w19, .L118
	mov	w3, 16
	lsr	w2, w19, 1
	mov	x1, x21
	mov	x0, x20
	bl	util_mixer_pro
.L118:
	mov	w0, w19
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	action_mixer_16bit, .-action_mixer_16bit
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	engine_mix_data.isra.11, %function
engine_mix_data.isra.11:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x21, x0
	mov	x20, x1
	mov	w22, w2
	mov	x19, x3
	mov	x0, x30
	bl	_mcount
	mov	w0, w22
	bl	aip_get_ring_out
	ldr	w1, [x21]
	ldr	w3, [x20]
	cmp	w1, 16
	beq	.L125
	mov	x2, x19
	adrp	x1, action_mixer_24bit
	add	x1, x1, :lo12:action_mixer_24bit
	bl	rbuf_read_data_with_action
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L125:
	mov	x2, x19
	adrp	x1, action_mixer_16bit
	add	x1, x1, :lo12:action_mixer_16bit
	bl	rbuf_read_data_with_action
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	engine_mix_data.isra.11, .-engine_mix_data.isra.11
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	engine_process
	.type	engine_process, %function
engine_process:
	stp	x29, x30, [sp, -208]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	stp	x27, x28, [sp, 80]
	adrp	x19, .LANCHOR0
	add	x19, x19, :lo12:.LANCHOR0
	mov	w20, 0
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L197:
	ldr	x0, [x19]
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L127
	ldr	w1, [x0, 4]
	ands	w1, w1, 3
	beq	.L129
	cmp	w1, 1
	bne	.L287
	ldr	x27, [x19, 48]
	cbz	x27, .L132
	ldr	w1, [x27, 92]
	cmp	w1, 1
	beq	.L288
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L198
.L303:
	ldr	w1, [x0, 4]
	and	w1, w1, -121
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
.L198:
	ldr	w0, [x27, 92]
	cmp	w0, 1
	beq	.L289
	.p2align 3
.L132:
	add	w20, w20, 1
	add	x19, x19, 8
	cmp	w20, 6
	bne	.L197
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x27, x28, [sp, 80]
	ldp	x29, x30, [sp], 208
	ret
	.p2align 2
.L129:
	mov	w0, w20
	bl	engine_proc_chn_start_cmd
.L127:
	ldr	x27, [x19, 48]
	cbz	x27, .L132
.L297:
	ldr	w0, [x27, 92]
	cmp	w0, 1
	bne	.L132
.L289:
	ldr	w0, [x27]
	mov	w22, 0
	str	w0, [x29, 120]
	mov	w23, 0
	ldr	w0, [x27, 4]
	mov	w25, 0
	str	w0, [x29, 104]
	mov	w28, 0
	ldr	w0, [x27, 16]
	ldr	x24, [x19]
	str	w0, [x29, 116]
	ldr	w0, [x24, 12]
	str	wzr, [x29, 124]
	and	w0, w0, 65535
	stp	w0, wzr, [x29, 108]
	and	x21, x0, 65535
	.p2align 3
.L141:
	asr	x0, x21, x28
	tbz	x0, 0, .L136
	mov	w0, w28
	add	x1, x29, 172
	bl	aop_get_chn_status
	ldr	w0, [x29, 172]
	cmp	w0, 1
	bne	.L136
	ldr	w0, [x29, 176]
	mov	x1, 1
	lsl	x26, x1, x28
	cmp	w0, 1
	beq	.L290
	orr	w25, w25, w26
.L138:
	ldr	w1, [x29, 120]
	ldr	w0, [x29, 180]
	cmp	w1, w0
	beq	.L291
.L139:
	orr	w23, w23, w26
.L140:
	ldr	w2, [x29, 116]
	mov	w0, w28
	ldr	w1, [x29, 188]
	mul	w1, w2, w1
	bl	aop_query_rbf_free
	cmp	w0, 0
	orr	w4, w22, w26
	csel	w22, w4, w22, ne
.L136:
	add	w28, w28, 1
	cmp	w28, 8
	bne	.L141
	ldr	w0, [x29, 108]
	adrp	x21, engine_reclaim_in
	stp	wzr, w0, [x27, 40]
	add	x21, x21, :lo12:engine_reclaim_in
	ldr	w0, [x29, 124]
	str	w0, [x27, 48]
	ldr	w0, [x29, 112]
	str	xzr, [x27, 32]
	str	w25, [x27, 52]
	str	w0, [x27, 56]
	stp	w23, w22, [x27, 60]
	mov	w23, 0
	mov	x22, 1
	ldr	w0, [x24, 8]
	and	w0, w0, 65535
	str	w0, [x27, 28]
	b	.L153
	.p2align 2
.L142:
	add	w23, w23, 1
	cmp	w23, 8
	beq	.L152
.L293:
	ldr	w0, [x27, 28]
.L153:
	uxtw	x0, w0
	asr	x0, x0, x23
	tbz	x0, 0, .L142
	mov	w0, w23
	add	x1, x29, 136
	bl	aip_get_chn_status
	ldr	w0, [x29, 136]
	cmp	w0, 1
	beq	.L292
.L144:
	ldr	w1, [x27, 80]
	mov	w0, w23
	mov	x2, x21
	add	w23, w23, 1
	bl	aip_reg_reclaim_fun
	cmp	w23, 8
	bne	.L293
.L152:
	ldr	w0, [x24, 4]
	ubfx	x0, x0, 13, 1
	str	w0, [x27, 72]
	ldr	w0, [x24, 4]
	ubfx	x0, x0, 14, 1
	str	w0, [x27, 76]
	ldr	x21, [x19, 48]
	ldr	w0, [x21, 44]
	uxtw	x2, w0
	tbnz	x0, 0, .L199
	mov	w0, 1
.L156:
	asr	x1, x2, x0
	tbnz	x1, 0, .L155
	add	w0, w0, 1
	cmp	w0, 8
	bne	.L156
.L168:
	ldr	w22, [x21, 12]
	cbnz	w22, .L294
	ldp	w0, w1, [x21, 48]
	ldr	w2, [x21, 64]
	bl	engine_check_aop_space.isra.0
	cbz	w0, .L132
	mov	x0, x21
	bl	engine_check_aip_data
	cbz	w0, .L132
	mov	w0, 5
	str	w0, [x29, 172]
	mov	w1, 0
	ldr	w2, [x21, 120]
	ldr	x0, [x21, 112]
	bl	adsp_memset
	ldr	w1, [x21, 176]
	cbz	w1, .L177
	cmp	w1, 7
	ldr	w2, [x21, 84]
	ldr	x0, [x21, 112]
	beq	.L295
	uxtw	x2, w2
	mov	w1, 0
	bl	adsp_memset
.L179:
	ldr	w0, [x21, 176]
	sub	w0, w0, #1
	str	w0, [x21, 176]
.L177:
	add	x23, x21, 84
	b	.L183
	.p2align 2
.L181:
	add	w22, w22, 1
	cmp	w22, 8
	beq	.L296
.L183:
	ldr	w0, [x21, 28]
	asr	x0, x0, x22
	tbz	x0, 0, .L181
	mov	w0, w22
	add	x1, x29, 172
	bl	aip_get_pcm_type
	ldr	w0, [x29, 172]
	cmp	w0, 1
	bhi	.L181
	ldr	x3, [x21, 112]
	mov	w2, w22
	mov	x1, x23
	mov	x0, x21
	bl	engine_mix_data.isra.11
	b	.L181
	.p2align 2
.L287:
	ldr	w1, [x0, 4]
	mov	w2, 4
	bfi	w1, w2, 3, 4
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, w2
	str	w1, [x0, 4]
	ldr	x27, [x19, 48]
	cbnz	x27, .L297
	b	.L132
	.p2align 2
.L291:
	ldr	w1, [x29, 104]
	ldr	w0, [x29, 184]
	cmp	w1, w0
	bne	.L139
	ldr	w0, [x29, 112]
	orr	w0, w0, w26
	str	w0, [x29, 112]
	b	.L140
	.p2align 2
.L292:
	ldr	w0, [x29, 140]
	lsl	x26, x22, x23
	cmp	w0, 1
	beq	.L298
	ldr	w0, [x27, 36]
	orr	w0, w0, w26
	str	w0, [x27, 36]
.L146:
	ldr	w0, [x27, 12]
	cbz	w0, .L147
	ldr	w25, [x27, 84]
	mov	w0, w23
	bl	aip_query_busy
	cmp	w25, w0
	bhi	.L144
	cbz	w25, .L144
.L300:
	ldr	w0, [x27, 40]
	orr	w26, w0, w26
	str	w26, [x27, 40]
	b	.L144
	.p2align 2
.L290:
	ldr	w0, [x29, 124]
	orr	w0, w0, w26
	str	w0, [x29, 124]
	b	.L138
	.p2align 2
.L147:
	mov	w0, w23
	add	x1, x29, 172
	bl	aip_get_chn_status
	ldr	w25, [x27, 120]
	ldr	w0, [x29, 172]
	cmp	w0, 1
	beq	.L299
	mov	w0, w23
	bl	aip_query_fifo_busy
	cmp	w0, w25
	bcc	.L144
.L304:
	cbnz	w25, .L300
	b	.L144
	.p2align 2
.L294:
	ldr	w0, [x21, 28]
	uxtw	x1, w0
	tbnz	x0, 0, .L200
	mov	w22, 1
	asr	x0, x1, x22
	tbnz	x0, 0, .L170
.L301:
	add	w22, w22, 1
	cmp	w22, 8
	beq	.L132
	asr	x0, x1, x22
	tbz	x0, 0, .L301
.L170:
	cmp	w22, 8
	beq	.L132
.L169:
	ldp	w0, w1, [x21, 48]
	ldr	w2, [x21, 64]
	bl	engine_check_aop_space.isra.0
	cbz	w0, .L132
	mov	w0, w22
	bl	aip_peek_add_mute_size
	ldr	w23, [x21, 84]
	cmp	w0, w23
	csel	w24, w0, w23, ls
	cbz	w24, .L172
	mov	w23, 0
	.p2align 3
.L176:
	ldr	w1, [x21, 44]
	mov	w0, w23
	asr	x1, x1, x23
	tbz	x1, 0, .L174
	bl	aop_get_ring_out
	cbz	x0, .L174
	mov	w2, w24
	mov	x1, 0
	bl	rbuf_write_data_dma
	mov	w0, w23
	bl	aop_proc_delay_compensation
.L174:
	add	w23, w23, 1
	cmp	w23, 8
	bne	.L176
	mov	w1, w24
	mov	w0, w22
	bl	aip_poke_add_mute_size
	b	.L132
	.p2align 2
.L155:
	cmp	w0, 8
	beq	.L168
.L154:
	add	x1, x29, 172
	bl	aop_get_chn_status
	ldr	w0, [x29, 204]
	cmp	w0, 4
	bhi	.L168
	mov	w22, 0
	mov	x24, 1
	.p2align 3
.L165:
	ldr	w0, [x21, 28]
	asr	x0, x0, x22
	tbz	x0, 0, .L159
	ldr	w0, [x21, 40]
	asr	x0, x0, x22
	tbnz	x0, 0, .L159
	mov	w0, w22
	bl	aip_check_data_status_stop
	cmp	w0, 1
	beq	.L159
	mov	w0, w22
	add	x1, x29, 136
	bl	aip_get_chn_status
	ldr	w0, [x29, 136]
	cbz	w0, .L159
	ldr	w1, [x21, 72]
	mov	w0, w22
	bl	aip_continue_output
	ldr	w0, [x21, 12]
	cbnz	w0, .L302
	ldr	w0, [x21, 72]
	cbz	w0, .L159
	mov	w0, w22
	bl	aip_query_fifo_busy
	ldr	w1, [x21, 84]
	cmp	w0, w1
	bcc	.L159
	ldr	w0, [x21, 40]
	lsl	x1, x24, x22
	orr	w0, w0, w1
	str	w0, [x21, 40]
	.p2align 3
.L159:
	add	w22, w22, 1
	cmp	w22, 8
	bne	.L165
	b	.L168
	.p2align 2
.L298:
	ldr	w0, [x27, 32]
	orr	w0, w0, w26
	str	w0, [x27, 32]
	b	.L146
	.p2align 2
.L288:
	str	wzr, [x27, 92]
	ldr	w1, [x0, 4]
	tbz	x1, 2, .L303
	b	.L198
	.p2align 2
.L299:
	ldr	w1, [x29, 188]
	lsl	w0, w25, 1
	cmp	w1, 1
	csel	w25, w0, w25, ne
	mov	w0, w23
	bl	aip_query_fifo_busy
	cmp	w0, w25
	bcc	.L144
	b	.L304
.L302:
	mov	w0, w22
	bl	aip_query_busy
	cbnz	w0, .L305
.L162:
	ldr	w0, [x21, 72]
	cbz	w0, .L159
	mov	w23, 0
	b	.L164
	.p2align 2
.L163:
	add	w23, w23, 1
	cmp	w23, 8
	beq	.L159
.L164:
	ldr	w0, [x21, 44]
	asr	x0, x0, x23
	tbz	x0, 0, .L163
	ldr	w3, [x21, 76]
	mov	w0, w23
	ldp	w1, w2, [x29, 144]
	bl	aop_continue_output
	b	.L163
.L199:
	mov	w0, 0
	b	.L154
.L296:
	mov	w0, 5
	str	w0, [x29, 172]
	ldr	w22, [x21, 12]
	cbnz	w22, .L132
	b	.L187
	.p2align 2
.L185:
	add	w22, w22, 1
	cmp	w22, 8
	beq	.L186
.L187:
	ldr	w0, [x21, 28]
	asr	x0, x0, x22
	tbz	x0, 0, .L185
	mov	w0, w22
	add	x1, x29, 172
	bl	aip_get_pcm_type
	ldr	w0, [x29, 172]
	cmp	w0, 2
	bne	.L185
	ldr	x3, [x21, 112]
	mov	w2, w22
	add	x1, x21, 84
	mov	x0, x21
	bl	engine_mix_data.isra.11
.L186:
	ldr	w0, [x21, 12]
	ldp	w23, w1, [x21, 48]
	stp	w23, w1, [x29, 128]
	cbnz	w0, .L132
	add	x26, x29, 128
	mov	x24, 0
.L189:
	cbz	w23, .L190
	uxtw	x23, w23
	add	x25, x21, 112
	add	x27, x21, 144
	mov	w22, 0
	.p2align 3
.L196:
	mov	w0, w22
	asr	x1, x23, x22
	tbz	x1, 0, .L192
	bl	aop_write_buff_proc_try_cnt
	add	x1, x29, 172
	mov	w0, w22
	bl	aop_get_chn_status
	ldr	w1, [x21, 64]
	mov	w0, w22
	asr	x1, x1, x22
	tbz	x1, 0, .L192
	bl	aop_write_buff_proc_ok_cnt
	ldr	w3, [x21, 68]
	mov	x2, 32
	mov	x1, x25
	add	x0, x29, 136
	cbz	w3, .L194
	ldr	w3, [x29, 192]
	cbz	w3, .L306
.L194:
	bl	adsp_memcpy
.L195:
	add	x1, x29, 136
	mov	w0, w22
	bl	aop_write_rbf_data
	mov	w0, w22
	bl	aop_proc_delay_compensation
.L192:
	add	w22, w22, 1
	cmp	w22, 8
	bne	.L196
.L190:
	add	x24, x24, 4
	cmp	x24, 8
	beq	.L132
	ldr	w23, [x24, x26]
	b	.L189
.L172:
	mov	x0, x21
	bl	engine_check_aip_data
	cbz	w0, .L132
	ldr	w1, [x21, 44]
	mov	w2, w23
	mov	w0, w22
	bl	engine_out_raw_frame
	b	.L132
.L306:
	mov	x1, x27
	bl	adsp_memcpy
	b	.L195
.L305:
	ldr	w1, [x21, 44]
	mov	w2, w0
	mov	w0, w22
	bl	engine_out_raw_frame
	b	.L162
.L295:
	ldr	w5, [x21, 88]
	ldp	w3, w4, [x21]
	ldr	x1, [x21, 184]
	udiv	w2, w2, w5
	mul	w2, w2, w4
	bl	util_mixer_pro
	ldr	w2, [x21, 84]
	mov	w1, 0
	ldr	x0, [x21, 184]
	bl	adsp_memset
	b	.L179
.L200:
	mov	w22, 0
	b	.L169
	.size	engine_process, .-engine_process
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	engine_init
	.type	engine_init, %function
engine_init:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x19, x0
	adrp	x20, .LANCHOR0
	add	x22, x19, 768
	add	x20, x20, :lo12:.LANCHOR0
	mov	w21, 1
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L308:
	str	x19, [x20]
	mov	x0, x19
	mov	x2, 128
	mov	w1, 0
	bl	adsp_memset
	add	x19, x19, 128
	ldr	w0, [x19, -124]
	add	x20, x20, 8
	cmp	x19, x22
	orr	w0, w0, 4
	str	w0, [x19, -124]
	ldr	w0, [x19, -124]
	bfi	w0, w21, 0, 2
	str	w0, [x19, -124]
	str	xzr, [x20, 40]
	bne	.L308
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	engine_init, .-engine_init
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	engine_deinit
	.type	engine_deinit, %function
engine_deinit:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	adrp	x19, .LANCHOR0
	add	x19, x19, :lo12:.LANCHOR0
	add	x20, x19, 48
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L311:
	ldr	x0, [x19, 48]
	add	x19, x19, 8
	bl	engine_destroy_chn
	str	xzr, [x19, 40]
	cmp	x19, x20
	bne	.L311
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	engine_deinit, .-engine_deinit
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	engine_proc_cmd
	.type	engine_proc_cmd, %function
engine_proc_cmd:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 8]
	cmp	w0, 5
	bhi	.L313
	ldr	w1, [x19]
	cbz	w1, .L316
	cmp	w1, 1
	bne	.L313
	uxtw	x0, w0
	adrp	x1, .LANCHOR0
	add	x3, x0, 6
	add	x2, x1, :lo12:.LANCHOR0
	ldr	x2, [x2, x3, lsl 3]
	cbz	x2, .L313
	ldr	w3, [x2, 92]
	cmp	w3, 1
	bne	.L319
	str	wzr, [x2, 92]
.L319:
	add	x1, x1, :lo12:.LANCHOR0
	ldr	x0, [x1, x0, lsl 3]
	ldr	w1, [x0, 4]
	tbnz	x1, 2, .L313
	ldr	w1, [x0, 4]
	and	w1, w1, -121
	str	w1, [x0, 4]
	ldr	w1, [x0, 4]
	orr	w1, w1, 4
	str	w1, [x0, 4]
.L313:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L316:
	bl	engine_proc_chn_start_cmd
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	engine_proc_cmd, .-engine_proc_cmd
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_engine_rm, %object
	.size	g_engine_rm, 96
g_engine_rm:
	.zero	96
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
