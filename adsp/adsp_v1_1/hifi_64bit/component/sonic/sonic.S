#include "hi_asm_define.h"
	.arch armv8-a
	.file	"sonic.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	find_pitch_period_in_range, %function
find_pitch_period_in_range:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	w19, w1
	mov	w21, w2
	mov	x20, x0
	mov	x22, x3
	mov	x23, x4
	mov	x0, x30
	bl	_mcount
	cmp	w19, w21
	bgt	.L11
	mov	x13, 0
	mov	x2, 1
	mov	w14, 255
	mov	w0, 0
	.p2align 3
.L10:
	sxtw	x12, w19
	sxtw	x15, w14
	cmp	w19, 0
	add	x1, x20, x12, lsl 1
	ble	.L3
	mov	x6, 0
	mov	x11, 0
	.p2align 3
.L6:
	ldrsh	w9, [x20, x6, lsl 1]
	ldrsh	w8, [x1, x6, lsl 1]
	add	x6, x6, 1
	and	w10, w9, 65535
	and	w5, w8, 65535
	cmp	w9, w8
	sub	w7, w10, w5
	sub	w5, w5, w10
	and	x7, x7, 65535
	and	x5, x5, 65535
	csel	x5, x5, x7, lt
	cmp	w19, w6
	add	x11, x11, x5
	bgt	.L6
	mul	x5, x11, x15
	cbz	w0, .L7
	sxtw	x1, w0
	mul	x3, x12, x2
	mul	x1, x1, x11
	cmp	x3, x1
	bls	.L8
.L7:
	mov	x2, x11
	mov	w0, w19
.L8:
	mul	x1, x12, x13
	cmp	x1, x5
	csel	x12, x12, x15, cc
	csel	x13, x11, x13, cc
	csel	w14, w19, w14, cc
.L9:
	add	w19, w19, 1
	cmp	w21, w19
	bge	.L10
	udiv	x12, x13, x12
	sxtw	x1, w0
	udiv	x2, x2, x1
	str	w2, [x22]
	str	w12, [x23]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L3:
	cbz	w0, .L13
	mul	x1, x12, x2
	cbnz	x1, .L14
	mov	x12, x15
	b	.L9
	.p2align 2
.L13:
	mov	x12, x15
	mov	w0, w19
	mov	x2, 0
	b	.L9
.L14:
	mov	x5, 0
	mov	x11, 0
	b	.L7
.L11:
	mov	x1, 0
	mov	x2, 1
	mov	w12, 0
	mov	w0, 0
	udiv	x2, x2, x1
	str	w2, [x22]
	str	w12, [x23]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.size	find_pitch_period_in_range, .-find_pitch_period_in_range
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	find_pitch_period, %function
find_pitch_period:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	mov	x20, x0
	mov	x21, x1
	mov	w19, w2
	mov	x0, x30
	bl	_mcount
	ldr	w22, [x20, 108]
	ldr	w8, [x20, 64]
	ldp	w1, w2, [x20, 92]
	cmp	w22, 4000
	ble	.L65
	ldr	w0, [x20, 60]
	cbnz	w0, .L65
	mov	w0, 19923
	movk	w0, 0x1062, lsl 16
	smull	x0, w22, w0
	asr	x0, x0, 40
	sub	w22, w0, w22, asr 31
	cmp	w22, 1
	ccmp	w8, 1, 0, eq
	beq	.L63
	sdiv	w1, w1, w22
	mul	w8, w8, w22
	sdiv	w2, w2, w22
.L25:
	ldr	w3, [x20, 100]
	ldr	x0, [x20, 24]
	sdiv	w3, w3, w22
	cmp	w3, 0
	ble	.L29
	sub	w10, w8, #1
	sub	w3, w3, #1
	mov	x4, 2
	add	x9, x0, 2
	ubfiz	x11, x10, 1, 32
	add	x9, x9, x3, uxtw 1
	add	x11, x11, x4
	mov	x7, x21
	add	x10, x4, x10, uxtw 1
	.p2align 3
.L31:
	mov	w4, 0
	cmp	w8, 0
	ble	.L33
	mov	x3, x7
	add	x6, x10, x7
	mov	w4, 0
	.p2align 3
.L30:
	ldrsh	w5, [x3], 2
	add	w4, w4, w5
	cmp	x3, x6
	bne	.L30
	sdiv	w4, w4, w8
	add	x7, x7, x11
	sxth	w4, w4
.L33:
	strh	w4, [x0], 2
	cmp	x0, x9
	bne	.L31
	ldr	x0, [x20, 24]
