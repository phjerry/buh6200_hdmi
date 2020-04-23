#include "hi_asm_define.h"
	.arch armv8-a
	.file	"system_timer.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	timer_sw_func, %function
timer_sw_func:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x1, x0, :lo12:.LANCHOR0
	ldr	w0, [x0, #:lo12:.LANCHOR0]
	cmp	w0, 2
	beq	.L7
.L2:
	mov	w1, 5
	mov	w0, 0
	bl	adsp_mod_timer
	ldp	x29, x30, [sp], 16
	ret
	.p2align 2
.L7:
	ldr	x0, [x1, 8]
	cbz	x0, .L2
	blr	x0
	mov	w1, 5
	mov	w0, 0
	bl	adsp_mod_timer
	ldp	x29, x30, [sp], 16
	ret
	.size	timer_sw_func, .-timer_sw_func
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	timer_reg_isr_func
	.type	timer_reg_isr_func, %function
timer_reg_isr_func:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0+8
	str	x19, [x0, #:lo12:.LANCHOR0+8]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	timer_reg_isr_func, .-timer_reg_isr_func
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	timer_del_isr_func
	.type	timer_del_isr_func, %function
timer_del_isr_func:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0+8
	ldp	x29, x30, [sp], 16
	str	xzr, [x0, #:lo12:.LANCHOR0+8]
	ret
	.size	timer_del_isr_func, .-timer_del_isr_func
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	timer_set_enable
	.type	timer_set_enable, %function
timer_set_enable:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	mov	w1, 2
	ldp	x29, x30, [sp], 16
	str	w1, [x0, #:lo12:.LANCHOR0]
	ret
	.size	timer_set_enable, .-timer_set_enable
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	timer_set_disable
	.type	timer_set_disable, %function
timer_set_disable:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	mov	w1, 3
	ldp	x29, x30, [sp], 16
	str	w1, [x0, #:lo12:.LANCHOR0]
	ret
	.size	timer_set_disable, .-timer_set_disable
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	timer_init
	.type	timer_init, %function
timer_init:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	mov	w3, 1
	adrp	x0, .LANCHOR0
	mov	w2, 5
	adrp	x1, timer_sw_func
	str	w3, [x0, #:lo12:.LANCHOR0]
	add	x1, x1, :lo12:timer_sw_func
	mov	w0, 0
	bl	adsp_add_timer
	ldp	x29, x30, [sp], 16
	ret
	.size	timer_init, .-timer_init
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	timer_deinit
	.type	timer_deinit, %function
timer_deinit:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x1, .LANCHOR0
	add	x2, x1, :lo12:.LANCHOR0
	mov	w0, 0
	str	wzr, [x1, #:lo12:.LANCHOR0]
	str	xzr, [x2, 8]
	bl	adsp_del_timer_sync
	ldp	x29, x30, [sp], 16
	ret
	.size	timer_deinit, .-timer_deinit
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_timer_status, %object
	.size	g_timer_status, 4
g_timer_status:
	.zero	4
	.zero	4
	.type	g_timer_rm, %object
	.size	g_timer_rm, 8
g_timer_rm:
	.zero	8
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
