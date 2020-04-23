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
	.file	"aoe_aip.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_calc_proc_block_size, %function
aip_calc_proc_block_size:
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
	mov	r3, r1
	mov	r4, r0
	mov	r1, r2
	mov	r0, r3
	bl	util_calc_frame_size
	add	r2, r4, r4, lsl #2
	movw	r3, #19923
	movt	r3, 4194
	mov	r1, #25856
	movt	r1, 4
	umull	r2, r3, r2, r3
	movw	ip, #48000
	cmp	r4, ip
	cmpne	r4, r1
	lsr	r3, r3, #6
	bne	.L2
	ands	r2, r3, #255
	addne	r3, r3, #256
	subne	r3, r3, r2
	mul	r0, r0, r3
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L2:
	tst	r3, #3
	bicne	r3, r3, #3
	addne	r3, r3, #4
	mul	r0, r0, r3
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_calc_proc_block_size, .-aip_calc_proc_block_size
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_update_fifo_latency, %function
aip_update_fifo_latency:
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
	mov	r4, r0
	ldr	r0, [r0, #136]
	mov	r6, r1
	bl	rbuf_query_busy
	ldr	r3, [r4, #20]
	cmp	r3, #1
	ldreq	r3, [r4, #364]
	mov	r5, r0
	addeq	r5, r0, r3
	ldr	r0, [r4, #332]
	cmp	r0, #0
	beq	.L13
	bl	sonic_get_left_bytes
	add	r5, r5, r0
.L13:
	mov	r0, r5
	ldr	r2, [r4, #88]
	ldr	r1, [r4, #248]
	bl	util_latency_ms
	ldr	r3, [r6, #4]
	bfi	r3, r0, #18, #12
	str	r3, [r6, #4]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_update_fifo_latency, .-aip_update_fifo_latency
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_update_track_latency, %function
aip_update_track_latency:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #44)
	sub	sp, sp, #44
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r4, r0
	mov	r6, r1
	mov	r0, #0
	sub	r1, fp, #64
	bl	aop_get_chn_status
	ldr	r0, [r4, #136]
	bl	rbuf_query_busy
	ldr	r2, [r4, #88]
	ldr	r1, [r4, #248]
	bl	util_latency_ms
	mov	r5, r0
	ldr	r0, [r4, #132]
	bl	rbuf_query_busy
	ldr	r2, [r4, #32]
	ldr	r1, [r4, #252]
	bl	util_latency_ms
	ldr	r3, [fp, #-32]
	add	r0, r5, r0
	add	r0, r0, r3
	ldr	r3, [r6, #56]
	cmp	r0, #1000
	bficc	r3, r0, #16, #10
	movwcs	r2, #1023
	bfics	r3, r2, #16, #10
	str	r3, [r6, #56]
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_update_track_latency, .-aip_update_track_latency
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_update_chn_reg, %function
aip_update_chn_reg:
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
	ldr	r3, .L21
	add	r2, r0, #8
	ldr	r5, [r3, r0, lsl #2]
	ldr	r4, [r3, r2, lsl #2]
	mov	r1, r5
	mov	r0, r4
	bl	aip_update_fifo_latency
	mov	r1, r5
	mov	r0, r4
	bl	aip_update_track_latency
	mov	r2, #6
	mov	r3, #0
	str	r2, [r4, #368]
	str	r3, [r4, #372]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L22:
	.align	2
.L21:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_update_chn_reg, .-aip_update_chn_reg
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_destroy_chn.part.0, %function
aip_destroy_chn.part.0:
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
	mov	r3, #0
	mov	r4, r0
	str	r3, [r0, #152]
	bl	aip_sonic_deinit
	ldr	r0, [r4, #132]
	cmp	r0, #0
	beq	.L24
	bl	rbuf_destroy
.L24:
	ldr	r0, [r4, #136]
	cmp	r0, #0
	beq	.L25
	bl	rbuf_destroy
.L25:
	ldr	r0, [r4, #148]
	cmp	r0, #0
	beq	.L26
	bl	fade_destroy
.L26:
	ldr	r0, [r4, #140]
	cmp	r0, #0
	beq	.L27
	bl	lsrc_destroy
.L27:
	ldr	r0, [r4, #144]
	cmp	r0, #0
	beq	.L28
	bl	resampler_free
.L28:
	ldr	r1, [r4, #236]
	cmp	r1, #0
	beq	.L29
	mov	r0, #70
	bl	HI_ADSP_FREE
.L29:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_destroy_chn.part.0, .-aip_destroy_chn.part.0
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_reclaim_fifo, %function
aip_reclaim_fifo:
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
	mov	r4, r0
	ldr	r0, [r0, #136]
	cmp	r0, #0
	ldmfdeq	sp, {r4, r5, r6, r7, fp, sp, pc}
	bl	rbuf_query_busy
	ldr	r3, [r4, #244]
	cmp	r0, r3
	bcc	.L53
	bl	aoe_get_wk_buf
	ldr	r5, [r4, #244]
	mov	r2, r5
	mov	r1, r0
	mov	r6, r0
	ldr	r0, [r4, #136]
	bl	rbuf_read_data
	cmp	r5, r0
	bne	.L53
	ldr	r3, [r4, #272]
	cmp	r3, #0
	bne	.L63
.L52:
	ldr	r3, [r4, #372]
	ldr	r2, [r4, #244]
	cmp	r3, #0
	beq	.L53
	mov	r1, r6
	ldr	r0, [r4, #368]
	blx	r3
.L53:
	ldr	r0, [r4, #136]
	bl	rbuf_flush
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L63:
	ldr	r0, [r4, #148]
	mov	r2, #6
	mov	r1, #2
	bl	fade_active
	mov	r2, #256
	mov	r1, r6
	ldr	r0, [r4, #148]
	bl	fade_out_process
	b	.L52
	UNWIND(.fnend)
	.size	aip_reclaim_fifo, .-aip_reclaim_fifo
	.align	2
	.global	aip_get_pcm_type
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_pcm_type, %function
aip_get_pcm_type:
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
	ldr	r3, .L67
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	ldrne	r3, [r3]
	moveq	r3, #5
	ubfxne	r3, r3, #11, #3
	str	r3, [r1]
	ldmfd	sp, {fp, sp, pc}
.L68:
	.align	2
.L67:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_pcm_type, .-aip_get_pcm_type
	.align	2
	.global	aip_get_rbf_fifo_attr
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_rbf_fifo_attr, %function
aip_get_rbf_fifo_attr:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #8)
	sub	sp, sp, #8
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L73
	mov	r4, r1
	ldr	r5, [r3, r0, lsl #2]
	add	r0, r1, #80
	ldr	r1, [r5, #4]
	and	r1, r1, #3
	bl	aoe_reg_read_bit_depth
	cmp	r0, #0
	bne	.L72
	ldr	r1, [r5, #4]
	add	r0, r4, #84
	ubfx	r1, r1, #2, #2
	bl	aoe_reg_read_channel
	cmp	r0, #0
	bne	.L72
	ldr	r1, [r5, #4]
	add	r0, r4, #88
	ubfx	r1, r1, #4, #4
	bl	aoe_reg_read_sampel_rate
	cmp	r0, #0
	bne	.L72
	ldr	r1, [r5, #4]
	add	r0, r4, #92
	ubfx	r1, r1, #8, #2
	bl	aoe_reg_read_data_format
	cmp	r0, #0
	bne	.L72
	ldr	r1, [r5, #4]
	sub	r0, fp, #32
	ubfx	r1, r1, #11, #7
	bl	aoe_reg_read_buf_latency_ms
	subs	r6, r0, #0
	bne	.L72
	ldr	r3, [fp, #-32]
	cmp	r3, #80
	bhi	.L72
	ldr	r1, [r4, #80]
	ldr	r0, [r4, #84]
	bl	util_calc_frame_size
	ldr	r2, [r4, #88]
	str	r6, [r4, #96]
	str	r6, [r4, #116]
	mov	r7, r0
	mov	r1, r0
	mov	r0, #80
	bl	util_latency_ms2_byte_size
	ldr	r3, [fp, #-32]
	add	r2, r5, #68
	str	r7, [r4, #120]
	str	r2, [r4, #108]
	mov	r1, r7
	add	r5, r5, #72
	str	r3, [r4, #128]
	str	r5, [r4, #112]
	ldr	r2, [r4, #88]
	str	r0, [r4, #104]
	mov	r0, r3
	bl	util_latency_ms2_byte_size
	str	r0, [r4, #124]
	mov	r0, r6
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L72:
	mvn	r6, #0
	mov	r0, r6
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L74:
	.align	2
.L73:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_rbf_fifo_attr, .-aip_get_rbf_fifo_attr
	.align	2
	.global	aip_get_rbf_in_attr
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_rbf_in_attr, %function
aip_get_rbf_in_attr:
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
	ldr	r3, .L87
	mov	r4, r1
	ldr	r5, [r3, r0, lsl #2]
	add	r0, r1, #24
	ldr	r1, [r5]
	and	r1, r1, #3
	bl	aoe_reg_read_bit_depth
	cmp	r0, #0
	bne	.L78
	ldr	r1, [r5]
	add	r0, r4, #28
	ubfx	r1, r1, #2, #3
	bl	aoe_reg_read_channel
	cmp	r0, #0
	bne	.L78
	ldr	r1, [r5]
	add	r0, r4, #32
	ubfx	r1, r1, #5, #4
	bl	aoe_reg_read_sampel_rate
	cmp	r0, #0
	beq	.L81
	ldr	r3, [r5]
	ubfx	r3, r3, #5, #4
	cmp	r3, #15
	beq	.L85
.L81:
	ldr	r1, [r5]
	add	r0, r4, #36
	ubfx	r1, r1, #9, #2
	bl	aoe_reg_read_data_format
	cmp	r0, #0
	bne	.L78
	ldr	r1, [r5]
	mov	r0, r4
	ubfx	r1, r1, #11, #3
	bl	aoe_reg_read_pcm_type
	subs	r6, r0, #0
	bne	.L78
	ldr	r3, [r5]
	ldr	r1, [r4, #24]
	ubfx	r3, r3, #14, #1
	ldr	r0, [r4, #28]
	str	r3, [r4, #4]
	bl	util_calc_frame_size
	str	r6, [fp, #-36]
	mov	r3, #1
	str	r6, [fp, #-32]
	str	r3, [r4, #44]
	ldr	r3, [r5, #16]
	str	r3, [fp, #-36]
	ldr	r2, [r5, #20]
	str	r3, [r4, #48]
	str	r2, [fp, #-32]
	ldr	r3, [r5, #24]
	ubfx	r3, r3, #0, #24
	str	r3, [r4, #52]
	ldr	r2, [r5, #24]
	tst	r2, #16777216
	bne	.L86
	add	r2, r5, #28
	str	r2, [r4, #56]
	add	r2, r5, #36
	str	r2, [r4, #60]
.L83:
	ldr	r2, [r5, #24]
	mov	ip, #0
	str	r0, [r4, #68]
	mov	r1, #16
	ubfx	r2, r2, #24, #1
	str	r3, [r4, #72]
	str	r2, [r4, #64]
	mov	r3, #2
	mov	r2, #8
	mov	r0, r6
	str	ip, [r4, #76]
	str	r1, [r4, #12]
	str	r2, [r4, #16]
	str	r3, [r4, #20]
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
.L86:
	str	r6, [fp, #-36]
	str	r6, [fp, #-32]
	ldr	r2, [r5, #28]
	str	r2, [fp, #-36]
	ldr	r1, [r5, #32]
	str	r2, [r4, #56]
	str	r1, [fp, #-32]
	str	r6, [fp, #-36]
	str	r6, [fp, #-32]
	ldr	r2, [r5, #36]
	str	r2, [fp, #-36]
	ldr	r1, [r5, #40]
	str	r2, [r4, #60]
	str	r1, [fp, #-32]
	b	.L83
.L85:
	ldr	r3, [r5, #12]
	movw	r2, #56928
	movt	r2, 2
	ubfx	r3, r3, #0, #24
	str	r3, [r4, #32]
	sub	r3, r3, #4000
	cmp	r3, r2
	bhi	.L78
	ldr	r3, [r4, #28]
	cmp	r3, #2
	bhi	.L78
	ldr	r3, [r4, #24]
	cmp	r3, #16
	beq	.L81
.L78:
	mvn	r6, #0
	mov	r0, r6
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
.L88:
	.align	2
.L87:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_rbf_in_attr, .-aip_get_rbf_in_attr
	.align	2
	.global	aip_get_chn_create_attr
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_chn_create_attr, %function
aip_get_chn_create_attr:
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
	mov	r4, r0
	mov	r5, r1
	bl	aip_get_rbf_in_attr
	cmp	r0, #0
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	mov	r1, r5
	mov	r0, r4
	bl	aip_get_rbf_fifo_attr
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_get_chn_create_attr, .-aip_get_chn_create_attr
	.align	2
	.global	aip_is_chn_attr_change
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_is_chn_attr_change, %function
aip_is_chn_attr_change:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #140)
	sub	sp, sp, #140
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L92
	add	r2, r0, #8
	sub	r1, fp, #152
	ldr	r4, [r3, r2, lsl #2]
	bl	aip_get_chn_create_attr
	ldr	r2, [fp, #-124]
	ldr	r1, [fp, #-128]
	ldr	r3, [r4, #28]
	ldr	ip, [r4, #24]
	subs	r3, r3, r2
	ldr	r0, [r4, #32]
	ldr	r2, [fp, #-120]
	movne	r3, #1
	cmp	ip, r1
	addne	r3, r3, #1
	ldr	ip, [r4, #36]
	cmp	r0, r2
	addne	r3, r3, #1
	ldr	r1, [fp, #-116]
	ldr	r0, [r4]
	ldr	r2, [fp, #-152]
	cmp	ip, r1
	addne	r3, r3, #1
	ldr	ip, [r4, #4]
	ldr	r1, [fp, #-148]
	cmp	r0, r2
	addne	r3, r3, #1
	ldr	r0, [r4, #48]
	ldr	r2, [fp, #-104]
	cmp	ip, r1
	addne	r3, r3, #1
	ldr	ip, [r4, #52]
	ldr	r1, [fp, #-100]
	cmp	r0, r2
	addne	r3, r3, #1
	ldr	r2, [r4, #64]
	ldr	r0, [fp, #-88]
	cmp	ip, r1
	addne	r3, r3, #1
	ldr	ip, [r4, #80]
	ldr	r1, [fp, #-72]
	cmp	r2, r0
	addne	r3, r3, #1
	ldr	r2, [r4, #84]
	ldr	r0, [fp, #-68]
	cmp	ip, r1
	addne	r3, r3, #1
	ldr	ip, [r4, #88]
	ldr	r1, [fp, #-64]
	cmp	r2, r0
	addne	r3, r3, #1
	ldr	r0, [r4, #92]
	ldr	r2, [fp, #-60]
	cmp	ip, r1
	addne	r3, r3, #1
	cmp	r0, r2
	moveq	r0, r3
	addne	r0, r3, #1
	sub	sp, fp, #16
	ldmfd	sp, {r4, fp, sp, pc}
.L93:
	.align	2
.L92:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_is_chn_attr_change, .-aip_is_chn_attr_change
	.align	2
	.global	aip_create_chn
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_create_chn, %function
aip_create_chn:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #184)
	sub	sp, sp, #184
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r2, .L147
	sub	r1, fp, #160
	mov	r3, #0
	mov	r5, r0
	str	r3, [fp, #-204]
	ldr	r6, [r2, r0, lsl #2]
	str	r3, [fp, #-200]
	str	r3, [fp, #-196]
	str	r3, [fp, #-192]
	bl	aip_get_chn_create_attr
	subs	r7, r0, #0
	bne	.L97
	mov	r1, #384
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	subs	r4, r0, #0
	beq	.L97
	mov	r2, #384
	mov	r1, r7
	bl	adsp_memset
	sub	r1, fp, #160
	mov	r2, #132
	mov	r0, r4
	bl	adsp_memcpy
	str	r5, [r4, #256]
	sub	r1, fp, #116
	sub	r0, fp, #196
	bl	rbuf_create_ext
	ldr	r3, [fp, #-196]
	cmp	r3, #0
	beq	.L98
	ldr	r2, [fp, #-124]
	str	r3, [r4, #132]
	cmp	r2, #0
	beq	.L144
	str	r7, [r6, #60]
	ldr	r3, [r6, #64]
	bfi	r3, r7, #0, #16
	str	r3, [r6, #64]
.L100:
	mov	r1, #768
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	cmp	r0, #0
	str	r0, [r4, #236]
	beq	.L98
	ldr	r3, [fp, #-124]
	cmp	r3, #0
	ldrne	r2, [r4]
	bne	.L102
	ldr	r0, [fp, #-128]
	ldr	r1, [fp, #-72]
	cmp	r0, r1
	beq	.L103
	cmp	r0, #32000
	beq	.L104
	bls	.L145
.L104:
	mov	r3, #1
	mov	ip, #0
	str	r3, [r4, #260]
	ldr	r2, [fp, #-76]
	ldr	r3, [fp, #-80]
	str	ip, [sp]
	bl	resampler_create
	cmp	r0, #0
	beq	.L98
	ldr	r1, [fp, #-72]
	str	r0, [r4, #144]
.L103:
	ldr	ip, [fp, #-76]
	mov	r2, r1
	ldr	r3, [fp, #-80]
	sub	r0, fp, #204
	str	ip, [sp]
	bl	lsrc_create
	ldr	r3, [fp, #-204]
	cmp	r3, #0
	beq	.L98
	str	r3, [r4, #140]
	mov	r5, #0
	str	r5, [r6, #80]
	ldr	r3, [r4]
	cmp	r3, r5
	bne	.L146
	ldr	r3, [r6, #8]
	ubfx	r3, r3, #15, #1
	str	r3, [r4, #272]
	b	.L112
.L97:
	mov	r4, #0
	mov	r0, r4
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L98:
	mov	r0, r4
	mov	r4, #0
	bl	aip_destroy_chn.part.0
	mov	r0, r4
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L144:
	sub	r0, fp, #192
	sub	r1, fp, #64
	bl	rbuf_create
	ldr	r0, [fp, #-192]
	cmp	r0, #0
	beq	.L98
	str	r0, [r4, #136]
	sub	r1, fp, #188
	bl	rbuf_query_info
	ldrh	r2, [fp, #-176]
	ldr	r3, [r6, #64]
	bfi	r3, r2, #0, #16
	str	r3, [r6, #64]
	b	.L100
.L146:
	ldr	r3, [fp, #-132]
	sub	r0, fp, #200
	ldr	r2, [fp, #-136]
	ldr	r1, [fp, #-128]
	bl	fade_create
	ldr	r3, [fp, #-200]
	cmp	r3, r5
	beq	.L98
	ldr	r2, [r4]
	str	r3, [r4, #148]
	str	r5, [r4, #224]
.L102:
	ldr	r3, [r6, #8]
	cmp	r2, #4
	ubfx	r3, r3, #15, #1
	str	r3, [r4, #272]
	moveq	r3, #24576
	streq	r3, [r4, #380]
	beq	.L113
.L112:
	mov	r3, #6144
	str	r3, [r4, #380]
.L113:
	mov	r5, #0
	mov	r3, #17
	ldr	r1, [r4, #28]
	mov	r2, #256
	str	r3, [r4, #312]
	str	r2, [r4, #308]
	str	r5, [r4, #156]
	ldr	r2, [r4, #24]
	str	r5, [r4, #160]
	str	r5, [r4, #164]
	str	r5, [r4, #208]
	ldr	r0, [r4, #32]
	bl	aip_calc_proc_block_size
	ldr	r2, [r4, #80]
	ldr	r1, [r4, #84]
	str	r0, [r4, #240]
	ldr	r0, [r4, #88]
	bl	aip_calc_proc_block_size
	ldr	r1, [r4, #80]
	str	r0, [r4, #244]
	ldr	r0, [r4, #84]
	bl	util_calc_frame_size
	ldr	r1, [r4, #24]
	str	r0, [r4, #248]
	ldr	r0, [r4, #28]
	bl	util_calc_frame_size
	mov	r3, #121
	mov	r2, #1
	str	r5, [r4, #184]
	str	r5, [r4, #196]
	str	r5, [r4, #200]
	str	r5, [r4, #364]
	str	r5, [r4, #360]
	str	r2, [r4, #152]
	str	r3, [r4, #188]
	str	r3, [r4, #192]
	str	r0, [r4, #252]
	mov	r0, r4
	bl	aip_sonic_init
	mov	r0, r4
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L145:
	movw	r3, #12000
	cmp	r0, r3
	beq	.L104
	b	.L104
.L148:
	.align	2
.L147:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_create_chn, .-aip_create_chn
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_start_chn_cmd_proc, %function
aip_start_chn_cmd_proc:
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
	ldr	r4, .L166
	add	r7, r0, #8
	mov	r6, r0
	ldr	r5, [r4, r7, lsl #2]
	cmp	r5, #0
	beq	.L163
	ldr	r3, [r5, #152]
	cmp	r3, #0
	beq	.L164
	cmp	r3, #2
	ldrne	r3, [r4, r0, lsl #2]
	beq	.L165
.L156:
	ldr	r2, [r3, #8]
	ubfx	r2, r2, #3, #1
	cmp	r2, #0
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r1, [r3, #8]
	bfi	r1, r2, #4, #4
	str	r1, [r3, #8]
	ldr	r2, [r3, #8]
	orr	r2, r2, #8
	str	r2, [r3, #8]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L164:
	bl	aip_is_chn_attr_change
	cmp	r0, #0
	ldr	r0, [r4, r7, lsl #2]
	moveq	r3, #1
	streq	r3, [r5, #152]
	beq	.L151
	cmp	r0, #0
	beq	.L154
	bl	aip_destroy_chn.part.0
.L154:
	mov	r0, r6
.L163:
	bl	aip_create_chn
	str	r0, [r4, r7, lsl #2]
.L151:
	cmp	r0, #0
	ldr	r3, [r4, r6, lsl #2]
	bne	.L156
	ldr	r2, [r3, #8]
	tst	r2, #8
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r2, [r3, #8]
	mov	r1, #1
	bfi	r2, r1, #4, #4
	str	r2, [r3, #8]
	ldr	r2, [r3, #8]
	orr	r2, r2, #8
	str	r2, [r3, #8]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L165:
	mov	r1, #1
	ldr	r0, [r5, #148]
	str	r1, [r5, #152]
	mov	r2, #8
	bl	fade_active
	ldr	r0, [r4, r7, lsl #2]
	b	.L151
.L167:
	.align	2
.L166:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_start_chn_cmd_proc, .-aip_start_chn_cmd_proc
	.align	2
	.global	aip_flush_chn_ctx
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_flush_chn_ctx, %function
aip_flush_chn_ctx:
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
	mov	r4, r0
	ldr	r0, [r0, #144]
	cmp	r0, #0
	beq	.L169
	bl	resample_flush
.L169:
	ldr	r0, [r4, #140]
	cmp	r0, #0
	beq	.L170
	bl	lsrc_flush
.L170:
	ldr	r0, [r4, #148]
	mov	r3, #0
	str	r3, [r4, #232]
	cmp	r0, r3
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	mov	r2, #8
	mov	r1, #1
	bl	fade_active
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_flush_chn_ctx, .-aip_flush_chn_ctx
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_flush_chn, %function
aip_flush_chn:
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
	tst	r1, #1
	mov	r4, r1
	mov	r5, r0
	bne	.L199
.L182:
	tst	r4, #2
	beq	.L183
	ldr	r0, [r5, #132]
	cmp	r0, #0
	beq	.L183
	bl	rbuf_flush
.L183:
	tst	r4, #4
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	mov	r0, r5
	bl	aip_sonic_flush
	ldr	r0, [r5, #136]
	cmp	r0, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	bl	rbuf_flush
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L199:
	bl	aip_flush_chn_ctx
	b	.L182
	UNWIND(.fnend)
	.size	aip_flush_chn, .-aip_flush_chn
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_pause_chn_cmd_proc, %function
aip_pause_chn_cmd_proc:
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
	ldr	r5, .L206
	add	r3, r0, #8
	mov	r6, r0
	ldr	r4, [r5, r3, lsl #2]
	cmp	r4, #0
	ldmfdeq	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r7, [r4, #152]
	cmp	r7, #1
	beq	.L205
.L203:
	ldr	r2, [r5, r6, lsl #2]
	mov	r1, #0
	mov	r3, #2
	str	r1, [r4, #376]
	str	r3, [r4, #20]
	ldr	r3, [r2, #8]
	ubfx	r3, r3, #3, #1
	cmp	r3, r1
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r1, [r2, #8]
	bfi	r1, r3, #4, #4
	str	r1, [r2, #8]
	ldr	r3, [r2, #8]
	orr	r3, r3, #8
	str	r3, [r2, #8]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L205:
	mov	r0, r4
	bl	aip_reclaim_fifo
	mov	r1, r7
	mov	r0, r4
	bl	aip_flush_chn
	mov	r3, #2
	str	r3, [r4, #152]
	b	.L203
.L207:
	.align	2
.L206:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_pause_chn_cmd_proc, .-aip_pause_chn_cmd_proc
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_flush_chn_cmd_proc, %function
aip_flush_chn_cmd_proc:
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
	ldr	r5, .L216
	add	r3, r0, #8
	mov	r6, r0
	ldr	r4, [r5, r3, lsl #2]
	cmp	r4, #0
	ldmfdeq	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r7, [r4, #152]
	cmp	r7, #1
	beq	.L214
	cmp	r7, #2
	beq	.L215
.L212:
	ldr	r2, [r5, r6, lsl #2]
	mov	r1, #0
	mov	r3, #2
	str	r1, [r4, #376]
	str	r3, [r4, #20]
	ldr	r3, [r2, #8]
	ubfx	r3, r3, #3, #1
	cmp	r3, r1
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r1, [r2, #8]
	bfi	r1, r3, #4, #4
	str	r1, [r2, #8]
	ldr	r3, [r2, #8]
	orr	r3, r3, #8
	str	r3, [r2, #8]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L215:
	mov	r1, #7
	mov	r0, r4
	bl	aip_flush_chn
	b	.L212
.L214:
	mov	r0, r4
	bl	aip_reclaim_fifo
	mov	r0, r4
	mov	r1, #3
	bl	aip_flush_chn
	mov	r1, r7
	mov	r2, #8
	ldr	r0, [r4, #148]
	bl	fade_active
	b	.L212
.L217:
	.align	2
.L216:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_flush_chn_cmd_proc, .-aip_flush_chn_cmd_proc
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_stop_chn_cmd_proc, %function
aip_stop_chn_cmd_proc:
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
	ldr	r6, .L226
	add	r3, r0, #8
	mov	r7, r0
	ldr	r4, [r6, r0, lsl #2]
	ldr	r5, [r6, r3, lsl #2]
	cmp	r4, #0
	cmpne	r5, #0
	ldmfdeq	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r3, [r5, #152]
	sub	r2, r3, #1
	cmp	r2, #1
	bls	.L224
.L221:
	ldr	r3, [r4, #8]
	ubfx	r3, r3, #3, #1
	cmp	r3, #0
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r2, [r4, #8]
	bfi	r2, r3, #4, #4
	str	r2, [r4, #8]
	ldr	r3, [r4, #8]
	orr	r3, r3, #8
	str	r3, [r4, #8]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L224:
	cmp	r3, #1
	beq	.L225
.L222:
	mov	r1, #3
	mov	r0, r5
	bl	aip_flush_chn
	mov	r3, #0
	mov	r2, #2
	str	r3, [r5, #152]
	str	r2, [r5, #20]
	str	r3, [r4, #80]
	ldr	r4, [r6, r7, lsl #2]
	str	r3, [r5, #376]
	str	r3, [r5, #364]
	str	r3, [r5, #360]
	b	.L221
.L225:
	mov	r0, r5
	bl	aip_reclaim_fifo
	b	.L222
.L227:
	.align	2
.L226:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_stop_chn_cmd_proc, .-aip_stop_chn_cmd_proc
	.align	2
	.global	aip_flush_chn_ruf_in
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_flush_chn_ruf_in, %function
aip_flush_chn_ruf_in:
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
	ldr	r0, [r0, #132]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	bl	rbuf_flush
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_flush_chn_ruf_in, .-aip_flush_chn_ruf_in
	.align	2
	.global	aip_flush_chn_fifo
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_flush_chn_fifo, %function
aip_flush_chn_fifo:
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
	ldr	r0, [r0, #136]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	bl	rbuf_flush
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_flush_chn_fifo, .-aip_flush_chn_fifo
	.align	2
	.global	aip_get_chn_cmd
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_chn_cmd, %function
aip_get_chn_cmd:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #8)
	sub	sp, sp, #8
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L247
	ldr	r2, [r3, r0, lsl #2]
	ldr	r3, [r2, #8]
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-16]
	tst	r3, #8
	movne	r0, #4
	bne	.L239
	ldr	r3, [fp, #-16]
	and	r3, r3, #3
	cmp	r3, #3
	ldrls	pc, [pc, r3, asl #2]
	b	.L240
.L241:
	.word	.L246
	.word	.L242
	.word	.L243
	.word	.L244
.L244:
	mov	r0, #3
.L239:
	str	r0, [r1]
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
.L246:
	mov	r0, #0
	str	r0, [r1]
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
.L242:
	mov	r0, #1
	str	r0, [r1]
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
.L243:
	mov	r0, #2
	str	r0, [r1]
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
.L240:
	ldr	r3, [r2, #8]
	mov	ip, #4
	mov	r0, ip
	bfi	r3, ip, #4, #4
	str	r3, [r2, #8]
	ldr	r3, [r2, #8]
	orr	r3, r3, #8
	str	r3, [r2, #8]
	str	r0, [r1]
	sub	sp, fp, #12
	ldmfd	sp, {fp, sp, pc}
.L248:
	.align	2
.L247:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_chn_cmd, .-aip_get_chn_cmd
	.align	2
	.global	aip_ack_chn_cmd
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_ack_chn_cmd, %function
aip_ack_chn_cmd:
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
	ldr	r3, .L251
	ldr	r3, [r3, r0, lsl #2]
	ldr	r2, [r3, #8]
	tst	r2, #8
	ldmfdne	sp, {fp, sp, pc}
	ldr	r2, [r3, #8]
	bfi	r2, r1, #4, #4
	str	r2, [r3, #8]
	ldr	r2, [r3, #8]
	orr	r2, r2, #8
	str	r2, [r3, #8]
	ldmfd	sp, {fp, sp, pc}
.L252:
	.align	2
.L251:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_ack_chn_cmd, .-aip_ack_chn_cmd
	.align	2
	.global	aip_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_init, %function
aip_init:
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
	ldr	r5, .L257
	mov	r4, r0
	mov	r7, #0
	add	r6, r5, #32
.L254:
	mov	r2, #96
	mov	r1, #0
	mov	r0, r4
	str	r4, [r5, #4]!
	bl	adsp_memset
	ldr	r3, [r4, #8]
	mov	r1, #121
	mov	r2, #5
	orr	r3, r3, #8
	str	r3, [r4, #8]
	ldr	r3, [r4, #8]
	cmp	r5, r6
	add	r4, r4, #96
	bfi	r3, r1, #8, #7
	str	r3, [r4, #-88]
	ldr	r3, [r4, #-96]
	bfi	r3, r2, #11, #3
	str	r3, [r4, #-96]
	ldr	r3, [r4, #-4]
	orr	r3, r3, #4
	str	r3, [r4, #-4]
	str	r7, [r5, #32]
	bne	.L254
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L258:
	.align	2
.L257:
	.word	.LANCHOR0-4
	UNWIND(.fnend)
	.size	aip_init, .-aip_init
	.align	2
	.global	aip_deinit
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_deinit, %function
aip_deinit:
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
	ldr	r4, .L267
	mov	r6, #0
	add	r5, r4, #32
.L261:
	ldr	r0, [r4], #4
	cmp	r0, #0
	beq	.L260
	bl	aip_destroy_chn.part.0
.L260:
	cmp	r4, r5
	str	r6, [r4, #-4]
	bne	.L261
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L268:
	.align	2
.L267:
	.word	.LANCHOR0+32
	UNWIND(.fnend)
	.size	aip_deinit, .-aip_deinit
	.global	__aeabi_uidiv
	.align	2
	.global	aip_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_process, %function
aip_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #100)
	sub	sp, sp, #100
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r6, .L418
	movw	r7, #19923
	movt	r7, 4194
	mov	r4, #0
	sub	r8, r6, #32
.L340:
	sub	r1, fp, #72
	mov	r0, r4
	bl	aip_get_chn_cmd
	cmp	r0, #4
	beq	.L270
	ldr	r3, [fp, #-72]
	cmp	r3, #3
	ldrls	pc, [pc, r3, asl #2]
	b	.L270
.L272:
	.word	.L271
	.word	.L273
	.word	.L274
	.word	.L275
.L275:
	mov	r0, r4
	bl	aip_flush_chn_cmd_proc
.L270:
	ldr	r5, [r6]
	cmp	r5, #0
	mov	r2, r5
	beq	.L277
	ldr	r9, [r5, #152]
	cmp	r9, #1
	beq	.L278
.L302:
	mov	r0, r4
	bl	aip_update_chn_reg
.L277:
	add	r4, r4, #1
	add	r6, r6, #4
	cmp	r4, #8
	bne	.L340
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L274:
	mov	r0, r4
	bl	aip_pause_chn_cmd_proc
	b	.L270
.L271:
	mov	r0, r4
	bl	aip_start_chn_cmd_proc
	b	.L270
.L273:
	mov	r0, r4
	bl	aip_stop_chn_cmd_proc
	b	.L270
.L278:
	ldr	r10, [r6, #-32]
	cmp	r10, #0
	mov	r3, r10
	beq	.L342
	ldr	r3, [r5, #20]
	cmp	r3, #1
	bls	.L280
	ldr	r0, [r5, #132]
	bl	rbuf_query_busy
	subs	ip, r0, #0
	beq	.L281
	ldr	r1, [r5, #32]
	ldr	r2, [r5, #68]
	ldr	r0, [r10, #84]
	ldr	r3, [r10, #84]
	mul	r2, r1, r2
	ubfx	r3, r3, #10, #10
	ubfx	r0, r0, #0, #10
	ldr	r10, [r5, #20]
	mul	r0, r2, r0
	mul	r2, r2, r3
	umull	r0, r1, r0, r7
	umull	r2, r3, r2, r7
	lsr	r1, r1, #6
	cmp	r1, ip
	strd	r2, [fp, #-116]
	sub	r2, r10, #2
	clz	r2, r2
	ldr	r3, [fp, #-112]
	lsr	r2, r2, #5
	movls	r2, #0
	cmp	r2, #0
	lsr	r3, r3, #6
	bne	.L281
	sub	lr, r10, #3
	cmp	ip, r3
	clz	lr, lr
	lsr	lr, lr, #5
	movcs	lr, #0
	cmp	lr, #0
	bne	.L281
	str	r9, [r5, #20]
	mov	r1, r9
	mov	r2, #8
	ldr	r0, [r5, #148]
	bl	fade_active
.L281:
	ldr	r2, [r6]
	ldr	r10, [r6, #-32]
	cmp	r10, #0
	cmpne	r2, #0
	mov	r9, r2
	mov	r3, r10
	beq	.L282
.L280:
	ldr	r3, [r10, #92]
	str	r3, [fp, #-96]
	ldr	r3, [fp, #-96]
	tst	r3, #4
	bne	.L399
	ldr	r3, [fp, #-96]
	ands	r3, r3, #3
	beq	.L285
	cmp	r3, #1
	bne	.L397
.L399:
	mov	r3, r10
	mov	r9, r2
.L282:
	cmp	r9, #0
	beq	.L290
.L279:
	ldr	r2, [r3, #84]
	ubfx	r2, r2, #20, #1
	cmp	r2, #1
	streq	r2, [r9, #20]
	ldreq	r2, [r3, #84]
	bfceq	r2, #20, #1
	streq	r2, [r3, #84]
	ldr	r2, [r9, #36]
	cmp	r2, #0
	bne	.L401
	ldr	r1, [r3, #8]
	str	r1, [fp, #-108]
	ldr	r1, [r3, #48]
	str	r1, [fp, #-104]
	ldr	r1, [r3, #76]
	str	r2, [r9, #156]
	str	r1, [fp, #-100]
	ldr	r0, [fp, #-104]
	ldr	r1, [fp, #-104]
	ldr	r2, [fp, #-100]
	ldr	ip, [fp, #-100]
	uxtb	r0, r0
	ubfx	r1, r1, #8, #8
	str	r0, [r9, #160]
	uxtb	r2, r2
	str	r2, [r9, #176]
	ldr	r2, [fp, #-100]
	tst	ip, #2048
	str	r1, [r9, #164]
	ubfxeq	r2, r2, #8, #3
	streq	r2, [r9, #180]
	beq	.L294
	tst	r2, #2048
	ldrne	r2, [fp, #-100]
	ubfxne	r2, r2, #8, #3
	rsbne	r2, r2, #0
	strne	r2, [r9, #180]
.L294:
	ldr	r0, [fp, #-104]
	ldr	r1, [fp, #-104]
	ldr	r2, [fp, #-104]
	ubfx	r0, r0, #16, #1
	ubfx	r1, r1, #17, #3
	str	r1, [r9, #208]
	ubfx	r2, r2, #23, #2
	str	r2, [r9, #212]
	cmp	r2, #1
	str	r0, [r9, #204]
	ldrne	r2, [fp, #-104]
	moveq	r2, #256
	lsrne	r2, r2, #25
	rsbne	r2, r2, #0
	str	r2, [r9, #308]
	ldr	r2, [fp, #-104]
	ldr	r1, [fp, #-108]
	ubfx	r2, r2, #20, #3
	str	r2, [r9, #312]
	ldr	r2, [fp, #-108]
	tst	r1, #1610612736
	ubfxeq	r2, r2, #24, #5
	streq	r2, [r9, #228]
	bne	.L402
.L298:
	ldr	r1, [r3]
	ldr	r2, [r9, #88]
	ubfx	r1, r1, #14, #1
	str	r1, [r9, #4]
	ldr	ip, [r3, #4]
	ldr	r0, [fp, #-100]
	ldr	r1, [fp, #-100]
	ubfx	ip, ip, #10, #1
	ubfx	r0, r0, #24, #1
	str	r0, [r9, #268]
	ubfx	r1, r1, #12, #4
	ldr	r0, [fp, #-100]
	str	r1, [r9, #276]
	ldr	r1, [fp, #-108]
	ubfx	r0, r0, #16, #8
	str	ip, [r9, #264]
	str	r0, [r9, #280]
	ubfx	r1, r1, #15, #1
	str	r1, [r9, #272]
	ldr	r0, [r3, #4]
	ldr	r1, [r9, #248]
	ubfx	r0, r0, #11, #7
	str	r0, [r9, #128]
	bl	util_latency_ms2_byte_size
	add	r1, r9, #96
	str	r0, [r9, #124]
	ldr	r0, [r9, #136]
	bl	rbuf_update_attr
.L290:
	ldr	r9, [r5, #36]
	cmp	r9, #0
	bne	.L277
	ldr	r2, [r5, #360]
	ldr	r3, [r5, #20]
	cmp	r2, #2
	beq	.L403
	cmp	r3, #1
	bne	.L302
	ldr	r3, [r5, #264]
	cmp	r3, #1
	beq	.L302
	ldr	r10, [r5, #232]
	ldr	r1, [r5, #248]
	ldr	r9, [r5, #244]
	mul	r3, r1, r10
	cmp	r3, r9
	bcs	.L404
	ldr	r0, [r5, #332]
	cmp	r0, #0
	beq	.L309
	bl	sonic_get_out_left_bytes
	ldr	r10, [r5, #244]
	cmp	r0, r10
	bcs	.L405
	ldr	r3, [r5, #36]
	cmp	r3, #0
	movne	r9, r10
	beq	.L406
.L311:
	ldr	r0, [r5, #132]
	bl	rbuf_query_busy
	cmp	r0, r10
	bcc	.L302
	cmp	r10, #0
	beq	.L302
	ldr	r0, [r5, #136]
	cmp	r0, #0
	beq	.L302
	mov	r1, r9
	bl	rbuf_check_free
	cmp	r0, #0
	cmpne	r9, #0
	beq	.L302
	bl	aoe_get_wk_buf
	mov	r9, r0
	mov	r0, r10
	str	r9, [fp, #-72]
	ldr	r1, [r5, #252]
	bl	__aeabi_uidiv
	mov	r1, r9
	mov	r2, r10
	str	r0, [fp, #-64]
	ldr	r3, [r5, #28]
	str	r3, [fp, #-56]
	ldr	r3, [r5, #24]
	str	r3, [fp, #-60]
	ldr	r3, [r5, #32]
	str	r3, [fp, #-52]
	ldr	r3, [r5, #36]
	str	r3, [fp, #-48]
	ldr	r0, [r5, #132]
	bl	rbuf_read_data_dma
	str	r0, [fp, #-68]
	ldr	r3, [r5, #36]
	cmp	r3, #0
	str	r3, [fp, #-116]
	beq	.L407
.L316:
	ldr	r3, [r5, #204]
	cmp	r3, #1
	beq	.L408
.L336:
	ldr	r3, [r5, #232]
	ldr	r2, [r5, #248]
	ldr	r9, [r5, #244]
	mul	r2, r2, r3
	cmp	r2, #0
	bne	.L409
.L337:
	cmp	r9, #0
	ldr	r3, [fp, #-68]
	beq	.L338
	cmp	r9, r3
	movcs	r9, r3
	cmp	r9, #0
	beq	.L338
	mov	r2, r9
	ldr	r1, [fp, #-72]
	ldr	r0, [r5, #136]
	bl	rbuf_write_data
	ldr	r3, [fp, #-68]
.L338:
	subs	r10, r3, r9
	beq	.L339
	ldr	r1, [fp, #-72]
	mov	r2, r10
	ldr	r0, [r5, #236]
	add	r1, r1, r9
	bl	adsp_memcpy
.L339:
	mov	r0, r10
	ldr	r1, [r5, #248]
	bl	__aeabi_uidiv
	ldr	r3, [r5, #376]
	add	r3, r3, #1
	str	r3, [r5, #376]
	str	r0, [r5, #232]
	b	.L302
.L342:
	mov	r9, r5
	b	.L279
.L401:
	ldr	r3, [fp, #-104]
	ubfx	r3, r3, #16, #1
	str	r3, [r9, #204]
	b	.L290
.L402:
	ubfx	r2, r2, #29, #2
	cmp	r2, #1
	ldreq	r2, [fp, #-108]
	ubfxeq	r2, r2, #24, #5
	rsbeq	r2, r2, #0
	streq	r2, [r9, #228]
	b	.L298
.L397:
	ldr	r3, [r10, #92]
	mov	r2, #4
	bfi	r3, r2, #3, #4
	str	r3, [r10, #92]
	ldr	r3, [r10, #92]
	orr	r3, r3, r2
	str	r3, [r10, #92]
	ldr	r3, [r6, #-32]
	ldr	r9, [r6]
	b	.L282
.L403:
	cmp	r3, #1
	bne	.L302
	ldr	r0, [r5, #136]
	ldr	r10, [r5, #244]
	cmp	r0, #0
	beq	.L303
	mov	r1, r10
	bl	rbuf_check_free
	cmp	r0, #0
	movne	r9, r10
	ldr	r10, [r5, #244]
.L303:
	cmp	r9, r10
	bcc	.L302
	ldr	r3, [r5, #364]
	mov	r1, #0
	ldr	r0, [r5, #136]
	cmp	r10, r3
	movcs	r10, r3
	mov	r2, r10
	bl	rbuf_write_data
	ldr	r3, [r5, #364]
	sub	r10, r3, r10
	str	r10, [r5, #364]
	cmp	r10, #0
	bne	.L302
	mov	r3, #3
	ldr	r0, [r5, #148]
	str	r3, [r5, #360]
	mov	r2, #8
	mov	r1, #1
	bl	fade_active
	b	.L302
.L285:
	ldr	r3, [r2, #152]
	cmp	r3, #0
	beq	.L410
	ldr	r3, [r2, #88]
	ldr	r1, [r2, #248]
	ldr	r0, [r10, #92]
	ldr	ip, [r2, #36]
	mul	r1, r1, r3
	ubfx	r0, r0, #7, #25
	ldr	r3, [r2, #364]
	cmp	ip, #0
	mul	r0, r0, r1
	umull	r0, r1, r0, r7
	add	r3, r3, r1, lsr #6
	str	r3, [r2, #364]
	movne	r3, #2
	strne	r3, [r2, #360]
	beq	.L411
.L289:
	ldr	r3, [r10, #92]
	bfc	r3, #7, #25
	str	r3, [r10, #92]
	ldr	r3, [r10, #92]
	bfc	r3, #3, #4
	str	r3, [r10, #92]
.L400:
	ldr	r3, [r10, #92]
	orr	r3, r3, #4
	str	r3, [r10, #92]
	ldr	r3, [r6, #-32]
	ldr	r9, [r6]
	b	.L282
.L410:
	ldr	r2, [r10, #92]
	bfi	r2, r3, #7, #25
	str	r2, [r10, #92]
	ldr	r2, [r10, #92]
	bfi	r2, r3, #3, #4
	str	r2, [r10, #92]
	b	.L400
.L404:
	ldr	r0, [r5, #136]
	cmp	r0, #0
	beq	.L302
	mov	r1, r9
	bl	rbuf_check_free
	cmp	r0, #0
	cmpne	r9, #0
	beq	.L302
	ldr	r2, [r5, #244]
	ldr	r1, [r5, #236]
	ldr	r0, [r5, #136]
	bl	rbuf_write_data
	ldr	r1, [r5, #248]
	ldr	r0, [r5, #244]
	bl	__aeabi_uidiv
	ldr	r3, [r5, #232]
	sub	r3, r3, r0
	str	r3, [r5, #232]
	b	.L302
.L411:
	mov	r3, #1
	ldr	r0, [r2, #148]
	str	r3, [r2, #360]
	mov	r1, #2
	mov	r2, #8
	bl	fade_active
	b	.L289
.L309:
	ldr	r3, [r5, #84]
	ldr	r2, [r5, #88]
	str	r3, [fp, #-116]
	ldr	r3, [r5, #80]
.L341:
	mov	r0, r9
	str	r3, [fp, #-124]
	str	r2, [fp, #-120]
	bl	__aeabi_uidiv
	ldr	r2, [fp, #-116]
	ldr	ip, [r5, #140]
	ldr	r3, [fp, #-124]
	str	r2, [sp]
	ldr	r2, [fp, #-120]
	mov	r1, r2
	sub	r10, r0, r10
	mov	r0, ip
	bl	lsrc_update_change
	ldr	r1, [r5, #228]
	ldr	r0, [r5, #140]
	bl	lsrc_set_speed_adjust
	mov	r1, r10
	ldr	r0, [r5, #140]
	bl	lsrc_get_min_input_num
	ldr	r3, [r5, #144]
	cmp	r3, #0
	beq	.L313
	mov	r1, r0
	mov	r0, r3
	bl	resampler_get_min_input_num
.L313:
	tst	r0, #3
	ldr	r10, [r5, #252]
	bicne	r0, r0, #3
	addne	r0, r0, #4
	mul	r10, r10, r0
	b	.L311
.L405:
	mov	r0, r5
	bl	aip_dma_sonic_left_frame
	b	.L302
.L407:
	ldr	r10, [r5, #84]
	ldr	r3, [fp, #-56]
	ldr	r9, [r5, #80]
	cmp	r10, r3
	beq	.L317
	bls	.L318
	bl	aoe_get_wk_buf
	sub	r2, fp, #72
	mov	r1, r0
	mov	r0, r10
	bl	util_upmix_pcm_data
.L317:
	ldr	r3, [fp, #-60]
	cmp	r9, r3
	beq	.L319
	bl	aoe_get_wk_buf
	sub	r2, fp, #72
	mov	r1, r0
	mov	r0, r9
	bl	util_unify_bit_depth_pcm_data
.L319:
	ldr	r3, [r5, #144]
	cmp	r3, #0
	beq	.L320
	ldr	r10, [r5, #88]
	bl	aoe_get_wk_buf
	ldr	ip, [fp, #-52]
	mov	lr, #1
	ldr	r3, [fp, #-64]
	ldr	r1, [fp, #-72]
	mov	r9, r0
	ldr	r0, [r5, #144]
	mov	r2, r9
	stmib	sp, {r10, lr}
	str	ip, [sp]
	bl	resampler_process_frame
	ldr	r1, [fp, #-60]
	str	r9, [fp, #-72]
	str	r10, [fp, #-52]
	str	r0, [fp, #-64]
	ldr	r0, [fp, #-56]
	bl	util_calc_frame_size
	ldr	r3, [fp, #-64]
	mul	r3, r0, r3
	str	r3, [fp, #-68]
.L320:
	ldr	r0, [r5, #140]
	cmp	r0, #0
	beq	.L322
	ldr	r3, [r5, #256]
	ldr	r9, [r8, r3, lsl #2]
	bl	lsrc_check_do_src
	cmp	r0, #0
	beq	.L322
	ldr	r3, [fp, #-64]
	str	r3, [fp, #-120]
	bl	aoe_get_wk_buf
	ldr	r3, [fp, #-120]
	ldr	r1, [fp, #-72]
	mov	r2, r0
	mov	r10, r0
	ldr	r0, [r5, #140]
	bl	lsrc_process_frame
	ldr	r1, [fp, #-60]
	str	r10, [fp, #-72]
	str	r0, [fp, #-64]
	ldr	r0, [fp, #-56]
	bl	util_calc_frame_size
	ldr	r3, [fp, #-64]
	mul	r3, r0, r3
	str	r3, [fp, #-68]
	ldr	r3, [r9, #80]
	add	r3, r3, #1
	str	r3, [r9, #80]
.L322:
	ldr	r3, [r5, #212]
	cmp	r3, #1
	beq	.L412
.L324:
	mov	r2, #20
	add	r1, r5, #156
	sub	r0, fp, #92
	mov	r9, #0
	str	r9, [fp, #-88]
	str	r9, [fp, #-92]
	str	r9, [fp, #-84]
	str	r9, [fp, #-80]
	str	r9, [fp, #-76]
	bl	adsp_memcpy
	ldr	r2, [fp, #-88]
	ldr	r3, [r5, #176]
	ldr	r1, [r5, #180]
	add	r2, r3, r2
	cmp	r2, #120
	strls	r9, [fp, #-88]
	subhi	r2, r2, #121
	strhi	r2, [fp, #-116]
	strhi	r2, [fp, #-88]
	ldr	r2, [fp, #-84]
	ldr	r0, [fp, #-116]
	add	r3, r3, r2
	ldr	r2, [r5, #188]
	cmp	r3, #120
	subhi	r3, r3, #121
	movls	r3, #0
	cmp	r2, r0
	str	r3, [fp, #-84]
	ldr	r2, [fp, #-76]
	ldr	r3, [fp, #-80]
	add	r2, r2, r1
	str	r2, [fp, #-76]
	add	r3, r1, r3
	str	r3, [fp, #-80]
	bne	.L329
	ldr	r2, [r5, #196]
	cmp	r2, r3
	beq	.L413
.L329:
	add	r9, r5, #184
	mov	r1, #0
	sub	r0, fp, #72
	sub	r3, fp, #92
	mov	r2, r9
	bl	util_lr_fade_gain_process
	mov	r0, r9
	sub	r1, fp, #92
	mov	r2, #20
	bl	adsp_memcpy
.L330:
	ldr	r1, [r5, #208]
	sub	r3, r1, #1
	cmp	r3, #6
	bls	.L414
.L331:
	mov	r0, r5
	sub	r1, fp, #72
	bl	aip_sonic_pcm_data
	ldr	r0, [r5, #148]
	cmp	r0, #0
	beq	.L316
	ldr	r3, [r5, #360]
	cmp	r3, #1
	beq	.L415
	cmp	r3, #3
	beq	.L416
	ldr	r1, [r5, #272]
	cmp	r1, #0
	bne	.L417
	mov	r2, #8
	bl	fade_active
	b	.L316
.L409:
	ldr	r1, [r5, #236]
	sub	r9, r9, r2
	ldr	r0, [r5, #136]
	bl	rbuf_write_data
	b	.L337
.L408:
	ldr	r2, [fp, #-68]
	mov	r1, #0
	ldr	r0, [fp, #-72]
	bl	adsp_memset
	b	.L336
.L318:
	bl	aoe_get_wk_buf
	sub	r2, fp, #72
	mov	r1, r0
	mov	r0, r10
	bl	util_downmix_pcm_data
	b	.L317
.L417:
	ldr	r2, [fp, #-60]
	ldr	r1, [fp, #-52]
	ldr	r3, [fp, #-56]
	bl	fade_flush
	ldr	r2, [fp, #-64]
	ldr	r1, [fp, #-72]
	ldr	r0, [r5, #148]
	bl	fade_in_process
	b	.L316
.L412:
	add	r1, r5, #292
	sub	r0, fp, #72
	bl	util_dre_16_bit_process
	b	.L324
.L415:
	ldr	r3, [fp, #-56]
	ldr	r2, [fp, #-60]
	ldr	r1, [fp, #-52]
	bl	fade_flush
	ldr	r2, [fp, #-64]
	ldr	r1, [fp, #-72]
	ldr	r0, [r5, #148]
	bl	fade_out_process
	mov	r3, #2
	str	r3, [r5, #360]
	b	.L316
.L414:
	ldr	r2, [r5, #80]
	sub	r0, fp, #72
	bl	util_channel_mode_process
	b	.L331
.L413:
	sub	r2, fp, #92
	mov	r1, #0
	sub	r0, fp, #72
	bl	util_lr_gain_process
	b	.L330
.L416:
	ldr	r3, [fp, #-56]
	ldr	r2, [fp, #-60]
	ldr	r1, [fp, #-52]
	bl	fade_flush
	ldr	r2, [fp, #-64]
	ldr	r1, [fp, #-72]
	ldr	r0, [r5, #148]
	bl	fade_in_process
	mov	r3, #0
	str	r3, [r5, #360]
	b	.L316
.L406:
	ldr	r3, [r5, #84]
	mov	r9, r10
	ldr	r2, [r5, #88]
	ldr	r1, [r5, #248]
	str	r3, [fp, #-116]
	ldr	r10, [r5, #232]
	ldr	r3, [r5, #80]
	b	.L341
.L419:
	.align	2
.L418:
	.word	.LANCHOR0+32
	UNWIND(.fnend)
	.size	aip_process, .-aip_process
	.align	2
	.global	aip_get_chn_status
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_chn_status, %function
aip_get_chn_status:
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
	ldr	r3, .L424
	add	r0, r0, #8
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	beq	.L423
	ldr	r2, [r3, #152]
	str	r2, [r1]
	ldr	r2, [r3, #4]
	str	r2, [r1, #4]
	ldr	r2, [r3]
	str	r2, [r1, #8]
	ldr	r2, [r3, #380]
	str	r2, [r1, #12]
	ldr	r3, [r3, #264]
	str	r3, [r1, #16]
	ldmfd	sp, {fp, sp, pc}
.L423:
	str	r3, [r1]
	ldmfd	sp, {fp, sp, pc}
.L425:
	.align	2
.L424:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_chn_status, .-aip_get_chn_status
	.align	2
	.global	aip_check_data_status_stop
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_check_data_status_stop, %function
aip_check_data_status_stop:
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
	ldr	r3, .L429
	add	r0, r0, #8
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	ldrne	r0, [r3, #20]
	subne	r0, r0, #2
	clzne	r0, r0
	lsrne	r0, r0, #5
	moveq	r0, #1
	ldmfd	sp, {fp, sp, pc}
.L430:
	.align	2
.L429:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_check_data_status_stop, .-aip_check_data_status_stop
	.align	2
	.global	aip_query_fifo_info
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_query_fifo_info, %function
aip_query_fifo_info:
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
	ldr	r3, .L438
	add	r2, r0, #8
	mov	r0, r1
	ldr	r3, [r3, r2, lsl #2]
	cmp	r3, #0
	beq	.L435
	ldr	r2, [r3, #264]
	cmp	r2, #0
	beq	.L436
	ldr	r2, [r3, #152]
	cmp	r2, #1
	beq	.L437
.L435:
	mov	r2, #28
	mov	r1, #0
	bl	adsp_memset
	ldmfd	sp, {fp, sp, pc}
.L436:
	ldr	r0, [r3, #136]
	bl	rbuf_query_info
	ldmfd	sp, {fp, sp, pc}
.L437:
	ldr	r0, [r3, #132]
	bl	rbuf_query_info
	ldmfd	sp, {fp, sp, pc}
.L439:
	.align	2
.L438:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_query_fifo_info, .-aip_query_fifo_info
	.align	2
	.global	aip_query_fifo_busy
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_query_fifo_busy, %function
aip_query_fifo_busy:
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
	ldr	r3, .L444
	add	r0, r0, #8
	ldr	r0, [r3, r0, lsl #2]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r0, #264]
	cmp	r3, #0
	ldreq	r0, [r0, #136]
	ldrne	r0, [r0, #132]
	bl	rbuf_query_busy
	ldmfd	sp, {fp, sp, pc}
.L445:
	.align	2
.L444:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_query_fifo_busy, .-aip_query_fifo_busy
	.align	2
	.global	aip_query_busy
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_query_busy, %function
aip_query_busy:
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
	ldr	r3, .L452
	add	r0, r0, #8
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	beq	.L450
	ldr	r0, [r3, #132]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r3, #20]
	cmp	r3, #1
	beq	.L451
.L450:
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
.L451:
	bl	rbuf_query_busy
	ldmfd	sp, {fp, sp, pc}
.L453:
	.align	2
.L452:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_query_busy, .-aip_query_busy
	.align	2
	.global	aip_read_data_complete
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_read_data_complete, %function
aip_read_data_complete:
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
	ldr	r3, .L461
	add	r2, r0, #8
	ldr	r4, [r3, r2, lsl #2]
	cmp	r4, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r2, [r4, #264]
	cmp	r2, #0
	beq	.L460
	ldr	r0, [r4, #132]
	bl	rbuf_read_data_complete
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L460:
	ldr	r5, [r3, r0, lsl #2]
	ldr	r0, [r4, #136]
	bl	rbuf_read_data_complete
	mov	r0, r4
	mov	r1, r5
	bl	aip_update_fifo_latency
	mov	r1, r5
	mov	r0, r4
	bl	aip_update_track_latency
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L462:
	.align	2
.L461:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_read_data_complete, .-aip_read_data_complete
	.align	2
	.global	aip_under_flow_calc
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_under_flow_calc, %function
aip_under_flow_calc:
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
	ldr	r3, .L468
	ldr	r2, [r3, r0, lsl #2]
	cmp	r2, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r2, #64]
	ldr	r1, [r2, #64]
	lsr	r3, r3, #16
	add	r3, r3, #1
	uxth	r3, r3
	bfi	r1, r3, #16, #16
	str	r1, [r2, #64]
	ldmfd	sp, {fp, sp, pc}
.L469:
	.align	2
.L468:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_under_flow_calc, .-aip_under_flow_calc
	.align	2
	.global	aip_get_ring_in
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_ring_in, %function
aip_get_ring_in:
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
	ldr	r3, .L473
	add	r0, r0, #8
	ldr	r0, [r3, r0, lsl #2]
	cmp	r0, #0
	ldrne	r0, [r0, #132]
	ldmfd	sp, {fp, sp, pc}
.L474:
	.align	2
.L473:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_ring_in, .-aip_get_ring_in
	.align	2
	.global	aip_get_ring_out
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_ring_out, %function
aip_get_ring_out:
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
	ldr	r3, .L478
	add	r0, r0, #8
	ldr	r0, [r3, r0, lsl #2]
	cmp	r0, #0
	ldrne	r0, [r0, #136]
	ldmfd	sp, {fp, sp, pc}
.L479:
	.align	2
.L478:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_ring_out, .-aip_get_ring_out
	.align	2
	.global	aip_get_mute_status
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_get_mute_status, %function
aip_get_mute_status:
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
	ldr	r3, .L483
	add	r0, r0, #8
	ldr	r0, [r3, r0, lsl #2]
	cmp	r0, #0
	ldrne	r0, [r0, #204]
	ldmfd	sp, {fp, sp, pc}
.L484:
	.align	2
.L483:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_get_mute_status, .-aip_get_mute_status
	.align	2
	.global	aip_continue_output_cnt
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_continue_output_cnt, %function
aip_continue_output_cnt:
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
	ldr	r3, .L490
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	ldrne	r2, [r3, #88]
	addne	r2, r2, #1
	strne	r2, [r3, #88]
	ldmfd	sp, {fp, sp, pc}
.L491:
	.align	2
.L490:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_continue_output_cnt, .-aip_continue_output_cnt
	.align	2
	.global	aip_continue_output
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_continue_output, %function
aip_continue_output:
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
	ldr	r5, .L519
	add	r3, r0, #8
	mov	r6, r0
	ldr	r4, [r5, r3, lsl #2]
	cmp	r4, #0
	beq	.L492
	ldr	r3, [r4, #36]
	cmp	r3, #0
	beq	.L495
	cmp	r1, #1
	beq	.L516
	ldr	r3, [r4, #20]
	cmp	r3, #1
	beq	.L517
.L492:
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
.L495:
	ldr	r3, [r4, #376]
	cmp	r3, #256
	bhi	.L500
	ldr	r3, [r4, #152]
	cmp	r3, #1
	beq	.L492
.L500:
	ldr	r3, [r4, #20]
	bic	r2, r3, #2
	cmp	r2, #1
	bne	.L492
	cmp	r3, #1
	beq	.L518
.L501:
	cmp	r1, #1
	bne	.L492
	ldr	r2, [r4, #380]
	mov	r1, #0
	ldr	r0, [r4, #136]
	bl	rbuf_write_data
	ldr	r3, [r5, r6, lsl #2]
	cmp	r3, #0
	ldrne	r2, [r3, #88]
	addne	r2, r2, #1
	strne	r2, [r3, #88]
	b	.L492
.L517:
	cmp	r1, #0
	moveq	r3, #2
	bne	.L502
.L499:
	str	r3, [r4, #20]
	sub	sp, fp, #24
	ldmfd	sp, {r4, r5, r6, fp, sp, pc}
.L516:
	ldr	r3, [r5, r0, lsl #2]
	cmp	r3, #0
	ldrne	r2, [r3, #88]
	addne	r2, r2, #1
	strne	r2, [r3, #88]
.L497:
	ldr	r3, [r4, #20]
	cmp	r3, #1
	bne	.L492
.L502:
	mov	r3, #3
	b	.L499
.L518:
	mov	r3, #3
	mov	r0, r4
	str	r3, [r4, #20]
	str	r1, [fp, #-32]
	bl	aip_reclaim_fifo
	mov	r0, r4
	bl	aip_flush_chn_ctx
	ldr	r1, [fp, #-32]
	cmp	r1, #0
	streq	r1, [r4, #376]
	bne	.L501
	b	.L492
.L520:
	.align	2
.L519:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_continue_output, .-aip_continue_output
	.align	2
	.global	aip_reg_reclaim_fun
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_reg_reclaim_fun, %function
aip_reg_reclaim_fun:
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
	cmp	r0, #7
	bhi	.L524
	ldr	r3, .L525
	add	r0, r0, #8
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	beq	.L524
	str	r1, [r3, #368]
	mov	r0, #0
	str	r2, [r3, #372]
	ldmfd	sp, {fp, sp, pc}
.L524:
	mvn	r0, #0
	ldmfd	sp, {fp, sp, pc}
.L526:
	.align	2
.L525:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_reg_reclaim_fun, .-aip_reg_reclaim_fun
	.align	2
	.global	aip_peek_add_mute_size
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_peek_add_mute_size, %function
aip_peek_add_mute_size:
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
	ldr	r3, .L531
	add	r0, r0, #8
	ldr	r0, [r3, r0, lsl #2]
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r0, #360]
	cmp	r3, #2
	ldreq	r0, [r0, #364]
	movne	r0, #0
	ldmfd	sp, {fp, sp, pc}
.L532:
	.align	2
.L531:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_peek_add_mute_size, .-aip_peek_add_mute_size
	.align	2
	.global	aip_poke_add_mute_size
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_poke_add_mute_size, %function
aip_poke_add_mute_size:
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
	ldr	r3, .L539
	add	r0, r0, #8
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r2, [r3, #364]
	cmp	r2, r1
	movcc	r2, #0
	strcc	r2, [r3, #364]
	bcc	.L537
	sub	r2, r2, r1
	str	r2, [r3, #364]
	cmp	r2, #0
	ldmfdne	sp, {fp, sp, pc}
.L537:
	mov	r2, #0
	str	r2, [r3, #360]
	ldmfd	sp, {fp, sp, pc}
.L540:
	.align	2
.L539:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_poke_add_mute_size, .-aip_poke_add_mute_size
	.align	2
	.global	aip_proc_cmd
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_proc_cmd, %function
aip_proc_cmd:
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
	ldr	r3, [r0, #8]
	cmp	r3, #7
	ldmfdhi	sp, {fp, sp, pc}
	ldr	r2, [r0]
	cmp	r2, #5
	ldrls	pc, [pc, r2, asl #2]
	b	.L541
.L544:
	.word	.L543
	.word	.L545
	.word	.L546
	.word	.L547
	.word	.L541
	.word	.L548
.L548:
	ldr	r2, [r0, #12]
	cmp	r2, #20
	ldmfdne	sp, {fp, sp, pc}
	ldr	r2, .L552
	add	r3, r3, #8
	ldr	r3, [r2, r3, lsl #2]
	cmp	r3, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r1, [r0, #16]
	mov	r0, r3
	bl	aip_set_speed
.L541:
	ldmfd	sp, {fp, sp, pc}
.L547:
	mov	r0, r3
	bl	aip_flush_chn_cmd_proc
	ldmfd	sp, {fp, sp, pc}
.L543:
	mov	r0, r3
	bl	aip_start_chn_cmd_proc
	ldmfd	sp, {fp, sp, pc}
.L545:
	mov	r0, r3
	bl	aip_stop_chn_cmd_proc
	ldmfd	sp, {fp, sp, pc}
.L546:
	mov	r0, r3
	bl	aip_pause_chn_cmd_proc
	ldmfd	sp, {fp, sp, pc}
.L553:
	.align	2
.L552:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aip_proc_cmd, .-aip_proc_cmd
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_aip_rm, %object
	.size	g_aip_rm, 64
g_aip_rm:
	.space	64
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