.L29:
	add	x4, x29, 60
	add	x3, x29, 56
	bl	find_pitch_period_in_range
	cmp	w22, 1
	beq	.L27
	mul	w1, w0, w22
	lsl	w0, w22, 2
	ldp	w2, w3, [x20, 92]
	sub	w4, w1, w0
	ldr	w7, [x20, 64]
	add	w0, w0, w1
	cmp	w2, w4
	csel	w1, w2, w4, ge
	cmp	w3, w0
	csel	w2, w3, w0, le
	cmp	w7, 1
	beq	.L63
	ldr	w3, [x20, 100]
	ldr	x0, [x20, 24]
	cmp	w3, 0
	ble	.L36
	sub	w9, w7, #1
	sub	w3, w3, #1
	mov	x4, 2
	add	x8, x0, 2
	ubfiz	x10, x9, 1, 32
	add	x8, x8, x3, uxtw 1
	add	x10, x10, x4
	add	x9, x4, x9, uxtw 1
	.p2align 3
.L38:
	mov	w4, 0
	cmp	w7, 0
	ble	.L40
	mov	x3, x21
	add	x6, x9, x21
	mov	w4, 0
	.p2align 3
.L37:
	ldrsh	w5, [x3], 2
	add	w4, w4, w5
	cmp	x3, x6
	bne	.L37
	sdiv	w4, w4, w7
	add	x21, x21, x10
	sxth	w4, w4
.L40:
	strh	w4, [x0], 2
	cmp	x8, x0
	bne	.L38
	ldr	x0, [x20, 24]
.L36:
	add	x4, x29, 60
	add	x3, x29, 56
	bl	find_pitch_period_in_range
.L27:
	ldr	w2, [x29, 56]
	mov	w1, w0
	cbz	w2, .L41
	ldr	w1, [x20, 112]
	cbz	w1, .L45
	cbz	w19, .L42
	lsl	w4, w2, 1
	ldr	w5, [x29, 60]
	add	w3, w4, w2
	cmp	w5, w3
	bgt	.L45
	ldr	w3, [x20, 116]
	add	w3, w3, w3, lsl 1
	cmp	w4, w3
	csel	w1, w1, w0, gt
.L41:
	stp	w0, w2, [x20, 112]
	mov	w0, w1
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L65:
	mov	w22, 1
	cmp	w8, 1
	bne	.L25
.L63:
	add	x4, x29, 60
	add	x3, x29, 56
	mov	x0, x21
	bl	find_pitch_period_in_range
	b	.L27
	.p2align 2
.L45:
	stp	w0, w2, [x20, 112]
	mov	w1, w0
	mov	w0, w1
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
.L42:
	ldr	w3, [x20, 116]
	stp	w0, w2, [x20, 112]
	cmp	w2, w3
	csel	w1, w1, w0, gt
	ldp	x19, x20, [sp, 16]
	mov	w0, w1
	ldp	x21, x22, [sp, 32]
	ldp	x29, x30, [sp], 64
	ret
	.size	find_pitch_period, .-find_pitch_period
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	overlap_add, %function
overlap_add:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	str	x23, [sp, 48]
	mov	w23, w1
	mov	w19, w0
	mov	x21, x2
	mov	x20, x3
	mov	x22, x4
	mov	x0, x30
	bl	_mcount
	cmp	w23, 0
	ble	.L66
	sub	w1, w23, #1
	add	x0, x20, 2
	mov	x3, x20
	mov	x4, x22
	mov	x2, x21
	add	x1, x0, x1, uxtw 1
	.p2align 3
.L70:
	cmp	w19, 0
	ble	.L68
	mov	x6, 0
	mov	w5, 0
	.p2align 3
.L69:
	ldrsh	w7, [x4, x6]
	sub	w8, w19, w5
	ldrsh	w0, [x3, x6]
	mul	w7, w7, w5
	add	w5, w5, 1
	madd	w0, w0, w8, w7
	cmp	w19, w5
	sdiv	w0, w0, w19
	strh	w0, [x2, x6]
	add	x6, x6, x23, sxtw 1
	bne	.L69
.L68:
	add	x3, x3, 2
	add	x4, x4, 2
	add	x2, x2, 2
	cmp	x3, x1
	bne	.L70
.L66:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.size	overlap_add, .-overlap_add
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	free_stream_buffers, %function
free_stream_buffers:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	x1, [x19]
	cbz	x1, .L74
	mov	w0, 70
	bl	adsp_vfree
	str	xzr, [x19]
.L74:
	ldr	x1, [x19, 8]
	cbz	x1, .L75
	mov	w0, 70
	bl	adsp_vfree
	str	xzr, [x19, 8]
.L75:
	ldr	x1, [x19, 16]
	cbz	x1, .L76
	mov	w0, 70
	bl	adsp_vfree
	str	xzr, [x19, 16]
