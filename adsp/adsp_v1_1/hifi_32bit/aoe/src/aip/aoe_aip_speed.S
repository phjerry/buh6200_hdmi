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
	.file	"aoe_aip_speed.c"
	.text
	.align	2
	.global	aip_reset_speed
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_reset_speed, %function
aip_reset_speed:
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
	ldr	r0, [r0, #332]
	cmp	r0, #0
	beq	.L2
	mov	r1, #256
	bl	sonic_set_speed
.L2:
	mov	r3, #5
	mov	r2, #1
	str	r3, [r4, #340]
	mov	r3, #0
	str	r2, [r4, #344]
	str	r3, [r4, #348]
	str	r3, [r4, #352]
	str	r3, [r4, #356]
	str	r3, [r4, #336]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_reset_speed, .-aip_reset_speed
	.align	2
	.global	aip_set_speed
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_set_speed, %function
aip_set_speed:
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
	cmp	r1, #0
	cmpne	r0, #0
	mov	r5, r0
	mov	r4, r1
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r0, #332]
	cmp	r3, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r1, #4]
	cmp	r3, #0
	ldmfdlt	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r1, #8]
	cmp	r3, #1000
	ldmfdcs	sp, {r4, r5, fp, sp, pc}
	mov	r2, #20
	add	r0, r0, #340
	bl	adsp_memcpy
	ldr	r3, [r4, #4]
	ldr	r2, [r4, #8]
	add	r3, r3, r3, lsl #2
	lsl	r1, r3, #3
	add	r3, r1, r3, lsl #1
	lsl	r1, r3, #3
	add	r3, r1, r3, lsl #1
	add	r3, r2, r3, lsl #1
	sub	r2, r3, #1000
	sub	r2, r2, #1
	cmp	r2, #1000
	bcc	.L29
	sub	r3, r3, #200
	cmp	r3, #800
	bcs	.L13
	ldr	r3, [r4, #12]
	cmp	r3, #0
	bne	.L14
	ldr	r3, [r4, #16]
	cmp	r3, #0
	bne	.L14
	ldr	r0, [r5, #332]
	mov	r3, #2
	str	r3, [r5, #336]
	cmp	r0, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
.L28:
	ldr	r2, [r4, #8]
	movw	r3, #19923
	movt	r3, 4194
	ldr	r1, [r4, #4]
	lsl	r2, r2, #8
	lsl	r1, r1, #8
	umull	r2, r3, r2, r3
	add	r1, r1, r3, lsr #6
	bl	sonic_set_speed
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L29:
	ldr	r3, [r4, #12]
	cmp	r3, #0
	bne	.L12
	ldr	r3, [r4, #16]
	cmp	r3, #0
	bne	.L12
	ldr	r0, [r5, #332]
	mov	r3, #1
	str	r3, [r5, #336]
	cmp	r0, #0
	bne	.L28
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L12:
	mov	r3, #3
	str	r3, [r5, #336]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L13:
	ldr	r0, [r5, #332]
	cmp	r0, #0
	beq	.L15
	mov	r1, #256
	bl	sonic_set_speed
.L15:
	mov	r3, #5
	mov	r2, #1
	str	r3, [r5, #340]
	mov	r3, #0
	str	r2, [r5, #344]
	str	r3, [r5, #348]
	str	r3, [r5, #352]
	str	r3, [r5, #356]
	str	r3, [r5, #336]
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L14:
	mov	r3, #5
	str	r3, [r5, #336]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_set_speed, .-aip_set_speed
	.align	2
	.global	aip_dma_sonic_left_frame
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_dma_sonic_left_frame, %function
aip_dma_sonic_left_frame:
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
	ldr	r3, [r0, #332]
	mov	r7, r0
	cmp	r3, #0
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	bl	aoe_get_wk_buf
	mov	r4, #256
	mov	r6, #0
	mov	r8, r0
	mov	r5, r0
	b	.L33
.L37:
	cmp	r4, #0
	add	r6, r6, r0
	add	r5, r5, r0
	ble	.L32
.L33:
	mov	r2, r4
	mov	r1, r5
	ldr	r0, [r7, #332]
	bl	sonic_read_short_from_stream
	cmp	r0, #0
	sub	r4, r4, r0
	lsl	r0, r0, #2
	bgt	.L37
.L32:
	mov	r2, r6
	mov	r1, r8
	ldr	r0, [r7, #136]
	bl	rbuf_write_data
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_dma_sonic_left_frame, .-aip_dma_sonic_left_frame
	.align	2
	.global	aip_sonic_pcm_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_sonic_pcm_data, %function
aip_sonic_pcm_data:
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
	ldr	r3, [r0, #332]
	mov	r6, r0
	mov	r7, r1
	cmp	r3, #0
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	ldr	r3, [r0, #336]
	cmp	r3, #2
	bhi	.L70
.L42:
	bl	aoe_get_wk_buf
	ldr	r2, [r7, #8]
	ldr	r1, [r7]
	mov	r8, r0
	ldr	r0, [r6, #332]
	bl	sonic_write_short_to_stream
	cmp	r0, #0
	ldmfdle	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	mov	r5, r8
	mov	r4, #256
	mov	r9, #0
	b	.L54
.L71:
	cmp	r4, #0
	add	r9, r9, r0
	add	r5, r5, r0
	ble	.L53
.L54:
	mov	r2, r4
	mov	r1, r5
	ldr	r0, [r6, #332]
	bl	sonic_read_short_from_stream
	cmp	r0, #0
	sub	r4, r4, r0
	lsl	r0, r0, #2
	bgt	.L71
.L53:
	str	r9, [r7, #4]
	lsr	r9, r9, #2
	str	r8, [r7]
	str	r9, [r7, #8]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L70:
	ldr	r0, [r0, #132]
	bl	rbuf_query_busy
	mov	r4, r0
	ldr	r0, [r6, #332]
	bl	sonic_get_in_left_bytes
	ldr	r2, [r6, #88]
	ldr	r1, [r6, #248]
	add	r0, r0, r4
	bl	util_latency_ms
	ldr	r3, [r6, #336]
	sub	r3, r3, #3
	cmp	r3, #3
	ldrls	pc, [pc, r3, asl #2]
	b	.L42
.L44:
	.word	.L43
	.word	.L47
	.word	.L46
	.word	.L47
.L47:
	ldr	r3, [r6, #356]
	cmp	r0, r3
	bcs	.L42
	ldr	r0, [r6, #332]
	cmp	r0, #0
	beq	.L51
	mov	r1, #256
	bl	sonic_set_speed
.L51:
	mov	r3, #0
	str	r3, [r6, #336]
	b	.L42
.L43:
	ldr	r3, [r6, #352]
	cmp	r0, r3
	bcc	.L42
	ldr	r0, [r6, #332]
	cmp	r0, #0
	beq	.L49
	ldr	r2, [r6, #348]
	movw	r3, #19923
	movt	r3, 4194
	ldr	r1, [r6, #344]
	lsl	r2, r2, #8
	lsl	r1, r1, #8
	umull	r2, r3, r2, r3
	add	r1, r1, r3, lsr #6
	bl	sonic_set_speed
.L49:
	mov	r3, #4
	str	r3, [r6, #336]
	b	.L42
.L46:
	ldr	r3, [r6, #352]
	cmp	r0, r3
	bcc	.L42
	ldr	r0, [r6, #332]
	cmp	r0, #0
	beq	.L50
	ldr	r2, [r6, #348]
	movw	r3, #19923
	movt	r3, 4194
	ldr	r1, [r6, #344]
	lsl	r2, r2, #8
	lsl	r1, r1, #8
	umull	r2, r3, r2, r3
	add	r1, r1, r3, lsr #6
	bl	sonic_set_speed
.L50:
	mov	r3, #6
	str	r3, [r6, #336]
	b	.L42
	UNWIND(.fnend)
	.size	aip_sonic_pcm_data, .-aip_sonic_pcm_data
	.align	2
	.global	aip_sonic_flush
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_sonic_flush, %function
aip_sonic_flush:
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
	ldr	r0, [r0, #332]
	cmp	r0, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	bl	sonic_destroy_stream
	ldr	r1, [r4, #84]
	ldr	r0, [r4, #88]
	bl	sonic_create_stream
	str	r0, [r4, #332]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_sonic_flush, .-aip_sonic_flush
	.align	2
	.global	aip_sonic_deinit
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_sonic_deinit, %function
aip_sonic_deinit:
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
	ldr	r0, [r0, #332]
	cmp	r0, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	bl	sonic_destroy_stream
	mov	r3, #0
	str	r3, [r4, #332]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_sonic_deinit, .-aip_sonic_deinit
	.align	2
	.global	aip_sonic_init
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	aip_sonic_init, %function
aip_sonic_init:
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
	ldr	r5, [r0, #36]
	mov	r4, r0
	cmp	r5, #0
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r0]
	cmp	r3, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	ldr	r1, [r0, #84]
	cmp	r1, #2
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	ldr	r0, [r0, #88]
	movw	r3, #48000
	cmp	r0, r3
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	ldr	r3, [r4, #80]
	cmp	r3, #16
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	bl	sonic_create_stream
	cmp	r0, #0
	str	r0, [r4, #332]
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	mov	r1, #256
	bl	sonic_set_speed
	mov	r2, #5
	mov	r3, #1
	str	r5, [r4, #348]
	str	r5, [r4, #352]
	str	r5, [r4, #356]
	str	r5, [r4, #336]
	str	r2, [r4, #340]
	str	r3, [r4, #344]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	aip_sonic_init, .-aip_sonic_init
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
