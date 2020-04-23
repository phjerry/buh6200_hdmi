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
	.file	"aoe_engine.c"
	.text
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_check_aip_data, %function
engine_check_aip_data:
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
	ldr	lr, [r0, #32]
	ldr	ip, [r0, #36]
	ldr	r3, [r0, #40]
	cmp	lr, #0
	cmpne	ip, #0
	bne	.L18
	cmp	r3, #0
	bne	.L14
	ldr	r0, [r0, #164]
	adds	r0, r0, #0
	movne	r0, #1
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L14:
	mov	r0, #1
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L18:
	mov	r2, #0
	mov	r4, #1
	mov	r0, r2
.L4:
	lsl	r1, r4, r2
	tst	lr, r1
	beq	.L6
	ands	r1, r3, r1
	mov	r0, #1
	beq	.L19
.L6:
	add	r2, r2, #1
	cmp	r2, #8
	bne	.L4
.L5:
	mov	r2, #0
	mov	lr, #1
.L8:
	lsl	r1, lr, r2
	tst	ip, r1
	beq	.L11
	tst	r3, r1
	beq	.L20
	orr	r0, r0, #2
.L11:
	add	r2, r2, #1
	cmp	r2, #8
	bne	.L8
.L9:
	and	r0, r0, #1
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L20:
	bic	r0, r0, #2
	b	.L9
.L19:
	mov	r0, r1
	b	.L5
	UNWIND(.fnend)
	.size	engine_check_aip_data, .-engine_check_aip_data
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_check_aop_space, %function
engine_check_aop_space:
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
	ldr	r3, [r0, #64]
	ldr	lr, [r0, #48]
	cmp	r3, #0
	ldr	r0, [r0, #52]
	beq	.L36
	cmp	lr, #0
	beq	.L23
	mov	r1, #0
	mov	r4, #1
	mov	ip, r1
.L25:
	lsl	r2, r4, r1
	tst	lr, r2
	beq	.L27
	ands	r2, r3, r2
	mov	ip, #1
	beq	.L44
.L27:
	add	r1, r1, #1
	cmp	r1, #8
	bne	.L25
.L26:
	cmp	r0, #0
	bne	.L34
.L33:
	and	r0, ip, #1
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L23:
	cmp	r0, #0
	movne	ip, lr
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
.L34:
	mov	r2, #0
	mov	r4, #1
.L29:
	lsl	r1, r4, r2
	tst	r0, r1
	beq	.L32
	tst	r3, r1
	beq	.L45
	orr	ip, ip, #2
.L32:
	add	r2, r2, #1
	cmp	r2, #8
	bne	.L29
.L30:
	cmp	lr, #0
	bne	.L33
	mov	r0, lr
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L36:
	mov	r0, r3
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L45:
	bic	ip, ip, #2
	b	.L30
.L44:
	mov	ip, r2
	b	.L26
	UNWIND(.fnend)
	.size	engine_check_aop_space, .-engine_check_aop_space
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_destroy_chn, %function
engine_destroy_chn:
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
	subs	r4, r0, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r5, [r4, #12]
	cmp	r5, #0
	beq	.L55
.L48:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L55:
	ldr	r1, [r4, #108]
	cmp	r1, #0
	beq	.L48
	mov	r0, #70
	bl	HI_ADSP_FREE
	str	r5, [r4, #108]
	b	.L48
	UNWIND(.fnend)
	.size	engine_destroy_chn, .-engine_destroy_chn
	.global	__aeabi_uidiv
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_create_chn, %function
engine_create_chn:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #36)
	sub	sp, sp, #36
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, .L75
	mov	r2, #28
	mov	r1, #0
	mov	r5, r0
	ldr	r4, [r3, r0, lsl #2]
	sub	r0, fp, #64
	bl	adsp_memset
	sub	r0, fp, #64
	ldr	r1, [r4]
	and	r1, r1, #3
	bl	aoe_reg_read_bit_depth
	cmp	r0, #0
	bne	.L59
	ldr	r1, [r4]
	sub	r0, fp, #60
	ubfx	r1, r1, #2, #2
	bl	aoe_reg_read_channel
	cmp	r0, #0
	bne	.L59
	ldr	r1, [r4]
	sub	r0, fp, #56
	ubfx	r1, r1, #4, #4
	bl	aoe_reg_read_sampel_rate
	cmp	r0, #0
	bne	.L59
	ldr	r1, [r4]
	sub	r0, fp, #52
	ubfx	r1, r1, #8, #2
	bl	aoe_reg_read_data_format
	subs	r6, r0, #0
	bne	.L59
	mov	r1, #196
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	subs	r4, r0, #0
	beq	.L59
	mov	r1, r6
	mov	r2, #196
	bl	adsp_memset
	mov	r2, #28
	sub	r1, fp, #64
	mov	r0, r4
	bl	adsp_memcpy
	ldr	r1, [r4]
	ldr	r0, [r4, #4]
	bl	util_calc_frame_size
	ldr	r2, [r4, #8]
	movw	ip, #19923
	movt	ip, 4194
	mov	lr, #60928
	movt	lr, 2
	add	r1, r2, r2, lsl #2
	movw	r3, #48000
	cmp	r2, lr
	cmpne	r2, r3
	umull	r2, r3, r1, ip
	lsr	r3, r3, #6
	beq	.L73
	tst	r3, #3
	bicne	r2, r3, #3
	addne	r3, r2, #4
.L61:
	mul	r3, r3, r0
	ldr	r1, [r4]
	ldr	r0, [r4, #4]
	str	r3, [r4, #84]
	bl	util_calc_frame_size
	ldr	r7, [r4, #84]
	mov	r1, r0
	str	r0, [r4, #88]
	mov	r0, r7
	bl	__aeabi_uidiv
	ldr	r6, [r4, #12]
	cmp	r6, #0
	str	r0, [r4, #16]
	beq	.L74
.L62:
	mov	r3, #1
	str	r5, [r4, #80]
	str	r3, [r4, #92]
.L56:
	mov	r0, r4
	sub	sp, fp, #32
	ldmfd	sp, {r4, r5, r6, r7, r8, fp, sp, pc}
.L73:
	tst	r3, #255
	bicne	r2, r3, #255
	addne	r3, r2, #256
	b	.L61
.L74:
	ldr	lr, [r4]
	add	r3, r4, #108
	ldr	ip, [r4, #8]
	ldr	r2, [r4, #4]
	mov	r1, r3
	str	lr, [r4, #120]
	str	ip, [r4, #128]
	str	r7, [r4, #112]
	mov	r7, r3
	str	r0, [r4, #116]
	add	r0, r4, #136
	str	r2, [r4, #124]
	mov	r2, #28
	str	r6, [r4, #108]
	str	r6, [r4, #132]
	bl	adsp_memcpy
	mov	r1, r7
	mov	r2, #28
	add	r0, r4, #168
	bl	adsp_memcpy
	ldr	r2, [r4, #112]
	ldr	r8, [r4, #140]
	mov	r0, #70
	add	r8, r8, r2, lsl #1
	mov	r1, r8
	bl	HI_ADSP_MALLOC
	subs	r7, r0, #0
	beq	.L63
	mov	r2, r8
	mov	r1, r6
	bl	adsp_memset
	ldr	r3, [r4, #112]
	str	r7, [r4, #108]
	add	r7, r7, r3
	str	r7, [r4, #168]
	add	r7, r7, r3
	str	r7, [r4, #136]
	b	.L62
.L59:
	mov	r4, #0
	mov	r0, r4
	sub	sp, fp, #32
	ldmfd	sp, {r4, r5, r6, r7, r8, fp, sp, pc}
.L63:
	mov	r0, r4
	mov	r4, r7
	bl	engine_destroy_chn
	b	.L56
.L76:
	.align	2
.L75:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	engine_create_chn, .-engine_create_chn
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_out_raw_frame, %function
engine_out_raw_frame:
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
	mov	r5, r1
	mov	r7, r2
	mov	r6, r0
	bl	aip_get_ring_in
	subs	r4, r0, #0
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	mov	r0, r6
	bl	aip_get_mute_status
	cmp	r0, #0
	movne	r8, #0
	beq	.L92
.L79:
	mov	r0, r4
	mov	r2, r7
	mov	r1, r8
	mov	r4, #0
	bl	rbuf_read_data_dma
	mov	r6, #1
.L83:
	ands	r3, r5, r6, lsl r4
	mov	r0, r4
	beq	.L81
	bl	aop_get_ring_out
	mov	r2, r7
	mov	r1, r8
	cmp	r0, #0
	beq	.L81
	bl	rbuf_write_data_dma
	mov	r0, r4
	bl	aop_proc_delay_compensation
.L81:
	add	r4, r4, #1
	cmp	r4, #8
	bne	.L83
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L92:
	bl	aoe_get_wk_buf
	mov	r8, r0
	b	.L79
	UNWIND(.fnend)
	.size	engine_out_raw_frame, .-engine_out_raw_frame
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_mix_data, %function
engine_mix_data:
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
	mov	r0, r1
	mov	r5, r2
	bl	aip_get_ring_out
	ldr	r3, [r4]
	mov	r2, r5
	cmp	r3, #16
	ldr	r3, [r4, #84]
	ldreq	r1, .L96
	ldrne	r1, .L96+4
	bl	rbuf_read_data_with_action
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L97:
	.align	2
.L96:
	.word	action_mixer_16bit
	.word	action_mixer_24bit
	UNWIND(.fnend)
	.size	engine_mix_data, .-engine_mix_data
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_proc_chn_start_cmd, %function
engine_proc_chn_start_cmd:
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
	ldr	r4, .L111
	add	r6, r0, #6
	mov	r5, r0
	ldr	r3, [r4, r6, lsl #2]
	cmp	r3, #0
	beq	.L109
	ldr	r2, [r3, #92]
	cmp	r2, #0
	beq	.L110
.L104:
	mov	r2, #1
	str	r2, [r3, #92]
.L103:
	ldr	r2, [r4, r5, lsl #2]
	ldr	r3, [r2, #4]
	ubfx	r3, r3, #2, #1
	cmp	r3, #0
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r1, [r2, #4]
	bfi	r1, r3, #3, #4
	str	r1, [r2, #4]
	ldr	r3, [r2, #4]
	orr	r3, r3, #4
	str	r3, [r2, #4]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L110:
	mov	r0, r3
	bl	engine_destroy_chn
	mov	r0, r5
	bl	engine_create_chn
	cmp	r0, #0
	mov	r3, r0
	str	r0, [r4, r6, lsl #2]
	bne	.L104
.L108:
	ldr	r3, [r4, r5, lsl #2]
	ldr	r2, [r3, #4]
	tst	r2, #4
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r2, [r3, #4]
	mov	r1, #3
	bfi	r2, r1, #3, #4
	str	r2, [r3, #4]
	ldr	r2, [r3, #4]
	orr	r2, r2, #4
	str	r2, [r3, #4]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L109:
	bl	engine_create_chn
	cmp	r0, #0
	str	r0, [r4, r6, lsl #2]
	movne	r3, #1
	strne	r3, [r0, #92]
	bne	.L103
	b	.L108
.L112:
	.align	2
.L111:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	engine_proc_chn_start_cmd, .-engine_proc_chn_start_cmd
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_reclaim_in, %function
engine_reclaim_in:
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
	cmp	r0, #5
	mov	r5, r1
	mov	r6, r2
	bhi	.L116
	ldr	r2, .L118
	add	r0, r0, #6
	clz	r3, r1
	ldr	r4, [r2, r0, lsl #2]
	lsr	r3, r3, #5
	cmp	r4, #0
	movne	r0, r3
	moveq	r0, #1
	cmp	r0, #0
	bne	.L116
	ldr	r3, [r4, #84]
	cmp	r6, r3
	ldmfdne	sp, {r4, r5, r6, r7, fp, sp, pc}
	ldr	r1, [r4, #88]
	mov	r0, r6
	bl	__aeabi_uidiv
	ldr	r2, [r4, #4]
	ldr	r3, [r4]
	mov	r1, r5
	mul	r2, r2, r0
	ldr	r0, [r4, #168]
	bl	util_mixer_pro
	mov	r3, #7
	mov	r0, r6
	str	r3, [r4, #164]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L116:
	mov	r0, #0
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L119:
	.align	2
.L118:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	engine_reclaim_in, .-engine_reclaim_in
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	action_mixer_24bit, %function
action_mixer_24bit:
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
	subs	r4, r2, #0
	beq	.L121
	mov	r3, #24
	lsr	r2, r4, #2
	bl	util_mixer_pro
.L121:
	mov	r0, r4
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	action_mixer_24bit, .-action_mixer_24bit
	.align	2
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	action_mixer_16bit, %function
action_mixer_16bit:
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
	subs	r4, r2, #0
	beq	.L126
	mov	r3, #16
	lsr	r2, r4, #1
	bl	util_mixer_pro
.L126:
	mov	r0, r4
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	action_mixer_16bit, .-action_mixer_16bit
	.align	2
	.global	engine_process
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_process, %function
engine_process:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #116)
	sub	sp, sp, #116
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r8, .L311
	mov	r9, #0
	mov	r10, #1
.L201:
	ldr	r2, [r8], #4
	ldr	r3, [r2, #4]
	ubfx	r3, r3, #2, #1
	cmp	r3, #0
	bne	.L131
	ldr	r1, [r2, #4]
	ands	r1, r1, #3
	beq	.L133
	cmp	r1, #1
	bne	.L295
	ldr	r4, [r8, #20]
	cmp	r4, #0
	beq	.L136
	ldr	r1, [r4, #92]
	cmp	r1, #1
	streq	r3, [r4, #92]
	ldr	r3, [r2, #4]
	ubfx	r3, r3, #2, #1
	cmp	r3, #0
	bne	.L202
	ldr	r1, [r2, #4]
	bfi	r1, r3, #3, #4
	str	r1, [r2, #4]
	ldr	r3, [r2, #4]
	orr	r3, r3, #4
	str	r3, [r2, #4]
.L202:
	ldr	r3, [r4, #92]
	cmp	r3, #1
	beq	.L296
.L136:
	add	r9, r9, #1
	cmp	r9, #6
	bne	.L201
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L133:
	mov	r0, r9
	bl	engine_proc_chn_start_cmd
.L131:
	ldr	r4, [r8, #20]
	cmp	r4, #0
	beq	.L136
.L302:
	ldr	r3, [r4, #92]
	cmp	r3, #1
	bne	.L136
.L296:
	ldr	r1, [r4]
	mov	r3, #0
	ldr	r2, [r8, #-4]
	mov	r6, r3
	str	r3, [fp, #-128]
	str	r1, [fp, #-136]
	ldr	r1, [r4, #4]
	str	r2, [fp, #-144]
	str	r3, [fp, #-120]
	str	r1, [fp, #-152]
	str	r3, [fp, #-148]
	ldr	r5, [r2, #12]
	ldr	r2, [r4, #16]
	str	r3, [fp, #-124]
	uxth	r5, r5
	str	r3, [fp, #-132]
	str	r2, [fp, #-140]
.L145:
	lsl	r7, r10, r6
	tst	r5, r7
	beq	.L140
	sub	r1, fp, #80
	mov	r0, r6
	bl	aop_get_chn_status
	ldr	r1, [fp, #-80]
	cmp	r1, #1
	bne	.L140
	ldr	r1, [fp, #-76]
	cmp	r1, #1
	ldr	r1, [fp, #-72]
	ldreq	r3, [fp, #-132]
	ldrne	r3, [fp, #-124]
	orreq	r3, r3, r7
	orrne	r3, r3, r7
	streq	r3, [fp, #-132]
	strne	r3, [fp, #-124]
	ldr	r3, [fp, #-136]
	cmp	r3, r1
	beq	.L297
.L143:
	ldr	r3, [fp, #-120]
	orr	r3, r3, r7
	str	r3, [fp, #-120]
.L144:
	ldr	r3, [fp, #-140]
	mov	r0, r6
	ldr	r1, [fp, #-64]
	mul	r1, r1, r3
	bl	aop_query_rbf_free
	cmp	r0, #0
	ldrne	r3, [fp, #-128]
	orrne	r3, r3, r7
	strne	r3, [fp, #-128]
.L140:
	add	r6, r6, #1
	cmp	r6, #8
	bne	.L145
	ldr	r2, [fp, #-132]
	mov	r3, #0
	str	r5, [r4, #44]
	mov	r5, r3
	str	r3, [r4, #32]
	str	r2, [r4, #48]
	ldr	r2, [fp, #-124]
	str	r3, [r4, #36]
	str	r3, [r4, #40]
	str	r2, [r4, #52]
	ldr	r2, [fp, #-148]
	ldr	r3, [fp, #-144]
	str	r2, [r4, #56]
	ldr	r2, [fp, #-120]
	str	r2, [r4, #60]
	ldr	r2, [fp, #-128]
	str	r2, [r4, #64]
	ldr	r3, [r3, #8]
	uxth	r3, r3
	str	r3, [r4, #28]
	b	.L157
.L146:
	add	r5, r5, #1
	cmp	r5, #8
	beq	.L156
.L299:
	ldr	r3, [r4, #28]
.L157:
	lsl	r6, r10, r5
	tst	r6, r3
	beq	.L146
	sub	r1, fp, #108
	mov	r0, r5
	bl	aip_get_chn_status
	ldr	r3, [fp, #-108]
	cmp	r3, #1
	beq	.L298
.L148:
	mov	r0, r5
	ldr	r2, .L311+4
	ldr	r1, [r4, #80]
	add	r5, r5, #1
	bl	aip_reg_reclaim_fun
	cmp	r5, #8
	bne	.L299
.L156:
	ldr	r2, [fp, #-144]
	ldr	r3, [r2, #4]
	ubfx	r3, r3, #13, #1
	str	r3, [r4, #72]
	ldr	r3, [r2, #4]
	ubfx	r3, r3, #14, #1
	str	r3, [r4, #76]
	ldr	r4, [r8, #20]
	ldr	r3, [r4, #44]
	tst	r3, #1
	bne	.L203
	mov	r0, #1
.L160:
	ands	r2, r3, r10, lsl r0
	bne	.L159
	add	r0, r0, #1
	cmp	r0, #8
	bne	.L160
.L172:
	ldr	r5, [r4, #12]
	cmp	r5, #0
	bne	.L300
	mov	r0, r4
	bl	engine_check_aop_space
	cmp	r0, #0
	beq	.L136
	mov	r0, r4
	bl	engine_check_aip_data
	cmp	r0, #0
	beq	.L136
	mov	r3, #5
	mov	r1, r5
	str	r3, [fp, #-80]
	ldr	r2, [r4, #112]
	ldr	r0, [r4, #108]
	bl	adsp_memset
	ldr	r3, [r4, #164]
	cmp	r3, #0
	beq	.L187
	cmp	r3, #7
	ldr	r6, [r4, #108]
	ldr	r2, [r4, #84]
	beq	.L301
	mov	r0, r6
	mov	r1, r5
	bl	adsp_memset
.L183:
	ldr	r3, [r4, #164]
	sub	r3, r3, #1
	str	r3, [r4, #164]
	b	.L187
.L295:
	ldr	r3, [r2, #4]
	mov	r1, #4
	bfi	r3, r1, #3, #4
	str	r3, [r2, #4]
	ldr	r3, [r2, #4]
	orr	r3, r3, r1
	str	r3, [r2, #4]
	ldr	r4, [r8, #20]
	cmp	r4, #0
	bne	.L302
	b	.L136
.L298:
	ldr	r3, [fp, #-104]
	cmp	r3, #1
	ldreq	r3, [r4, #32]
	ldrne	r3, [r4, #36]
	orreq	r3, r3, r6
	orrne	r3, r3, r6
	streq	r3, [r4, #32]
	strne	r3, [r4, #36]
	ldr	r3, [r4, #12]
	cmp	r3, #0
	beq	.L151
	mov	r0, r5
	ldr	r7, [r4, #84]
	bl	aip_query_busy
	cmp	r7, r0
	bhi	.L148
	cmp	r7, #0
	ldrne	r3, [r4, #40]
	orrne	r6, r3, r6
	strne	r6, [r4, #40]
	b	.L148
.L297:
	ldr	r1, [fp, #-68]
	ldr	r3, [fp, #-152]
	cmp	r3, r1
	bne	.L143
	ldr	r3, [fp, #-148]
	orr	r3, r3, r7
	str	r3, [fp, #-148]
	b	.L144
.L151:
	sub	r1, fp, #80
	mov	r0, r5
	bl	aip_get_chn_status
	ldr	r3, [fp, #-80]
	ldr	r7, [r4, #112]
	cmp	r3, #1
	beq	.L303
	mov	r0, r5
	bl	aip_query_fifo_busy
	cmp	r0, r7
	bcc	.L148
.L306:
	cmp	r7, #0
	ldrne	r3, [r4, #40]
	orrne	r6, r3, r6
	strne	r6, [r4, #40]
	b	.L148
.L300:
	ldr	r3, [r4, #28]
	tst	r3, #1
	bne	.L204
	mov	r5, #1
	ands	r2, r3, r10, lsl r5
	bne	.L174
.L304:
	add	r5, r5, #1
	cmp	r5, #8
	beq	.L136
	ands	r2, r3, r10, lsl r5
	beq	.L304
.L174:
	cmp	r5, #8
	beq	.L136
.L173:
	mov	r0, r4
	bl	engine_check_aop_space
	cmp	r0, #0
	beq	.L136
	mov	r0, r5
	bl	aip_peek_add_mute_size
	ldr	r6, [r4, #84]
	cmp	r0, r6
	movcc	r7, r0
	movcs	r7, r6
	cmp	r7, #0
	beq	.L176
	mov	r6, #0
.L180:
	ldr	r3, [r4, #44]
	mov	r0, r6
	ands	r3, r3, r10, lsl r6
	beq	.L178
	bl	aop_get_ring_out
	mov	r1, #0
	mov	r2, r7
	cmp	r0, r1
	beq	.L178
	bl	rbuf_write_data_dma
	mov	r0, r6
	bl	aop_proc_delay_compensation
.L178:
	add	r6, r6, #1
	cmp	r6, #8
	bne	.L180
	mov	r1, r7
	mov	r0, r5
	bl	aip_poke_add_mute_size
	b	.L136
.L159:
	cmp	r0, #8
	beq	.L172
.L158:
	sub	r1, fp, #80
	bl	aop_get_chn_status
	ldr	r3, [fp, #-48]
	cmp	r3, #4
	bhi	.L172
	mov	r5, #0
.L169:
	ldr	r3, [r4, #28]
	lsl	r6, r10, r5
	tst	r6, r3
	beq	.L163
	ldr	r3, [r4, #40]
	tst	r6, r3
	bne	.L163
	mov	r0, r5
	bl	aip_check_data_status_stop
	cmp	r0, #1
	beq	.L163
	sub	r1, fp, #108
	mov	r0, r5
	bl	aip_get_chn_status
	ldr	r3, [fp, #-108]
	cmp	r3, #0
	beq	.L163
	ldr	r1, [r4, #72]
	mov	r0, r5
	bl	aip_continue_output
	ldr	r3, [r4, #12]
	cmp	r3, #0
	bne	.L305
	ldr	r3, [r4, #72]
	cmp	r3, #0
	beq	.L163
	mov	r0, r5
	bl	aip_query_fifo_busy
	ldr	r3, [r4, #84]
	cmp	r0, r3
	ldrcs	r3, [r4, #40]
	orrcs	r6, r3, r6
	strcs	r6, [r4, #40]
.L163:
	add	r5, r5, #1
	cmp	r5, #8
	bne	.L169
	b	.L172
.L303:
	ldr	r3, [fp, #-64]
	mov	r0, r5
	cmp	r3, #1
	lslne	r7, r7, #1
	bl	aip_query_fifo_busy
	cmp	r0, r7
	bcc	.L148
	b	.L306
.L308:
	sub	r1, fp, #80
	mov	r0, r5
	bl	aip_get_pcm_type
	ldr	r3, [fp, #-80]
	cmp	r3, #1
	bhi	.L185
	ldr	r2, [r4, #108]
	mov	r1, r5
	mov	r0, r4
	bl	engine_mix_data
.L185:
	add	r5, r5, #1
	cmp	r5, #8
	beq	.L307
.L187:
	ldr	r3, [r4, #28]
	ands	r3, r3, r10, lsl r5
	beq	.L185
	b	.L308
.L305:
	mov	r0, r5
	bl	aip_query_busy
	subs	r2, r0, #0
	bne	.L309
.L166:
	ldr	r3, [r4, #72]
	cmp	r3, #0
	beq	.L163
	mov	r6, #0
	b	.L168
.L167:
	add	r6, r6, #1
	cmp	r6, #8
	beq	.L163
.L168:
	ldr	r3, [r4, #44]
	ands	r3, r3, r10, lsl r6
	beq	.L167
	ldr	r3, [r4, #76]
	mov	r0, r6
	ldr	r2, [fp, #-96]
	ldr	r1, [fp, #-100]
	bl	aop_continue_output
	b	.L167
.L307:
	mov	r3, #5
	str	r3, [fp, #-80]
	ldr	r5, [r4, #12]
	cmp	r5, #0
	bne	.L136
	b	.L191
.L189:
	add	r5, r5, #1
	cmp	r5, #8
	beq	.L190
.L191:
	ldr	r3, [r4, #28]
	ands	r3, r3, r10, lsl r5
	beq	.L189
	sub	r1, fp, #80
	mov	r0, r5
	bl	aip_get_pcm_type
	ldr	r3, [fp, #-80]
	cmp	r3, #2
	bne	.L189
	mov	r1, r5
	ldr	r2, [r4, #108]
	mov	r0, r4
	bl	engine_mix_data
.L190:
	ldr	r6, [r4, #12]
	ldr	r5, [r4, #48]
	ldr	r3, [r4, #52]
	cmp	r6, #0
	str	r5, [fp, #-116]
	str	r3, [fp, #-112]
	bne	.L136
	str	r8, [fp, #-120]
.L193:
	cmp	r5, #0
	beq	.L194
	mov	r8, #0
.L200:
	lsl	r7, r10, r8
	mov	r0, r8
	tst	r5, r7
	beq	.L196
	bl	aop_write_buff_proc_try_cnt
	mov	r0, r8
	sub	r1, fp, #80
	bl	aop_get_chn_status
	ldr	r3, [r4, #64]
	mov	r0, r8
	tst	r7, r3
	beq	.L196
	bl	aop_write_buff_proc_ok_cnt
	ldr	r3, [r4, #68]
	sub	r7, fp, #108
	mov	r2, #28
	cmp	r3, #0
	add	r1, r4, #108
	mov	r0, r7
	beq	.L198
	ldr	r3, [fp, #-60]
	cmp	r3, #0
	beq	.L310
.L198:
	bl	adsp_memcpy
.L199:
	mov	r1, r7
	mov	r0, r8
	bl	aop_write_rbf_data
	mov	r0, r8
	bl	aop_proc_delay_compensation
.L196:
	add	r8, r8, #1
	cmp	r8, #8
	bne	.L200
.L194:
	add	r6, r6, #1
	cmp	r6, #2
	beq	.L293
	sub	r3, fp, #116
	ldr	r5, [r3, r6, lsl #2]
	b	.L193
.L203:
	mov	r0, #0
	b	.L158
.L176:
	mov	r0, r4
	bl	engine_check_aip_data
	cmp	r0, #0
	beq	.L136
	mov	r2, r6
	ldr	r1, [r4, #44]
	mov	r0, r5
	bl	engine_out_raw_frame
	b	.L136
.L310:
	sub	r7, fp, #108
	add	r1, r4, #136
	mov	r0, r7
	bl	adsp_memcpy
	b	.L199
.L309:
	ldr	r1, [r4, #44]
	mov	r0, r5
	bl	engine_out_raw_frame
	b	.L166
.L301:
	ldr	r1, [r4, #88]
	mov	r0, r2
	bl	__aeabi_uidiv
	ldr	r2, [r4, #4]
	ldr	r1, [r4, #168]
	ldr	r3, [r4]
	mul	r2, r2, r0
	mov	r0, r6
	bl	util_mixer_pro
	ldr	r2, [r4, #84]
	mov	r1, r5
	ldr	r0, [r4, #168]
	bl	adsp_memset
	b	.L183
.L293:
	ldr	r8, [fp, #-120]
	b	.L136
.L204:
	mov	r5, #0
	b	.L173
.L312:
	.align	2
.L311:
	.word	.LANCHOR0
	.word	engine_reclaim_in
	UNWIND(.fnend)
	.size	engine_process, .-engine_process
	.align	2
	.global	engine_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_init, %function
engine_init:
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
	ldr	r5, .L317
	add	r7, r0, #768
	mov	r4, r0
	mov	r6, #0
.L314:
	mov	r2, #128
	mov	r0, r4
	mov	r1, #0
	str	r4, [r5], #4
	bl	adsp_memset
	ldr	r3, [r4, #4]
	mov	r2, #1
	add	r4, r4, #128
	orr	r3, r3, #4
	str	r3, [r4, #-124]
	ldr	r3, [r4, #-124]
	bfi	r3, r2, #0, #2
	str	r3, [r4, #-124]
	cmp	r4, r7
	str	r6, [r5, #20]
	bne	.L314
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L318:
	.align	2
.L317:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	engine_init, .-engine_init
	.align	2
	.global	engine_deinit
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_deinit, %function
engine_deinit:
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
	ldr	r4, .L323
	mov	r6, #0
	add	r5, r4, #24
.L320:
	ldr	r0, [r4, #4]!
	bl	engine_destroy_chn
	cmp	r4, r5
	str	r6, [r4]
	bne	.L320
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L324:
	.align	2
.L323:
	.word	.LANCHOR0+20
	UNWIND(.fnend)
	.size	engine_deinit, .-engine_deinit
	.align	2
	.global	engine_proc_cmd
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	engine_proc_cmd, %function
engine_proc_cmd:
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
	cmp	r3, #5
	ldmfdhi	sp, {fp, sp, pc}
	ldr	r2, [r0]
	cmp	r2, #0
	beq	.L328
	cmp	r2, #1
	ldmfdne	sp, {fp, sp, pc}
	ldr	r1, .L336
	add	r2, r3, #6
	ldr	r2, [r1, r2, lsl #2]
	cmp	r2, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r0, [r2, #92]
	cmp	r0, #1
	moveq	r0, #0
	streq	r0, [r2, #92]
	ldr	r2, [r1, r3, lsl #2]
	ldr	r3, [r2, #4]
	ubfx	r3, r3, #2, #1
	cmp	r3, #0
	ldmfdne	sp, {fp, sp, pc}
	ldr	r1, [r2, #4]
	bfi	r1, r3, #3, #4
	str	r1, [r2, #4]
	ldr	r3, [r2, #4]
	orr	r3, r3, #4
	str	r3, [r2, #4]
	ldmfd	sp, {fp, sp, pc}
.L328:
	mov	r0, r3
	bl	engine_proc_chn_start_cmd
	ldmfd	sp, {fp, sp, pc}
.L337:
	.align	2
.L336:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	engine_proc_cmd, .-engine_proc_cmd
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_engine_rm, %object
	.size	g_engine_rm, 48
g_engine_rm:
	.space	48
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
