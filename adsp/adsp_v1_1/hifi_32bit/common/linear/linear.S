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
	.file	"linear.c"
	.text
	.global	__aeabi_idiv
	.align	2
	.global	lsrc_create
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_create, %function
lsrc_create:
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
	movw	ip, #60927
	sub	lr, r1, #1
	movt	ip, 2
	cmp	lr, ip
	mov	r7, #0
	mov	r5, r1
	mov	r6, r0
	mov	r4, r2
	mov	r8, r3
	str	r7, [r0]
	ldmfdhi	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	sub	r2, r2, #1
	cmp	r2, ip
	ldmfdhi	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	mov	r0, r4
	bl	__aeabi_idiv
	cmp	r0, #6
	ldmfdgt	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	mov	r1, #76
	mov	r0, #70
	bl	HI_ADSP_MALLOC
	subs	r9, r0, #0
	ldmfdeq	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	mov	r1, r7
	mov	r2, #76
	bl	adsp_memset
	cmp	r4, #8000
	beq	.L11
	ldr	r1, .L26
	movw	r2, #11025
	mov	r3, #1
	b	.L8
.L25:
	ldr	r2, [r1, #4]!
.L8:
	cmp	r2, r4
	beq	.L24
	add	r3, r3, #1
	cmp	r3, #13
	bne	.L25
.L7:
	ldr	r3, [fp, #4]
	cmp	r3, #0
	beq	.L9
	mov	r2, #0
	add	r3, r9, #32
	mov	r1, r2
.L10:
	ldr	r0, [fp, #4]
	add	r2, r2, #1
	str	r1, [r3, #4]!
	cmp	r2, r0
	bne	.L10
.L9:
	ldr	r3, [fp, #4]
	str	r8, [r9]
	str	r5, [r9, #12]
	stmib	r9, {r3, r5}
	str	r4, [r9, #16]
	str	r9, [r6]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L24:
	ldr	r2, .L26+4
	add	r3, r2, r3, lsl #2
	ldr	r3, [r3, #52]
.L5:
	str	r3, [r9, #32]
	b	.L7
.L11:
	movw	r3, #3145
	movt	r3, 2
	b	.L5
.L27:
	.align	2
.L26:
	.word	.LANCHOR0+4
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	lsrc_create, .-lsrc_create
	.align	2
	.global	lsrc_destroy
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_destroy, %function
lsrc_destroy:
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
	subs	r1, r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	mov	r0, #70
	bl	HI_ADSP_FREE
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	lsrc_destroy, .-lsrc_destroy
	.align	2
	.global	lsrc_get_max_output_num
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_get_max_output_num, %function
lsrc_get_max_output_num:
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
	subs	r3, r0, #0
	mov	r2, r1
	beq	.L35
	ldr	r0, [r3, #16]
	ldr	r1, [r3, #8]
	mul	r0, r0, r2
	bl	__aeabi_idiv
	add	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
.L35:
	mvn	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	lsrc_get_max_output_num, .-lsrc_get_max_output_num
	.global	__aeabi_idivmod
	.align	2
	.global	lsrc_get_min_input_num
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_get_min_input_num, %function
lsrc_get_min_input_num:
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
	subs	r3, r0, #0
	mov	r2, r1
	beq	.L38
	ldr	r0, [r3, #8]
	ldr	r1, [r3, #16]
	mul	r0, r0, r2
	bl	__aeabi_idivmod
	cmp	r1, #0
	addne	r0, r0, #1
	ldmfd	sp, {fp, sp, pc}
.L38:
	mvn	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	lsrc_get_min_input_num, .-lsrc_get_min_input_num
	.align	2
	.global	lsrc_check_update
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_check_update, %function
lsrc_check_update:
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
	ldm	r0, {r4, ip}
	ldr	lr, [fp, #4]
	subs	ip, ip, lr
	ldr	lr, [r0, #12]
	ldr	r0, [r0, #16]
	movne	ip, #1
	cmp	r4, r3
	moveq	r3, ip
	addne	r3, ip, #1
	cmp	lr, r1
	addne	r3, r3, #1
	cmp	r0, r2
	moveq	r0, r3
	addne	r0, r3, #1
	adds	r0, r0, #0
	movne	r0, #1
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	lsrc_check_update, .-lsrc_check_update
	.align	2
	.global	lsrc_update_change
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_update_change, %function
lsrc_update_change:
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
	ldm	r0, {r4, ip}
	ldr	lr, [fp, #4]
	ldr	r5, [r0, #12]
	subs	ip, ip, lr
	movne	ip, #1
	cmp	r4, r3
	addne	ip, ip, #1
	ldr	r4, [r0, #16]
	cmp	r5, r1
	addne	ip, ip, #1
	cmp	r4, r2
	addne	ip, ip, #1
	cmp	ip, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
	cmp	r2, #8000
	stm	r0, {r3, lr}
	str	r1, [r0, #8]
	mov	r3, #0
	str	r1, [r0, #12]
	str	r2, [r0, #16]
	str	r3, [r0, #20]
	str	r3, [r0, #24]
	str	r3, [r0, #28]
	beq	.L50
	ldr	ip, .L65
	movw	r1, #11025
	mov	r3, #1
	b	.L48
.L63:
	ldr	r1, [ip, #4]!
.L48:
	cmp	r1, r2
	beq	.L62
	add	r3, r3, #1
	cmp	r3, #13
	bne	.L63
	cmp	lr, #0
	ldmfdeq	sp, {r4, r5, fp, sp, pc}
.L64:
	mov	r3, #0
	add	r0, r0, #32
	mov	r2, r3
.L49:
	add	r3, r3, #1
	str	r2, [r0, #4]!
	cmp	r3, lr
	bne	.L49
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L62:
	ldr	r2, .L65+4
	add	r3, r2, r3, lsl #2
	ldr	r3, [r3, #52]
.L45:
	cmp	lr, #0
	str	r3, [r0, #32]
	bne	.L64
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L50:
	movw	r3, #3145
	movt	r3, 2
	b	.L45
.L66:
	.align	2
.L65:
	.word	.LANCHOR0+4
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	lsrc_update_change, .-lsrc_update_change
	.align	2
	.global	lsrc_flush
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_flush, %function
lsrc_flush:
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
	ldr	ip, [r0, #16]
	mov	r3, #0
	str	r3, [r0, #20]
	cmp	ip, #8000
	str	r3, [r0, #24]
	beq	.L74
	ldr	r1, .L85
	movw	r2, #11025
	mov	r3, #1
	b	.L71
.L84:
	ldr	r2, [r1, #4]!
.L71:
	cmp	r2, ip
	beq	.L83
	add	r3, r3, #1
	cmp	r3, #13
	bne	.L84
.L70:
	ldr	r2, [r0, #4]
	cmp	r2, #0
	ldmfdeq	sp, {fp, sp, pc}
	mov	r3, #0
	add	r0, r0, #32
	mov	r1, r3
.L73:
	add	r3, r3, #1
	str	r1, [r0, #4]!
	cmp	r3, r2
	bne	.L73
	ldmfd	sp, {fp, sp, pc}
.L83:
	ldr	r2, .L85+4
	add	r3, r2, r3, lsl #2
	ldr	r3, [r3, #52]
.L68:
	str	r3, [r0, #32]
	b	.L70
.L74:
	movw	r3, #3145
	movt	r3, 2
	b	.L68
.L86:
	.align	2
.L85:
	.word	.LANCHOR0+4
	.word	.LANCHOR0
	UNWIND(.fnend)
	.size	lsrc_flush, .-lsrc_flush
	.align	2
	.global	lsrc_check_do_src
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_check_do_src, %function
lsrc_check_do_src:
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
	ldr	r2, [r0, #8]
	ldr	r3, [r0, #16]
	cmp	r2, r3
	beq	.L90
	mov	r0, #1
	ldmfd	sp, {fp, sp, pc}
.L90:
	ldr	r0, [r0, #24]
	adds	r0, r0, #0
	movne	r0, #1
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	lsrc_check_do_src, .-lsrc_check_do_src
	.align	2
	.global	lsrc_set_speed_adjust
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_set_speed_adjust, %function
lsrc_set_speed_adjust:
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
	ldr	r2, [r0, #28]
	add	ip, r1, #100
	sub	r3, r2, r1
	clz	r3, r3
	lsr	r3, r3, #5
	cmp	ip, #200
	orrhi	r3, r3, #1
	cmp	r3, #0
	ldmfdne	sp, {r4, r5, fp, sp, pc}
	clz	r3, r2
	cmp	r1, #0
	add	ip, r1, #1000
	movw	r4, #19923
	lsr	r3, r3, #5
	moveq	r3, #0
	cmp	r3, #0
	movt	r4, 4194
	str	r1, [r0, #28]
	movne	r3, #1
	strne	r3, [r0, #24]
	adds	r2, r2, #0
	movne	r2, #1
	cmp	r1, #0
	movne	r2, #0
	cmp	r2, #0
	mov	r2, #0
	str	r2, [r0, #20]
	movne	r3, #1
	strne	r3, [r0, #24]
	ldr	r3, [r0, #12]
	mul	r3, r3, ip
	smull	r4, r5, r3, r4
	asr	r3, r3, #31
	rsb	r3, r3, r5, asr #6
	str	r3, [r0, #8]
	ldmfd	sp, {r4, r5, fp, sp, pc}
	UNWIND(.fnend)
	.size	lsrc_set_speed_adjust, .-lsrc_set_speed_adjust
	.align	2
	.global	lsrc_process_frame
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	lsrc_process_frame, %function
lsrc_process_frame:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	UNWIND(.pad #44)
	sub	sp, sp, #44
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r3, #0
	ldr	r4, [r0, #24]
	ldr	lr, [r0]
	movle	ip, #0
	movgt	ip, #1
	cmp	r4, #0
	str	r1, [fp, #-80]
	ldr	r1, [r0, #8]
	str	r2, [fp, #-72]
	movne	r2, ip
	moveq	r2, #0
	cmp	lr, #16
	str	r3, [fp, #-76]
	mov	ip, r1
	ldr	r10, [r0, #4]
	ldr	r3, [r0, #16]
	beq	.L187
	cmp	r2, #0
	streq	r2, [fp, #-48]
	bne	.L188
.L134:
	cmp	ip, r3
	movne	r2, #0
	strne	r2, [fp, #-60]
	beq	.L189
.L141:
	ldr	r2, [fp, #-76]
	cmp	r2, #0
	ble	.L145
	ldr	r2, [r0, #20]
	cmp	r3, r2
	ble	.L146
	ldr	lr, [fp, #-48]
	lsl	ip, r10, #2
	ldr	r5, [fp, #-72]
	mov	r9, #0
	ldr	r3, [fp, #-60]
	movt	r9, 32767
	str	ip, [fp, #-56]
	mla	ip, ip, lr, r5
	mul	r1, r10, r3
	ldr	r3, [r0, #16]
	str	ip, [fp, #-52]
	mov	r4, r3
	ldr	ip, [fp, #-80]
	add	r1, ip, r1, lsl #2
	str	r1, [fp, #-64]
.L147:
	cmp	r10, #0
	ble	.L152
	add	r7, r0, #32
	ldr	r6, [fp, #-52]
	ldr	r5, [fp, #-64]
	mov	ip, #0
	b	.L150
.L190:
	ldr	r2, [r0, #20]
	ldr	r4, [r0, #16]
.L150:
	ldr	r1, [r0, #32]
	ldr	lr, [r5], #4
	mul	r3, r2, r1
	.syntax divided
	smull r8,r3,lr,r3
	.arm
	.syntax unified
	sub	r2, r4, r2
	ldr	lr, [r7, #4]!
	mul	r2, r1, r2
	.syntax divided
	smull r1,r2,lr,r2
	.arm
	.syntax unified
	add	r3, r3, r2
	add	ip, ip, #1
	mov	r2, r9
	lsr	r3, r3, #14
	lsrs	r1, r3, #15
	lsleq	r2, r3, #16
	cmp	ip, r10
	str	r2, [r6], #4
	bne	.L190
	ldr	r3, [r0, #16]
	ldr	r2, [r0, #20]
	mov	r4, r3
.L152:
	ldr	r1, [r0, #8]
	ldr	ip, [fp, #-48]
	add	r2, r2, r1
	ldr	lr, [fp, #-56]
	add	ip, ip, #1
	cmp	r2, r3
	str	ip, [fp, #-48]
	ldr	ip, [fp, #-52]
	str	r2, [r0, #20]
	add	ip, ip, lr
	str	ip, [fp, #-52]
	blt	.L147
.L146:
	ldr	r1, [fp, #-76]
	sub	r2, r2, r3
	str	r2, [r0, #20]
	sub	r1, r1, #1
	str	r1, [fp, #-68]
	cmp	r1, #0
	ble	.L145
	ldr	lr, [fp, #-60]
	lsl	r1, r10, #2
	ldr	r4, [fp, #-80]
	mov	r8, #0
	add	ip, lr, #1
	str	r1, [fp, #-56]
	ldr	r3, [r0, #16]
	movt	r8, 32767
	mla	lr, r1, lr, r4
	str	ip, [fp, #-84]
	mla	r1, r1, ip, r4
	str	lr, [fp, #-64]
	str	r1, [fp, #-60]
.L153:
	cmp	r3, r2
	mov	r7, r3
	ble	.L157
	ldr	r1, [fp, #-48]
	ldr	ip, [fp, #-56]
	ldr	lr, [fp, #-72]
	mla	r1, ip, r1, lr
	str	r1, [fp, #-52]
.L160:
	cmp	r10, #0
	ble	.L158
	ldr	r6, [fp, #-52]
	mov	ip, #0
	ldr	r5, [fp, #-64]
	ldr	r4, [fp, #-60]
	b	.L156
.L191:
	ldr	r2, [r0, #20]
	ldr	r7, [r0, #16]
.L156:
	ldr	r1, [r0, #32]
	ldr	lr, [r4], #4
	mul	r3, r2, r1
	.syntax divided
	smull r9,r3,lr,r3
	.arm
	.syntax unified
	sub	r2, r7, r2
	ldr	lr, [r5], #4
	mul	r2, r1, r2
	.syntax divided
	smull r1,r2,lr,r2
	.arm
	.syntax unified
	add	r3, r3, r2
	add	ip, ip, #1
	mov	r2, r8
	lsr	r3, r3, #14
	lsrs	r1, r3, #15
	lsleq	r2, r3, #16
	cmp	ip, r10
	str	r2, [r6], #4
	bne	.L191
	ldr	r3, [r0, #16]
	ldr	r2, [r0, #20]
	mov	r7, r3
.L158:
	ldr	r1, [r0, #8]
	ldr	ip, [fp, #-48]
	add	r2, r2, r1
	ldr	lr, [fp, #-56]
	add	ip, ip, #1
	cmp	r2, r3
	str	ip, [fp, #-48]
	ldr	ip, [fp, #-52]
	str	r2, [r0, #20]
	add	ip, ip, lr
	str	ip, [fp, #-52]
	blt	.L160
.L157:
	ldr	r1, [fp, #-68]
	sub	r2, r2, r3
	ldr	ip, [fp, #-60]
	subs	r1, r1, #1
	str	r1, [fp, #-68]
	ldr	r1, [fp, #-56]
	str	r2, [r0, #20]
	add	ip, ip, r1
	str	ip, [fp, #-60]
	ldr	ip, [fp, #-64]
	add	r1, ip, r1
	str	r1, [fp, #-64]
	bne	.L153
	ldr	r3, [fp, #-76]
	ldr	r2, [fp, #-84]
	sub	r3, r3, #2
	add	r3, r3, r2
	str	r3, [fp, #-60]
.L145:
	cmp	r10, #0
	ble	.L101
	ldr	r3, [fp, #-60]
	add	r0, r0, #32
	ldr	r1, [fp, #-80]
	mov	r2, #0
	mul	r3, r10, r3
	add	r3, r1, r3, lsl #2
.L161:
	add	r2, r2, #1
	ldr	r1, [r3], #4
	cmp	r2, r10
	str	r1, [r0, #4]!
	bne	.L161
.L101:
	ldr	r0, [fp, #-48]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L189:
	ldr	ip, [fp, #-76]
	cmp	ip, #0
	ble	.L101
	ldr	r3, [fp, #-48]
	lsl	r5, r10, #2
	ldr	r2, [fp, #-72]
	ldr	r4, [fp, #-80]
	mla	lr, r5, r3, r2
.L142:
	cmp	r10, #0
	movgt	r1, lr
	movgt	r2, r4
	movgt	r3, #0
	ble	.L144
.L143:
	add	r3, r3, #1
	ldr	r0, [r2], #4
	cmp	r3, r10
	str	r0, [r1], #4
	bne	.L143
.L144:
	subs	ip, ip, #1
	add	r4, r4, r5
	add	lr, lr, r5
	bne	.L142
.L114:
	ldr	r3, [fp, #-48]
	ldr	r2, [fp, #-76]
	add	r3, r3, r2
	str	r3, [fp, #-48]
	ldr	r0, [fp, #-48]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L188:
	cmp	r1, r3
	beq	.L135
	cmp	r10, #0
	ble	.L137
	ldr	r2, [fp, #-80]
	add	r1, r0, #32
	sub	ip, r2, #4
	mov	r2, #0
.L140:
	add	r2, r2, #1
	ldr	lr, [ip, #4]!
	cmp	r2, r10
	str	lr, [r1, #4]!
	bne	.L140
.L137:
	ldr	r1, [fp, #-76]
	mov	r2, #0
	str	r2, [r0, #24]
	sub	r1, r1, #1
	str	r2, [fp, #-48]
	str	r1, [fp, #-76]
	mov	r2, #1
	str	r2, [fp, #-60]
	b	.L141
.L187:
	cmp	r2, #0
	streq	r2, [fp, #-48]
	bne	.L192
.L103:
	cmp	r1, r3
	movne	r2, #0
	strne	r2, [fp, #-64]
	beq	.L193
.L110:
	ldr	r2, [fp, #-76]
	cmp	r2, #0
	ble	.L117
	ldr	r2, [r0, #20]
	cmp	r3, r2
	ble	.L118
	ldr	r3, [fp, #-64]
	lsl	ip, r10, #1
	ldr	lr, [fp, #-48]
	movw	r9, #32767
	ldr	r5, [fp, #-72]
	mul	r1, r10, r3
	str	ip, [fp, #-56]
	mla	lr, ip, lr, r5
	ldr	r3, [r0, #16]
	add	r7, r10, r1
	ldr	ip, [fp, #-80]
	str	r10, [fp, #-52]
	add	r7, ip, r7, lsl #1
	mov	r4, r3
	add	r1, ip, r1, lsl #1
	str	r1, [fp, #-60]
	mov	r10, lr
.L119:
	ldr	r1, [fp, #-52]
	cmp	r1, #0
	ble	.L124
	add	r6, r0, #32
	mov	r5, r10
	ldr	ip, [fp, #-60]
	b	.L122
.L194:
	ldr	r2, [r0, #20]
	ldr	r4, [r0, #16]
.L122:
	ldr	r1, [r0, #32]
	ldrsh	lr, [ip], #2
	mul	r3, r2, r1
	.syntax divided
	smull r8,r3,lr,r3
	.arm
	.syntax unified
	sub	r2, r4, r2
	ldr	lr, [r6, #4]!
	mul	r2, r1, r2
	.syntax divided
	smull r1,r2,lr,r2
	.arm
	.syntax unified
	add	r3, r3, r2
	lsl	r3, r3, #2
	asr	r2, r3, #31
	cmp	r2, r3, asr #15
	eorne	r3, r2, r9
	cmp	r7, ip
	strh	r3, [r5], #2
	bne	.L194
	ldr	r3, [r0, #16]
	ldr	r2, [r0, #20]
	mov	r4, r3
.L124:
	ldr	r1, [r0, #8]
	ldr	ip, [fp, #-48]
	add	r2, r2, r1
	str	r2, [r0, #20]
	add	ip, ip, #1
	cmp	r2, r3
	str	ip, [fp, #-48]
	ldr	ip, [fp, #-56]
	add	r10, r10, ip
	blt	.L119
	ldr	r10, [fp, #-52]
.L118:
	ldr	r1, [fp, #-76]
	sub	r2, r2, r3
	str	r2, [r0, #20]
	sub	r1, r1, #1
	str	r1, [fp, #-68]
	cmp	r1, #0
	ble	.L117
	ldr	lr, [fp, #-64]
	lsl	ip, r10, #1
	ldr	r3, [r0, #16]
	movw	r9, #32767
	add	r1, lr, #1
	str	r1, [fp, #-84]
	mov	r4, lr
	ldr	lr, [fp, #-80]
	mla	r7, r10, r1, r10
	str	ip, [fp, #-52]
	mla	r4, ip, r4, lr
	str	r10, [fp, #-64]
	mla	r1, ip, r1, lr
	add	r7, lr, r7, lsl #1
	str	r4, [fp, #-56]
	str	r1, [fp, #-60]
.L125:
	cmp	r3, r2
	mov	r6, r3
	ble	.L129
	ldr	r1, [fp, #-48]
	ldr	ip, [fp, #-52]
	ldr	lr, [fp, #-72]
	mla	r10, ip, r1, lr
.L132:
	ldr	r1, [fp, #-64]
	cmp	r1, #0
	ble	.L130
	mov	r5, r10
	ldr	r4, [fp, #-56]
	ldr	ip, [fp, #-60]
	b	.L128
.L195:
	ldr	r2, [r0, #20]
	ldr	r6, [r0, #16]
.L128:
	ldr	r1, [r0, #32]
	ldrsh	lr, [ip], #2
	mul	r3, r2, r1
	.syntax divided
	smull r8,r3,lr,r3
	.arm
	.syntax unified
	sub	r2, r6, r2
	ldrsh	lr, [r4], #2
	mul	r2, r1, r2
	.syntax divided
	smull r1,r2,lr,r2
	.arm
	.syntax unified
	add	r3, r3, r2
	lsl	r3, r3, #2
	asr	r2, r3, #31
	cmp	r2, r3, asr #15
	eorne	r3, r2, r9
	cmp	r7, ip
	strh	r3, [r5], #2
	bne	.L195
	ldr	r3, [r0, #16]
	ldr	r2, [r0, #20]
	mov	r6, r3
.L130:
	ldr	r1, [r0, #8]
	ldr	ip, [fp, #-48]
	add	r2, r2, r1
	str	r2, [r0, #20]
	add	ip, ip, #1
	cmp	r2, r3
	str	ip, [fp, #-48]
	ldr	ip, [fp, #-52]
	add	r10, r10, ip
	blt	.L132
.L129:
	ldr	r1, [fp, #-68]
	sub	r2, r2, r3
	ldr	ip, [fp, #-60]
	subs	r1, r1, #1
	str	r1, [fp, #-68]
	ldr	r1, [fp, #-52]
	str	r2, [r0, #20]
	add	ip, ip, r1
	str	ip, [fp, #-60]
	ldr	ip, [fp, #-56]
	add	r7, r7, r1
	add	r1, ip, r1
	str	r1, [fp, #-56]
	bne	.L125
	ldr	r3, [fp, #-76]
	ldr	r2, [fp, #-84]
	sub	r3, r3, #2
	ldr	r10, [fp, #-64]
	add	r3, r3, r2
	str	r3, [fp, #-64]
.L117:
	cmp	r10, #0
	ble	.L101
	ldr	r3, [fp, #-64]
	add	r0, r0, #32
	ldr	r2, [fp, #-80]
	mul	r3, r10, r3
	add	r1, r10, r3
	add	r3, r2, r3, lsl #1
	add	r1, r2, r1, lsl #1
.L133:
	ldrsh	r2, [r3], #2
	cmp	r1, r3
	str	r2, [r0, #4]!
	bne	.L133
	ldr	r0, [fp, #-48]
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L193:
	ldr	ip, [fp, #-76]
	cmp	ip, #0
	ble	.L101
	ldr	r3, [fp, #-80]
	lsl	lr, r10, #1
	ldr	r2, [fp, #-72]
	rsb	r5, lr, #0
	add	r0, r3, lr
	sub	r4, r2, r3
	ldr	r3, [fp, #-48]
	sub	r4, r4, lr
	mla	r4, lr, r3, r4
.L112:
	cmp	r10, #0
	addgt	r3, r5, r0
	addgt	r2, r4, r0
	ble	.L116
.L113:
	ldrsh	r1, [r3], #2
	cmp	r3, r0
	strh	r1, [r2], #2
	bne	.L113
.L116:
	subs	ip, ip, #1
	add	r0, r0, lr
	bne	.L112
	b	.L114
.L135:
	cmp	r10, #0
	ble	.L138
	ldr	r3, [fp, #-72]
	add	r1, r0, #32
	sub	r2, r3, #4
	mov	r3, #0
.L139:
	add	r3, r3, #1
	ldr	ip, [r1, #4]!
	cmp	r3, r10
	str	ip, [r2, #4]!
	bne	.L139
	ldr	ip, [r0, #8]
	ldr	r3, [r0, #16]
.L138:
	mov	r2, #0
	str	r2, [r0, #24]
	mov	r2, #1
	str	r2, [fp, #-48]
	b	.L134
.L192:
	cmp	r1, r3
	beq	.L104
	cmp	r10, #0
	ble	.L106
	ldr	r2, [fp, #-80]
	add	r1, r0, #32
	add	lr, r2, r10, lsl #1
.L109:
	ldrsh	ip, [r2], #2
	cmp	lr, r2
	str	ip, [r1, #4]!
	bne	.L109
.L106:
	ldr	r1, [fp, #-76]
	mov	r2, #0
	str	r2, [r0, #24]
	sub	r1, r1, #1
	str	r2, [fp, #-48]
	str	r1, [fp, #-76]
	mov	r2, #1
	str	r2, [fp, #-64]
	b	.L110
.L104:
	cmp	r10, #0
	ble	.L107
	ldr	r3, [fp, #-72]
	add	r2, r0, #32
	add	ip, r3, r10, lsl #1
.L108:
	ldr	r1, [r2, #4]!
	strh	r1, [r3], #2
	cmp	ip, r3
	bne	.L108
	ldr	r1, [r0, #8]
	ldr	r3, [r0, #16]
.L107:
	mov	r2, #0
	str	r2, [r0, #24]
	mov	r2, #1
	str	r2, [fp, #-48]
	b	.L103
	UNWIND(.fnend)
	.size	lsrc_process_frame, .-lsrc_process_frame
	.section	.rodata
	.align	2
	.set	.LANCHOR0,. + 0
	.type	g_sample_rate_tab, %object
	.size	g_sample_rate_tab, 52
g_sample_rate_tab:
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
	.type	g_ivt_sf_tab, %object
	.size	g_ivt_sf_tab, 52
g_ivt_sf_tab:
	.word	134217
	.word	97391
	.word	89478
	.word	67109
	.word	48696
	.word	44739
	.word	33554
	.word	24348
	.word	22369
	.word	12174
	.word	11185
	.word	6087
	.word	5592
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
