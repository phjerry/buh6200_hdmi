#include "hi_asm_define.h"
	.arch armv8-a
	.file	"linear.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_create
	.type	lsrc_create, %function
lsrc_create:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	x21, x0
	mov	w19, w1
	mov	x0, x30
	mov	w20, w2
	mov	w23, w3
	mov	w22, w4
	bl	_mcount
	str	xzr, [x21]
	sub	w1, w19, #1
	mov	w0, 60927
	movk	w0, 0x2, lsl 16
	cmp	w1, w0
	bhi	.L1
	sub	w1, w20, #1
	cmp	w1, w0
	bhi	.L1
	sdiv	w0, w20, w19
	cmp	w0, 6
	ble	.L24
.L1:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L24:
	str	x24, [x29, 56]
	mov	w1, 76
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x24, x0
	cbz	x0, .L22
	mov	x2, 76
	mov	w1, 0
	bl	adsp_memset
	mov	w0, 8000
	cmp	w20, w0
	beq	.L11
	adrp	x2, .LANCHOR0
	add	x2, x2, :lo12:.LANCHOR0
	add	x2, x2, 56
	mov	x0, 1
	mov	w1, 11025
	b	.L8
	.p2align 2
.L26:
	ldr	w1, [x2, x0, lsl 2]
.L8:
	cmp	w1, w20
	mov	w1, w0
	add	x0, x0, 1
	beq	.L25
	cmp	x0, 13
	bne	.L26
.L7:
	cbz	w22, .L9
	sub	w1, w22, #1
	mov	x2, 40
	add	x0, x24, 36
	add	x1, x2, x1, uxtw 2
	add	x1, x24, x1
	.p2align 3
.L10:
	str	wzr, [x0], 4
	cmp	x0, x1
	bne	.L10
.L9:
	stp	w23, w22, [x24]
	stp	w19, w19, [x24, 8]
	str	w20, [x24, 16]
	str	x24, [x21]
	ldr	x24, [x29, 56]
	b	.L1
	.p2align 2
.L22:
	ldr	x24, [x29, 56]
	b	.L1
	.p2align 2
.L25:
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	w0, [x0, w1, uxtw 2]
	str	w0, [x24, 32]
	b	.L7
.L11:
	mov	w0, 3145
	movk	w0, 0x2, lsl 16
	str	w0, [x24, 32]
	b	.L7
	.size	lsrc_create, .-lsrc_create
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_destroy
	.type	lsrc_destroy, %function
lsrc_destroy:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L27
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L27:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	lsrc_destroy, .-lsrc_destroy
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_get_max_output_num
	.type	lsrc_get_max_output_num, %function
lsrc_get_max_output_num:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	cbz	x20, .L34
	ldr	w0, [x20, 16]
	ldr	w1, [x20, 8]
	mul	w0, w19, w0
	sdiv	w0, w0, w1
	add	w0, w0, 1
.L32:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
.L34:
	mov	w0, -1
	b	.L32
	.size	lsrc_get_max_output_num, .-lsrc_get_max_output_num
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_get_min_input_num
	.type	lsrc_get_min_input_num, %function
lsrc_get_min_input_num:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	cbz	x20, .L37
	ldr	w1, [x20, 8]
	ldr	w2, [x20, 16]
	mul	w1, w19, w1
	sdiv	w0, w1, w2
	msub	w1, w0, w2, w1
	cmp	w1, 0
	cinc	w0, w0, ne
.L35:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
.L37:
	mov	w0, -1
	b	.L35
	.size	lsrc_get_min_input_num, .-lsrc_get_min_input_num
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_check_update
	.type	lsrc_check_update, %function
lsrc_check_update:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	x19, x0
	mov	w21, w1
	mov	w20, w2
	mov	w23, w3
	mov	w22, w4
	mov	x0, x30
	bl	_mcount
	ldp	w0, w4, [x19]
	ldp	w1, w2, [x19, 12]
	cmp	w0, w23
	cset	w0, ne
	cmp	w4, w22
	cinc	w0, w0, ne
	cmp	w1, w21
	cinc	w0, w0, ne
	cmp	w2, w20
	cinc	w0, w0, ne
	ldr	x23, [sp, 48]
	cmp	w0, 0
	cset	w0, ne
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.size	lsrc_check_update, .-lsrc_check_update
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_update_change
	.type	lsrc_update_change, %function
