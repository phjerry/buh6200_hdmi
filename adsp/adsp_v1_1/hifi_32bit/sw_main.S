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
	.file	"sw_main.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	ao_engine_task, %function
ao_engine_task:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r4, .L13
	mov	r7, #0
	movw	r5, #19264
	movt	r5, 76
	mov	r6, r7
	ldr	r0, [r4]
	bl	aoe_init
	bl	adsp_set_task_uninterrupt
	mov	r3, #0
	mov	r2, #0
	strh	r7, [r4, #16]
	strd	r2, [r4, #8]
	strh	r7, [r4, #18]
	bl	aoe_sched_statistics_init
	movw	r3, #42405
	movt	r3, 42405
	str	r3, [r4, #20]
.L4:
	bl	adsp_task_should_stop
	subs	r7, r0, #0
	bne	.L11
.L6:
	bl	adsp_get_time_ms
	str	r7, [r4, #12]
	str	r0, [r4, #8]
	bl	aoe_proc_thread_sw
	bl	adsp_get_time_ms
	ldr	r3, [r4, #8]
	sub	r0, r0, r3
	cmp	r0, #4
	bhi	.L12
.L3:
	bl	aoe_get_thread_status
	cmp	r0, #1
	beq	.L4
	bl	adsp_get_time_ms
	str	r6, [r4, #12]
	str	r0, [r4, #8]
	mov	r0, r5
	bl	adsp_schedule_task
	bl	adsp_get_time_ms
	ldr	r3, [r4, #8]
	sub	r0, r0, r3
	cmp	r0, #41
	bls	.L4
	ldrh	r1, [r4, #16]
	mov	r0, #1
	add	r1, r1, r0
	uxth	r1, r1
	strh	r1, [r4, #16]
	bl	aoe_sched_statistics_show
	bl	adsp_task_should_stop
	subs	r7, r0, #0
	beq	.L6
.L11:
	bl	adsp_set_task_running
	bl	aoe_deinit
	mov	r0, #0
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L12:
	ldrh	r1, [r4, #18]
	mov	r0, r7
	add	r1, r1, #1
	uxth	r1, r1
	strh	r1, [r4, #18]
	bl	aoe_sched_statistics_show
	b	.L3
.L14:
	.align	2
.L13:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	ao_engine_task, .-ao_engine_task
	.align	2
	.global	aoe_sw_engine_create
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_sw_engine_create, %function
aoe_sw_engine_create:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	subs	r3, r0, #0
	beq	.L20
	ldr	r5, .L25
	mov	ip, #0
	ldr	r2, .L25+4
	ldr	r1, .L25+8
	mov	r0, r5
	str	ip, [r5, #20]
	str	r3, [r0], #24
	bl	adsp_create_task
	subs	r7, r0, #0
	beq	.L23
.L15:
	mov	r0, r7
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L23:
	bl	timer_init
	movw	r6, #42405
	bl	timer_set_enable
	ldr	r3, [r5, #20]
	movt	r6, 42405
	cmp	r3, r6
	beq	.L15
	mov	r0, #1
	movw	r4, #511
	bl	adsp_msleep
	b	.L19
.L24:
	bl	adsp_msleep
	subs	r4, r4, #1
	beq	.L15
.L19:
	ldr	r3, [r5, #20]
	mov	r0, #1
	cmp	r3, r6
	bne	.L24
	mov	r0, r7
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L20:
	mvn	r7, #0
	b	.L15
.L26:
	.align	2
.L25:
	.word	.LANCHOR0
	.word	ao_engine_task
	.word	.LC0
	UNWIND(.fnend)
	.size	aoe_sw_engine_create, .-aoe_sw_engine_create
	.align	2
	.global	aoe_sw_engine_destory
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_sw_engine_destory, %function
aoe_sw_engine_destory:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r4, .L28
	bl	timer_set_disable
	bl	timer_deinit
	ldr	r0, [r4, #24]
	bl	adsp_destory_task
	mov	r3, #0
	mov	r0, r3
	str	r3, [r4, #24]
	str	r3, [r4]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L29:
	.align	2
.L28:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_sw_engine_destory, .-aoe_sw_engine_destory
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_dsp_reg, %object
	.size	g_dsp_reg, 4
g_dsp_reg:
	.space	4
	.space	4
	.type	g_begin_pts, %object
	.size	g_begin_pts, 8
g_begin_pts:
	.space	8
	.type	g_sch_timeout_cnt, %object
	.size	g_sch_timeout_cnt, 2
g_sch_timeout_cnt:
	.space	2
	.type	g_excute_timeout_cnt, %object
	.size	g_excute_timeout_cnt, 2
g_excute_timeout_cnt:
	.space	2
	.type	g_aoe_running_flag, %object
	.size	g_aoe_running_flag, 4
g_aoe_running_flag:
	.space	4
	.type	g_aoe_thread, %object
	.size	g_aoe_thread, 4
g_aoe_thread:
	.space	4
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	ASCII(.ascii	"ao_engine_task\000" )
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
