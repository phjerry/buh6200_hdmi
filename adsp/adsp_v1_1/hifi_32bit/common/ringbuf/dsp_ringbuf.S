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
	.file	"dsp_ringbuf.c"
	.text
	.align	2
	.global	rbuf_create
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_create, %function
rbuf_create:
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
	mov	r6, r0
	mov	r5, r1
	mov	r0, #70
	mov	r1, #52
	bl	HI_ADSP_MALLOC
	subs	r4, r0, #0
	beq	.L7
	mov	r2, #52
	mov	r1, #0
	bl	adsp_memset
	mov	r2, #36
	mov	r1, r5
	mov	r0, r4
	bl	adsp_memcpy
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.L4
	ldr	r7, [r4, #4]
	ldr	r1, [r4, #8]
.L5:
	ldr	r2, [r4, #12]
	mov	r3, #0
	ldr	r0, [r4, #16]
	str	r7, [r4, #44]
	str	r2, [r4, #36]
	str	r0, [r4, #40]
	str	r1, [r4, #48]
	str	r3, [r2]
	ldr	r2, [r4, #40]
	str	r3, [r2]
.L7:
	str	r4, [r6]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L4:
	ldr	r2, [r5, #24]
	mov	r0, #70
	ldr	r1, [r5, #8]
	bl	hi_adsp_vmalloc_aligned
	subs	r7, r0, #0
	strne	r7, [r4, #4]
	ldrne	r1, [r5, #8]
	strne	r1, [r4, #8]
	bne	.L5
.L9:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	str	r7, [r6]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_create, .-rbuf_create
	.align	2
	.global	rbuf_create_ext
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_create_ext, %function
rbuf_create_ext:
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
	mov	r6, r0
	mov	r5, r1
	mov	r0, #70
	mov	r1, #52
	bl	HI_ADSP_MALLOC
	subs	r4, r0, #0
	beq	.L16
	mov	r2, #52
	mov	r1, #0
	bl	adsp_memset
	mov	r2, #36
	mov	r1, r5
	mov	r0, r4
	bl	adsp_memcpy
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.L13
	ldr	r7, [r4, #4]
	ldr	r3, [r4, #8]
.L14:
	ldr	r1, [r4, #16]
	ldr	r2, [r4, #12]
	str	r7, [r4, #44]
	str	r3, [r4, #48]
	str	r1, [r4, #40]
	str	r2, [r4, #36]
.L16:
	str	r4, [r6]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
.L13:
	ldr	r2, [r5, #24]
	mov	r0, #70
	ldr	r1, [r5, #8]
	bl	hi_adsp_vmalloc_aligned
	subs	r7, r0, #0
	strne	r7, [r4, #4]
	ldrne	r3, [r5, #8]
	strne	r3, [r4, #8]
	bne	.L14
.L17:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	str	r7, [r6]
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_create_ext, .-rbuf_create_ext
	.align	2
	.global	rbuf_destroy
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_destroy, %function
rbuf_destroy:
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
	ldr	r3, [r4]
	cmp	r3, #0
	beq	.L24
.L20:
	mov	r1, r4
	mov	r0, #70
	bl	HI_ADSP_FREE
	ldmfd	sp, {r4, r5, fp, sp, pc}
.L24:
	ldr	r1, [r4, #4]
	mov	r0, #70
	bl	hi_adsp_vfree_aligned
	b	.L20
	UNWIND(.fnend)
	.size	rbuf_destroy, .-rbuf_destroy
	.align	2
	.global	rbuf_read_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_read_data, %function
rbuf_read_data:
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
	subs	r7, r0, #0
	mov	r8, r1
	moveq	r4, r7
	beq	.L25
	ldr	r0, [r7, #40]
	ldr	r1, [r7, #36]
	ldr	r3, [r7, #48]
	ldr	r5, [r0]
	ldr	r1, [r1]
	cmp	r5, r1
	addhi	r1, r1, r3
	sub	r1, r1, r5
	cmp	r2, r1
	movcc	r4, r2
	movcs	r4, r1
	cmp	r8, #0
	beq	.L29
	sub	r3, r3, r5
	ldr	r1, [r7, #44]
	cmp	r3, r4
	mov	r0, r8
	add	r1, r1, r5
	movcc	r6, r3
	movcs	r6, r4
	mov	r2, r6
	bl	adsp_memcpy
	add	r0, r8, r6
	sub	r2, r4, r6
	ldr	r1, [r7, #44]
	bl	adsp_memcpy
	ldr	r3, [r7, #48]
	ldr	r0, [r7, #40]
.L29:
	add	r5, r5, r4
	cmp	r3, r5
	subls	r5, r5, r3
	str	r5, [r0]
.L25:
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_read_data, .-rbuf_read_data
	.align	2
	.global	rbuf_write_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_write_data, %function
rbuf_write_data:
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
	subs	r5, r0, #0
	mov	r4, r2
	mov	r8, r1
	beq	.L37
	ldr	r3, [r5, #36]
	cmp	r2, #0
	ldr	r6, [r3]
	bne	.L47
.L37:
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L47:
	ldr	r7, [r5, #48]
	ldr	r0, [r5, #44]
	sub	r7, r7, r6
	cmp	r7, r2
	add	r0, r0, r6
	movcs	r7, r2
	cmp	r1, #0
	mov	r2, r7
	sub	r9, r4, r7
	beq	.L48
	bl	adsp_memcpy
	mov	r2, r9
	add	r1, r8, r7
	ldr	r0, [r5, #44]
	bl	adsp_memcpy
.L39:
	ldr	r3, [r5, #48]
	add	r6, r4, r6
	ldr	r2, [r5, #36]
	mov	r0, r4
	cmp	r3, r6
	subls	r6, r6, r3
	str	r6, [r2]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L48:
	bl	adsp_memset
	mov	r2, r9
	mov	r1, r8
	ldr	r0, [r5, #44]
	bl	adsp_memset
	b	.L39
	UNWIND(.fnend)
	.size	rbuf_write_data, .-rbuf_write_data
	.align	2
	.global	rbuf_read_data_complete
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_read_data_complete, %function
rbuf_read_data_complete:
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
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r0, #40]
	ldr	r2, [r0, #48]
	ldr	r0, [r3]
	add	r1, r1, r0
	cmp	r2, r1
	subls	r1, r1, r2
	str	r1, [r3]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_read_data_complete, .-rbuf_read_data_complete
	.align	2
	.global	rbuf_write_data_complete
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_write_data_complete, %function
rbuf_write_data_complete:
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
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r3, [r0, #36]
	ldr	r2, [r0, #48]
	ldr	r0, [r3]
	add	r1, r1, r0
	cmp	r2, r1
	subls	r1, r1, r2
	str	r1, [r3]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_write_data_complete, .-rbuf_write_data_complete
	.align	2
	.global	rbuf_query_info
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_query_info, %function
rbuf_query_info:
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
	subs	r2, r0, #0
	mov	r0, r1
	beq	.L67
	ldr	r3, [r2, #28]
	str	r3, [r1, #24]
	ldr	r3, [r2, #44]
	str	r3, [r1]
	ldr	r1, [r2, #48]
	str	r1, [r0, #12]
	ldr	r3, [r2, #36]
	ldr	ip, [r3]
	str	ip, [r0, #8]
	ldr	r3, [r2, #40]
	ldr	r3, [r3]
	cmp	ip, r3
	addcc	ip, r1, ip
	str	r3, [r0, #4]
	sub	r3, ip, r3
	str	r3, [r0, #16]
	sub	r1, r1, r3
	str	r1, [r0, #20]
	ldmfd	sp, {fp, sp, pc}
.L67:
	mov	r1, r2
	mov	r2, #28
	bl	adsp_memset
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_query_info, .-rbuf_query_info
	.align	2
	.global	rbuf_query_busy_provide_rptr
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_query_busy_provide_rptr, %function
rbuf_query_busy_provide_rptr:
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
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	ip, [r0, #36]
	ldr	r3, [r1]
	ldr	r2, [r0, #48]
	ldr	r0, [ip]
	cmp	r0, r3
	subcc	r3, r2, r3
	subcs	r0, r0, r3
	addcc	r0, r3, r0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_query_busy_provide_rptr, .-rbuf_query_busy_provide_rptr
	.align	2
	.global	rbuf_query_busy
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_query_busy, %function
rbuf_query_busy:
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
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r1, [r0, #36]
	ldr	r2, [r0, #40]
	ldr	ip, [r0, #48]
	ldr	r3, [r1]
	ldr	r0, [r2]
	cmp	r3, r0
	addcc	r3, r3, ip
	sub	r0, r3, r0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_query_busy, .-rbuf_query_busy
	.align	2
	.global	rbuf_query_free
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_query_free, %function
rbuf_query_free:
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
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	r2, [r0, #36]
	ldr	r3, [r0, #40]
	ldr	r1, [r0, #24]
	ldr	r2, [r2]
	ldr	r3, [r3]
	ldr	r0, [r0, #48]
	cmp	r2, r3
	addcc	r2, r2, r0
	sub	r3, r2, r3
	sub	r0, r0, r3
	cmp	r1, r0
	subcc	r0, r0, r1
	movcs	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_query_free, .-rbuf_query_free
	.align	2
	.global	rbuf_check_free
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_check_free, %function
rbuf_check_free:
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
	cmp	r0, #0
	ldmfdeq	sp, {fp, sp, pc}
	ldr	ip, [r0, #36]
	ldr	r3, [r0, #40]
	ldr	r2, [r0, #48]
	ldr	ip, [ip]
	ldr	r3, [r3]
	cmp	ip, r3
	addcc	ip, ip, r2
	sub	r3, ip, r3
	ldr	ip, [r0, #28]
	add	lr, r3, r1
	sub	r0, r2, r3
	cmp	lr, ip
	bhi	.L89
	cmp	r1, r0
	movcs	r0, #0
	movcc	r0, #1
	ldmfd	sp, {fp, sp, pc}
.L89:
	mov	r0, #0
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_check_free, .-rbuf_check_free
	.align	2
	.global	rbuf_flush
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_flush, %function
rbuf_flush:
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
	cmp	r0, #0
	ldrne	r2, [r0, #36]
	ldrne	r3, [r0, #40]
	ldrne	r2, [r2]
	strne	r2, [r3]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_flush, .-rbuf_flush
	.align	2
	.global	rbuf_read_data_dma
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_read_data_dma, %function
rbuf_read_data_dma:
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
	subs	r6, r0, #0
	mov	r8, r1
	moveq	r4, r6
	beq	.L96
	ldr	r1, [r6, #40]
	cmp	r2, #0
	ldr	r3, [r6, #36]
	ldr	r7, [r1]
	ldr	r4, [r3]
	beq	.L105
	ldr	r3, [r6, #48]
	cmp	r7, r4
	addhi	r4, r4, r3
	sub	r4, r4, r7
	cmp	r2, r4
	movcc	r4, r2
	cmp	r8, #0
	beq	.L100
	sub	r3, r3, r7
	ldr	r1, [r6, #44]
	cmp	r3, r4
	movcc	r5, r3
	movcs	r5, r4
	cmp	r5, #0
	bne	.L114
.L101:
	subs	r2, r4, r5
	bne	.L102
.L113:
	ldr	r3, [r6, #48]
	ldr	r1, [r6, #40]
.L100:
	add	r2, r7, r4
	cmp	r2, r3
	subcs	r2, r2, r3
	str	r2, [r1]
.L96:
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L105:
	mov	r4, r2
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L102:
	add	r0, r8, r5
	bl	adsp_memcpy
	b	.L113
.L114:
	add	r1, r1, r7
	mov	r2, r5
	mov	r0, r8
	bl	adsp_memcpy
	ldr	r1, [r6, #44]
	b	.L101
	UNWIND(.fnend)
	.size	rbuf_read_data_dma, .-rbuf_read_data_dma
	.align	2
	.global	rbuf_write_data_dma
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_write_data_dma, %function
rbuf_write_data_dma:
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
	subs	r5, r0, #0
	mov	r4, r2
	mov	r8, r1
	beq	.L116
	ldr	r3, [r5, #36]
	cmp	r2, #0
	ldr	r6, [r3]
	bne	.L141
.L116:
	mov	r0, r4
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L141:
	ldr	r7, [r5, #48]
	ldr	r0, [r5, #44]
	sub	r7, r7, r6
	cmp	r7, r2
	add	r3, r0, r6
	movcs	r7, r2
	cmp	r1, #0
	sub	r9, r2, r7
	beq	.L142
	cmp	r7, #0
	bne	.L143
.L121:
	mov	r2, r9
	add	r1, r8, r7
	bl	adsp_memcpy
.L120:
	ldr	r3, [r5, #48]
	add	r6, r4, r6
	ldr	r2, [r5, #36]
	mov	r0, r4
	cmp	r3, r6
	subls	r6, r6, r3
	str	r6, [r2]
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, fp, sp, pc}
.L142:
	cmp	r7, #0
	bne	.L144
.L118:
	mov	r2, r9
	mov	r1, #0
	bl	adsp_memset
	b	.L120
.L144:
	mov	r2, r7
	mov	r0, r3
	bl	adsp_memset
	cmp	r9, #0
	beq	.L120
	ldr	r0, [r5, #44]
	b	.L118
.L143:
	mov	r0, r3
	mov	r2, r7
	bl	adsp_memcpy
	cmp	r9, #0
	beq	.L120
	ldr	r0, [r5, #44]
	b	.L121
	UNWIND(.fnend)
	.size	rbuf_write_data_dma, .-rbuf_write_data_dma
	.align	2
	.global	rbuf_copy_data
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_copy_data, %function
rbuf_copy_data:
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
	clz	r3, r1
	cmp	r0, #0
	mov	r5, r2
	mov	r4, r0
	lsr	r3, r3, #5
	moveq	r3, #1
	cmp	r3, #0
	mov	r6, r1
	beq	.L200
.L169:
	mov	r0, #0
.L145:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L200:
	cmn	r0, #36
	moveq	r2, #1
	movne	r2, #0
	cmn	r1, #36
	movne	r1, #0
	moveq	r1, #1
	cmp	r5, #0
	moveq	r2, #1
	orrs	r2, r1, r2
	bne	.L169
	ldr	r3, [r0, #36]
	ldr	r1, [r0, #40]
	ldr	r7, [r0, #48]
	ldr	r2, [r3]
	ldr	r3, [r1]
	cmp	r2, r3
	addcc	r2, r2, r7
	sub	r3, r2, r3
	cmp	r5, r3
	bhi	.L169
	ldr	r2, [r6, #36]
	ldr	r3, [r6, #40]
	ldr	r0, [r6, #48]
	ldr	ip, [r2]
	ldr	r3, [r3]
	cmp	ip, r3
	addcc	ip, ip, r0
	sub	r3, ip, r3
	sub	r3, r0, r3
	cmp	r5, r3
	bcs	.L169
	ldr	r8, [r1]
	ldr	r9, [r4, #44]
	sub	r7, r7, r8
	ldr	r3, [r2]
	cmp	r7, r5
	movcs	r7, r5
	cmp	r7, #0
	moveq	r0, r7
	bne	.L201
.L151:
	ldr	r2, [r2]
	subs	r3, r5, r7
	str	r2, [fp, #-48]
	bne	.L202
.L158:
	ldr	r3, [r4, #48]
	add	r5, r5, r8
	ldr	r2, [r4, #40]
	cmp	r3, r5
	subls	r5, r5, r3
	str	r5, [r2]
	b	.L145
.L201:
	sub	r10, r0, r3
	ldr	r0, [r6, #44]
	cmp	r10, r7
	add	ip, r0, r3
	movcs	r10, r7
	adds	r2, r9, r8
	sub	r9, r7, r10
	str	r2, [fp, #-48]
	beq	.L203
	cmp	r10, #0
	bne	.L204
.L156:
	str	r3, [fp, #-52]
	mov	r2, r9
	ldr	r3, [fp, #-48]
	add	r1, r3, r10
	bl	adsp_memcpy
	ldr	r3, [fp, #-52]
.L155:
	ldr	r2, [r6, #48]
	add	r3, r7, r3
	ldr	r1, [r6, #36]
	mov	r0, r7
	cmp	r2, r3
	subls	r3, r3, r2
	str	r3, [r1]
	ldr	r9, [r4, #44]
	ldr	r2, [r6, #36]
	b	.L151
.L202:
	ldr	r10, [r6, #48]
	ldr	r0, [r6, #44]
	sub	r10, r10, r2
	cmp	r10, r3
	add	ip, r0, r2
	movcs	r10, r3
	cmp	r9, #0
	sub	r2, r3, r10
	str	r2, [fp, #-52]
	beq	.L205
	cmp	r10, #0
	bne	.L206
.L163:
	ldr	r2, [fp, #-52]
	add	r1, r9, r10
	str	r3, [fp, #-56]
	bl	adsp_memcpy
	ldr	r3, [fp, #-56]
.L162:
	ldr	r2, [fp, #-48]
	add	r0, r7, r3
	ldr	r1, [r6, #48]
	ldr	ip, [r6, #36]
	add	r2, r3, r2
	cmp	r1, r2
	subls	r2, r2, r1
	str	r2, [ip]
	b	.L158
.L203:
	cmp	r10, #0
	bne	.L207
.L153:
	mov	r2, r9
	mov	r1, #0
	str	r3, [fp, #-48]
	bl	adsp_memset
	ldr	r3, [fp, #-48]
	b	.L155
.L205:
	cmp	r10, #0
	bne	.L208
.L160:
	ldr	r2, [fp, #-52]
	mov	r1, #0
	str	r3, [fp, #-56]
	bl	adsp_memset
	ldr	r3, [fp, #-56]
	b	.L162
.L204:
	mov	r0, ip
	mov	r2, r10
	ldr	r1, [fp, #-48]
	str	r3, [fp, #-52]
	bl	adsp_memcpy
	cmp	r9, #0
	ldr	r3, [fp, #-52]
	beq	.L155
	ldr	r0, [r6, #44]
	b	.L156
.L206:
	mov	r0, ip
	mov	r2, r10
	mov	r1, r9
	str	r3, [fp, #-56]
	bl	adsp_memcpy
	ldr	r3, [fp, #-52]
	cmp	r3, #0
	ldr	r3, [fp, #-56]
	beq	.L162
	ldr	r0, [r6, #44]
	b	.L163
.L208:
	mov	r2, r10
	mov	r1, r9
	mov	r0, ip
	str	r3, [fp, #-56]
	bl	adsp_memset
	ldr	r3, [fp, #-52]
	cmp	r3, #0
	ldr	r3, [fp, #-56]
	beq	.L162
	ldr	r0, [r6, #44]
	b	.L160
.L207:
	mov	r2, r10
	ldr	r1, [fp, #-48]
	mov	r0, ip
	str	r3, [fp, #-52]
	bl	adsp_memset
	cmp	r9, #0
	ldr	r3, [fp, #-52]
	beq	.L155
	ldr	r0, [r6, #44]
	b	.L153
	UNWIND(.fnend)
	.size	rbuf_copy_data, .-rbuf_copy_data
	.align	2
	.global	rbuf_update_attr
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_update_attr, %function
rbuf_update_attr:
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
	cmp	r0, #0
	ldrne	r3, [r1, #32]
	strne	r3, [r0, #32]
	ldrne	r3, [r1, #28]
	strne	r3, [r0, #28]
	ldmfd	sp, {fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_update_attr, .-rbuf_update_attr
	.align	2
	.global	rbuf_read_data_with_action
	.syntax unified
	.arm
	.fpu vfpv3-d16
	.type	rbuf_read_data_with_action, %function
rbuf_read_data_with_action:
	UNWIND(.fnstart)


	UNWIND(.movsp ip)
	mov	ip, sp
	push	{r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr, pc}
	UNWIND(.pad #4)
	UNWIND(.save {r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr})
	UNWIND(.setfp fp, ip, #-4)
	sub	fp, ip, #4
	push	{lr}
	bl	__gnu_mcount_nc
	subs	r4, r0, #0
	mov	r7, r1
	mov	r8, r2
	mov	r5, r3
	beq	.L220
	ldr	r2, [r4, #40]
	ldr	r3, [r4, #36]
	ldr	r6, [r4, #48]
	ldr	r9, [r2]
	ldr	r3, [r3]
	cmp	r9, r3
	addhi	r3, r3, r6
	sub	r3, r3, r9
	cmp	r5, r3
	bhi	.L220
	sub	r6, r6, r9
	ldr	r1, [r4, #44]
	cmp	r6, r5
	mov	r0, r8
	add	r1, r1, r9
	movcs	r6, r5
	mov	r2, r6
	blx	r7
	sub	r2, r5, r6
	ldr	r1, [r4, #44]
	add	r5, r5, r9
	mov	r10, r0
	add	r0, r8, r6
	blx	r7
	ldr	r3, [r4, #48]
	ldr	r2, [r4, #40]
	cmp	r3, r5
	subls	r5, r5, r3
	str	r5, [r2]
	add	r0, r10, r0
	ldmfd	sp, {r3, r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
.L220:
	mov	r0, #0
	ldmfd	sp, {r3, r4, r5, r6, r7, r8, r9, r10, fp, sp, pc}
	UNWIND(.fnend)
	.size	rbuf_read_data_with_action, .-rbuf_read_data_with_action
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Thu Nov 14 21:11:51 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",%progbits