lsrc_update_change:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	x19, x0
	mov	w22, w1
	mov	w21, w2
	mov	w23, w3
	mov	w20, w4
	mov	x0, x30
	bl	_mcount
	ldp	w0, w3, [x19]
	ldp	w2, w1, [x19, 12]
	cmp	w0, w23
	cset	w0, ne
	cmp	w3, w20
	cinc	w0, w0, ne
	cmp	w2, w22
	cinc	w0, w0, ne
	cmp	w1, w21
	cinc	w0, w0, ne
	cbz	w0, .L41
	stp	w23, w20, [x19]
	mov	w0, 8000
	stp	w22, w22, [x19, 8]
	cmp	w21, w0
	str	w21, [x19, 16]
	str	xzr, [x19, 20]
	str	wzr, [x19, 28]
	beq	.L48
	adrp	x2, .LANCHOR0
	add	x2, x2, :lo12:.LANCHOR0
	add	x2, x2, 56
	mov	x0, 1
	mov	w1, 11025
	b	.L46
	.p2align 2
.L60:
	ldr	w1, [x2, x0, lsl 2]
.L46:
	cmp	w1, w21
	mov	w1, w0
	add	x0, x0, 1
	beq	.L59
	cmp	x0, 13
	bne	.L60
.L45:
	cbz	w20, .L41
	sub	w20, w20, #1
	add	x0, x19, 36
	add	x19, x19, x20, uxtw 2
	add	x19, x19, 40
	.p2align 3
.L47:
	str	wzr, [x0], 4
	cmp	x0, x19
	bne	.L47
.L41:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L59:
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	w0, [x0, w1, uxtw 2]
	str	w0, [x19, 32]
	b	.L45
	.p2align 2
.L48:
	mov	w0, 3145
	movk	w0, 0x2, lsl 16
	str	w0, [x19, 32]
	b	.L45
	.size	lsrc_update_change, .-lsrc_update_change
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_flush
	.type	lsrc_flush, %function
lsrc_flush:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	mov	w0, 8000
	ldr	w3, [x19, 16]
	str	xzr, [x19, 20]
	cmp	w3, w0
	beq	.L68
	adrp	x2, .LANCHOR0
	add	x2, x2, :lo12:.LANCHOR0
	add	x2, x2, 56
	mov	x0, 1
	mov	w1, 11025
	b	.L65
	.p2align 2
.L77:
	ldr	w1, [x2, x0, lsl 2]
.L65:
	cmp	w1, w3
	mov	w1, w0
	add	x0, x0, 1
	beq	.L76
	cmp	x0, 13
	bne	.L77
.L64:
	ldr	w1, [x19, 4]
	cbz	w1, .L61
	sub	w1, w1, #1
	add	x2, x19, 40
	add	x0, x19, 36
	add	x1, x2, x1, uxtw 2
	.p2align 3
.L67:
	str	wzr, [x0], 4
	cmp	x0, x1
	bne	.L67
.L61:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L76:
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	w0, [x0, w1, uxtw 2]
	str	w0, [x19, 32]
	b	.L64
	.p2align 2
.L68:
	mov	w0, 3145
	movk	w0, 0x2, lsl 16
	str	w0, [x19, 32]
	b	.L64
	.size	lsrc_flush, .-lsrc_flush
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_check_do_src
	.type	lsrc_check_do_src, %function
lsrc_check_do_src:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w1, [x19, 8]
	ldr	w0, [x19, 16]
	cmp	w1, w0
	beq	.L81
	mov	w0, 1
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L81:
	ldr	w0, [x19, 24]
	ldr	x19, [sp, 16]
	cmp	w0, 0
	cset	w0, ne
	ldp	x29, x30, [sp], 32
	ret
	.size	lsrc_check_do_src, .-lsrc_check_do_src
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_set_speed_adjust
	.type	lsrc_set_speed_adjust, %function
lsrc_set_speed_adjust:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	add	w1, w19, 100
	ldr	w0, [x20, 28]
	cmp	w1, 200
	ccmp	w0, w19, 4, ls
	bne	.L92
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L92:
	cmp	w19, 0
	ccmp	w0, 0, 0, ne
	beq	.L93
.L84:
	cmp	w19, 0
	ccmp	w0, 0, 4, eq
	bne	.L94
.L85:
	ldr	w2, [x20, 12]
	add	w0, w19, 1000
	mov	w1, 19923
	str	wzr, [x20, 20]
	movk	w1, 0x1062, lsl 16
	str	w19, [x20, 28]
	mul	w0, w0, w2
	smull	x1, w0, w1
	asr	x1, x1, 38
	sub	w0, w1, w0, asr 31
	str	w0, [x20, 8]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L94:
	mov	w0, 1
	str	w0, [x20, 24]
	b	.L85
	.p2align 2
