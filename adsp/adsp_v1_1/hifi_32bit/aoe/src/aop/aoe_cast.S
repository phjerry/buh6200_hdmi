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
	.file	"aoe_cast.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_calc_size, %function
cast_calc_size:
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
	mov	r4, r1
	mov	r5, r0
	ldr	r1, [r1]
	ldr	r0, [r4, #4]
	bl	util_calc_frame_size
	ldr	r2, [r4, #8]
	movw	r3, #19923
	movt	r3, 4194
	mul	r2, r2, r5
	umull	r2, r3, r2, r3
	lsr	r3, r3, #6
	tst	r3, #3
	bicne	r3, r3, #3
	addne	r3, r3, #4
	mul	r0, r0, r3
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	cast_calc_size, .-cast_calc_size
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_timer_handle, %function
cast_timer_handle:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r6, .L25
	mov	r5, #0
.L16:
	ldr	r4, [r6], #4
	cmp	r4, #0
	beq	.L8
	mov	r0, r5
	bl	adsp_spin_lock
	ldr	r0, [r4, #36]
	ldr	r7, [r4, #32]
	bl	rbuf_query_busy
	mov	r8, r0
	ldr	r0, [r4, #40]
	bl	rbuf_query_free
	cmp	r7, r8
	bhi	.L9
	cmp	r7, r0
	mov	r2, r7
	ldr	r0, [r4, #36]
	bhi	.L15
.L22:
	mov	r3, #1
	ldr	r1, [r4, #40]
	bl	rbuf_copy_data
	ldr	r2, [r4, #44]
	add	r2, r2, #1
	str	r2, [r4, #44]
.L11:
	ldr	r3, [r4, #56]
	add	r3, r3, #1
	str	r3, [r4, #56]
	ldr	r3, [r6, #-20]
	str	r2, [r3, #28]
	ldr	r2, [r4, #48]
	str	r2, [r3, #32]
	ldr	r2, [r4, #52]
	str	r2, [r3, #36]
	ldr	r2, [r4, #56]
	str	r2, [r3, #40]
	ldr	r2, [r4, #60]
	str	r2, [r3, #24]
	ldr	r2, [r4, #64]
	str	r2, [r3, #16]
	ldr	r2, [r4, #68]
	str	r2, [r3, #20]
.L12:
	mov	r0, r5
	bl	adsp_spin_unlock
.L8:
	add	r5, r5, #1
	cmp	r5, #4
	bne	.L16
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L9:
	cmp	r8, #0
	bne	.L13
	cmp	r7, r0
	bls	.L24
	ldr	r3, [r4, #52]
	add	r3, r3, #1
	str	r3, [r4, #52]
	b	.L12
.L13:
	cmp	r8, r0
	mov	r2, r8
	ldr	r0, [r4, #36]
	bls	.L22
.L15:
	mov	r1, #0
	bl	rbuf_read_data
	ldr	r3, [r4, #52]
	add	r3, r3, #1
	str	r3, [r4, #52]
	b	.L12
.L24:
	mov	r2, r7
	mov	r1, r8
	ldr	r0, [r4, #40]
	bl	rbuf_write_data_dma
	ldr	r3, [r4, #48]
	ldr	r2, [r4, #44]
	add	r3, r3, #1
	str	r3, [r4, #48]
	b	.L11
.L26:
	.align	2
.L25:
	.word	.LANCHOR0+36
	UNWIND(.fnend)
	.size	cast_timer_handle, .-cast_timer_handle
	.align	2
	.global	cast_set_aop_buf
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_set_aop_buf, %function
cast_set_aop_buf:
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
	cmp	r1, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, .L38
	ldr	r2, [r3, #4]
	cmp	r0, r2
	beq	.L31
	ldr	r2, [r3, #8]
	cmp	r0, r2
	beq	.L32
	ldr	r2, [r3, #12]
	cmp	r0, r2
	beq	.L33
	ldr	r2, [r3, #16]
	cmp	r0, r2
	ldmfdne	sp, {fp, sp, pc}
	mov	r2, #3
	b	.L29
.L31:
	mov	r2, #0
.L29:
	add	r3, r3, r2, lsl #2
	ldr	r3, [r3, #36]
	str	r1, [r3, #40]
	ldmfd	sp, {fp, sp, pc}
.L32:
	mov	r2, #1
	b	.L29
.L33:
	mov	r2, #2
	b	.L29
.L39:
	.align	2
.L38:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	cast_set_aop_buf, .-cast_set_aop_buf
	.align	2
	.global	cast_destroy
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_destroy, %function
cast_destroy:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r4, .L51
	ldr	r3, [r4, #4]
	cmp	r0, r3
	beq	.L44
	ldr	r3, [r4, #8]
	cmp	r0, r3
	beq	.L45
	ldr	r3, [r4, #12]
	cmp	r0, r3
	beq	.L46
	ldr	r3, [r4, #16]
	cmp	r0, r3
	ldmfdne	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	mov	r7, #3
	b	.L41
.L44:
	mov	r7, #0
.L41:
	add	r5, r7, #8
	add	r5, r4, r5, lsl #2
	ldr	r6, [r5, #4]
	cmp	r6, #0
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	mov	r0, r7
	ldr	r8, [r6, #40]
	bl	adsp_spin_lock
	mov	r1, r6
	mov	r0, #70
	bl	HI_ADSP_FREE
	mov	r0, r7
	mov	r3, #0
	str	r3, [r5, #4]
	bl	adsp_spin_unlock
	mov	r0, r7
	bl	adsp_spin_deinit
	mov	r0, r8
	bl	rbuf_destroy
	ldr	r3, [r4]
	sub	r3, r3, #1
	str	r3, [r4]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L45:
	mov	r7, #1
	b	.L41
.L46:
	mov	r7, #2
	b	.L41
.L52:
	.align	2
.L51:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	cast_destroy, .-cast_destroy
	.align	2
	.global	cast_create
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_create, %function
cast_create:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #20)
	sub	sp, sp, #20
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r5, .L66
	mov	r10, r3
	mov	r3, #0
	mov	r8, r0
	mov	r7, r1
	mov	r9, r2
	ldr	r6, [r5, #36]
	str	r3, [fp, #-48]
	cmp	r6, r3
	beq	.L54
	ldr	r3, [r5, #40]
	cmp	r3, #0
	beq	.L60
	ldr	r3, [r5, #44]
	cmp	r3, #0
	beq	.L61
	ldr	r3, [r5, #48]
	cmp	r3, #0
	beq	.L62
.L57:
	mov	r7, #0
.L53:
	mov	r0, r7
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L61:
	mov	r6, #2
.L54:
	mov	r1, #72
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	subs	r4, r0, #0
	beq	.L57
	mov	r2, #72
	mov	r1, #0
	bl	adsp_memset
	add	r3, r4, #12
	str	r3, [fp, #-56]
	add	r2, r5, r6, lsl #2
	mov	r1, r3
	ldr	r3, [fp, #4]
	str	r9, [r4, #12]
	mov	r0, #65
	str	r10, [r4, #16]
	mov	r9, #0
	str	r3, [r4, #20]
	ldr	r10, [r2, #20]
	str	r9, [r7]
	bl	cast_calc_size
	add	r2, r10, #8
	str	r2, [r7, #12]
	add	r2, r10, #12
	str	r2, [r7, #16]
	str	r0, [r7, #8]
	ldr	r1, [r4, #12]
	ldr	r0, [r4, #16]
	bl	util_calc_frame_size
	ldr	r2, [r7, #8]
	mov	r1, r7
	str	r0, [r7, #24]
	sub	r0, fp, #48
	str	r2, [r10, #4]
	str	r8, [r10]
	bl	rbuf_create
	ldr	r7, [fp, #-48]
	cmp	r7, r9
	beq	.L58
	ldr	r3, [fp, #-56]
	add	ip, r5, r6, lsl #2
	ldr	r2, [r5]
	mov	r0, #5
	str	r7, [r4, #36]
	mov	r1, r3
	str	r8, [ip, #4]
	add	r3, r2, #1
	str	r3, [r5]
	bl	cast_calc_size
	add	r5, r5, r6, lsl #2
	str	r0, [r4, #32]
	mov	r0, r6
	bl	adsp_spin_init
	mov	r1, r9
	mov	r2, #28
	add	r0, r4, #44
	bl	adsp_memset
	ldr	r7, [fp, #-48]
	str	r4, [r5, #36]
	mov	r0, r7
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L60:
	mov	r6, #1
	b	.L54
.L62:
	mov	r6, #3
	b	.L54
.L58:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	b	.L53
.L67:
	.align	2
.L66:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	cast_create, .-cast_create
	.align	2
	.global	cast_fifo_try_cnt
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_fifo_try_cnt, %function
cast_fifo_try_cnt:
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
	ldr	r3, .L79
	ldr	r2, [r3, #4]
	cmp	r0, r2
	beq	.L72
	ldr	r2, [r3, #8]
	cmp	r0, r2
	beq	.L73
	ldr	r2, [r3, #12]
	cmp	r0, r2
	beq	.L74
	ldr	r2, [r3, #16]
	cmp	r0, r2
	ldmfdne	sp, {fp, sp, pc}
	mov	r2, #3
	b	.L69
.L72:
	mov	r2, #0
.L69:
	add	r3, r3, r2, lsl #2
	ldr	r3, [r3, #36]
	cmp	r3, #0
	ldrne	r2, [r3, #64]
	addne	r2, r2, #1
	strne	r2, [r3, #64]
	ldmfd	sp, {fp, sp, pc}
.L73:
	mov	r2, #1
	b	.L69
.L74:
	mov	r2, #2
	b	.L69
.L80:
	.align	2
.L79:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	cast_fifo_try_cnt, .-cast_fifo_try_cnt
	.align	2
	.global	cast_fifo_free_cnt
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_fifo_free_cnt, %function
cast_fifo_free_cnt:
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
	ldr	r3, .L92
	ldr	r2, [r3, #4]
	cmp	r0, r2
	beq	.L85
	ldr	r2, [r3, #8]
	cmp	r0, r2
	beq	.L86
	ldr	r2, [r3, #12]
	cmp	r0, r2
	beq	.L87
	ldr	r2, [r3, #16]
	cmp	r0, r2
	ldmfdne	sp, {fp, sp, pc}
	mov	r2, #3
	b	.L82
.L85:
	mov	r2, #0
.L82:
	add	r3, r3, r2, lsl #2
	ldr	r3, [r3, #36]
	cmp	r3, #0
	ldrne	r2, [r3, #68]
	addne	r2, r2, #1
	strne	r2, [r3, #68]
	ldmfd	sp, {fp, sp, pc}
.L86:
	mov	r2, #1
	b	.L82
.L87:
	mov	r2, #2
	b	.L82
.L93:
	.align	2
.L92:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	cast_fifo_free_cnt, .-cast_fifo_free_cnt
	.align	2
	.global	cast_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_init, %function
cast_init:
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
	mov	r2, #64
	mov	r6, r0
	mov	r1, #0
	ldr	r0, .L97
	add	r4, r6, #1024
	bl	adsp_memset
	ldr	r5, .L97+4
	add	r6, r6, #1216
	mov	r7, #0
.L95:
	mov	r0, r4
	mov	r2, #48
	mov	r1, #0
	str	r4, [r5, #16]
	bl	adsp_memset
	mov	r3, #8
	str	r3, [r4], #48
	cmp	r4, r6
	str	r7, [r5, #32]
	str	r3, [r5], #4
	bne	.L95
	ldr	r0, .L97+8
	bl	timer_reg_isr_func
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L98:
	.align	2
.L97:
	.word	.LANCHOR0
	.word	.LANCHOR0+4
	.word	cast_timer_handle
	UNWIND(.fnend)
	.size	cast_init, .-cast_init
	.align	2
	.global	cast_deinit
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	cast_deinit, %function
cast_deinit:
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
	bl	timer_del_isr_func
	ldr	r3, .L100
	mov	r2, #0
	str	r2, [r3, #36]
	str	r2, [r3, #40]
	str	r2, [r3, #44]
	str	r2, [r3, #48]
	ldmfd	sp, {fp, sp, pc}
.L101:
	.align	2
.L100:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	cast_deinit, .-cast_deinit
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_cast_rm, %object
	.size	g_cast_rm, 64
g_cast_rm:
	.space	64
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