.L76:
	ldr	x1, [x19, 24]
	cbz	x1, .L73
	mov	w0, 70
	bl	adsp_vfree
	str	xzr, [x19, 24]
.L73:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	free_stream_buffers, .-free_stream_buffers
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	allocate_stream_buffers, %function
allocate_stream_buffers:
	stp	x29, x30, [sp, -96]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	str	x27, [sp, 80]
	mov	w19, 57471
	mov	w21, w1
	movk	w19, 0x7e07, lsl 16
	mov	w24, w2
	asr	w23, w21, 31
	mov	x20, x0
	smull	x19, w21, w19
	mov	x0, x30
	bl	_mcount
	mov	w0, 70
	asr	x19, x19, 37
	sub	w19, w19, w23
	lsl	w22, w19, 1
	lsl	w26, w19, 4
	str	w26, [x20, 68]
	mul	w27, w22, w24
	lsl	w25, w27, 4
	mov	w1, w25
	bl	adsp_vmalloc
	str	x0, [x20]
	cbz	x0, .L93
	str	w26, [x20, 76]
	mov	w1, w25
	mov	w0, 70
	bl	adsp_vmalloc
	str	x0, [x20, 8]
	cbz	x0, .L93
	str	w22, [x20, 72]
	lsl	w1, w27, 1
	mov	w0, 70
	bl	adsp_vmalloc
	str	x0, [x20, 16]
	cbz	x0, .L93
	lsl	w1, w19, 2
	mov	w0, 70
	bl	adsp_vmalloc
	str	x0, [x20, 24]
	cbz	x0, .L93
	mov	w1, 34079
	str	xzr, [x20, 48]
	movk	w1, 0x51eb, lsl 16
	str	w24, [x20, 64]
	str	w22, [x20, 100]
	mov	w0, 1
	smull	x1, w21, w1
	stp	w21, wzr, [x20, 108]
	asr	x1, x1, 39
	sub	w23, w1, w23
	stp	w23, w19, [x20, 92]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldr	x27, [sp, 80]
	ldp	x29, x30, [sp], 96
	ret
	.p2align 2
.L93:
	mov	x0, x20
	bl	free_stream_buffers
	mov	x1, x20
	mov	w0, 70
	bl	adsp_vfree
	ldr	x27, [sp, 80]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x29, x30, [sp], 96
	ret
	.size	allocate_stream_buffers, .-allocate_stream_buffers
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	move_new_samples_to_pitch_buffer, %function
move_new_samples_to_pitch_buffer:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x19, x0
	mov	w21, w1
	mov	x0, x30
	bl	_mcount
	ldp	w20, w3, [x19, 84]
	ldr	w0, [x19, 72]
	sub	w20, w20, w21
	add	w1, w3, w20
	cmp	w1, w0
	ble	.L106
	mov	w0, 0
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L106:
	ldr	w4, [x19, 64]
	sxtw	x2, w20
	ldp	x1, x0, [x19, 8]
	sbfiz	x6, x4, 1, 32
	mul	w5, w21, w4
	mul	w3, w3, w4
	mul	x2, x6, x2
	add	x1, x1, x5, sxtw 1
	add	x0, x0, x3, sxtw 1
	bl	adsp_memcpy
	ldr	w1, [x19, 88]
	mov	w0, 1
	add	w20, w1, w20
	stp	w21, w20, [x19, 84]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.size	move_new_samples_to_pitch_buffer, .-move_new_samples_to_pitch_buffer
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	remove_pitch_samples, %function
remove_pitch_samples:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x20, x0
	mov	w19, w1
	mov	x0, x30
	bl	_mcount
	cbz	w19, .L107
	ldr	w0, [x20, 88]
	subs	w2, w0, w19
	bne	.L113
.L109:
	str	w2, [x20, 88]
.L107:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L113:
	ldr	w3, [x20, 64]
	sxtw	x2, w2
	ldr	x1, [x20, 16]
	sbfiz	x4, x3, 1, 32
	mov	x0, x1
	mul	w3, w3, w19
	mul	x2, x4, x2
	add	x1, x1, x3, sxtw 1
	bl	adsp_memmove
	ldr	w2, [x20, 88]
	sub	w2, w2, w19
	b	.L109
	.size	remove_pitch_samples, .-remove_pitch_samples
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_speed
	.type	sonic_get_speed, %function
sonic_get_speed:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 32]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_speed, .-sonic_get_speed
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_speed
	.type	sonic_set_speed, %function
sonic_set_speed:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	str	w20, [x19, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_speed, .-sonic_set_speed
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_pitch
	.type	sonic_get_pitch, %function
sonic_get_pitch:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 40]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_pitch, .-sonic_get_pitch
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_pitch
	.type	sonic_set_pitch, %function