.L93:
	mov	w1, 1
	str	w1, [x20, 24]
	b	.L84
	.size	lsrc_set_speed_adjust, .-lsrc_set_speed_adjust
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	lsrc_process_frame
	.type	lsrc_process_frame, %function
lsrc_process_frame:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x19, x0
	mov	w20, w3
	mov	x21, x1
	mov	x22, x2
	mov	x0, x30
	bl	_mcount
	ldr	w1, [x19, 24]
	ldp	w0, w2, [x19]
	cmp	w1, 0
	cset	w1, ne
	ldr	w3, [x19, 8]
	cmp	w20, 0
	ldr	w15, [x19, 16]
	cset	w4, gt
	mov	w5, w3
	cmp	w0, 16
	and	w1, w1, w4
	mov	w0, 0
	beq	.L179
	cbnz	w1, .L180
.L128:
	mov	w11, 0
	cmp	w5, w15
	beq	.L181
	cmp	w20, 0
	ble	.L140
.L183:
	ldr	w5, [x19, 20]
	cmp	w15, w5
	ble	.L141
	mul	w14, w11, w2
	ldr	w15, [x19, 16]
	mul	w10, w0, w2
	mov	w13, 2147418112
	add	w12, w14, w2
	mov	w1, w15
	sub	w10, w10, w14
	.p2align 3
.L142:
	add	x7, x19, 36
	mov	w3, w14
	cmp	w2, 0
	ble	.L147
	.p2align 3
.L145:
	ldr	w6, [x19, 32]
	sub	w1, w1, w5
	ldr	w4, [x21, w3, sxtw 2]
	add	w8, w10, w3
	ldr	w9, [x7], 4
	add	w3, w3, 1
	mul	w5, w6, w5
	mul	w1, w1, w6
	smull	x4, w5, w4
	smull	x1, w1, w9
	asr	x4, x4, 32
	asr	x1, x1, 32
	add	w4, w4, w1
	lsr	w4, w4, 14
	cmp	wzr, w4, lsr 15
	lsl	w4, w4, 16
	csel	w4, w4, w13, eq
	str	w4, [x22, w8, sxtw 2]
	cmp	w12, w3
	ldp	w1, w5, [x19, 16]
	bne	.L145
	mov	w15, w1
.L147:
	ldr	w3, [x19, 8]
	add	w0, w0, 1
	add	w10, w10, w2
	add	w5, w5, w3
	str	w5, [x19, 20]
	cmp	w5, w15
	blt	.L142
.L141:
	sub	w5, w5, w15
	str	w5, [x19, 20]
	sub	w12, w20, #1
	cmp	w12, 0
	ble	.L140
	add	w14, w11, 1
	ldr	w15, [x19, 16]
	mul	w11, w11, w2
	mov	w9, 2147418112
	mul	w10, w14, w2
	add	w13, w10, w2
	mov	w7, w13
	.p2align 3
.L148:
	mov	w4, w15
	cmp	w15, w5
	ble	.L152
	mul	w3, w0, w2
	sub	w8, w11, w10
	sub	w3, w3, w10
	.p2align 3
.L155:
	mov	w6, w10
	cmp	w2, 0
	ble	.L153
	.p2align 3
.L151:
	ldr	w1, [x19, 32]
	sub	w4, w4, w5
	add	w16, w8, w6
	ldr	w17, [x21, w6, sxtw 2]
	add	w15, w3, w6
	add	w6, w6, 1
	ldr	w16, [x21, w16, sxtw 2]
	mul	w5, w1, w5
	mul	w4, w4, w1
	smull	x5, w5, w17
	smull	x4, w4, w16
	asr	x5, x5, 32
	asr	x4, x4, 32
	add	w4, w4, w5
	lsr	w4, w4, 14
	cmp	wzr, w4, lsr 15
	lsl	w4, w4, 16
	csel	w4, w4, w9, eq
	str	w4, [x22, w15, sxtw 2]
	cmp	w7, w6
	ldp	w4, w5, [x19, 16]
	bne	.L151
	mov	w15, w4
.L153:
	ldr	w1, [x19, 8]
	add	w0, w0, 1
	add	w3, w3, w2
	add	w5, w5, w1
	str	w5, [x19, 20]
	cmp	w5, w15
	blt	.L155
.L152:
	sub	w5, w5, w15
	str	w5, [x19, 20]
	mov	w10, w13
	add	w7, w7, w2
	add	w11, w11, w2
	subs	w12, w12, #1
	beq	.L154
	add	w13, w13, w2
	b	.L148
