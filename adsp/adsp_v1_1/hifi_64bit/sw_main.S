#include "hi_asm_define.h"
	.arch armv8-a
	.file	"sw_main.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	ao_engine_task, %function
ao_engine_task:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x20, x0, :lo12:.LANCHOR0
	mov	w21, 19264
	mov	x19, x20
	ldr	x0, [x0, #:lo12:.LANCHOR0]
	movk	w21, 0x4c, lsl 16
	bl	aoe_init
	bl	adsp_set_task_uninterrupt
	str	xzr, [x20, 8]
	strh	wzr, [x20, 16]
	strh	wzr, [x20, 18]
	bl	aoe_sched_statistics_init
	mov	w0, 42405
	movk	w0, 0xa5a5, lsl 16
	str	w0, [x20, 20]
	.p2align 3
.L4:
	bl	adsp_task_should_stop
	cbnz	w0, .L11
.L6:
	bl	adsp_get_time_ms
	uxtw	x0, w0
	str	x0, [x19, 8]
	bl	aoe_proc_thread_sw
	bl	adsp_get_time_ms
	ldr	x1, [x19, 8]
	sub	w0, w0, w1
	cmp	w0, 4
	bhi	.L12
.L3:
	bl	aoe_get_thread_status
	cmp	w0, 1
	beq	.L4
	bl	adsp_get_time_ms
	uxtw	x1, w0
	mov	w0, w21
	str	x1, [x19, 8]
	bl	adsp_schedule_task
	bl	adsp_get_time_ms
	ldr	x1, [x19, 8]
	sub	w0, w0, w1
	cmp	w0, 41
	bls	.L4
	ldrh	w1, [x20, 16]
	mov	w0, 1
	add	w1, w1, w0
	and	w1, w1, 65535
	strh	w1, [x20, 16]
	bl	aoe_sched_statistics_show
	bl	adsp_task_should_stop
	cbz	w0, .L6
.L11:
	bl	adsp_set_task_running
	bl	aoe_deinit
	ldr	x21, [sp, 32]
	mov	w0, 0
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L12:
	ldrh	w1, [x19, 18]
	mov	w0, 0
	add	w1, w1, 1
	and	w1, w1, 65535
	strh	w1, [x19, 18]
	bl	aoe_sched_statistics_show
	b	.L3
	.size	ao_engine_task, .-ao_engine_task
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_sw_engine_create
	.type	aoe_sw_engine_create, %function
aoe_sw_engine_create:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	str	x22, [sp, 40]
	mov	x19, x0
	mov	x0, x30
	bl	_mcount
	cbz	x19, .L18
	adrp	x0, .LANCHOR0
	str	x20, [x29, 24]
	add	x20, x0, :lo12:.LANCHOR0
	adrp	x2, ao_engine_task
	str	x19, [x0, #:lo12:.LANCHOR0]
	adrp	x1, .LC0
	add	x2, x2, :lo12:ao_engine_task
	add	x1, x1, :lo12:.LC0
	add	x0, x20, 24
	str	wzr, [x20, 20]
	bl	adsp_create_task
	mov	w22, w0
	cbz	w0, .L22
	ldr	x20, [x29, 24]
.L13:
	mov	w0, w22
	ldr	x19, [sp, 16]
	ldr	x22, [sp, 40]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L22:
	str	x21, [x29, 32]
	bl	timer_init
	bl	timer_set_enable
	mov	w21, 42405
	ldr	w0, [x20, 20]
	movk	w21, 0xa5a5, lsl 16
	cmp	w0, w21
	beq	.L21
	mov	w19, 511
	mov	w0, 1
	bl	adsp_msleep
	b	.L17
	.p2align 2
.L23:
	bl	adsp_msleep
	subs	w19, w19, #1
	beq	.L21
.L17:
	ldr	w1, [x20, 20]
	mov	w0, 1
	cmp	w1, w21
	bne	.L23
.L21:
	mov	w0, w22
	ldr	x19, [sp, 16]
	ldp	x20, x21, [x29, 24]
	ldr	x22, [sp, 40]
	ldp	x29, x30, [sp], 48
	ret
.L18:
	mov	w22, -1
	b	.L13
	.size	aoe_sw_engine_create, .-aoe_sw_engine_create
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_sw_engine_destory
	.type	aoe_sw_engine_destory, %function
aoe_sw_engine_destory:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	mov	x0, x30
	adrp	x19, .LANCHOR0
	add	x20, x19, :lo12:.LANCHOR0
	bl	_mcount
	bl	timer_set_disable
	bl	timer_deinit
	ldr	x0, [x20, 24]
	bl	adsp_destory_task
	str	xzr, [x19, #:lo12:.LANCHOR0]
	mov	w0, 0
	str	xzr, [x20, 24]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_sw_engine_destory, .-aoe_sw_engine_destory
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_dsp_reg, %object
	.size	g_dsp_reg, 8
g_dsp_reg:
	.zero	8
	.type	g_begin_pts, %object
	.size	g_begin_pts, 8
g_begin_pts:
	.zero	8
	.type	g_sch_timeout_cnt, %object
	.size	g_sch_timeout_cnt, 2
g_sch_timeout_cnt:
	.zero	2
	.type	g_excute_timeout_cnt, %object
	.size	g_excute_timeout_cnt, 2
g_excute_timeout_cnt:
	.zero	2
	.type	g_aoe_running_flag, %object
	.size	g_aoe_running_flag, 4
g_aoe_running_flag:
	.zero	4
	.type	g_aoe_thread, %object
	.size	g_aoe_thread, 8
g_aoe_thread:
	.zero	8
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align	3
.LC0:
	ASCII(.string	"ao_engine_task" )
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