sonic_set_pitch:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	str	w20, [x19, 40]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_pitch, .-sonic_set_pitch
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_rate
	.type	sonic_get_rate, %function
sonic_get_rate:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 44]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_rate, .-sonic_get_rate
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_rate
	.type	sonic_set_rate, %function
sonic_set_rate:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	str	w20, [x19, 44]
	str	xzr, [x19, 48]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_rate, .-sonic_set_rate
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_chord_pitch
	.type	sonic_get_chord_pitch, %function
sonic_get_chord_pitch:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 56]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_chord_pitch, .-sonic_get_chord_pitch
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_chord_pitch
	.type	sonic_set_chord_pitch, %function
sonic_set_chord_pitch:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	str	w20, [x19, 56]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_chord_pitch, .-sonic_set_chord_pitch
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_quality
	.type	sonic_get_quality, %function
sonic_get_quality:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 60]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_quality, .-sonic_get_quality
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_quality
	.type	sonic_set_quality, %function
sonic_set_quality:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	str	w20, [x19, 60]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_quality, .-sonic_set_quality
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_volume
	.type	sonic_get_volume, %function
sonic_get_volume:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 36]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_volume, .-sonic_get_volume
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_volume
	.type	sonic_set_volume, %function
sonic_set_volume:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	str	w20, [x19, 36]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_volume, .-sonic_set_volume
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_destroy_stream
	.type	sonic_destroy_stream, %function
sonic_destroy_stream:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	mov	x0, x19
	bl	free_stream_buffers
	mov	x1, x19
	mov	w0, 70
	bl	adsp_vfree
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_destroy_stream, .-sonic_destroy_stream
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_create_stream
	.type	sonic_create_stream, %function
sonic_create_stream:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	w21, w1
	mov	w20, w0
	mov	x0, x30
	bl	_mcount
	mov	w1, 120
	mov	w0, 70
	bl	adsp_vmalloc
	mov	x19, x0
	cbz	x0, .L130
	mov	x2, 120
	mov	w1, 0
	bl	adsp_memset
	mov	w2, w21
	mov	w1, w20
	mov	x0, x19
	bl	allocate_stream_buffers
	cbz	w0, .L130
	stp	xzr, xzr, [x19, 48]
	mov	x0, 1099511628032
	stp	x0, x0, [x19, 32]
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L130:
	mov	x19, 0
	ldr	x21, [sp, 32]
	mov	x0, x19
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	sonic_create_stream, .-sonic_create_stream
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_sample_rate
	.type	sonic_get_sample_rate, %function
sonic_get_sample_rate:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 108]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_sample_rate, .-sonic_get_sample_rate
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_sample_rate
	.type	sonic_set_sample_rate, %function
sonic_set_sample_rate:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	x0, x19
	bl	free_stream_buffers
	ldr	w2, [x19, 64]
	mov	w1, w20
	mov	x0, x19
	bl	allocate_stream_buffers
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_sample_rate, .-sonic_set_sample_rate
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_num_channels
	.type	sonic_get_num_channels, %function
sonic_get_num_channels:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 64]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_get_num_channels, .-sonic_get_num_channels
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_set_num_channels
	.type	sonic_set_num_channels, %function
sonic_set_num_channels:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x19, x0
	mov	w20, w1
	mov	x0, x30
	bl	_mcount
	mov	x0, x19
	bl	free_stream_buffers
	ldr	w1, [x19, 108]
	mov	w2, w20
	mov	x0, x19
	bl	allocate_stream_buffers
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_set_num_channels, .-sonic_set_num_channels
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_read_short_from_stream
	.type	sonic_read_short_from_stream, %function
sonic_read_short_from_stream:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	str	x23, [sp, 48]
	mov	x20, x0
	mov	x23, x1
	mov	w21, w2
	mov	x0, x30
	bl	_mcount
	ldr	w19, [x20, 84]
	cbz	w19, .L138
	str	x22, [x29, 40]
	cmp	w19, w21
	ldr	w0, [x20, 64]
	ldr	x1, [x20, 8]
	sxtw	x3, w0
	ble	.L140
	sbfiz	x2, x21, 1, 32
	sub	w22, w19, w21
	mov	x0, x23
	mul	x2, x2, x3
	bl	adsp_memcpy
	cmp	w22, 0
	ble	.L142
	ldr	w3, [x20, 64]
	sxtw	x2, w22
	ldr	x1, [x20, 8]
	mov	w19, w21
	sbfiz	x4, x3, 1, 32
	mov	x0, x1
	mul	w21, w3, w21
	mul	x2, x4, x2
	add	x1, x1, x21, sxtw 1
	bl	adsp_memmove
	str	w22, [x20, 84]
	ldr	x22, [x29, 40]