.L181:
	cmp	w20, 0
	ble	.L95
	mul	w8, w0, w2
	mov	w7, w20
	mov	w5, 0
	.p2align 3
.L136:
	cmp	w2, 0
	ble	.L182
	mov	w1, w5
	add	w5, w2, w5
	sub	w6, w8, w1
	.p2align 3
.L137:
	add	w3, w6, w1
	ldr	w4, [x21, w1, sxtw 2]
	add	w1, w1, 1
	cmp	w5, w1
	str	w4, [x22, w3, sxtw 2]
	bne	.L137
	add	w8, w8, w2
	subs	w7, w7, #1
	bne	.L136
.L108:
	add	w0, w0, w20
.L95:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
.L180:
	cmp	w3, w15
	beq	.L129
	cmp	w2, 0
	ble	.L131
	sub	w4, w2, #1
	mov	x0, 0
	add	x4, x4, 1
	lsl	x4, x4, 2
.L134:
	add	x1, x19, x0
	ldr	w3, [x21, x0]
	add	x0, x0, 4
	cmp	x4, x0
	str	w3, [x1, 36]
	bne	.L134
.L131:
	str	wzr, [x19, 24]
	sub	w20, w20, #1
	mov	w0, 0
	mov	w11, 1
	cmp	w20, 0
	bgt	.L183
.L140:
	cmp	w2, 0
	ble	.L95
	mul	w11, w11, w2
	add	x19, x19, 36
	add	w2, w2, w11
	.p2align 3
.L156:
	ldr	w1, [x21, w11, sxtw 2]
	add	w11, w11, 1
	str	w1, [x19], 4
	cmp	w2, w11
	bne	.L156
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
.L154:
	sub	w11, w20, #2
	add	w11, w11, w14
	b	.L140
.L179:
	cbnz	w1, .L184
.L97:
	mov	w11, 0
	cmp	w3, w15
	beq	.L185
.L104:
	cmp	w20, 0
	ble	.L111
	ldr	w3, [x19, 20]
	cmp	w15, w3
	ble	.L112
	mul	w13, w11, w2
	ldr	w15, [x19, 16]
	mul	w10, w0, w2
	add	w12, w13, w2
	mov	w1, w15
	sub	w10, w10, w13
.L113:
	add	x7, x19, 36
	mov	w5, w13
	cmp	w2, 0
	ble	.L118
	.p2align 3
.L116:
	ldr	w6, [x19, 32]
	sub	w4, w1, w3
	ldr	w9, [x7], 4
	add	w8, w10, w5
	ldrsh	w1, [x21, w5, sxtw 1]
	add	w5, w5, 1
	mul	w3, w6, w3
	mul	w4, w4, w6
	smull	x3, w1, w3
	smull	x4, w4, w9
	asr	x1, x3, 32
	asr	x3, x4, 32
	add	w1, w1, w3
	lsl	w1, w1, 2
	asr	w3, w1, 31
	cmp	w3, w1, asr 15
	eor	w3, w3, 32767
	csel	w1, w3, w1, ne
	strh	w1, [x22, w8, sxtw 1]
	cmp	w12, w5
	ldp	w1, w3, [x19, 16]
	bne	.L116
	mov	w15, w1
.L118:
	ldr	w4, [x19, 8]
	add	w0, w0, 1
	add	w10, w10, w2
	add	w3, w3, w4
	str	w3, [x19, 20]
	cmp	w3, w15
	blt	.L113
.L112:
	sub	w3, w3, w15
	str	w3, [x19, 20]
	sub	w12, w20, #1
	cmp	w12, 0
	ble	.L111
	add	w14, w11, 1
	ldr	w15, [x19, 16]
	mul	w11, w11, w2
	mul	w10, w14, w2
	add	w13, w10, w2
	mov	w8, w13
	.p2align 3
.L119:
	mov	w1, w15
	cmp	w15, w3
	ble	.L123
	mul	w7, w0, w2
	sub	w9, w11, w10
	sub	w7, w7, w10
	.p2align 3
.L126:
	mov	w5, w10
	cmp	w2, 0
	ble	.L124
	.p2align 3
