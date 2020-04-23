#include "hi_asm_define.h"
	.arch armv8-a
	.file	"resample.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	cvt_short_short, %function
cvt_short_short:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	w23, w3
	mov	w22, w4
	mov	x19, x0
	mov	x21, x1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	cmp	w23, 0
	ccmp	w22, 2, 0, eq
	bne	.L2
	sbfiz	x20, x20, 1, 32
	add	x3, x21, x20
	cmp	x21, x3
	beq	.L8
	sub	x0, x20, #2
	mov	x2, 0
	lsr	x0, x0, 1
	add	x0, x0, 1
	lsl	x4, x0, 1
	.p2align 3
.L4:
	add	x19, x19, 4
	ldrsh	w1, [x21, x2]
	strh	w1, [x19, -4]
	ldrsh	w1, [x3, x2]
	add	x2, x2, 2
	strh	w1, [x19, -2]
	cmp	x2, x4
	bne	.L4
	lsl	w0, w0, 1
	ldr	x23, [sp, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L2:
	cmp	w22, 2
	lsl	w0, w20, 1
	csel	w20, w0, w20, eq
	sbfiz	x20, x20, 1, 32
	add	x0, x21, x20
	cmp	x21, x0
	beq	.L8
	mov	x0, 0
	.p2align 3
.L6:
	ldrsh	w1, [x21, x0]
	strh	w1, [x19, x0]
	add	x0, x0, 2
	cmp	x0, x20
	bne	.L6
	ubfx	x0, x0, 1, 32
	ldr	x23, [sp, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L8:
	mov	w0, 0
	ldr	x23, [sp, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.size	cvt_short_short, .-cvt_short_short
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	cvt2int, %function
cvt2int:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	mov	w24, w4
	mov	w22, w5
	mov	x19, x0
	mov	x21, x1
	mov	w20, w2
	mov	w23, w3
	mov	x0, x30
	bl	_mcount
	cmp	w24, 0
	ccmp	w22, 2, 0, eq
	bne	.L13
	cmp	w23, 16
	beq	.L31
	sbfiz	x20, x20, 2, 32
	add	x3, x21, x20
	cmp	x21, x3
	beq	.L25
	sub	x0, x20, #4
	mov	x2, 0
	lsr	x0, x0, 2
	add	x0, x0, 1
	lsl	x4, x0, 2
	.p2align 3
.L17:
	add	x19, x19, 8
	ldr	w1, [x21, x2]
	str	w1, [x19, -8]
	ldr	w1, [x3, x2]
	add	x2, x2, 4
	str	w1, [x19, -4]
	cmp	x2, x4
	bne	.L17
.L30:
	lsl	w0, w0, 1
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L13:
	cmp	w22, 2
	lsl	w0, w20, 1
	csel	w20, w0, w20, eq
	cmp	w23, 16
	beq	.L32
	sbfiz	x20, x20, 2, 32
	add	x0, x21, x20
	cmp	x21, x0
	beq	.L25
	mov	x0, 0
	.p2align 3
.L21:
	ldr	w1, [x21, x0]
	str	w1, [x19, x0]
	add	x0, x0, 4
	cmp	x0, x20
	bne	.L21
	ubfx	x0, x0, 2, 32
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L32:
	sbfiz	x20, x20, 1, 32
	add	x0, x21, x20
	cmp	x21, x0
	beq	.L25
	sub	x2, x20, #2
	mov	x0, 0
	lsr	x2, x2, 1
	add	x2, x2, 1
	.p2align 3
.L20:
	ldrsh	w1, [x21, x0, lsl 1]
	lsl	w1, w1, 16
	str	w1, [x19, x0, lsl 2]
	add	x0, x0, 1
	cmp	x0, x2
	bne	.L20
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L31:
	sbfiz	x20, x20, 1, 32
	add	x3, x21, x20
	cmp	x21, x3
	beq	.L25
	sub	x0, x20, #2
	mov	x1, 0
	lsr	x0, x0, 1
	add	x0, x0, 1
	lsl	x4, x0, 1
	.p2align 3
.L16:
	ldrsh	w2, [x21, x1]
	add	x19, x19, 8
	lsl	w2, w2, 16
	str	w2, [x19, -8]
	ldrsh	w2, [x3, x1]
	add	x1, x1, 2
	cmp	x1, x4
	lsl	w2, w2, 16
	str	w2, [x19, -4]
	bne	.L16
	b	.L30
	.p2align 2
.L25:
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.size	cvt2int, .-cvt2int
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	resampler_mono_process24, %function
resampler_mono_process24:
	stp	x29, x30, [sp, -80]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	str	x25, [sp, 64]
	mov	x21, x0
	mov	x24, x1
	mov	x0, x30
	mov	w25, w2
	mov	x23, x3
	bl	_mcount
	ldr	w3, [x21, 40]
	mov	w5, 1
	ldr	x0, [x21, 72]
	mov	w2, w25
	ldr	w19, [x21, 20]
	mov	x1, x24
	mov	w4, w5
	ldr	w20, [x21, 28]
	ldr	x22, [x21, 104]
	bl	cvt2int
	sbfiz	x1, x0, 2, 32
	sxtw	x2, w19
	ldrsw	x12, [x21, 32]
	ldp	x3, x11, [x21, 72]
	neg	x0, x2, lsl 2
	add	x0, x0, 4
	ldr	x10, [x21, 88]
	add	x2, x3, x0
	add	x0, x2, x1
	add	x12, x2, x12, lsl 2
	cmp	x0, x12
	bls	.L50
	add	x13, x23, 4
	.p2align 3
.L49:
	sub	x7, x12, #4
	cmp	w19, 0
	ldr	w14, [x21, 40]
	ble	.L35
	mov	x9, x12
	mov	x6, 0
	mov	w8, 0
	mov	w16, 0
	mov	w15, 0
	.p2align 3
.L36:
	ldr	w24, [x7], -8
	add	w8, w8, 2
	ldr	w30, [x9], 8
	cmp	w19, w8
	ldr	w3, [x10, x6]
	ldr	w2, [x11, x6]
	add	x6, x6, 4
	and	w5, w3, -65536
	ldr	w18, [x7, 4]
	and	w4, w2, -65536
	lsl	w3, w3, 16
	lsl	w2, w2, 16
	ldr	w17, [x9, -4]
	smull	x3, w3, w24
	smull	x2, w2, w30
	smull	x5, w5, w18
	smull	x4, w4, w17
	asr	x3, x3, 32
	asr	x2, x2, 32
	add	w3, w15, w3
	add	w2, w16, w2
	asr	x5, x5, 32
	asr	x4, x4, 32
	add	w15, w3, w5
	add	w16, w2, w4
	bgt	.L36
	add	w2, w16, w15
	cmp	w14, 16
	beq	.L56
	cmp	w14, 18
	beq	.L57
.L41:
	cmp	w14, 20
	beq	.L58
	add	w2, w2, 64
	mov	x4, x13
	asr	w5, w2, 31
	ubfx	x3, x2, 7, 24
	eor	w6, w5, 8388607
	cmp	w5, w2, asr 30
	csel	w2, w3, w6, eq
	str	w2, [x13, -4]
.L40:
	ldr	w20, [x22, w20, sxtw 2]
	add	x13, x13, 4
	ldr	x2, [x21, 96]
	ubfx	x11, x20, 8, 12
	lsr	w10, w20, 20
	ubfx	x3, x20, 1, 7
	asr	w20, w20, 20
	mul	w11, w11, w19
	add	x12, x12, x3, lsl 2
	mul	w10, w10, w19
	cmp	x0, x12
	add	x11, x2, x11, sxtw 1
	add	x10, x2, x10, sxtw 1
	bhi	.L49
	sub	x4, x4, x23
	ubfx	x19, x4, 2, 32
.L34:
	sub	x3, x12, x0
	ldr	x0, [x21, 64]
	ldrsw	x2, [x21, 24]
	asr	x3, x3, 2
	stp	w20, w3, [x21, 28]
	stp	x11, x10, [x21, 80]
	add	x1, x0, x1
	lsl	x2, x2, 2
	bl	adsp_memmove
	ldr	x25, [sp, 64]
	mov	w0, w19
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 80
	ret
	.p2align 2
.L56:
	add	w2, w2, 16384
	asr	w4, w2, 31
	sbfx	x5, x2, 15, 16
	eor	w3, w4, 32767
	cmp	w4, w2, asr 30
	csel	w2, w3, w5, ne
.L43:
	mov	x4, x13
	str	w2, [x13, -4]
	b	.L40
	.p2align 2
.L57:
	add	w2, w2, 4096
	asr	w3, w2, 31
	cmp	w3, w2, asr 30
	beq	.L42
	eor	w2, w3, 131071
	b	.L43
	.p2align 2
.L35:
	cmp	w14, 16
	beq	.L51
	mov	w2, 0
	cmp	w14, 18
	bne	.L41
	mov	w2, 4096
.L42:
	ubfx	x2, x2, 13, 18
	b	.L43
	.p2align 2
.L58:
	add	w2, w2, 1024
	mov	x4, x13
	asr	w5, w2, 31
	ubfx	x3, x2, 11, 20
	eor	w6, w5, 524287
	cmp	w5, w2, asr 30
	csel	w2, w3, w6, eq
	str	w2, [x13, -4]
	b	.L40
.L51:
	mov	w2, 0
	b	.L43
.L50:
	mov	w19, 0
	b	.L34
	.size	resampler_mono_process24, .-resampler_mono_process24
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	resampler_stereo_process24, %function
resampler_stereo_process24:
	stp	x29, x30, [sp, -96]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	mov	x22, x0
	mov	x21, x3
	mov	x0, x30
	mov	x24, x1
	mov	w25, w2
	mov	w26, w4
	bl	_mcount
	ldr	w19, [x22, 20]
	ldr	w3, [x22, 40]
	mov	w2, w25
	ldr	x0, [x22, 72]
	mov	x1, x24
	mov	w5, 2
	mov	w4, w26
	ldr	w20, [x22, 28]
	ldr	x23, [x22, 104]
	bl	cvt2int
	sbfiz	x1, x0, 2, 32
	ldp	x2, x14, [x22, 72]
	sub	w3, w19, #1
	ldrsw	x15, [x22, 32]
	ldp	x13, x16, [x22, 88]
	sub	x0, x2, x3, sxtw 3
	add	x17, x0, x1
	add	x15, x0, x15, lsl 2
	cmp	x17, x15
	mov	x0, x21
	bls	.L68
	stp	x27, x28, [x29, 80]
	.p2align 3
.L67:
	sub	x9, x15, #4
	cmp	w19, 0
	ble	.L61
	mov	x10, x15
	mov	x11, 0
	mov	w12, 0
	mov	w25, 0
	mov	w26, 0
	.p2align 3
.L62:
	ldr	w4, [x13, x11]
	add	w12, w12, 2
	ldr	w2, [x9]
	cmp	w19, w12
	ldp	w7, w27, [x9, -8]
	lsl	w8, w4, 16
	sub	x9, x9, #16
	ldr	w5, [x14, x11]
	ldp	w3, w28, [x10]
	smull	x24, w8, w2
	ldr	w6, [x10, 8]
	and	w4, w4, -65536
	add	x10, x10, 16
	lsl	w18, w5, 16
	asr	x2, x24, 32
	ldr	w24, [x9, 4]
	and	w5, w5, -65536
	smull	x7, w4, w7
	smull	x30, w18, w3
	add	w2, w26, w2
	smull	x8, w8, w27
	add	x11, x11, 4
	smull	x4, w4, w24
	ldr	w24, [x10, -4]
	smull	x18, w18, w28
	asr	x3, x30, 32
	smull	x6, w5, w6
	add	w3, w25, w3
	asr	x8, x8, 32
	asr	x7, x7, 32
	smull	x5, w5, w24
	asr	x18, x18, 32
	add	w2, w2, w18
	add	w3, w3, w8
	asr	x6, x6, 32
	asr	x4, x4, 32
	add	w2, w2, w7
	add	w3, w3, w6
	asr	x5, x5, 32
	add	w25, w3, w4
	add	w26, w2, w5
	bgt	.L62
	add	w3, w25, 64
	add	w2, w26, 64
	asr	w4, w3, 31
	asr	w6, w3, 7
	cmp	w4, w3, asr 30
	asr	w5, w2, 31
	lsl	w3, w4, 8
	lsl	w4, w6, 8
	eor	w3, w3, 2147483392
	asr	w6, w2, 7
	csel	w3, w3, w4, ne
	str	w3, [x21]
	cmp	w5, w2, asr 30
	beq	.L65
	eor	w5, w5, 8388607
.L66:
	add	x21, x21, 8
	lsl	w5, w5, 8
	str	w5, [x21, -4]
	ldr	w20, [x23, w20, sxtw 2]
	ubfx	x14, x20, 8, 12
	lsr	w13, w20, 20
	add	x15, x15, x20, uxtb 2
	asr	w20, w20, 20
	cmp	x17, x15
	mul	w14, w14, w19
	mul	w13, w13, w19
	add	x14, x16, x14, sxtw 1
	add	x13, x16, x13, sxtw 1
	bhi	.L67
	sub	x0, x21, x0
	ldp	x27, x28, [x29, 80]
	asr	x0, x0, 2
	add	x0, x0, x0, lsr 63
	ubfx	x19, x0, 1, 32
.L60:
	ldr	x0, [x22, 64]
	ldrsw	x2, [x22, 24]
	stp	w20, wzr, [x22, 28]
	add	x1, x0, x1
	stp	x14, x13, [x22, 80]
	lsl	x2, x2, 2
	bl	adsp_memmove
	mov	w0, w19
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L61:
	mov	w6, 0
	str	wzr, [x21]
.L65:
	and	w5, w6, 16777215
	b	.L66
.L68:
	mov	w19, 0
	b	.L60
	.size	resampler_stereo_process24, .-resampler_stereo_process24
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	resample_config
	.type	resample_config, %function
resample_config:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w19, w1
	mov	w21, w2
	mov	x20, x0
	mov	x0, x30
	bl	_mcount
	cmp	w19, w21
	beq	.L73
	mov	w0, w21
	mov	w1, w19
	b	.L76
	.p2align 2
.L98:
	sub	w1, w1, w0
	cmp	w0, w1
	beq	.L97
.L76:
	cmp	w1, w0
	bgt	.L98
	sub	w0, w0, w1
	cmp	w0, w1
	bne	.L76
.L97:
	cbz	w0, .L90
	sdiv	w9, w21, w0
	sdiv	w5, w19, w0
	cmp	w9, 1280
	bgt	.L90
.L85:
	add	w0, w9, w5
	ldr	w10, [x20, 16]
	sub	w0, w0, #1
	sdiv	w0, w0, w9
	mul	w0, w0, w10
	cmp	w0, 255
	bgt	.L90
	cmp	w19, 0
	add	w6, w19, 3
	csel	w6, w6, w19, lt
	add	w2, w21, 3
	cmp	w21, 0
	add	w1, w19, w19, lsr 31
	add	w8, w21, w21, lsr 31
	csel	w2, w2, w21, lt
	adrp	x0, .LANCHOR0
	asr	w1, w1, 1
	asr	w8, w8, 1
	asr	w6, w6, 2
	asr	w2, w2, 2
	add	x0, x0, :lo12:.LANCHOR0
	mov	w4, 0
	b	.L82
	.p2align 2
.L78:
	cmp	w3, w1
	beq	.L99
	cmp	w3, w6
	beq	.L100
.L81:
	add	w4, w4, 1
	add	x0, x0, 32
	cmp	w4, 40
	beq	.L90
.L82:
	ldr	w3, [x0]
	cmp	w3, w19
	bne	.L78
	ldr	w7, [x0, 4]
	cmp	w7, w21
	bne	.L78
.L79:
	cmp	w4, 40
	beq	.L90
	ldr	w3, [x0, 12]
	ldr	x2, [x0, 24]
	str	x2, [x20, 96]
	ldr	w1, [x20, 40]
	ldr	x4, [x0, 16]
	lsl	w0, w3, 1
	str	wzr, [x20, 36]
	cmp	w1, 16
	str	x4, [x20, 104]
	mul	w10, w0, w10
	beq	.L96
	sub	w0, w1, #18
	tst	w0, -3
	ccmp	w1, 24, 4, ne
	bne	.L84
.L96:
	ldr	x0, [x20, 64]
	add	x0, x0, x10, sxtw 2
	str	x0, [x20, 72]
.L84:
	sub	w1, w9, #1
	stp	w9, w5, [x20]
	stp	w19, w21, [x20, 8]
	mov	w0, 0
	stp	w3, w10, [x20, 20]
	mul	w1, w1, w3
	str	xzr, [x20, 28]
	add	x1, x2, x1, sxtw 1
	stp	x1, x2, [x20, 80]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L99:
	ldr	w7, [x0, 4]
	cmp	w7, w8
	beq	.L79
	cmp	w3, w6
	bne	.L81
.L100:
	ldr	w3, [x0, 4]
	cmp	w3, w2
	beq	.L79
	add	w4, w4, 1
	add	x0, x0, 32
	cmp	w4, 40
	bne	.L82
.L90:
	mov	w0, -1
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L73:
	cbz	w19, .L90
	mov	w5, 1
	mov	w9, w5
	b	.L85
	.size	resample_config, .-resample_config
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	resampler_create
	.type	resampler_create, %function
resampler_create:
	stp	x29, x30, [sp, -128]!
	add	x29, sp, 0
	stp	x22, x23, [sp, 40]
	str	x20, [sp, 24]
	stp	x24, x25, [sp, 56]
	str	x26, [sp, 72]
	mov	w20, w0
	mov	w24, w1
	mov	x0, x30
	mov	w22, w2
	mov	w26, w3
	mov	w23, w4
	bl	_mcount
	mov	w0, 32000
	cmp	w20, w0
	beq	.L103
	ble	.L167
	mov	w0, 22664
	movk	w0, 0x1, lsl 16
	cmp	w20, w0
	beq	.L103
	ble	.L168
	mov	w0, 45328
	movk	w0, 0x2, lsl 16
	cmp	w20, w0
	beq	.L103
	mov	w0, 60928
	movk	w0, 0x2, lsl 16
	cmp	w20, w0
	beq	.L103
	mov	w0, 30464
	movk	w0, 0x1, lsl 16
	cmp	w20, w0
	beq	.L103
	.p2align 3
.L102:
	add	x0, x29, 128
	mov	w4, w22
	mov	w3, w26
	mov	w2, w24
	mov	w1, w20
	str	xzr, [x0, -8]!
	bl	lsrc_create
	ldr	x25, [x29, 120]
	cbz	x25, .L101
	mov	w1, 128
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x25, x0
	ldr	x0, [x29, 120]
	cbz	x25, .L169
	mov	w1, 1
	str	w1, [x25, 116]
	str	x0, [x25, 120]
.L101:
	mov	x0, x25
	ldr	x20, [sp, 24]
	ldp	x22, x23, [sp, 40]
	ldp	x24, x25, [sp, 56]
	ldr	x26, [sp, 72]
	ldp	x29, x30, [sp], 128
	ret
	.p2align 2
.L178:
	mov	w0, 8000
	cmp	w20, w0
	beq	.L103
	mov	w0, 11025
	cmp	w20, w0
	bne	.L102
	.p2align 3
.L103:
	mov	w0, 32000
	cmp	w24, w0
	beq	.L107
	ble	.L170
	mov	w0, 22664
	movk	w0, 0x1, lsl 16
	cmp	w24, w0
	beq	.L107
	ble	.L171
	mov	w0, 45328
	movk	w0, 0x2, lsl 16
	cmp	w24, w0
	beq	.L107
	mov	w0, 60928
	movk	w0, 0x2, lsl 16
	cmp	w24, w0
	beq	.L107
	mov	w0, 30464
	movk	w0, 0x1, lsl 16
	cmp	w24, w0
	bne	.L102
	.p2align 3
.L107:
	cmp	w20, w24
	bge	.L102
	sub	w0, w22, #1
	cmp	w0, 1
	bhi	.L102
	and	w0, w26, -3
	cmp	w0, 16
	beq	.L132
	sub	w0, w26, #20
	tst	w0, -5
	bne	.L102
.L132:
	mov	w1, w24
	mov	w0, w20
	str	x21, [x29, 32]
	b	.L116
	.p2align 2
.L173:
	sub	w0, w0, w1
	cmp	w0, w1
	beq	.L172
.L116:
	cmp	w1, w0
	blt	.L173
	sub	w1, w1, w0
	cmp	w0, w1
	bne	.L116
.L172:
	cbz	w0, .L161
	sdiv	w21, w24, w0
	sdiv	w0, w20, w0
	cmp	w21, 1280
	str	w0, [x29, 108]
	bgt	.L161
	add	w0, w21, w0
	sub	w0, w0, #1
	sdiv	w0, w0, w21
	mul	w0, w0, w22
	cmp	w0, 255
	bgt	.L161
	cmp	w20, 0
	add	w4, w20, 3
	csel	w4, w4, w20, lt
	add	w6, w24, 3
	cmp	w24, 0
	add	w2, w20, w20, lsr 31
	add	w7, w24, w24, lsr 31
	csel	w6, w6, w24, lt
	str	x19, [x29, 16]
	asr	w2, w2, 1
	adrp	x19, .LANCHOR0
	asr	w7, w7, 1
	asr	w4, w4, 2
	asr	w6, w6, 2
	mov	w1, 0
	add	x19, x19, :lo12:.LANCHOR0
	b	.L121
	.p2align 2
.L117:
	cmp	w0, w2
	beq	.L174
.L119:
	cmp	w0, w4
	beq	.L175
.L120:
	add	w1, w1, 1
	add	x19, x19, 32
	cmp	w1, 40
	beq	.L176
.L121:
	ldr	w0, [x19]
	cmp	w20, w0
	bne	.L117
	ldr	w5, [x19, 4]
	cmp	w24, w5
	bne	.L117
	ldr	w5, [x19, 8]
	cmp	w5, 2
	bne	.L117
.L118:
	stp	x27, x28, [x29, 80]
	cmp	w1, 40
	beq	.L163
	ldr	w28, [x19, 12]
	mov	w1, 128
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	mov	x25, x0
	lsl	w27, w28, 1
	mul	w27, w27, w22
	cbz	x0, .L163
	ldr	x1, [x19, 24]
	cmp	w26, 16
	str	x1, [x0, 96]
	ldr	x0, [x19, 16]
	stp	wzr, w26, [x25, 36]
	str	x0, [x25, 104]
	beq	.L177
	sub	w0, w26, #18
	tst	w0, -3
	ccmp	w26, 24, 4, ne
	bne	.L125
.L166:
	mov	w1, 16640
	mov	w0, 70
	bl	HI_ADSP_MALLOC
	str	x0, [x25, 64]
	cbz	x0, .L164
	sbfiz	x19, x27, 2, 32
	mov	w1, 0
	mov	x2, x19
	bl	adsp_memset
	ldr	x0, [x25, 64]
	ldr	x1, [x25, 96]
	add	x19, x0, x19
	str	x19, [x25, 72]
.L125:
	sub	w0, w21, #1
	ldr	w2, [x29, 108]
	stp	w21, w2, [x25]
	stp	w20, w24, [x25, 8]
	mul	w0, w0, w28
	stp	w22, w28, [x25, 16]
	str	w27, [x25, 24]
	str	xzr, [x25, 28]
	stp	w23, wzr, [x25, 112]
	add	x0, x1, x0, sxtw 1
	stp	x0, x1, [x25, 80]
	mov	x0, x25
	str	xzr, [x25, 120]
	ldr	x19, [x29, 16]
	ldr	x21, [x29, 32]
	ldp	x27, x28, [x29, 80]
	ldr	x20, [sp, 24]
	ldp	x22, x23, [sp, 40]
	ldp	x24, x25, [sp, 56]
	ldr	x26, [sp, 72]
	ldp	x29, x30, [sp], 128
	ret
	.p2align 2
.L167:
	mov	w0, 12000
	cmp	w20, w0
	beq	.L103
	ble	.L178
	mov	w0, 22050
	cmp	w20, w0
	beq	.L103
	mov	w0, 24000
	cmp	w20, w0
	beq	.L103
	mov	w0, 16000
	cmp	w20, w0
	bne	.L102
	b	.L103
	.p2align 2
.L170:
	mov	w0, 12000
	cmp	w24, w0
	beq	.L107
	ble	.L179
	mov	w0, 22050
	cmp	w24, w0
	beq	.L107
	mov	w0, 24000
	cmp	w24, w0
	beq	.L107
	mov	w0, 16000
	cmp	w24, w0
	bne	.L102
	b	.L107
	.p2align 2
.L174:
	ldr	w5, [x19, 4]
	cmp	w5, w7
	bne	.L119
	ldr	w5, [x19, 8]
	cmp	w5, 2
	beq	.L118
	cmp	w0, w4
	bne	.L120
	.p2align 3
.L175:
	ldr	w0, [x19, 4]
	cmp	w0, w6
	bne	.L120
	ldr	w0, [x19, 8]
	cmp	w0, 2
	beq	.L118
	add	w1, w1, 1
	add	x19, x19, 32
	cmp	w1, 40
	bne	.L121
	.p2align 3
.L176:
	ldr	x19, [x29, 16]
	ldr	x21, [x29, 32]
	b	.L102
	.p2align 2
.L161:
	ldr	x21, [x29, 32]
	b	.L102
	.p2align 2
.L168:
	mov	w0, 44100
	cmp	w20, w0
	beq	.L103
	mov	w0, 48000
	cmp	w20, w0
	beq	.L103
	b	.L102
	.p2align 2
.L179:
	mov	w0, 8000
	cmp	w24, w0
	beq	.L107
	mov	w0, 11025
	cmp	w24, w0
	beq	.L107
	b	.L102
	.p2align 2
.L171:
	mov	w0, 44100
	cmp	w24, w0
	beq	.L107
	mov	w0, 48000
	cmp	w24, w0
	beq	.L107
	b	.L102
	.p2align 2
.L177:
	cbnz	w23, .L166
	add	w1, w27, 4096
	mov	w0, 70
	lsl	w1, w1, 1
	bl	HI_ADSP_MALLOC
	str	x0, [x25, 48]
	cbz	x0, .L164
	sbfiz	x19, x27, 1, 32
	mov	w1, 0
	mov	x2, x19
	bl	adsp_memset
	ldr	x0, [x25, 48]
	ldr	x1, [x25, 96]
	add	x19, x0, x19
	str	x19, [x25, 56]
	b	.L125
.L169:
	bl	lsrc_destroy
	b	.L101
.L163:
	ldr	x19, [x29, 16]
	ldr	x21, [x29, 32]
	ldp	x27, x28, [x29, 80]
	b	.L102
.L164:
	mov	x1, x25
	mov	w0, 70
	bl	HI_ADSP_FREE
	ldr	x19, [x29, 16]
	ldr	x21, [x29, 32]
	ldp	x27, x28, [x29, 80]
	b	.L102
	.size	resampler_create, .-resampler_create
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	resampler_free
	.type	resampler_free, %function
resampler_free:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L180
	ldr	w0, [x19, 116]
	cbz	w0, .L205
	ldr	x0, [x19, 120]
	bl	lsrc_destroy
.L202:
	mov	x1, x19
	mov	w0, 70
	bl	HI_ADSP_FREE
.L180:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L205:
	ldr	w0, [x19, 40]
	cmp	w0, 16
	beq	.L206
	sub	w1, w0, #18
	tst	w1, -3
	ccmp	w0, 24, 4, ne
	bne	.L202
.L204:
	ldr	x1, [x19, 64]
	cbz	x1, .L202
.L201:
	mov	w0, 70
	bl	HI_ADSP_FREE
	b	.L202
	.p2align 2
.L206:
	ldr	w0, [x19, 112]
	cbnz	w0, .L204
	ldr	x1, [x19, 48]
	cbnz	x1, .L201
	b	.L202
	.size	resampler_free, .-resampler_free
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	resample_flush
	.type	resample_flush, %function
resample_flush:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L207
	ldr	w0, [x19, 116]
	cbz	w0, .L217
.L207:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L217:
	ldr	w0, [x19]
	ldr	w3, [x19, 20]
	sub	w0, w0, #1
	ldr	x2, [x19, 96]
	ldr	w1, [x19, 40]
	str	xzr, [x19, 28]
	mul	w0, w0, w3
	str	wzr, [x19, 36]
	cmp	w1, 16
	add	x0, x2, x0, sxtw 1
	stp	x0, x2, [x19, 80]
	beq	.L218
	sub	w0, w1, #18
	tst	w0, -3
	ccmp	w1, 24, 4, ne
	bne	.L207
	ldr	w2, [x19, 24]
	mov	w1, 0
	ldr	x0, [x19, 64]
	add	w2, w2, 4096
	sbfiz	x2, x2, 2, 32
.L216:
	bl	adsp_memset
	ldrsw	x0, [x19, 24]
	ldr	x1, [x19, 64]
	add	x0, x1, x0, lsl 2
	str	x0, [x19, 72]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L218:
	ldr	w0, [x19, 112]
	ldr	w2, [x19, 24]
	add	w2, w2, 4096
	cbz	w0, .L212
	sbfiz	x2, x2, 2, 32
	mov	w1, 0
	ldr	x0, [x19, 64]
	b	.L216
	.p2align 2
.L212:
	ldr	x0, [x19, 48]
	mov	w1, 0
	sbfiz	x2, x2, 1, 32
	bl	adsp_memset
	ldrsw	x0, [x19, 24]
	ldr	x1, [x19, 48]
	add	x0, x1, x0, lsl 1
	str	x0, [x19, 56]
	b	.L207
	.size	resample_flush, .-resample_flush
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	new_resampler_check
	.type	new_resampler_check, %function
new_resampler_check:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	x19, x0
	mov	w20, w1
	mov	w21, w2
	mov	w22, w3
	mov	w23, w4
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L222
	ldr	w0, [x19, 116]
	cbnz	w0, .L221
	ldr	w1, [x19, 8]
	mov	w0, 1
	cmp	w20, w1
	beq	.L226
.L219:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L226:
	ldr	w1, [x19, 12]
	cmp	w21, w1
	bne	.L219
	ldr	w1, [x19, 16]
	cmp	w22, w1
	bne	.L219
	ldr	w0, [x19, 40]
	cmp	w0, w23
	cset	w0, ne
	b	.L219
	.p2align 2
.L221:
	ldr	x0, [x19, 120]
	mov	w4, w22
	mov	w3, w23
	mov	w2, w21
	mov	w1, w20
	bl	lsrc_check_update
	ldr	x23, [sp, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
.L222:
	mov	w0, -2147483647
	b	.L219
	.size	new_resampler_check, .-new_resampler_check
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	resampler_get_max_output_num
	.type	resampler_get_max_output_num, %function
resampler_get_max_output_num:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L230
	ldr	w0, [x19, 116]
	cbnz	w0, .L229
	cmp	w20, 2048
	bgt	.L232
	tbnz	x20, 0, .L232
	ldr	w0, [x19, 16]
	ldp	w1, w2, [x19, 8]
	sub	w0, w0, #1
	add	w0, w0, w20
	mul	w0, w0, w2
	sdiv	w0, w0, w1
	add	w0, w0, 1
.L227:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L229:
	ldr	x0, [x19, 120]
	mov	w1, w20
	bl	lsrc_get_max_output_num
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L232:
	mov	w0, -2147483645
	b	.L227
.L230:
	mov	w0, -2147483647
	b	.L227
	.size	resampler_get_max_output_num, .-resampler_get_max_output_num
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	resampler_get_min_input_num
	.type	resampler_get_min_input_num, %function
resampler_get_min_input_num:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	w19, w1
	mov	x20, x0
	mov	x0, x30
	bl	_mcount
	mov	w0, w19
	cbz	x20, .L233
	ldr	w0, [x20, 116]
	cbnz	w0, .L235
	ldp	w0, w2, [x20, 8]
	mov	w3, -2147483645
	ldr	w1, [x20, 16]
	mul	w19, w19, w0
	sdiv	w0, w19, w2
	msub	w2, w0, w2, w19
	cmp	w2, 0
	cinc	w0, w0, ne
	mul	w1, w0, w1
	and	w1, w1, 1
	add	w0, w0, w1
	cmp	w0, 2049
	csel	w0, w0, w3, lt
.L233:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L235:
	ldr	x0, [x20, 120]
	mov	w1, w19
	bl	lsrc_get_min_input_num
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	resampler_get_min_input_num, .-resampler_get_min_input_num
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	resampler_process_frame
	.type	resampler_process_frame, %function
resampler_process_frame:
	stp	x29, x30, [sp, -96]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x24, x25, [sp, 56]
	stp	x26, x27, [sp, 72]
	mov	x20, x0
	mov	x25, x1
	mov	x19, x2
	mov	w24, w3
	mov	w21, w4
	mov	w22, w5
	mov	w26, w6
	mov	x0, x30
	bl	_mcount
	cbz	x20, .L253
	ldr	w27, [x20, 116]
	cbz	w27, .L280
	ldr	x0, [x20, 120]
	mov	w3, w24
	mov	x2, x19
	mov	x1, x25
	bl	lsrc_process_frame
	mov	w27, w0
.L244:
	mov	w0, w27
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x24, x25, [sp, 56]
	ldp	x26, x27, [sp, 72]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L280:
	cbz	x25, .L272
	cmp	w24, 2048
	bgt	.L249
	tbnz	x24, 0, .L249
	cbz	x19, .L272
	ldr	w0, [x20, 8]
	cmp	w21, w0
	bne	.L250
	ldr	w0, [x20, 12]
	cmp	w22, w0
	beq	.L251
.L250:
	mov	w2, w22
	mov	w1, w21
	mov	x0, x20
	bl	resample_config
.L251:
	mov	w1, w24
	mov	x0, x20
	bl	resampler_get_max_output_num
	tbnz	w0, #31, .L281
	ldr	w4, [x20, 16]
	cmp	w4, 2
	beq	.L282
	cmp	w4, 1
	bne	.L273
	ldr	w0, [x20, 40]
	cmp	w0, 16
	beq	.L283
	sub	w1, w0, #18
	tst	w1, -3
	ccmp	w0, 24, 4, ne
	bne	.L279
.L265:
	mov	x3, x19
	mov	w2, w24
	mov	x1, x25
	mov	x0, x20
	bl	resampler_mono_process24
	mov	w27, w0
	mov	w0, w27
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x24, x25, [sp, 56]
	ldp	x26, x27, [sp, 72]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L282:
	ldr	w0, [x20, 40]
	cmp	w0, 16
	beq	.L284
	sub	w1, w0, #18
	tst	w1, -3
	ccmp	w0, 24, 4, ne
	bne	.L279
.L256:
	mov	w4, w26
	mov	x3, x19
	mov	w2, w24
	mov	x1, x25
	mov	x0, x20
	bl	resampler_stereo_process24
	mov	w27, w0
	b	.L244
	.p2align 2
.L281:
	mov	w1, -2147483647
	cmp	w0, w1
	beq	.L253
	mov	w1, -2147483645
	cmp	w0, w1
	beq	.L249
	mov	w27, 8
	movk	w27, 0x8000, lsl 16
	mov	w0, w27
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x24, x25, [sp, 56]
	ldp	x26, x27, [sp, 72]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L249:
	mov	w27, -2147483645
	b	.L244
	.p2align 2
.L283:
	ldr	w0, [x20, 112]
	cbnz	w0, .L265
	str	x23, [x29, 48]
	mov	w3, w26
	mov	w2, w24
	mov	x1, x25
	ldr	x0, [x20, 56]
	ldr	w21, [x20, 20]
	ldr	w22, [x20, 28]
	ldr	x23, [x20, 104]
	bl	cvt_short_short
	sbfiz	x1, x0, 1, 32
	sxtw	x2, w21
	ldrsw	x10, [x20, 32]
	ldr	x3, [x20, 56]
	mov	x0, x19
	neg	x2, x2, lsl 1
	add	x2, x2, 2
	add	x2, x3, x2
	add	x13, x2, x1
	add	x10, x2, x10, lsl 1
	ldp	x12, x11, [x20, 80]
	cmp	x13, x10
	bls	.L266
	.p2align 3
.L270:
	sub	x8, x10, #2
	add	x19, x19, 2
	cmp	w21, 0
	ble	.L274
	mov	w9, 0
	mov	w15, 0
	mov	w14, 0
	mov	x6, 0
	.p2align 3
.L268:
	ldrsh	w2, [x8], -4
	add	x3, x10, x6
	ldr	w16, [x11, x6]
	add	w9, w9, 2
	ldr	w7, [x12, x6]
	cmp	w21, w9
	lsl	w2, w2, 16
	ldrsh	w5, [x3, 2]
	ldrsh	w4, [x8, 2]
	lsl	w17, w16, 16
	and	w16, w16, -65536
	ldrsh	w3, [x10, x6]
	lsl	w5, w5, 16
	add	x6, x6, 4
	lsl	w4, w4, 16
	smull	x2, w2, w17
	lsl	w3, w3, 16
	smull	x4, w4, w16
	asr	x2, x2, 32
	add	w2, w14, w2
	asr	x4, x4, 32
	add	w14, w2, w4
	lsl	w2, w7, 16
	and	w7, w7, -65536
	smull	x3, w3, w2
	smull	x5, w5, w7
	asr	x3, x3, 32
	add	w3, w15, w3
	asr	x5, x5, 32
	add	w15, w3, w5
	bgt	.L268
	add	w4, w14, w15
	add	w4, w4, 16384
	asr	w3, w4, 31
	sbfx	x5, x4, 15, 16
	eor	w2, w3, 32767
	cmp	w3, w4, asr 30
	csel	w2, w2, w5, ne
.L267:
	strh	w2, [x19, -2]
	ldr	w22, [x23, w22, sxtw 2]
	ldr	x2, [x20, 96]
	and	x3, x22, 254
	ubfx	x12, x22, 8, 12
	lsr	w11, w22, 20
	add	x10, x10, x3
	asr	w22, w22, 20
	cmp	x13, x10
	mul	w12, w12, w21
	mul	w11, w11, w21
	add	x12, x2, x12, sxtw 1
	add	x11, x2, x11, sxtw 1
	bhi	.L270
	sub	x0, x19, x0
	ubfx	x27, x0, 1, 32
.L266:
	sub	x3, x10, x13
	ldr	x0, [x20, 48]
	ldrsw	x2, [x20, 24]
	asr	x3, x3, 1
	stp	w22, w3, [x20, 28]
	stp	x12, x11, [x20, 80]
	add	x1, x0, x1
	lsl	x2, x2, 1
	bl	adsp_memmove
	ldr	x23, [x29, 48]
	b	.L244
	.p2align 2
.L284:
	ldr	w0, [x20, 112]
	cbnz	w0, .L256
	str	x23, [x29, 48]
	mov	w3, w26
	mov	w2, w24
	mov	x1, x25
	ldr	x0, [x20, 56]
	ldr	w21, [x20, 20]
	ldr	w22, [x20, 28]
	ldr	x23, [x20, 104]
	bl	cvt_short_short
	sbfiz	x1, x0, 1, 32
	ldr	x2, [x20, 56]
	sub	w3, w21, #1
	ldrsw	x10, [x20, 32]
	ldp	x13, x12, [x20, 80]
	sub	x0, x2, x3, sxtw 2
	add	x11, x0, x1
	add	x10, x0, x10, lsl 1
	cmp	x11, x10
	mov	x0, x19
	bls	.L257
	.p2align 3
.L263:
	sub	x8, x10, #4
	cmp	w21, 0
	ble	.L258
	mov	x14, x10
	mov	w9, 0
	mov	w17, 0
	mov	w2, 0
	mov	x7, 0
	.p2align 3
.L259:
	ldr	w6, [x8], -8
	add	w9, w9, 2
	ldr	w16, [x12, x7]
	cmp	w21, w9
	ldr	w5, [x14], 8
	lsl	w18, w6, 16
	and	w6, w6, -65536
	lsl	w3, w16, 16
	ldr	w15, [x13, x7]
	ldr	w4, [x8, 4]
	and	w16, w16, -65536
	add	x7, x7, 4
	smull	x18, w18, w3
	smull	x6, w6, w3
	ldr	w3, [x14, -4]
	asr	x18, x18, 32
	asr	x6, x6, 32
	add	w2, w2, w18
	add	w6, w17, w6
	lsl	w18, w15, 16
	lsl	w17, w5, 16
	and	w5, w5, -65536
	and	w15, w15, -65536
	smull	x5, w5, w18
	smull	x17, w17, w18
	asr	x5, x5, 32
	add	w5, w6, w5
	lsl	w6, w4, 16
	asr	x17, x17, 32
	and	w4, w4, -65536
	add	w2, w2, w17
	smull	x6, w6, w16
	smull	x4, w4, w16
	asr	x6, x6, 32
	add	w2, w2, w6
	and	w6, w3, -65536
	lsl	w3, w3, 16
	asr	x4, x4, 32
	add	w4, w5, w4
	smull	x6, w6, w15
	smull	x3, w3, w15
	asr	x6, x6, 32
	asr	x3, x3, 32
	add	w17, w4, w6
	add	w2, w2, w3
	bgt	.L259
	add	w2, w2, 16384
	add	w6, w17, 16384
	asr	w3, w2, 31
	asr	w5, w6, 31
	cmp	w3, w2, asr 30
	eor	w3, w3, 32767
	sbfx	x2, x2, 15, 16
	asr	w4, w6, 15
	csel	w2, w3, w2, ne
	eor	w3, w5, 32767
	cmp	w5, w6, asr 30
	strh	w2, [x19], 4
	csel	w4, w3, w4, ne
.L262:
	strh	w4, [x19, -2]
	ldr	w22, [x23, w22, sxtw 2]
	ldr	x2, [x20, 96]
	ubfx	x13, x22, 8, 12
	lsr	w12, w22, 20
	add	x10, x10, x22, uxtb 1
	asr	w22, w22, 20
	cmp	x11, x10
	mul	w13, w13, w21
	mul	w12, w12, w21
	add	x13, x2, x13, sxtw 1
	add	x12, x2, x12, sxtw 1
	bhi	.L263
	sub	x0, x19, x0
	asr	x0, x0, 1
	add	x0, x0, x0, lsr 63
	ubfx	x27, x0, 1, 32
.L257:
	sub	x3, x10, x11
	ldr	x0, [x20, 48]
	ldrsw	x2, [x20, 24]
	asr	x3, x3, 1
	stp	w22, w3, [x20, 28]
	stp	x13, x12, [x20, 80]
	add	x1, x0, x1
	lsl	x2, x2, 1
	bl	adsp_memmove
	ldr	x23, [x29, 48]
	b	.L244
	.p2align 2
.L274:
	mov	w2, 0
	b	.L267
	.p2align 2
.L258:
	strh	wzr, [x19], 4
	mov	w4, 0
	b	.L262
	.p2align 2
.L253:
	mov	w27, -2147483647
	b	.L244
.L272:
	mov	w27, 2
	movk	w27, 0x8000, lsl 16
	b	.L244
.L273:
	mov	w27, 5
	movk	w27, 0x8000, lsl 16
	b	.L244
.L279:
	mov	w27, 6
	movk	w27, 0x8000, lsl 16
	b	.L244
	.size	resampler_process_frame, .-resampler_process_frame
	.data
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_resampler_info, %object
	.size	g_resampler_info, 1280
g_resampler_info:
	.word	32000
	.word	48000
	.word	1
	.word	8
	.xword	SRC_res_32_48_mid_nextphasetab
	.xword	SRC_res_32_48_mid_filttab
	.word	44100
	.word	48000
	.word	1
	.word	8
	.xword	SRC_res_44_48_mid_nextphasetab
	.xword	SRC_res_44_48_mid_filttab
	.word	48000
	.word	48000
	.word	1
	.word	8
	.xword	res_48_48_nextphasetab
	.xword	res_48_48_good_filttab
	.word	8000
	.word	11025
	.word	2
	.word	8
	.xword	SRC_res_8_11_good_nextphasetab
	.xword	SRC_res_8_11_mid_filttab
	.word	16000
	.word	22050
	.word	2
	.word	8
	.xword	SRC_res_8_11_good_nextphasetab
	.xword	SRC_res_8_11_mid_filttab
	.word	8000
	.word	22050
	.word	2
	.word	8
	.xword	SRC_res_8_22_good_nextphasetab
	.xword	SRC_res_8_22_mid_filttab
	.word	16000
	.word	44100
	.word	2
	.word	8
	.xword	SRC_res_8_22_good_nextphasetab
	.xword	SRC_res_8_22_mid_filttab
	.word	8000
	.word	44100
	.word	2
	.word	8
	.xword	SRC_res_8_44_good_nextphasetab
	.xword	SRC_res_8_44_mid_filttab
	.word	11025
	.word	12000
	.word	2
	.word	8
	.xword	SRC_res_11_12_good_nextphasetab
	.xword	SRC_res_11_12_mid_filttab
	.word	22050
	.word	24000
	.word	2
	.word	8
	.xword	SRC_res_11_12_good_nextphasetab
	.xword	SRC_res_11_12_mid_filttab
	.word	11025
	.word	16000
	.word	2
	.word	8
	.xword	SRC_res_11_16_good_nextphasetab
	.xword	SRC_res_11_16_mid_filttab
	.word	22050
	.word	32000
	.word	2
	.word	8
	.xword	SRC_res_11_16_good_nextphasetab
	.xword	SRC_res_11_16_mid_filttab
	.word	11025
	.word	24000
	.word	2
	.word	8
	.xword	SRC_res_11_24_good_nextphasetab
	.xword	SRC_res_11_24_mid_filttab
	.word	22050
	.word	48000
	.word	2
	.word	8
	.xword	SRC_res_11_24_good_nextphasetab
	.xword	SRC_res_11_24_mid_filttab
	.word	11025
	.word	32000
	.word	2
	.word	8
	.xword	SRC_res_11_32_good_nextphasetab
	.xword	SRC_res_11_32_mid_filttab
	.word	11025
	.word	48000
	.word	2
	.word	8
	.xword	SRC_res_11_48_good_nextphasetab
	.xword	SRC_res_11_48_mid_filttab
	.word	12000
	.word	22050
	.word	2
	.word	8
	.xword	SRC_res_12_22_good_nextphasetab
	.xword	SRC_res_12_22_mid_filttab
	.word	24000
	.word	44100
	.word	2
	.word	8
	.xword	SRC_res_12_22_good_nextphasetab
	.xword	SRC_res_12_22_mid_filttab
	.word	12000
	.word	44100
	.word	2
	.word	8
	.xword	SRC_res_12_44_good_nextphasetab
	.xword	SRC_res_12_44_mid_filttab
	.word	8000
	.word	12000
	.word	2
	.word	16
	.xword	SRC_res_8_12_good_nextphasetab
	.xword	SRC_res_8_12_good_filttab
	.word	8000
	.word	16000
	.word	2
	.word	16
	.xword	SRC_res_8_16_good_nextphasetab
	.xword	SRC_res_8_16_good_filttab
	.word	8000
	.word	24000
	.word	2
	.word	16
	.xword	SRC_res_8_24_good_nextphasetab
	.xword	SRC_res_8_24_good_filttab
	.word	8000
	.word	32000
	.word	2
	.word	16
	.xword	SRC_res_8_32_good_nextphasetab
	.xword	SRC_res_8_32_good_filttab
	.word	8000
	.word	48000
	.word	2
	.word	16
	.xword	SRC_res_8_48_good_nextphasetab
	.xword	SRC_res_8_48_good_filttab
	.word	11025
	.word	22050
	.word	2
	.word	16
	.xword	SRC_res_8_16_good_nextphasetab
	.xword	SRC_res_8_16_good_filttab
	.word	11025
	.word	44100
	.word	2
	.word	16
	.xword	SRC_res_8_32_good_nextphasetab
	.xword	SRC_res_8_32_good_filttab
	.word	12000
	.word	16000
	.word	2
	.word	16
	.xword	SRC_res_12_16_good_nextphasetab
	.xword	SRC_res_12_16_good_filttab
	.word	12000
	.word	24000
	.word	2
	.word	16
	.xword	SRC_res_8_16_good_nextphasetab
	.xword	SRC_res_8_16_good_filttab
	.word	12000
	.word	32000
	.word	2
	.word	16
	.xword	SRC_res_12_32_good_nextphasetab
	.xword	SRC_res_12_32_good_filttab
	.word	12000
	.word	48000
	.word	2
	.word	16
	.xword	SRC_res_8_32_good_nextphasetab
	.xword	SRC_res_8_32_good_filttab
	.word	16000
	.word	24000
	.word	2
	.word	16
	.xword	SRC_res_8_12_good_nextphasetab
	.xword	SRC_res_8_12_good_filttab
	.word	16000
	.word	32000
	.word	2
	.word	16
	.xword	SRC_res_8_16_good_nextphasetab
	.xword	SRC_res_8_16_good_filttab
	.word	16000
	.word	48000
	.word	2
	.word	16
	.xword	SRC_res_8_24_good_nextphasetab
	.xword	SRC_res_8_24_good_filttab
	.word	22050
	.word	44100
	.word	2
	.word	16
	.xword	SRC_res_8_16_good_nextphasetab
	.xword	SRC_res_8_16_good_filttab
	.word	24000
	.word	32000
	.word	2
	.word	16
	.xword	SRC_res_12_16_good_nextphasetab
	.xword	SRC_res_12_16_good_filttab
	.word	24000
	.word	48000
	.word	2
	.word	16
	.xword	SRC_res_8_16_good_nextphasetab
	.xword	SRC_res_8_16_good_filttab
	.word	32000
	.word	44100
	.word	2
	.word	16
	.xword	SRC_res_8_11_good_nextphasetab
	.xword	SRC_res_8_11_good_filttab
	.word	32000
	.word	48000
	.word	2
	.word	16
	.xword	SRC_res_8_12_good_nextphasetab
	.xword	SRC_res_8_12_good_filttab
	.word	44100
	.word	48000
	.word	2
	.word	36
	.xword	SRC_res_11_12_good_nextphasetab
	.xword	SRC_res_11_12_good_filttab
	.word	48000
	.word	48000
	.word	2
	.word	8
	.xword	res_48_48_nextphasetab
	.xword	res_48_48_good_filttab
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