.L138:
	mov	w0, w19
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L140:
	smull	x2, w0, w19
	mov	w22, 0
	mov	x0, x23
	lsl	x2, x2, 1
	bl	adsp_memcpy
.L141:
	str	w22, [x20, 84]
	mov	w0, w19
	ldr	x22, [x29, 40]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldr	x23, [sp, 48]
	ldp	x29, x30, [sp], 64
	ret
	.p2align 2
.L142:
	mov	w19, w21
	b	.L141
	.size	sonic_read_short_from_stream, .-sonic_read_short_from_stream
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_samples_available
	.type	sonic_samples_available, %function
sonic_samples_available:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 84]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	sonic_samples_available, .-sonic_samples_available
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_write_short_to_stream
	.type	sonic_write_short_to_stream, %function
sonic_write_short_to_stream:
	stp	x29, x30, [sp, -128]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	str	x23, [sp, 48]
	str	x28, [sp, 88]
	mov	x19, x0
	mov	x21, x1
	mov	w20, w2
	mov	x0, x30
	bl	_mcount
	ldr	w28, [x19, 80]
	cbz	w20, .L220
	ldr	w1, [x19, 68]
	add	w0, w20, w28
	cmp	w1, w0
	blt	.L169
	str	x22, [x29, 40]
	sbfiz	x2, x20, 1, 32
	stp	x25, x26, [x29, 64]
	ldr	w3, [x19, 64]
	ldr	x0, [x19]
	sxtw	x1, w3
	mul	w3, w28, w3
	mul	x2, x2, x1
	add	x0, x0, x3, sxtw 1
	cbz	x21, .L231
	mov	x1, x21
	bl	adsp_memcpy
.L152:
	ldr	w3, [x19, 80]
	add	w28, w20, w3
	str	w28, [x19, 80]
.L148:
	ldr	w5, [x19, 32]
	ldp	w1, w26, [x19, 40]
	lsl	w5, w5, 8
	ldr	w23, [x19, 56]
	ldr	w22, [x19, 84]
	mul	w0, w1, w26
	cmp	w23, 0
	sdiv	w25, w5, w1
	asr	w0, w0, 8
	csel	w26, w0, w26, eq
	cmp	w25, 256
	beq	.L154
	ldr	w21, [x19, 100]
	cmp	w21, w28
	bgt	.L155
	lsl	w23, w25, 1
	str	x24, [x29, 56]
	sub	w1, w23, #256
	mov	w24, 256
	sub	w24, w24, w25
	mov	w0, 512
	mov	w23, 0
	sub	w0, w0, w25
	str	x27, [x29, 80]
	stp	w0, w1, [x29, 116]
	sub	w1, w25, #256
	str	w1, [x29, 124]
	.p2align 3
.L167:
	ldr	w3, [x19, 64]
	ldr	x4, [x19]
	ldr	w0, [x19, 104]
	mul	w1, w23, w3
	cmp	w0, 0
	add	x27, x4, x1, sxtw 1
	ble	.L156
	ldr	w20, [x19, 100]
	ldr	w1, [x19, 84]
	cmp	w20, w0
	ldr	w2, [x19, 76]
	csel	w20, w20, w0, le
	add	w0, w1, w20
	cmp	w2, w0
	bge	.L157
.L230:
	ldr	w23, [x19, 56]
	ldr	x24, [x29, 56]
	ldr	x27, [x29, 80]
.L155:
	cbnz	w23, .L232
.L170:
	cmp	w26, 256
	beq	.L199
	ldr	w21, [x19, 108]
	mov	w0, 16384
	lsl	w20, w21, 8
	sdiv	w20, w20, w26
	cmp	w20, w0
	ccmp	w21, w0, 0, le
	ble	.L181
	.p2align 3
.L182:
	asr	w20, w20, 1
	asr	w21, w21, 1
	cmp	w20, 16384
	ccmp	w21, w0, 0, le
	bgt	.L182
.L181:
	ldr	w0, [x19, 84]
	cmp	w22, w0
	beq	.L199
	str	x24, [x29, 56]
	mov	w1, w22
	mov	x0, x19
	ldr	w24, [x19, 64]
	bl	move_new_samples_to_pitch_buffer
	cbz	w0, .L226
	ldr	w0, [x19, 88]
	sub	w0, w0, #1
	cmp	w0, 0
	ble	.L200
	sub	w9, w24, #1
	mov	x0, 2
	mov	w1, 0
	add	x9, x0, x9, uxtw 1
