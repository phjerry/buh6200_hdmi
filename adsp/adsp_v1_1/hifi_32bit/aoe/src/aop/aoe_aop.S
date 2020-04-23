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
	.file	"aoe_aop.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_calc_size, %function
aop_calc_size:
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
	.size	aop_calc_size, .-aop_calc_size
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_create_chn, %function
aop_create_chn:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #80)
	sub	sp, sp, #80
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r7, .L26
	mov	r3, #0
	str	r3, [fp, #-100]
	mov	r6, r0
	ldr	r4, [r7, r0, lsl #2]
	sub	r0, fp, #84
	ldr	r1, [r4]
	and	r1, r1, #3
	bl	aoe_reg_read_bit_depth
	cmp	r0, #0
	bne	.L10
	ldr	r1, [r4]
	sub	r0, fp, #80
	ubfx	r1, r1, #2, #2
	bl	aoe_reg_read_channel
	cmp	r0, #0
	bne	.L10
	ldr	r1, [r4]
	sub	r0, fp, #76
	ubfx	r1, r1, #4, #4
	bl	aoe_reg_read_sampel_rate
	cmp	r0, #0
	bne	.L10
	ldr	r1, [r4]
	sub	r0, fp, #72
	ubfx	r1, r1, #8, #2
	bl	aoe_reg_read_data_format
	cmp	r0, #0
	bne	.L10
	ldr	r1, [r4]
	sub	r0, fp, #96
	lsr	r1, r1, #20
	bl	aoe_reg_read_buf_latency_ms
	subs	r5, r0, #0
	bne	.L10
	ldr	r3, [r4]
	ldr	r1, [fp, #-84]
	ldr	r0, [fp, #-80]
	ubfx	r3, r3, #10, #1
	str	r3, [fp, #-68]
	bl	util_calc_frame_size
	mov	r3, #1
	str	r3, [fp, #-64]
	ldr	r3, [r4, #24]
	str	r5, [fp, #-92]
	ubfx	r3, r3, #24, #1
	str	r5, [fp, #-88]
	str	r3, [fp, #-44]
	cmp	r3, #0
	ldr	r3, [r4, #16]
	str	r3, [fp, #-92]
	ldr	r2, [r4, #20]
	str	r3, [fp, #-60]
	str	r2, [fp, #-88]
	ldr	r3, [r4, #24]
	ubfx	r3, r3, #0, #24
	str	r3, [fp, #-56]
	bne	.L24
	add	r3, r4, #28
	add	r4, r4, #36
	str	r3, [fp, #-52]
	str	r4, [fp, #-48]
.L12:
	ldr	r3, [fp, #-96]
	mov	r1, r0
	ldr	r2, [fp, #-76]
	str	r0, [fp, #-40]
	mov	r0, r3
	str	r3, [fp, #-32]
	bl	util_latency_ms2_byte_size
	sub	r1, fp, #64
	str	r0, [fp, #-36]
	sub	r0, fp, #100
	bl	rbuf_create
	ldr	r4, [fp, #-100]
	cmp	r4, #0
	beq	.L7
	mov	r1, #136
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	subs	r4, r0, #0
	beq	.L13
	mov	r2, #136
	mov	r1, #0
	bl	adsp_memset
	mov	r2, #56
	sub	r1, fp, #84
	add	r0, r4, #48
	bl	adsp_memcpy
	ldr	r2, [fp, #-100]
	ldr	r3, [r7, r6, lsl #2]
	str	r2, [r4, #112]
	ldr	r2, [r3, #4]
	tst	r2, #524288
	bne	.L25
.L14:
	mov	r3, #0
	str	r3, [r4, #108]
.L16:
	mov	r3, #0
	mov	r2, #121
	mov	r1, #1
	str	r3, [r4, #20]
	str	r3, [r4, #32]
	str	r3, [r4, #36]
	str	r3, [r4]
	str	r3, [r4, #12]
	str	r3, [r4, #16]
	str	r3, [r4, #124]
	str	r3, [r4, #120]
	str	r2, [r4, #24]
	str	r2, [r4, #28]
	str	r2, [r4, #4]
	str	r2, [r4, #8]
	str	r1, [r4, #116]
.L7:
	mov	r0, r4
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L24:
	str	r5, [fp, #-92]
	str	r5, [fp, #-88]
	ldr	r3, [r4, #28]
	str	r3, [fp, #-92]
	ldr	r2, [r4, #32]
	str	r3, [fp, #-52]
	str	r2, [fp, #-88]
	str	r5, [fp, #-92]
	str	r5, [fp, #-88]
	ldr	r3, [r4, #36]
	str	r3, [fp, #-92]
	ldr	r2, [r4, #40]
	str	r3, [fp, #-48]
	str	r2, [fp, #-88]
	b	.L12
.L25:
	ldr	ip, [r3, #4]
	ubfx	ip, ip, #18, #1
	cmp	ip, #1
	bne	.L14
	ldr	lr, [r4, #56]
	add	r1, r4, #68
	ldr	r3, [r4, #52]
	mov	r0, r6
	ldr	r2, [r4, #48]
	str	ip, [r4, #108]
	str	lr, [sp]
	bl	cast_create
	subs	r5, r0, #0
	beq	.L17
	mov	r0, r6
	ldr	r1, [r4, #112]
	bl	cast_set_aop_buf
	str	r5, [r4, #112]
	b	.L16
.L10:
	mov	r4, #0
	mov	r0, r4
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L17:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
.L13:
	ldr	r0, [fp, #-100]
	mov	r4, #0
	bl	rbuf_destroy
	b	.L7
.L27:
	.align	2
.L26:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_create_chn, .-aop_create_chn
	.align	2
	.global	aop_flush_chn_buf
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_flush_chn_buf, %function
aop_flush_chn_buf:
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
	ldr	r0, [r0, #112]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	bl	rbuf_flush
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aop_flush_chn_buf, .-aop_flush_chn_buf
	.align	2
	.global	aop_proc_delay_compensation
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_proc_delay_compensation, %function
aop_proc_delay_compensation:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #32)
	sub	sp, sp, #32
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L54
	add	r2, r0, #8
	ldr	r4, [r3, r2, lsl #2]
	cmp	r4, #0
	beq	.L33
	ldr	r2, [r4, #116]
	cmp	r2, #0
	beq	.L33
	ldr	r2, [r4, #128]
	ldr	r5, [r3, r0, lsl #2]
	cmp	r2, #1
	beq	.L52
	ldr	r6, [r4, #120]
	cmp	r6, #9
	bhi	.L53
.L33:
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L53:
	add	r7, r4, #48
	sub	r1, fp, #56
	ldr	r0, [r4, #112]
	bl	rbuf_query_info
	mov	r0, r6
	mov	r1, r7
	bl	aop_calc_size
	mov	r1, r7
	mov	r6, r0
	mov	r0, #10
	bl	aop_calc_size
	ldr	r2, [fp, #-40]
	cmp	r6, r2
	bcc	.L33
	add	r6, r6, r0
	subs	r6, r6, r2
	beq	.L33
	ldr	r2, [fp, #-36]
	mov	r1, #0
	ldr	r0, [r4, #112]
	cmp	r6, r2
	movcc	r2, r6
	bl	rbuf_write_data
	ldr	r3, [r5, #100]
	add	r3, r3, #1
	str	r3, [r5, #100]
	b	.L33
.L52:
	mov	r3, #0
	str	r3, [fp, #-56]
	str	r3, [fp, #-52]
	sub	r1, fp, #56
	str	r3, [fp, #-48]
	str	r3, [fp, #-44]
	str	r3, [fp, #-40]
	str	r3, [fp, #-36]
	str	r3, [fp, #-32]
	ldr	r3, [r5, #4]
	orr	r3, r3, #1048576
	str	r3, [r5, #4]
	ldr	r0, [r4, #112]
	bl	rbuf_query_info
	ldr	r0, [r4, #112]
	cmp	r0, #0
	beq	.L36
	bl	rbuf_flush
.L36:
	add	r6, r4, #48
	ldr	r0, [r4, #124]
	mov	r1, r6
	bl	aop_calc_size
	mov	r1, r6
	mov	r7, r0
	ldr	r0, [r4, #120]
	bl	aop_calc_size
	ldr	r2, [fp, #-40]
	mov	r1, #0
	cmp	r7, r2
	mov	r3, r0
	addls	r3, r0, r2
	ldr	r2, [fp, #-36]
	subls	r3, r3, r7
	ldr	r0, [r4, #112]
	cmp	r3, r2
	movcc	r2, r3
	bl	rbuf_write_data
	ldr	r3, [r5, #100]
	mov	r2, #0
	add	r3, r3, #1
	str	r3, [r5, #100]
	ldr	r3, [r4, #120]
	str	r2, [r4, #128]
	str	r3, [r4, #124]
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L55:
	.align	2
.L54:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_proc_delay_compensation, .-aop_proc_delay_compensation
	.align	2
	.global	aop_destroy_chn
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_destroy_chn, %function
aop_destroy_chn:
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
	ldr	r3, .L63
	add	r2, r0, #8
	ldr	r4, [r3, r2, lsl #2]
	cmp	r4, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r4, #108]
	cmp	r3, #1
	beq	.L62
.L58:
	ldr	r0, [r4, #112]
	bl	rbuf_destroy
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L62:
	bl	cast_destroy
	b	.L58
.L64:
	.align	2
.L63:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_destroy_chn, .-aop_destroy_chn
	.align	2
	.global	aop_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_init, %function
aop_init:
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
	ldr	r5, .L68
	mov	r8, r0
	mov	r4, r0
	add	r6, r5, #32
	mov	r7, #0
.L66:
	mov	r0, r4
	mov	r2, #128
	mov	r1, #0
	str	r4, [r5, #4]!
	bl	adsp_memset
	ldr	r3, [r4, #4]
	cmp	r5, r6
	add	r4, r4, #128
	orr	r3, r3, #4
	str	r3, [r4, #-124]
	str	r7, [r5, #32]
	bne	.L66
	mov	r0, r8
	bl	cast_init
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L69:
	.align	2
.L68:
	.word	.LANCHOR0-4
	UNWIND(.fnend)
	.size	aop_init, .-aop_init
	.align	2
	.global	aop_deinit
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_deinit, %function
aop_deinit:
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
	ldr	r5, .L73
	mov	r4, #0
	mov	r6, r4
.L71:
	mov	r0, r4
	add	r4, r4, #1
	bl	aop_destroy_chn
	cmp	r4, #8
	str	r6, [r5], #4
	bne	.L71
	bl	cast_deinit
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L74:
	.align	2
.L73:
	.word	.LANCHOR0+32
	UNWIND(.fnend)
	.size	aop_deinit, .-aop_deinit
	.align	2
	.global	aop_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_process, %function
aop_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #12)
	sub	sp, sp, #12
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r6, #0
	ldr	r5, .L107
	mov	r7, r6
	mov	r8, #1
	b	.L93
.L105:
	cmp	r2, #1
	bne	.L102
	mov	r0, r6
	bl	aop_destroy_chn
	ldr	r3, [r5]
	str	r4, [r5, #32]
.L83:
	ldr	r2, [r3, #4]
	tst	r2, #4
	bne	.L86
.L94:
	ldr	r2, [r3, #4]
	bfc	r2, #3, #4
	str	r2, [r3, #4]
	ldr	r2, [r3, #4]
	orr	r2, r2, #4
	str	r2, [r3, #4]
.L77:
	ldr	r4, [r5, #32]
	cmp	r4, #0
	beq	.L86
	ldr	r3, [r4, #116]
	str	r7, [r4, #132]
	cmp	r3, #1
	beq	.L103
.L86:
	add	r6, r6, #1
	cmp	r6, #8
	beq	.L104
.L93:
	ldr	r3, [r5, #4]!
	ldr	r4, [r3, #4]
	ubfx	r4, r4, #2, #1
	cmp	r4, #0
	bne	.L77
	ldr	r2, [r3, #4]
	ands	r2, r2, #3
	bne	.L105
	mov	r0, r6
	bl	aop_create_chn
	cmp	r0, #0
	mov	r4, r0
	str	r0, [r5, #32]
	beq	.L106
	ldr	r3, [r5]
	ldr	r2, [r3, #4]
	tst	r2, #4
	beq	.L94
	ldr	r3, [r4, #116]
	str	r7, [r4, #132]
	cmp	r3, #1
	bne	.L86
.L103:
	ldr	r9, [r5]
	add	r6, r6, #1
	ldr	r0, [r4, #52]
	ldr	r3, [r9, #4]
	str	r3, [fp, #-52]
	ldr	r3, [r9, #12]
	str	r7, [r4, #20]
	str	r3, [fp, #-48]
	ldr	r2, [fp, #-48]
	ldr	r3, [fp, #-48]
	ldr	r1, [fp, #-48]
	ubfx	r3, r3, #8, #8
	str	r3, [r4, #28]
	ldr	r3, [fp, #-48]
	tst	r1, #524288
	uxtb	r2, r2
	str	r2, [r4, #24]
	ubfx	r3, r3, #16, #3
	ldr	r2, [fp, #-48]
	rsbne	r3, r3, #0
	str	r3, [r4, #32]
	str	r3, [r4, #36]
	tst	r2, #1073741824
	ldr	r3, [fp, #-48]
	ldr	r2, [fp, #-48]
	ubfx	r3, r3, #24, #6
	ldr	r1, [r4, #48]
	rsbne	r3, r3, #0
	str	r3, [r4, #40]
	ldr	r3, [fp, #-52]
	lsr	r2, r2, #31
	str	r2, [r4, #44]
	ubfx	r3, r3, #8, #1
	str	r3, [r4, #104]
	ldr	r10, [r9]
	lsr	r10, r10, #20
	str	r10, [r4, #100]
	bl	util_calc_frame_size
	ldr	r2, [r4, #56]
	mov	r1, r0
	mov	r0, r10
	bl	util_latency_ms2_byte_size
	add	r1, r4, #68
	str	r0, [r4, #96]
	ldr	r0, [r4, #112]
	bl	rbuf_update_attr
	ldr	r3, [r9, #4]
	ldr	r2, [r4, #124]
	ubfx	r3, r3, #21, #9
	ldr	r0, [r4, #112]
	cmp	r3, r2
	str	r3, [r4, #120]
	strne	r8, [r4, #128]
	bl	rbuf_query_busy
	ldr	r1, [r4, #48]
	mov	r9, r0
	ldr	r0, [r4, #52]
	bl	util_calc_frame_size
	ldr	r2, [r4, #56]
	mov	r1, r0
	mov	r0, r9
	bl	util_latency_ms
	cmp	r6, #8
	str	r0, [r4, #132]
	bne	.L93
.L104:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L102:
	ldr	r2, [r3, #4]
	mov	r1, #4
	bfi	r2, r1, #3, #4
	str	r2, [r3, #4]
	ldr	r2, [r3, #4]
	orr	r2, r2, r1
	str	r2, [r3, #4]
	b	.L77
.L106:
	ldr	r3, [r5]
	ldr	r2, [r3, #4]
	tst	r2, #4
	bne	.L83
	ldr	r2, [r3, #4]
	bfi	r2, r8, #3, #4
	str	r2, [r3, #4]
	ldr	r2, [r3, #4]
	orr	r2, r2, #4
	str	r2, [r3, #4]
	ldr	r3, [r5]
	ldr	r2, [r3, #4]
	tst	r2, #4
	beq	.L94
	b	.L77
.L108:
	.align	2
.L107:
	.word	.LANCHOR0-4
	UNWIND(.fnend)
	.size	aop_process, .-aop_process
	.align	2
	.global	aop_get_chn_status
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_get_chn_status, %function
aop_get_chn_status:
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
	ldr	r3, .L113
	add	r0, r0, #8
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	beq	.L112
	ldr	r2, [r3, #116]
	str	r2, [r1]
	ldr	r2, [r3, #64]
	str	r2, [r1, #4]
	ldr	r2, [r3, #92]
	str	r2, [r1, #16]
	ldr	r2, [r3, #48]
	str	r2, [r1, #8]
	ldr	r2, [r3, #52]
	str	r2, [r1, #12]
	ldr	r2, [r3, #104]
	str	r2, [r1, #20]
	ldr	r2, [r3, #108]
	str	r2, [r1, #24]
	ldr	r2, [r3, #120]
	str	r2, [r1, #28]
	ldr	r3, [r3, #132]
	str	r3, [r1, #32]
	ldmfd	sp, {fp, sp, pc}
.L112:
	str	r3, [r1]
	ldmfd	sp, {fp, sp, pc}
.L114:
	.align	2
.L113:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_get_chn_status, .-aop_get_chn_status
	.align	2
	.global	aop_query_rbf_free
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_query_rbf_free, %function
aop_query_rbf_free:
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
	ldr	r3, .L120
	add	r0, r0, #8
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	beq	.L117
	ldr	r0, [r3, #116]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r0, [r3, #112]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	bl	rbuf_check_free
	ldmfd	sp, {fp, sp, pc}
.L117:
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L121:
	.align	2
.L120:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_query_rbf_free, .-aop_query_rbf_free
	.align	2
	.global	aop_write_buff_proc_try_cnt
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_write_buff_proc_try_cnt, %function
aop_write_buff_proc_try_cnt:
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
	ldr	r3, .L127
	add	r2, r0, #8
	ldr	r3, [r3, r2, lsl #2]
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r3, #108]
	cmp	r3, #1
	ldmfdne	sp, {fp, sp, pc}
	bl	cast_fifo_try_cnt
	ldmfd	sp, {fp, sp, pc}
.L128:
	.align	2
.L127:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_write_buff_proc_try_cnt, .-aop_write_buff_proc_try_cnt
	.align	2
	.global	aop_write_buff_proc_ok_cnt
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_write_buff_proc_ok_cnt, %function
aop_write_buff_proc_ok_cnt:
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
	ldr	r3, .L134
	add	r2, r0, #8
	ldr	r3, [r3, r2, lsl #2]
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r3, #108]
	cmp	r3, #1
	ldmfdne	sp, {fp, sp, pc}
	bl	cast_fifo_free_cnt
	ldmfd	sp, {fp, sp, pc}
.L135:
	.align	2
.L134:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_write_buff_proc_ok_cnt, .-aop_write_buff_proc_ok_cnt
	.align	2
	.global	aop_write_rbf_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_write_rbf_data, %function
aop_write_rbf_data:
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
	ldr	r3, .L163
	add	r0, r0, #8
	mov	r5, r1
	ldr	r4, [r3, r0, lsl #2]
	cmp	r4, #0
	beq	.L151
	ldr	r0, [r4, #116]
	cmp	r0, #0
	ldmfdeq	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r3, [r1, #24]
	cmp	r3, #0
	beq	.L160
.L139:
	ldr	r3, [r4, #108]
	ldr	r0, [r4, #112]
	cmp	r3, #1
	ldm	r5, {r1, r2}
	beq	.L161
	bl	rbuf_write_data_dma
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L151:
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L160:
	ldr	r2, [r1, #12]
	ldr	r3, [r4, #48]
	cmp	r2, r3
	beq	.L159
	bl	aoe_get_wk_buf
	mov	r2, r5
	mov	r1, r0
	ldr	r0, [r4, #48]
	bl	util_unify_bit_depth_pcm_data
.L159:
	ldr	r2, [r5, #16]
	ldr	r3, [r4, #52]
	cmp	r2, r3
	beq	.L142
	bl	aoe_get_wk_buf
	mov	r2, r5
	mov	r1, r0
	ldr	r0, [r4, #52]
	bl	util_upmix_pcm_data
.L142:
	ldr	r3, [r4, #44]
	cmp	r3, #0
	beq	.L144
	bl	aoe_get_wk_buf
	ldr	r2, [r5, #4]
	mov	r1, #0
	mov	r6, r0
	bl	adsp_memset
	str	r6, [r5]
.L144:
	ldr	r3, [r4, #24]
	ldr	r2, [r4, #4]
	cmp	r3, r2
	beq	.L162
.L145:
	ldr	r2, [r4, #108]
	cmp	r2, #1
	beq	.L146
	bl	aoe_get_wk_buf
	add	r6, r4, #20
	ldm	r5, {r1, r2}
	mov	r7, r0
	bl	adsp_memcpy
	str	r7, [r5]
	mov	r2, r4
	ldr	r1, [r4, #40]
	mov	r0, r5
	mov	r3, r6
	bl	util_lr_fade_gain_process
	mov	r1, r6
	mov	r2, #20
	mov	r0, r4
	bl	adsp_memcpy
	b	.L139
.L161:
	bl	rbuf_write_data
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L162:
	ldr	r1, [r4, #32]
	ldr	r2, [r4, #12]
	cmp	r1, r2
	bne	.L145
.L146:
	cmp	r3, #121
	bne	.L148
	ldr	r3, [r4, #28]
	cmp	r3, #121
	bne	.L148
	ldr	r3, [r4, #32]
	cmp	r3, #0
	bne	.L148
	ldr	r3, [r4, #36]
	cmp	r3, #0
	bne	.L148
	ldr	r3, [r4, #40]
	cmp	r3, #0
	beq	.L139
.L148:
	bl	aoe_get_wk_buf
	ldm	r5, {r1, r2}
	mov	r6, r0
	bl	adsp_memcpy
	str	r6, [r5]
	add	r2, r4, #20
	ldr	r1, [r4, #40]
	mov	r0, r5
	bl	util_lr_gain_process
	b	.L139
.L164:
	.align	2
.L163:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_write_rbf_data, .-aop_write_rbf_data
	.align	2
	.global	aop_get_ring_out
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_get_ring_out, %function
aop_get_ring_out:
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
	ldr	r3, .L168
	add	r0, r0, #8
	ldr	r0, [r3, r0, lsl #2]
	cmp	r0, #0
	ldrne	r0, [r0, #112]
	ldmfd	sp, {fp, sp, pc}
.L169:
	.align	2
.L168:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_get_ring_out, .-aop_get_ring_out
	.align	2
	.global	aop_continue_output
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_continue_output, %function
aop_continue_output:
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
	ldr	ip, .L179
	add	r0, r0, #8
	mov	r5, r2
	ldr	r4, [ip, r0, lsl #2]
	cmp	r4, #0
	ldmfdeq	sp, {r4, r5, r6, r7, fp, sp, pc}
	cmp	r1, #4
	beq	.L178
	cmp	r1, #3
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	movw	r2, #2568
	ldr	r1, .L179+4
	ldr	r0, [r4, #112]
	bl	rbuf_write_data_dma
	sub	r2, r5, #2560
	sub	r2, r2, #8
	ldr	r0, [r4, #112]
	mov	r1, #0
	bl	rbuf_write_data_dma
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L178:
	cmp	r3, #1
	movw	r3, #1544
	movw	r6, #3260
	ldr	r1, .L179+8
	movne	r6, r3
	ldr	r3, .L179+12
	mov	r2, r6
	ldr	r0, [r4, #112]
	movne	r1, r3
	bl	rbuf_write_data_dma
	sub	r2, r5, r6
	ldr	r0, [r4, #112]
	mov	r1, #0
	bl	rbuf_write_data_dma
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L180:
	.align	2
.L179:
	.word	.LANCHOR0
	.word	.LANCHOR2-3380
	.word	.LANCHOR1+1544
	.word	.LANCHOR1
	UNWIND(.fnend)
	.size	aop_continue_output, .-aop_continue_output
	.align	2
	.global	aop_proc_cmd
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aop_proc_cmd, %function
aop_proc_cmd:
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
	ldr	r4, [r0, #8]
	cmp	r4, #7
	ldmfdhi	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r0]
	cmp	r3, #0
	beq	.L184
	cmp	r3, #1
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	mov	r0, r4
	bl	aop_destroy_chn
	ldr	r3, .L189
	add	r2, r4, #8
	mov	r1, #0
	str	r1, [r3, r2, lsl #2]
.L186:
	ldr	r2, [r3, r4, lsl #2]
	ldr	r3, [r2, #4]
	ubfx	r3, r3, #2, #1
	cmp	r3, #0
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	ldr	r1, [r2, #4]
	bfi	r1, r3, #3, #4
	str	r1, [r2, #4]
	ldr	r3, [r2, #4]
	orr	r3, r3, #4
	str	r3, [r2, #4]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L184:
	mov	r0, r4
	bl	aop_create_chn
	ldr	r3, .L189
	add	r2, r4, #8
	cmp	r0, #0
	str	r0, [r3, r2, lsl #2]
	bne	.L186
	ldr	r2, [r3, r4, lsl #2]
	ldr	r1, [r2, #4]
	tst	r1, #4
	bne	.L186
	ldr	r1, [r2, #4]
	mov	r0, #1
	bfi	r1, r0, #3, #4
	str	r1, [r2, #4]
	ldr	r1, [r2, #4]
	orr	r1, r1, #4
	str	r1, [r2, #4]
	b	.L186
.L190:
	.align	2
.L189:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aop_proc_cmd, .-aop_proc_cmd
	.section	.rodata
	.align	2
	.set	.LANCHOR1,. + 0
	.set	.LANCHOR2,. + 8184
	.type	g_ddp_heard, %object
	.size	g_ddp_heard, 1544
g_ddp_heard:
	.short	-1934
	.short	19999
	.short	21
	.short	1536
	.short	2935
	.short	767
	.short	16263
	.short	-8159
	.short	1568
	.short	0
	.short	2
	.short	0
	.short	7
	.short	-1727
	.short	318
	.short	-11256
	.short	2056
	.short	2050
	.short	514
	.short	512
	.short	-32640
	.short	-32640
	.short	8224
	.short	8224
	.short	2056
	.short	2056
	.short	12787
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12353
	.short	-6313
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12353
	.short	-1579
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3089
	.short	-395
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31995
	.short	-99
	.short	24382
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	-25
	.short	22479
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-16391
	.short	-10455
	.short	-5998
	.short	18887
	.short	95
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-16
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	9
	.short	5258
	.short	17698
	.short	-28344
	.short	-23470
	.short	10516
	.short	-30139
	.short	8895
	.short	-1
	.short	-1
	.short	-64
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	51
	.short	13107
	.short	13107
	.short	13107
	.short	13107
	.short	13107
	.short	13103
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-8
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	4
	.short	-30139
	.short	8849
	.short	18596
	.short	21033
	.short	5258
	.short	17698
	.short	-28321
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-16
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	9
	.short	5258
	.short	17698
	.short	-28344
	.short	-23470
	.short	10516
	.short	-30139
	.short	8895
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-32
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	18
	.short	10516
	.short	-30139
	.short	8849
	.short	18596
	.short	21033
	.short	5258
	.short	17791
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-1
	.short	-64
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	36
	.short	21033
	.short	5258
	.short	17698
	.short	-28344
	.short	-23470
	.short	10516
	.short	-29954
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	16448
	.short	20772
	.short	-28096
	.short	16465
	.short	9362
	.short	16448
	.short	20772
	.short	-28096
	.short	16465
	.short	9362
	.short	16448
	.short	20772
	.short	-28160
	.short	0
	.short	0
	.short	0
	.short	24528
	.short	-1342
	.type	g_atmos_heard, %object
	.size	g_atmos_heard, 3260
g_atmos_heard:
	.byte	114
	.byte	-8
	.byte	31
	.byte	78
	.byte	21
	.byte	0
	.byte	-76
	.byte	12
	.byte	119
	.byte	11
	.byte	89
	.byte	6
	.byte	103
	.byte	63
	.byte	-38
	.byte	-31
	.byte	0
	.byte	73
	.byte	1
	.byte	65
	.byte	1
	.byte	7
	.byte	0
	.byte	32
	.byte	0
	.byte	0
	.byte	0
	.byte	16
	.byte	0
	.byte	0
	.byte	-124
	.byte	35
	.byte	-122
	.byte	121
	.byte	97
	.byte	24
	.byte	-4
	.byte	-121
	.byte	-7
	.byte	-22
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-13
	.byte	127
	.byte	-25
	.byte	-85
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	125
	.byte	62
	.byte	-50
	.byte	-1
	.byte	-97
	.byte	-81
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-9
	.byte	-7
	.byte	58
	.byte	-1
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-33
	.byte	-25
	.byte	-22
	.byte	-4
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	127
	.byte	-97
	.byte	-82
	.byte	-13
	.byte	-29
	.byte	83
	.byte	-39
	.byte	-128
	.byte	27
	.byte	-14
	.byte	111
	.byte	-121
	.byte	60
	.byte	30
	.byte	-18
	.byte	120
	.byte	-18
	.byte	-18
	.byte	-32
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	-35
	.byte	-35
	.byte	-74
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-33
	.byte	-74
	.byte	107
	.byte	62
	.byte	-42
	.byte	90
	.byte	-83
	.byte	-75
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	62
	.byte	27
	.byte	112
	.byte	67
	.byte	-29
	.byte	-19
	.byte	-113
	.byte	-57
	.byte	-35
	.byte	29
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-36
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	59
	.byte	0
	.byte	-69
	.byte	-69
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	-25
	.byte	-37
	.byte	107
	.byte	-51
	.byte	-42
	.byte	90
	.byte	-81
	.byte	-75
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-61
	.byte	-25
	.byte	-56
	.byte	103
	.byte	29
	.byte	110
	.byte	120
	.byte	-68
	.byte	-29
	.byte	-15
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	0
	.byte	-128
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	7
	.byte	118
	.byte	119
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-7
	.byte	124
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-20
	.byte	-5
	.byte	125
	.byte	1
	.byte	103
	.byte	-128
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-49
	.byte	0
	.byte	24
	.byte	-16
	.byte	-15
	.byte	14
	.byte	-57
	.byte	-29
	.byte	-18
	.byte	-114
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	29
	.byte	0
	.byte	-35
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-13
	.byte	109
	.byte	-75
	.byte	-26
	.byte	107
	.byte	-83
	.byte	-41
	.byte	90
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-17
	.byte	-13
	.byte	7
	.byte	-100
	.byte	-3
	.byte	-13
	.byte	71
	.byte	-98
	.byte	108
	.byte	-128
	.byte	13
	.byte	-7
	.byte	-73
	.byte	-61
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-97
	.byte	13
	.byte	-72
	.byte	33
	.byte	-15
	.byte	118
	.byte	-57
	.byte	-29
	.byte	-18
	.byte	-114
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	29
	.byte	0
	.byte	-35
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-13
	.byte	109
	.byte	-75
	.byte	-26
	.byte	107
	.byte	-83
	.byte	-41
	.byte	90
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-31
	.byte	-13
	.byte	-28
	.byte	-77
	.byte	14
	.byte	55
	.byte	60
	.byte	-34
	.byte	-15
	.byte	120
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	3
	.byte	-69
	.byte	-69
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	124
	.byte	-66
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-7
	.byte	90
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-10
	.byte	125
	.byte	-66
	.byte	0
	.byte	51
	.byte	-64
	.byte	-113
	.byte	-57
	.byte	59
	.byte	30
	.byte	-69
	.byte	-69
	.byte	-72
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	119
	.byte	109
	.byte	119
	.byte	-37
	.byte	-74
	.byte	-73
	.byte	109
	.byte	-102
	.byte	-49
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	62
	.byte	95
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	103
	.byte	-128
	.byte	12
	.byte	-8
	.byte	120
	.byte	7
	.byte	-29
	.byte	-15
	.byte	119
	.byte	-57
	.byte	119
	.byte	119
	.byte	0
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	14
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-74
	.byte	-19
	.byte	109
	.byte	-37
	.byte	-7
	.byte	-74
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-9
	.byte	-7
	.byte	3
	.byte	-50
	.byte	-2
	.byte	-7
	.byte	35
	.byte	-49
	.byte	-1
	.byte	-61
	.byte	56
	.byte	88
	.byte	73
	.byte	0
	.byte	-32
	.byte	10
	.byte	-128
	.byte	-4
	.byte	24
	.byte	1
	.byte	17
	.byte	62
	.byte	0
	.byte	15
	.byte	-100
	.byte	3
	.byte	112
	.byte	0
	.byte	-128
	.byte	0
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	-33
	.byte	1
	.byte	0
	.byte	2
	.byte	-127
	.byte	3
	.byte	0
	.byte	-12
	.byte	-5
	.byte	7
	.byte	0
	.byte	-24
	.byte	-5
	.byte	14
	.byte	8
	.byte	-16
	.byte	-9
	.byte	29
	.byte	16
	.byte	-32
	.byte	-48
	.byte	1
	.byte	64
	.byte	126
	.byte	84
	.byte	0
	.byte	16
	.byte	-58
	.byte	60
	.byte	10
	.byte	32
	.byte	16
	.byte	4
	.byte	-127
	.byte	32
	.byte	8
	.byte	0
	.byte	65
	.byte	2
	.byte	-82
	.byte	10
	.byte	-72
	.byte	43
	.byte	-32
	.byte	-82
	.byte	-128
	.byte	9
	.byte	0
	.byte	-121
	.byte	-103
	.byte	-82
	.byte	104
	.byte	-51
	.byte	52
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	50
	.byte	-51
	.byte	48
	.byte	30
	.byte	-51
	.byte	108
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	-51
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	27
	.byte	0
	.byte	67
	.byte	62
	.byte	-19
	.byte	112
	.byte	-57
	.byte	-29
	.byte	29
	.byte	-113
	.byte	-35
	.byte	-35
	.byte	-36
	.byte	-35
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	59
	.byte	-74
	.byte	-69
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-51
	.byte	-25
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	-97
	.byte	-81
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	103
	.byte	-61
	.byte	110
	.byte	-56
	.byte	-68
	.byte	29
	.byte	-15
	.byte	120
	.byte	-69
	.byte	-29
	.byte	-69
	.byte	-69
	.byte	-128
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	7
	.byte	0
	.byte	119
	.byte	119
	.byte	-37
	.byte	118
	.byte	-74
	.byte	109
	.byte	124
	.byte	-37
	.byte	-83
	.byte	-7
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-8
	.byte	124
	.byte	-7
	.byte	108
	.byte	-61
	.byte	13
	.byte	-113
	.byte	-73
	.byte	60
	.byte	30
	.byte	119
	.byte	119
	.byte	119
	.byte	119
	.byte	0
	.byte	112
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	0
	.byte	-18
	.byte	-18
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-97
	.byte	111
	.byte	-83
	.byte	53
	.byte	90
	.byte	107
	.byte	-66
	.byte	-42
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	125
	.byte	-97
	.byte	47
	.byte	-128
	.byte	12
	.byte	-80
	.byte	-29
	.byte	-15
	.byte	-114
	.byte	-57
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-35
	.byte	29
	.byte	-37
	.byte	-35
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	-26
	.byte	-13
	.byte	-83
	.byte	-75
	.byte	90
	.byte	107
	.byte	-49
	.byte	-41
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	25
	.byte	-32
	.byte	3
	.byte	-2
	.byte	-34
	.byte	1
	.byte	120
	.byte	60
	.byte	-35
	.byte	-15
	.byte	-35
	.byte	-35
	.byte	-64
	.byte	-35
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	3
	.byte	0
	.byte	-69
	.byte	-69
	.byte	109
	.byte	-69
	.byte	-37
	.byte	-74
	.byte	-66
	.byte	109
	.byte	-42
	.byte	124
	.byte	-83
	.byte	-75
	.byte	90
	.byte	107
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	125
	.byte	62
	.byte	-128
	.byte	-13
	.byte	127
	.byte	-2
	.byte	-56
	.byte	-77
	.byte	-82
	.byte	-16
	.byte	0
	.byte	64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	6
	.byte	0
	.byte	-112
	.byte	-49
	.byte	59
	.byte	-36
	.byte	-15
	.byte	120
	.byte	-57
	.byte	-29
	.byte	119
	.byte	119
	.byte	119
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	14
	.byte	-19
	.byte	-18
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	-13
	.byte	-7
	.byte	-42
	.byte	90
	.byte	-83
	.byte	-75
	.byte	-25
	.byte	107
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-39
	.byte	-16
	.byte	27
	.byte	-14
	.byte	111
	.byte	-121
	.byte	60
	.byte	30
	.byte	-18
	.byte	120
	.byte	-18
	.byte	-18
	.byte	-32
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	0
	.byte	-35
	.byte	-35
	.byte	-74
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-33
	.byte	-74
	.byte	107
	.byte	62
	.byte	-42
	.byte	90
	.byte	-83
	.byte	-75
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	62
	.byte	27
	.byte	112
	.byte	67
	.byte	-29
	.byte	-19
	.byte	-113
	.byte	-57
	.byte	-35
	.byte	29
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-36
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	59
	.byte	0
	.byte	-69
	.byte	-69
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	-25
	.byte	-37
	.byte	107
	.byte	-51
	.byte	-42
	.byte	90
	.byte	-81
	.byte	-75
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-33
	.byte	-25
	.byte	11
	.byte	96
	.byte	3
	.byte	-20
	.byte	120
	.byte	60
	.byte	-29
	.byte	-15
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	0
	.byte	-128
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	7
	.byte	118
	.byte	119
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-7
	.byte	124
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	6
	.byte	-8
	.byte	-128
	.byte	127
	.byte	119
	.byte	-64
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-32
	.byte	124
	.byte	-97
	.byte	63
	.byte	-14
	.byte	-20
	.byte	3
	.byte	60
	.byte	-56
	.byte	103
	.byte	29
	.byte	110
	.byte	120
	.byte	-68
	.byte	-29
	.byte	-15
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	-69
	.byte	0
	.byte	-128
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	7
	.byte	118
	.byte	119
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-7
	.byte	124
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	108
	.byte	-8
	.byte	13
	.byte	-7
	.byte	-73
	.byte	-61
	.byte	30
	.byte	-113
	.byte	119
	.byte	60
	.byte	119
	.byte	119
	.byte	112
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-37
	.byte	-18
	.byte	-74
	.byte	109
	.byte	111
	.byte	-37
	.byte	53
	.byte	-97
	.byte	107
	.byte	-83
	.byte	-42
	.byte	90
	.byte	124
	.byte	-66
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	-97
	.byte	13
	.byte	-72
	.byte	33
	.byte	-15
	.byte	118
	.byte	-57
	.byte	-29
	.byte	-18
	.byte	-114
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-18
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	29
	.byte	0
	.byte	-35
	.byte	-35
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-13
	.byte	109
	.byte	-75
	.byte	-26
	.byte	107
	.byte	-83
	.byte	-41
	.byte	90
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-17
	.byte	-13
	.byte	5
	.byte	-80
	.byte	1
	.byte	-10
	.byte	60
	.byte	-98
	.byte	-15
	.byte	120
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	3
	.byte	-69
	.byte	-69
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	124
	.byte	-66
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-7
	.byte	90
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	3
	.byte	124
	.byte	-64
	.byte	63
	.byte	59
	.byte	96
	.byte	-113
	.byte	-57
	.byte	59
	.byte	30
	.byte	-69
	.byte	-69
	.byte	-72
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	119
	.byte	109
	.byte	119
	.byte	-37
	.byte	-74
	.byte	-73
	.byte	109
	.byte	-102
	.byte	-49
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	62
	.byte	95
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	112
	.byte	-66
	.byte	-49
	.byte	31
	.byte	121
	.byte	-10
	.byte	1
	.byte	30
	.byte	-28
	.byte	-77
	.byte	14
	.byte	55
	.byte	60
	.byte	-34
	.byte	-15
	.byte	120
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	-35
	.byte	0
	.byte	-64
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	3
	.byte	-69
	.byte	-69
	.byte	-74
	.byte	109
	.byte	109
	.byte	-37
	.byte	124
	.byte	-66
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-7
	.byte	90
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	54
	.byte	124
	.byte	-122
	.byte	124
	.byte	-37
	.byte	-31
	.byte	-113
	.byte	-57
	.byte	59
	.byte	30
	.byte	-69
	.byte	-69
	.byte	-72
	.byte	-69
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	119
	.byte	119
	.byte	109
	.byte	119
	.byte	-37
	.byte	-74
	.byte	-73
	.byte	109
	.byte	-102
	.byte	-49
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	62
	.byte	95
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	-49
	.byte	-122
	.byte	-36
	.byte	-112
	.byte	120
	.byte	59
	.byte	-29
	.byte	-15
	.byte	119
	.byte	-57
	.byte	119
	.byte	119
	.byte	0
	.byte	119
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	14
	.byte	0
	.byte	-18
	.byte	-18
	.byte	-74
	.byte	-19
	.byte	109
	.byte	-37
	.byte	-7
	.byte	-74
	.byte	90
	.byte	-13
	.byte	-75
	.byte	-42
	.byte	107
	.byte	-83
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	124
	.byte	62
	.byte	-9
	.byte	-7
	.byte	2
	.byte	-40
	.byte	0
	.byte	-5
	.byte	30
	.byte	-49
	.byte	120
	.byte	60
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	-18
	.byte	0
	.byte	-32
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-35
	.byte	1
	.byte	-35
	.byte	-35
	.byte	-37
	.byte	-74
	.byte	-74
	.byte	109
	.byte	62
	.byte	-33
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	124
	.byte	-83
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	-97
	.byte	-49
	.byte	1
	.byte	62
	.byte	-32
	.byte	-97
	.byte	29
	.byte	48
	.byte	-57
	.byte	-29
	.byte	29
	.byte	-113
	.byte	-35
	.byte	-35
	.byte	-36
	.byte	-35
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-69
	.byte	59
	.byte	-74
	.byte	-69
	.byte	109
	.byte	-37
	.byte	-37
	.byte	-74
	.byte	-51
	.byte	-25
	.byte	90
	.byte	107
	.byte	-75
	.byte	-42
	.byte	-97
	.byte	-81
	.byte	124
	.byte	62
	.byte	-13
	.byte	-7
	.byte	-49
	.byte	-25
	.byte	62
	.byte	-97
	.byte	-7
	.byte	124
	.byte	-25
	.byte	-13
	.byte	56
	.byte	-33
	.byte	-25
	.byte	15
	.byte	60
	.byte	-5
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-112
	.byte	113
	.byte	21
	.byte	-8
	.type	g_dd_heard, %object
	.size	g_dd_heard, 2568
g_dd_heard:
	.short	-1934
	.short	19999
	.short	1
	.short	20480
	.short	2935
	.short	3061
	.short	9264
	.short	-7684
	.short	1216
	.short	16129
	.short	-27299
	.short	5201
	.short	17695
	.short	-3157
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-13922
	.short	-15501
	.short	-50
	.short	-20577
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24791
	.short	-30999
	.short	14335
	.short	15038
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-23052
	.short	14143
	.short	-790
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3481
	.short	19051
	.short	32755
	.short	-21529
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6198
	.short	-27867
	.short	32255
	.short	-12615
	.short	20366
	.short	9288
	.short	-28382
	.short	17408
	.short	0
	.short	7
	.short	30961
	.short	-7225
	.short	30583
	.short	30583
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	3822
	.short	-4371
	.short	-18725
	.short	28086
	.short	-1549
	.short	23254
	.short	-19027
	.short	27623
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-4096
	.short	0
	.short	239
	.short	7740
	.short	30958
	.short	-4370
	.short	-4384
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	1
	.short	-8739
	.short	-8778
	.short	-9363
	.short	-18721
	.short	15979
	.short	23254
	.short	-19027
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	0
	.short	0
	.short	7651
	.short	-14449
	.short	7645
	.short	-8739
	.short	-9216
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	59
	.short	-17477
	.short	-18725
	.short	28086
	.short	-9241
	.short	-12949
	.short	23254
	.short	-19025
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6208
	.short	0
	.short	3
	.short	-17288
	.short	-3613
	.short	-17477
	.short	-17477
	.short	-32768
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	1911
	.short	30582
	.short	-9363
	.short	-18725
	.short	31993
	.short	-21141
	.short	23254
	.short	-18957
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-2048
	.short	0
	.short	119
	.short	-28898
	.short	15479
	.short	30583
	.short	30576
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	-4370
	.short	-4389
	.short	28086
	.short	-9361
	.short	-24779
	.short	-21141
	.short	23254
	.short	-16772
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	0
	.short	0
	.short	3824
	.short	8064
	.short	-32768
	.short	0
	.short	0
	.short	3825
	.short	-7225
	.short	-28946
	.short	-4370
	.short	-4608
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	29
	.short	-8739
	.short	-9363
	.short	-18725
	.short	28147
	.short	-6475
	.short	-21141
	.short	23255
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3104
	.short	0
	.short	1
	.short	-8644
	.short	30961
	.short	-8739
	.short	-8739
	.short	-16384
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	955
	.short	-17477
	.short	28086
	.short	-9363
	.short	-16772
	.short	-10571
	.short	-21141
	.short	23289
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31744
	.short	0
	.short	59
	.short	-14449
	.short	7739
	.short	-17477
	.short	-17480
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	30583
	.short	30573
	.short	-18725
	.short	28087
	.short	-12390
	.short	-10571
	.short	-21141
	.short	24382
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-32768
	.short	0
	.short	1912
	.short	-3613
	.short	-14473
	.short	30583
	.short	30464
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	14
	.short	-4370
	.short	-4682
	.short	-9363
	.short	-18695
	.short	-3238
	.short	-10571
	.short	-21141
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1552
	.short	0
	.short	0
	.short	-4322
	.short	15480
	.short	-4370
	.short	-4370
	.short	-8192
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	477
	.short	-8739
	.short	-18725
	.short	28086
	.short	-8386
	.short	27482
	.short	-10571
	.short	-21124
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15872
	.short	0
	.short	29
	.short	-8129
	.short	256
	.short	0
	.short	0
	.short	29
	.short	-7225
	.short	-28899
	.short	-8739
	.short	-8740
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	15291
	.short	-17482
	.short	-9363
	.short	-18725
	.short	-6195
	.short	27482
	.short	-10571
	.short	-20577
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-16384
	.short	0
	.short	956
	.short	30961
	.short	-7237
	.short	-17477
	.short	-17536
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	7
	.short	30583
	.short	30427
	.short	28086
	.short	-9348
	.short	-1619
	.short	27482
	.short	-10571
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31992
	.short	0
	.short	0
	.short	30607
	.short	7740
	.short	30583
	.short	30583
	.short	28672
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	238
	.short	-4370
	.short	-9363
	.short	-18725
	.short	28575
	.short	13741
	.short	27482
	.short	-10562
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24832
	.short	0
	.short	14
	.short	-3613
	.short	-14450
	.short	-4370
	.short	-4370
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	7645
	.short	-8741
	.short	28086
	.short	-9363
	.short	-3098
	.short	-19027
	.short	27482
	.short	-10289
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-8192
	.short	0
	.short	478
	.short	15480
	.short	-3619
	.short	-8739
	.short	-8768
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	3
	.short	-17477
	.short	-17555
	.short	-18725
	.short	28094
	.short	31958
	.short	-19027
	.short	27482
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	0
	.short	0
	.short	15296
	.short	32258
	.short	2
	.short	13056
	.short	-17629
	.short	-26289
	.short	-991
	.short	6727
	.short	31487
	.short	29080
	.short	-23057
	.short	-29454
	.short	-4832
	.short	23813
	.short	-15679
	.short	-11646
	.short	-27644
	.short	-9193
	.short	-11496
	.short	27765
	.short	-4564
	.short	28748
	.short	22386
	.short	-9652
	.short	32025
	.short	-26586
	.short	-12207
	.short	-25464
	.short	-16311
	.short	0
	.short	0
	.short	7651
	.short	-14449
	.short	7645
	.short	-8739
	.short	-9216
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	59
	.short	-17477
	.short	-18725
	.short	28086
	.short	-9241
	.short	-12949
	.short	23254
	.short	-19025
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6208
	.short	0
	.short	3
	.short	-17288
	.short	-3613
	.short	-17477
	.short	-17477
	.short	-32768
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	1911
	.short	30582
	.short	-9363
	.short	-18725
	.short	31993
	.short	-21141
	.short	23254
	.short	-18957
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-2048
	.short	0
	.short	119
	.short	-28898
	.short	15479
	.short	30583
	.short	30576
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	-4370
	.short	-4389
	.short	28086
	.short	-9361
	.short	-24779
	.short	-21141
	.short	23254
	.short	-16772
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	0
	.short	0
	.short	3825
	.short	-7225
	.short	-28946
	.short	-4370
	.short	-4608
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	29
	.short	-8739
	.short	-9363
	.short	-18725
	.short	28147
	.short	-6475
	.short	-21141
	.short	23255
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3104
	.short	0
	.short	1
	.short	-8644
	.short	30961
	.short	-8739
	.short	-8739
	.short	-16384
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	955
	.short	-17477
	.short	28086
	.short	-9363
	.short	-16772
	.short	-10571
	.short	-21141
	.short	23289
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31744
	.short	0
	.short	59
	.short	-16258
	.short	512
	.short	600
	.short	212
	.short	9149
	.short	13105
	.short	-22729
	.short	16036
	.short	-15202
	.short	-26341
	.short	11106
	.short	30344
	.short	-29965
	.short	1241
	.short	-27967
	.short	29184
	.short	-3268
	.short	-3349
	.short	24020
	.short	-19572
	.short	-32519
	.short	-6959
	.short	3355
	.short	-18517
	.short	-19798
	.short	23307
	.short	8213
	.short	-33
	.short	1998
	.short	-10983
	.short	5956
	.short	3239
	.short	487
	.short	-11141
	.short	29479
	.short	23683
	.short	7796
	.short	-10576
	.short	-19906
	.short	29171
	.short	-8203
	.short	23610
	.short	22025
	.short	-13961
	.short	-14870
	.short	-6248
	.short	-8952
	.short	0
	.short	0
	.short	7651
	.short	-14449
	.short	7645
	.short	-8739
	.short	-9216
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	59
	.short	-17477
	.short	-18725
	.short	28086
	.short	-9241
	.short	-12949
	.short	23254
	.short	-19025
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6208
	.short	0
	.short	3
	.short	-17288
	.short	-3613
	.short	-17477
	.short	-17477
	.short	-32768
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	1911
	.short	30582
	.short	-9363
	.short	-18725
	.short	31993
	.short	-21141
	.short	23254
	.short	-18957
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-2048
	.short	0
	.short	119
	.short	-28898
	.short	15479
	.short	30583
	.short	30576
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	-4370
	.short	-4389
	.short	28086
	.short	-9361
	.short	-24779
	.short	-21141
	.short	23254
	.short	-16772
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	0
	.short	0
	.short	3825
	.short	-7225
	.short	-28946
	.short	-4370
	.short	-4608
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	29
	.short	-8739
	.short	-9363
	.short	-18725
	.short	28147
	.short	-6475
	.short	-21141
	.short	23255
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3104
	.short	0
	.short	1
	.short	-8644
	.short	30961
	.short	-8739
	.short	-8739
	.short	-16384
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	955
	.short	-17477
	.short	28086
	.short	-9363
	.short	-16772
	.short	-10571
	.short	-21141
	.short	23289
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31744
	.short	0
	.short	59
	.short	-16258
	.short	512
	.short	599
	.short	229
	.short	-28850
	.short	-11275
	.short	19144
	.short	-17594
	.short	-17413
	.short	12295
	.short	-12136
	.short	-23958
	.short	31309
	.short	-32354
	.short	-21813
	.short	15570
	.short	20162
	.short	17720
	.short	22754
	.short	3071
	.short	-18724
	.short	-425
	.short	23134
	.short	-29073
	.short	12825
	.short	11129
	.short	12221
	.short	17828
	.short	17146
	.short	19487
	.short	22913
	.short	-20452
	.short	26112
	.short	-7734
	.short	22312
	.short	20312
	.short	7849
	.short	27384
	.short	-22221
	.short	-23847
	.short	-5514
	.short	26410
	.short	-12509
	.short	10525
	.short	18920
	.short	22753
	.short	18688
	.short	0
	.short	29
	.short	-7225
	.short	-28899
	.short	-8739
	.short	-8740
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	15291
	.short	-17482
	.short	-9363
	.short	-18725
	.short	-6195
	.short	27482
	.short	-10571
	.short	-20577
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-16384
	.short	0
	.short	956
	.short	30961
	.short	-7237
	.short	-17477
	.short	-17536
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	7
	.short	30583
	.short	30427
	.short	28086
	.short	-9348
	.short	-1619
	.short	27482
	.short	-10571
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31992
	.short	0
	.short	0
	.short	30607
	.short	7740
	.short	30583
	.short	30583
	.short	28672
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	238
	.short	-4370
	.short	-9363
	.short	-18725
	.short	28575
	.short	13741
	.short	27482
	.short	-10562
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-6193
	.short	-24832
	.short	0
	.short	14
	.short	-3613
	.short	-14450
	.short	-4370
	.short	-4370
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	7645
	.short	-8741
	.short	28086
	.short	-9363
	.short	-3098
	.short	-19027
	.short	27482
	.short	-10289
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	-1549
	.short	-8192
	.short	0
	.short	478
	.short	15480
	.short	-3619
	.short	-8739
	.short	-8768
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	3
	.short	-17477
	.short	-17555
	.short	-18725
	.short	28094
	.short	31958
	.short	-19027
	.short	27482
	.short	-1549
	.short	-6193
	.short	-24770
	.short	31993
	.short	-3097
	.short	-12385
	.short	15996
	.short	0
	.short	0
	.short	15296
	.short	29987
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_aop_rm, %object
	.size	g_aop_rm, 64
g_aop_rm:
	.space	64
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
