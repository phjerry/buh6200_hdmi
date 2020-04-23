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
	.file	"aoe_reg.c"
	.text
	.align	2
	.global	aoe_reg_read_bit_depth
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reg_read_bit_depth, %function
aoe_reg_read_bit_depth:
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
	cmp	r1, #1
	beq	.L3
	bcc	.L4
	cmp	r1, #2
	beq	.L5
	mvn	r3, #0
	mov	r2, #0
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L5:
	mov	r3, #0
	mov	r2, #24
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L4:
	mov	r3, #0
	mov	r2, #8
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L3:
	mov	r3, #0
	mov	r2, #16
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aoe_reg_read_bit_depth, .-aoe_reg_read_bit_depth
	.align	2
	.global	aoe_reg_read_channel
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reg_read_channel, %function
aoe_reg_read_channel:
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
	cmp	r1, #1
	beq	.L11
	bcc	.L12
	cmp	r1, #3
	beq	.L13
	mvn	r3, #0
	mov	r2, #0
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L13:
	mov	r3, #0
	mov	r2, #8
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L12:
	mov	r3, #0
	mov	r2, #1
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L11:
	mov	r3, #0
	mov	r2, #2
	str	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aoe_reg_read_channel, .-aoe_reg_read_channel
	.align	2
	.global	aoe_reg_read_pcm_type
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reg_read_pcm_type, %function
aoe_reg_read_pcm_type:
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
	cmp	r1, #4
	strls	r1, [r0]
	movls	r0, #0
	mvnhi	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aoe_reg_read_pcm_type, .-aoe_reg_read_pcm_type
	.align	2
	.global	aoe_reg_read_sampel_rate
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reg_read_sampel_rate, %function
aoe_reg_read_sampel_rate:
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
	cmp	r1, #13
	ldrls	r2, .L22
	movls	r3, #0
	mvnhi	r3, #0
	ldrls	r2, [r2, r1, lsl #2]
	strls	r2, [r0]
	mov	r0, r3
	ldmfd	sp, {fp, sp, pc}
.L23:
	.align	2
.L22:
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	aoe_reg_read_sampel_rate, .-aoe_reg_read_sampel_rate
	.align	2
	.global	aoe_reg_read_data_format
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reg_read_data_format, %function
aoe_reg_read_data_format:
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
	str	r1, [r0]
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aoe_reg_read_data_format, .-aoe_reg_read_data_format
	.align	2
	.global	aoe_reg_read_buf_latency_ms
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reg_read_buf_latency_ms, %function
aoe_reg_read_buf_latency_ms:
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
	str	r1, [r0]
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aoe_reg_read_buf_latency_ms, .-aoe_reg_read_buf_latency_ms
	.align	2
	.global	aoe_reg_read_buf_delay_ms
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aoe_reg_read_buf_delay_ms, %function
aoe_reg_read_buf_delay_ms:
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
	str	r1, [r0]
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	aoe_reg_read_buf_delay_ms, .-aoe_reg_read_buf_delay_ms
	.section	.rodata
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_aoe_sample_rate_tab, %object
	.size	g_aoe_sample_rate_tab, 56
g_aoe_sample_rate_tab:
	.word	8000
	.word	11025
	.word	12000
	.word	16000
	.word	22050
	.word	24000
	.word	32000
	.word	44100
	.word	48000
	.word	88200
	.word	96000
	.word	176400
	.word	192000
	.word	288000
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