.L184:
	ldp	w0, w4, [x19, 48]
	add	w7, w0, 1
	mul	w6, w4, w21
	mul	w2, w7, w20
	cmp	w2, w6
	ble	.L193
	ldr	w2, [x19, 84]
	ldr	w3, [x19, 76]
	add	w5, w2, 1
	cmp	w5, w3
	bgt	.L228
	sbfiz	x8, x1, 1, 32
	.p2align 3
.L194:
	ldp	x3, x5, [x19, 8]
	mul	w2, w24, w2
	cmp	w24, 0
	add	x5, x5, x8
	add	x3, x3, x2, sxtw 1
	ble	.L186
	add	x7, x3, x9
	b	.L188
	.p2align 2
.L187:
	ldp	w0, w4, [x19, 48]
	mul	w6, w21, w4
.L188:
	ldrsw	x2, [x19, 64]
	madd	w0, w20, w0, w20
	sub	w0, w0, w6
	sub	w6, w20, w0
	ldrsh	w2, [x5, x2, lsl 1]
	ldrsh	w4, [x5], 2
	mul	w0, w4, w0
	madd	w0, w2, w6, w0
	sdiv	w0, w0, w20
	strh	w0, [x3], 2
	cmp	x7, x3
	bne	.L187
	ldp	w0, w4, [x19, 48]
	add	w7, w0, 1
.L186:
	add	w4, w4, 1
	ldr	w3, [x19, 84]
	mul	w5, w7, w20
	str	w4, [x19, 52]
	add	w2, w3, 1
	str	w2, [x19, 84]
	mul	w6, w4, w21
	cmp	w5, w6
	ble	.L193
	ldr	w5, [x19, 76]
	add	w3, w3, 2
	cmp	w5, w3
	bge	.L194
.L228:
	ldr	x22, [x29, 40]
	mov	w0, w23
	ldp	x24, x25, [x29, 56]
	ldr	x26, [x29, 72]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldr	x23, [sp, 48]
	ldr	x28, [sp, 88]
	ldp	x29, x30, [sp], 128
	ret
	.p2align 2
.L154:
	ldr	w1, [x19, 76]
	add	w0, w22, w28
	cmp	w1, w0
	bge	.L233
	ldr	x22, [x29, 40]
	ldp	x25, x26, [x29, 64]
.L169:
	mov	w23, 0
.L147:
	mov	w0, w23
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldr	x23, [sp, 48]
	ldr	x28, [sp, 88]
	ldp	x29, x30, [sp], 128
	ret
	.p2align 2
.L220:
	str	x22, [x29, 40]
	stp	x25, x26, [x29, 64]
	b	.L148
	.p2align 2
.L232:
	str	x24, [x29, 56]
	mov	w23, 1
	ldr	w24, [x19, 40]
	cmp	w24, 256
	beq	.L228
	ldr	w0, [x19, 84]
	cmp	w22, w0
	beq	.L228
	mov	w1, w22
	mov	x0, x19
	ldr	w25, [x19, 64]
	bl	move_new_samples_to_pitch_buffer
	cbz	w0, .L226
	ldr	w1, [x19, 88]
	ldr	w0, [x19, 100]
	cmp	w1, w0
	blt	.L197
	sub	w20, w25, #1
	sbfiz	x21, x25, 1, 32
	mov	w23, 0
	lsl	x20, x20, 1
	b	.L180
	.p2align 2
.L234:
	sub	w4, w23, w22
	add	x3, x10, x26
	mov	w1, w25
	mov	w0, w22
	mul	w4, w4, w25
	add	x4, x10, x4, sxtw 1
	bl	overlap_add
	ldr	w3, [x19, 84]
	add	w3, w22, w3
.L173:
	ldr	w0, [x19, 88]
	ldr	w1, [x19, 100]
	str	w3, [x19, 84]
	sub	w0, w0, w23
	cmp	w0, w1
	blt	.L171
.L180:
	mul	w10, w25, w23
	ldr	x1, [x19, 16]
	mov	w2, 0
	mov	x0, x19
	sbfiz	x26, x10, 1, 32
	add	x1, x1, x26
	bl	find_pitch_period
	lsl	w22, w0, 8
	ldr	w1, [x19, 84]
	ldr	w2, [x19, 76]
	add	x4, x26, x20
	sdiv	w22, w22, w24
	asr	w22, w22, 8
	add	w3, w22, w1
	cmp	w2, w3
	blt	.L226
	ldp	x2, x10, [x19, 8]
	mul	w1, w25, w1
	add	w23, w23, w0
	cmp	w24, 255
	add	x2, x2, x1, sxtw 1
	bgt	.L234
	sub	w7, w22, w0
	cmp	w25, 0
	ble	.L173
	add	x11, x10, 2
	add	x10, x10, x26
	add	x11, x11, x4
	.p2align 3
