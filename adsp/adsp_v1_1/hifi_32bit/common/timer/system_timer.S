#include "hi_asm_define.h"
	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 0
	.eabi_attribute 18, 2
	.file	"system_timer.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	timer_sw_func, %function
timer_sw_func:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L8
	ldr	r2, [r3]
	cmp	r2, #2
	beq	.L7
.L2:
	mov	r1, #5
	mov	r0, #0
	bl	adsp_mod_timer
	ldmfd	sp, {fp, sp, pc}
.L7:
	ldr	r3, [r3, #4]
	cmp	r3, #0
	beq	.L2
	blx	r3
	b	.L2
.L9:
	.align	2
.L8:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	timer_sw_func, .-timer_sw_func
	.align	2
	.global	timer_reg_isr_func
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	timer_reg_isr_func, %function
timer_reg_isr_func:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L11
	str	r0, [r3, #4]
	ldmfd	sp, {fp, sp, pc}
.L12:
	.align	2
.L11:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	timer_reg_isr_func, .-timer_reg_isr_func
	.align	2
	.global	timer_del_isr_func
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	timer_del_isr_func, %function
timer_del_isr_func:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L14
	mov	r2, #0
	str	r2, [r3, #4]
	ldmfd	sp, {fp, sp, pc}
.L15:
	.align	2
.L14:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	timer_del_isr_func, .-timer_del_isr_func
	.align	2
	.global	timer_set_enable
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	timer_set_enable, %function
timer_set_enable:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L17
	mov	r2, #2
	str	r2, [r3]
	ldmfd	sp, {fp, sp, pc}
.L18:
	.align	2
.L17:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	timer_set_enable, .-timer_set_enable
	.align	2
	.global	timer_set_disable
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	timer_set_disable, %function
timer_set_disable:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L20
	mov	r2, #3
	str	r2, [r3]
	ldmfd	sp, {fp, sp, pc}
.L21:
	.align	2
.L20:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	timer_set_disable, .-timer_set_disable
	.align	2
	.global	timer_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	timer_init, %function
timer_init:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L23
	mov	r2, #5
	ldr	r1, .L23+4
	mov	r0, #0
	mov	ip, #1
	str	ip, [r3]
	bl	adsp_add_timer
	ldmfd	sp, {fp, sp, pc}
.L24:
	.align	2
.L23:
	.word	.LANCHOR0
	.word	timer_sw_func
	UNWIND(.fnend)
	.size	timer_init, .-timer_init
	.align	2
	.global	timer_deinit
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	timer_deinit, %function
timer_deinit:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r2, .L26
	mov	r3, #0
	mov	r0, r3
	str	r3, [r2, #4]
	str	r3, [r2]
	bl	adsp_del_timer_sync
	ldmfd	sp, {fp, sp, pc}
.L27:
	.align	2
.L26:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	timer_deinit, .-timer_deinit
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_timer_status, %object
	.size	g_timer_status, 4
g_timer_status:
	.space	4
	.type	g_timer_rm, %object
	.size	g_timer_rm, 4
g_timer_rm:
	.space	4
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
