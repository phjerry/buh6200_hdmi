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
	.file	"aoe_main.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_deinit_wk_buf, %function
aoe_deinit_wk_buf:
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
	ldr	r4, .L3
	mov	r0, #70
	ldr	r1, [r4, #12]
	bl	hi_adsp_vfree_aligned
	mov	r0, r4
	mov	r2, #16
	mov	r1, #0
	bl	adsp_memset
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L4:
	.align	2
.L3:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_deinit_wk_buf, .-aoe_deinit_wk_buf
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_proc_cmd, %function
aoe_proc_cmd:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #28)
	sub	sp, sp, #28
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r4, .L37
.L11:
	ldr	r3, [r4]
	ldr	r2, [r4, #8]
	ldr	r1, [r4, #20]
	cmp	r3, r2
	ldr	r2, [r4, #16]
	addlt	r0, r3, #1
	ldr	r5, [r4, #12]
	movge	r0, #1
	str	r0, [r4]
	ldr	r0, [r1]
	movge	r3, #0
	ldr	r1, [r2]
	ldr	r2, [r4, #4]
	sub	r1, r1, r0
	cmp	r1, #15
	mla	r5, r2, r3, r5
	bhi	.L34
.L5:
	sub	sp, fp, #32
	ldmfd	sp, {r4, r5, r6, r7, r8, fp, sp, pc}
.L34:
	ldr	r6, [r4, #24]
	ldr	r2, [r4, #28]
	and	r0, r0, r6
	add	r6, r6, #1
	sub	r6, r6, r0
	add	r2, r2, r0
	cmp	r6, #16
	sub	r0, fp, #56
	movcc	r7, r6
	movcs	r7, #16
	mov	r3, r7
	mov	r1, r7
	bl	memcpy_s
	cmp	r0, #0
	bne	.L5
	cmp	r6, #15
	bls	.L14
.L16:
	ldr	r0, [r4, #20]
	ldr	r1, [r4, #16]
	ldr	r2, [fp, #-44]
	ldr	r3, [r0]
	ldr	r1, [r1]
	add	r2, r2, #16
	sub	r1, r1, r3
	cmp	r1, r2
	bcc	.L5
	add	r3, r3, #16
	str	r3, [r0]
	ldr	r6, [fp, #-44]
	ldr	r2, [r4, #20]
	cmp	r6, #61440
	bhi	.L35
	ldr	r3, [r4, #16]
	ldr	r1, [r2]
	ldr	r3, [r3]
	sub	r3, r3, r1
	cmp	r6, r3
	movhi	r3, #0
	bls	.L36
.L19:
	cmp	r6, r3
	bne	.L5
	ldr	r3, [fp, #-52]
	str	r5, [fp, #-40]
	cmp	r3, #1
	beq	.L9
	bcc	.L7
	cmp	r3, #2
	bne	.L11
	sub	r0, fp, #56
	bl	aop_proc_cmd
	b	.L11
.L36:
	cmp	r5, #0
	beq	.L26
	ldr	r7, [r4, #24]
	mov	r0, r5
	ldr	r2, [r4, #28]
	and	r1, r1, r7
	add	r7, r7, #1
	sub	r7, r7, r1
	add	r2, r2, r1
	cmp	r6, r7
	movcc	r8, r6
	movcs	r8, r7
	mov	r3, r8
	mov	r1, r8
	bl	memcpy_s
	cmp	r0, #0
	bne	.L25
	cmp	r6, r7
	bhi	.L24
.L33:
	ldr	r2, [r4, #20]
	ldr	r1, [r2]
.L26:
	add	r1, r6, r1
	str	r1, [r2]
	mov	r3, r6
	ldr	r6, [fp, #-44]
	b	.L19
.L7:
	sub	r0, fp, #56
	bl	aip_proc_cmd
	b	.L11
.L9:
	sub	r0, fp, #56
	bl	engine_proc_cmd
	b	.L11
.L24:
	sub	r3, r6, r8
	add	r0, r5, r8
	ldr	r2, [r4, #28]
	mov	r1, r3
	bl	memcpy_s
	cmp	r0, #0
	beq	.L33
.L25:
	cmp	r6, #0
	ldrne	r6, [fp, #-44]
	movne	r3, #0
	bne	.L19
	b	.L33
.L14:
	rsb	r3, r7, #16
	sub	r2, fp, #56
	add	r0, r2, r7
	ldr	r2, [r4, #28]
	mov	r1, r3
	bl	memcpy_s
	cmp	r0, #0
	beq	.L16
	sub	sp, fp, #32
	ldmfd	sp, {r4, r5, r6, r7, r8, fp, sp, pc}
.L35:
	ldr	r3, [r2]
	add	r6, r3, r6
	str	r6, [r2]
	sub	sp, fp, #32
	ldmfd	sp, {r4, r5, r6, r7, r8, fp, sp, pc}
.L38:
	.align	2
.L37:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_proc_cmd, .-aoe_proc_cmd
	.align	2
	.global	aoe_reset
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reset, %function
aoe_reset:
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
	ldr	r3, .L40
	mov	r2, #0
	str	r2, [r3, #32]
	ldmfd	sp, {fp, sp, pc}
.L41:
	.align	2
.L40:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_reset, .-aoe_reset
	.align	2
	.global	aoe_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_init, %function
aoe_init:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #12)
	sub	sp, sp, #12
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r4, .L50
	mov	r5, r0
	ldr	r6, [r4, #32]
	cmp	r6, #0
	beq	.L49
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
.L49:
	add	r0, r0, #832
	str	r5, [r4, #36]
	bl	aip_init
	add	r0, r5, #1600
	bl	aop_init
	add	r0, r5, #64
	bl	engine_init
	mov	r1, r6
	mov	r2, #16
	mov	r0, r4
	bl	adsp_memset
	mov	r2, #32
	mov	r1, #122880
	mov	r0, #70
	bl	hi_adsp_vmalloc_aligned
	cmp	r0, #0
	strne	r0, [r4, #12]
	movne	r3, #61440
	strne	r3, [r4, #4]
	ldr	r3, [r4, #36]
	movne	r2, #2
	strne	r2, [r4, #8]
	mov	r2, #0
	str	r2, [fp, #-36]
	add	r0, r3, #28
	str	r2, [fp, #-32]
	add	lr, r3, #32
	ldr	r1, [r3, #16]
	ldr	r2, [r3, #24]
	str	r1, [fp, #-36]
	cmp	r2, #1
	ldr	r3, [r3, #20]
	str	r3, [fp, #-32]
	bls	.L45
	sub	ip, r2, #1
	tst	r2, ip
	movne	r3, #1
	moveq	r3, #0
	cmp	r0, #0
	moveq	r3, #1
	cmp	r3, #0
	bne	.L45
	cmp	r1, #0
	cmpne	lr, #0
	beq	.L45
	mov	r3, #1
	str	r0, [r4, #16]
	str	lr, [r4, #20]
	str	ip, [r4, #24]
	str	r1, [r4, #28]
	str	r3, [r4, #32]
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
.L45:
	bl	aoe_deinit_wk_buf
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
.L51:
	.align	2
.L50:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_init, .-aoe_init
	.align	2
	.global	aoe_deinit
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_deinit, %function
aoe_deinit:
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
	ldr	r4, .L57
	ldr	r3, [r4, #32]
	cmp	r3, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	bl	aip_deinit
	bl	aop_deinit
	bl	engine_deinit
	bl	aoe_deinit_wk_buf
	mov	r3, #0
	str	r3, [r4, #36]
	str	r3, [r4, #32]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L58:
	.align	2
.L57:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_deinit, .-aoe_deinit
	.align	2
	.global	aoe_flush_wk_buf
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_flush_wk_buf, %function
aoe_flush_wk_buf:
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
	ldr	r3, .L60
	mov	r2, #0
	str	r2, [r3]
	ldmfd	sp, {fp, sp, pc}
.L61:
	.align	2
.L60:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_flush_wk_buf, .-aoe_flush_wk_buf
	.align	2
	.global	aoe_get_wk_buf
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_get_wk_buf, %function
aoe_get_wk_buf:
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
	ldr	r3, .L65
	ldr	r2, [r3]
	ldr	r1, [r3, #8]
	ldr	r0, [r3, #4]
	cmp	r2, r1
	ldr	r1, [r3, #12]
	addlt	ip, r2, #1
	movge	r2, #0
	movge	ip, #1
	mla	r0, r0, r2, r1
	str	ip, [r3]
	ldmfd	sp, {fp, sp, pc}
.L66:
	.align	2
.L65:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_get_wk_buf, .-aoe_get_wk_buf
	.align	2
	.global	aoe_release_wk_buf
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_release_wk_buf, %function
aoe_release_wk_buf:
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
	ldr	r3, .L71
	ldr	r2, [r3]
	cmp	r2, #0
	ldreq	r2, [r3, #8]
	sub	r2, r2, #1
	str	r2, [r3]
	ldmfd	sp, {fp, sp, pc}
.L72:
	.align	2
.L71:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_release_wk_buf, .-aoe_release_wk_buf
	.align	2
	.global	aoe_get_wk_buf_size
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_get_wk_buf_size, %function
aoe_get_wk_buf_size:
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
	ldr	r3, .L74
	ldr	r0, [r3, #4]
	ldmfd	sp, {fp, sp, pc}
.L75:
	.align	2
.L74:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_get_wk_buf_size, .-aoe_get_wk_buf_size
	.align	2
	.global	aoe_sched_statistics_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_sched_statistics_init, %function
aoe_sched_statistics_init:
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
	ldr	r3, .L81
	ldr	r2, [r3, #32]
	cmp	r2, #0
	ldrne	r3, [r3, #36]
	movne	r2, #0
	strne	r2, [r3, #12]
	ldmfd	sp, {fp, sp, pc}
.L82:
	.align	2
.L81:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_sched_statistics_init, .-aoe_sched_statistics_init
	.align	2
	.global	aoe_sched_statistics_show
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_sched_statistics_show, %function
aoe_sched_statistics_show:
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
	ldr	r3, .L90
	ldr	r2, [r3, #32]
	cmp	r2, #0
	ldmfdeq	sp, {fp, sp, pc}
	cmp	r0, #0
	beq	.L89
	cmp	r0, #1
	ldreq	r3, [r3, #36]
	ldreq	r2, [r3, #12]
	bfieq	r2, r1, #16, #16
	streq	r2, [r3, #12]
	ldmfd	sp, {fp, sp, pc}
.L89:
	ldr	r3, [r3, #36]
	ldr	r2, [r3, #12]
	bfi	r2, r1, #0, #16
	str	r2, [r3, #12]
	ldmfd	sp, {fp, sp, pc}
.L91:
	.align	2
.L90:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_sched_statistics_show, .-aoe_sched_statistics_show
	.align	2
	.global	aoe_proc_thread_sw
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_proc_thread_sw, %function
aoe_proc_thread_sw:
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
	ldr	r4, .L96
	mov	r2, #0
	ldr	r3, [r4, #32]
	str	r2, [r4, #40]
	cmp	r3, r2
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	bl	aoe_proc_cmd
	bl	aip_process
	bl	aop_process
	bl	engine_process
	ldr	r3, [r4, #44]
	mov	r2, #1
	str	r2, [r4, #40]
	add	r3, r3, r2
	str	r3, [r4, #44]
	ands	r3, r3, #3
	streq	r3, [r4, #40]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L97:
	.align	2
.L96:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_proc_thread_sw, .-aoe_proc_thread_sw
	.align	2
	.global	aoe_proc_thread
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_proc_thread, %function
aoe_proc_thread:
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
	ldr	r4, .L102
	mov	r2, #0
	ldr	r3, [r4, #32]
	str	r2, [r4, #40]
	cmp	r3, r2
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	bl	aoe_proc_cmd
	bl	aip_process
	bl	aop_process
	bl	engine_process
	ldr	r3, [r4, #44]
	mov	r2, #1
	str	r2, [r4, #40]
	add	r3, r3, r2
	str	r3, [r4, #44]
	ands	r3, r3, r2
	streq	r3, [r4, #40]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L103:
	.align	2
.L102:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_proc_thread, .-aoe_proc_thread
	.align	2
	.global	aoe_get_thread_status
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_get_thread_status, %function
aoe_get_thread_status:
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
	ldr	r3, .L105
	ldr	r0, [r3, #40]
	ldmfd	sp, {fp, sp, pc}
.L106:
	.align	2
.L105:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_get_thread_status, .-aoe_get_thread_status
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_wkbuf, %object
	.size	g_wkbuf, 16
g_wkbuf:
	.space	16
	.type	g_aoe_cmd_fifo, %object
	.size	g_aoe_cmd_fifo, 16
g_aoe_cmd_fifo:
	.space	16
	.type	g_aoe_init_flag, %object
	.size	g_aoe_init_flag, 4
g_aoe_init_flag:
	.space	4
	.type	g_aoe_com_reg, %object
	.size	g_aoe_com_reg, 4
g_aoe_com_reg:
	.space	4
	.type	g_aoe_task_status, %object
	.size	g_aoe_task_status, 4
g_aoe_task_status:
	.space	4
	.type	g_aoe_task_ticks, %object
	.size	g_aoe_task_ticks, 4
g_aoe_task_ticks:
	.space	4
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