.L179:
	mov	x3, x2
	mov	x6, x10
	cmp	w22, 0
	ble	.L174
	mov	x8, x10
	mov	w1, 0
	b	.L178
	.p2align 2
.L235:
	ldrsh	w4, [x6]
	add	x6, x6, x21
	mul	w4, w4, w9
	sdiv	w4, w4, w0
	strh	w4, [x3]
.L176:
	add	w1, w1, 1
	add	x3, x3, x21
	cmp	w22, w1
	beq	.L174
.L178:
	sub	w9, w0, w1
	sub	w5, w1, w7
	cmp	w7, w1
	bgt	.L235
	ldrsh	w4, [x8]
	sub	w9, w0, w1
	cmp	w0, w1
	add	x8, x8, x21
	mul	w5, w5, w4
	ble	.L177
	ldrsh	w4, [x6]
	add	w1, w1, 1
	add	x6, x6, x21
	cmp	w22, w1
	madd	w5, w4, w9, w5
	sdiv	w5, w5, w0
	strh	w5, [x3]
	add	x3, x3, x21
	bne	.L178
.L174:
	add	x10, x10, 2
	add	x2, x2, 2
	cmp	x11, x10
	bne	.L179
	ldr	w3, [x19, 84]
	add	w3, w22, w3
	b	.L173
	.p2align 2
.L199:
	mov	w23, 1
	ldr	x22, [x29, 40]
	ldp	x25, x26, [x29, 64]
	mov	w0, w23
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldr	x23, [sp, 48]
	ldr	x28, [sp, 88]
	ldp	x29, x30, [sp], 128
	ret
	.p2align 2
.L156:
	mov	w2, 1
	mov	x1, x27
	mov	x0, x19
	bl	find_pitch_period
	cmp	w25, 256
	mov	w8, w0
	ble	.L159
	cmp	w25, 511
	ldr	w1, [x19, 64]
	ble	.L160
	lsl	w9, w0, 8
	ldr	w0, [x29, 124]
	sdiv	w9, w9, w0
	sxtw	x9, w9
.L161:
	ldr	w0, [x19, 84]
	mov	w20, w9
	ldr	w3, [x19, 76]
	add	w2, w0, w9
	cmp	w3, w2
	blt	.L230
	mul	w4, w8, w1
	ldr	x2, [x19, 8]
	mul	w0, w1, w0
	str	x9, [x29, 104]
	str	w8, [x29, 112]
	mov	x3, x27
	add	x4, x27, x4, sxtw 1
	add	x2, x2, x0, sxtw 1
	mov	w0, w9
	bl	overlap_add
	ldr	x9, [x29, 104]
	ldr	w8, [x29, 112]
	ldr	w0, [x19, 84]
	add	w8, w8, w9
	add	w23, w23, w8
	add	w9, w0, w9
	str	w9, [x19, 84]
	.p2align 3
.L158:
	cbz	w20, .L230
	add	w0, w21, w23
	cmp	w0, w28
	ble	.L167
	ldr	w20, [x19, 80]
	sub	w20, w20, w23
	cmp	w20, 0
	ble	.L168
	ldr	w1, [x19, 64]
	sxtw	x2, w20
	ldr	x3, [x19]
	sbfiz	x4, x1, 1, 32
	mov	x0, x3
	mul	w1, w23, w1
	mul	x2, x4, x2
	add	x1, x3, x1, sxtw 1
	bl	adsp_memmove
.L168:
	str	w20, [x19, 80]
	b	.L230
	.p2align 2
.L157:
	sbfiz	x4, x3, 1, 32
	sxtw	x2, w20
	mul	w3, w1, w3
	ldr	x0, [x19, 8]
	mov	x1, x27
	add	w23, w23, w20
	mul	x2, x4, x2
	add	x0, x0, x3, sxtw 1
	bl	adsp_memcpy
	ldr	w1, [x19, 84]
	ldr	w0, [x19, 104]
	add	w1, w1, w20
	str	w1, [x19, 84]
	sub	w0, w0, w20
	str	w0, [x19, 104]
	b	.L158
	.p2align 2
.L159:
	cmp	w25, 127
	ldr	w10, [x19, 64]
	bgt	.L163
	mul	w9, w25, w0
	sdiv	w9, w9, w24
	sxtw	x9, w9