.L122:
	ldr	w16, [x19, 32]
	sub	w4, w1, w3
	add	w1, w9, w5
	ldrsh	w6, [x21, w5, sxtw 1]
	add	w15, w7, w5
	add	w5, w5, 1
	ldrsh	w1, [x21, w1, sxtw 1]
	mul	w3, w16, w3
	mul	w4, w4, w16
	smull	x3, w6, w3
	smull	x1, w1, w4
	asr	x3, x3, 32
	asr	x1, x1, 32
	add	w1, w1, w3
	lsl	w1, w1, 2
	asr	w3, w1, 31
	cmp	w3, w1, asr 15
	eor	w3, w3, 32767
	csel	w1, w3, w1, ne
	strh	w1, [x22, w15, sxtw 1]
	cmp	w8, w5
	ldp	w1, w3, [x19, 16]
	bne	.L122
	mov	w15, w1
.L124:
	ldr	w4, [x19, 8]
	add	w0, w0, 1
	add	w7, w7, w2
	add	w3, w3, w4
	str	w3, [x19, 20]
	cmp	w3, w15
	blt	.L126
.L123:
	sub	w3, w3, w15
	str	w3, [x19, 20]
	mov	w10, w13
	add	w8, w8, w2
	add	w11, w11, w2
	subs	w12, w12, #1
	beq	.L125
	add	w13, w13, w2
	b	.L119
.L125:
	sub	w11, w20, #2
	add	w11, w11, w14
.L111:
	cmp	w2, 0
	ble	.L95
	mul	w11, w11, w2
	add	x19, x19, 36
	add	w2, w2, w11
	.p2align 3
.L127:
	ldrsh	w1, [x21, w11, sxtw 1]
	add	w11, w11, 1
	str	w1, [x19], 4
	cmp	w2, w11
	bne	.L127
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
.L182:
	add	w5, w2, w5
	add	w8, w8, w2
	subs	w7, w7, #1
	bne	.L136
	b	.L108
.L185:
	cmp	w20, 0
	ble	.L95
	mul	w8, w0, w2
	mov	w7, w20
	mov	w5, 0
.L106:
	cmp	w2, 0
	ble	.L186
	mov	w1, w5
	add	w5, w2, w5
	sub	w6, w8, w1
	.p2align 3
.L107:
	add	w3, w6, w1
	ldrsh	w4, [x21, w1, sxtw 1]
	add	w1, w1, 1
	cmp	w1, w5
	strh	w4, [x22, w3, sxtw 1]
	bne	.L107
.L110:
	add	w8, w8, w2
	subs	w7, w7, #1
	bne	.L106
	b	.L108
.L129:
	cmp	w2, 0
	ble	.L132
	sub	w1, w2, #1
	mov	x0, 0
	add	x1, x1, 1
	lsl	x1, x1, 2
.L133:
	add	x3, x19, x0
	ldr	w3, [x3, 36]
	str	w3, [x22, x0]
	add	x0, x0, 4
	cmp	x1, x0
	bne	.L133
	ldr	w5, [x19, 8]
	ldr	w15, [x19, 16]
.L132:
	mov	w0, 1
	str	wzr, [x19, 24]
	b	.L128
.L184:
	cmp	w3, w15
	beq	.L98
	cmp	w2, 0
	ble	.L100
	sub	w1, w2, #1
	add	x4, x19, 40
	mov	x3, x21
	add	x0, x19, 36
	add	x1, x4, x1, uxtw 2
.L103:
	ldrsh	w4, [x3], 2
	str	w4, [x0], 4
	cmp	x1, x0
	bne	.L103
.L100:
	sub	w20, w20, #1
	mov	w0, 0
	mov	w11, 1
	str	wzr, [x19, 24]
	b	.L104
.L186:
	add	w5, w2, w5
	b	.L110
.L98:
	cmp	w2, 0
	ble	.L101
	sub	w0, w2, #1
	add	x4, x19, 40
	add	x1, x19, 36
	mov	x3, x22
	add	x0, x4, x0, uxtw 2
.L102:
	ldr	w4, [x1], 4
	strh	w4, [x3], 2
	cmp	x0, x1
	bne	.L102
	ldr	w3, [x19, 8]
	ldr	w15, [x19, 16]
.L101:
	mov	w0, 1
	str	wzr, [x19, 24]
	b	.L97
	.size	lsrc_process_frame, .-lsrc_process_frame
	.section	.rodata
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_ivt_sf_tab, %object
	.size	g_ivt_sf_tab, 52
g_ivt_sf_tab:
	.word	134217
	.word	97391
	.word	89478
	.word	67109
	.word	48696
	.word	44739
	.word	33554
	.word	24348
	.word	22369
	.word	12174
	.word	11185
	.word	6087
	.word	5592
	.zero	4
	.type	g_sample_rate_tab, %object
	.size	g_sample_rate_tab, 52
g_sample_rate_tab:
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
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