.L164:
	ldr	w0, [x19, 84]
	add	w11, w8, w9
	ldr	w2, [x19, 76]
	add	w1, w11, w0
	cmp	w2, w1
	blt	.L230
	sxtw	x1, w8
	sbfiz	x2, x10, 1, 32
	ldr	x3, [x19, 8]
	mul	w0, w10, w0
	stp	w11, w8, [x29, 96]
	add	w23, w23, w9
	mul	x2, x2, x1
	mov	w20, w9
	mov	x1, x27
	add	x0, x3, x0, sxtw 1
	str	x9, [x29, 104]
	str	w10, [x29, 112]
	bl	adsp_memcpy
	ldr	w10, [x29, 112]
	mov	x4, x27
	ldr	w8, [x29, 100]
	ldr	w2, [x19, 84]
	mov	w1, w10
	ldr	x12, [x19, 8]
	ldr	x9, [x29, 104]
	mul	w3, w8, w10
	add	w8, w8, w2
	mov	w0, w9
	mul	w2, w8, w10
	add	x3, x27, x3, sxtw 1
	add	x2, x12, x2, sxtw 1
	bl	overlap_add
	ldr	w0, [x19, 84]
	ldr	w11, [x29, 96]
	add	w11, w0, w11
	str	w11, [x19, 84]
	b	.L158
	.p2align 2
.L231:
	mov	w1, 0
	bl	adsp_memset
	b	.L152
	.p2align 2
.L160:
	sxtw	x9, w0
	ldr	w0, [x29, 116]
	ldr	w2, [x29, 124]
	mul	w0, w0, w8
	sdiv	w0, w0, w2
	str	w0, [x19, 104]
	b	.L161
	.p2align 2
.L163:
	sxtw	x9, w0
	ldr	w0, [x29, 120]
	mul	w0, w8, w0
	sdiv	w0, w0, w24
	str	w0, [x19, 104]
	b	.L164
	.p2align 2
.L233:
	ldr	w3, [x19, 64]
	sxtw	x2, w28
	ldp	x1, x0, [x19]
	sbfiz	x4, x3, 1, 32
	mul	w3, w22, w3
	mul	x2, x4, x2
	add	x0, x0, x3, sxtw 1
	bl	adsp_memcpy
	ldr	w23, [x19, 56]
	ldr	w3, [x19, 84]
	add	w3, w3, w28
	stp	wzr, w3, [x19, 80]
	cbz	w23, .L170
	b	.L232
	.p2align 2
.L177:
	sdiv	w5, w5, w0
	strh	w5, [x3]
	b	.L176
	.p2align 2
.L226:
	ldr	x22, [x29, 40]
	ldp	x24, x25, [x29, 56]
	ldr	x26, [x29, 72]
	b	.L169
.L193:
	cmp	w7, w21
	beq	.L190
	str	w7, [x19, 48]
.L191:
	ldr	w0, [x19, 88]
	add	w1, w1, 1
	sub	w0, w0, #1
	cmp	w1, w0
	blt	.L184
.L183:
	mov	x0, x19
	mov	w23, 1
	bl	remove_pitch_samples
	ldr	x22, [x29, 40]
	ldp	x24, x25, [x29, 56]
	ldr	x26, [x29, 72]
	b	.L147
.L197:
	mov	w23, 0
.L171:
	mov	w1, w23
	b	.L183
.L190:
	str	wzr, [x19, 48]
	cmp	w4, w20
	bne	.L201
	str	wzr, [x19, 52]
	b	.L191
.L200:
	mov	w1, 0
	b	.L183
.L201:
	mov	w23, 1
	ldr	x22, [x29, 40]
	ldp	x24, x25, [x29, 56]
	ldr	x26, [x29, 72]
	b	.L147
	.size	sonic_write_short_to_stream, .-sonic_write_short_to_stream
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_left_bytes
	.type	sonic_get_left_bytes, %function
sonic_get_left_bytes:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldp	w0, w2, [x19, 80]
	ldr	w1, [x19, 64]
	add	w0, w0, w2
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	mul	w0, w0, w1
	lsl	w0, w0, 1
	ret
	.size	sonic_get_left_bytes, .-sonic_get_left_bytes
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_in_left_bytes
	.type	sonic_get_in_left_bytes, %function
sonic_get_in_left_bytes:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w1, [x19, 64]
	ldr	w0, [x19, 80]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	mul	w0, w0, w1
	lsl	w0, w0, 1
	ret
	.size	sonic_get_in_left_bytes, .-sonic_get_in_left_bytes
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	sonic_get_out_left_bytes
	.type	sonic_get_out_left_bytes, %function
sonic_get_out_left_bytes:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	ldr	w1, [x19, 64]
	ldr	w0, [x19, 84]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	mul	w0, w0, w1
	lsl	w0, w0, 1
	ret
	.size	sonic_get_out_left_bytes, .-sonic_get_out_left_bytes
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
