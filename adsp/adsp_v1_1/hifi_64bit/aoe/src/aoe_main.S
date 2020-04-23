#include "hi_asm_define.h"
	.arch armv8-a
	.file	"aoe_main.c"
	.text
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aoe_deinit_wk_buf, %function
aoe_deinit_wk_buf:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	adrp	x19, .LANCHOR0
	add	x19, x19, :lo12:.LANCHOR0
	mov	x0, x30
	bl	_mcount
	mov	w0, 70
	ldr	x1, [x19, 16]
	bl	hi_adsp_vfree_aligned
	mov	x2, 24
	mov	w1, 0
	mov	x0, x19
	bl	adsp_memset
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_deinit_wk_buf, .-aoe_deinit_wk_buf
	.global	_mcount
	.align	2
	.p2align 3,,7
	.type	aoe_proc_cmd, %function
aoe_proc_cmd:
	stp	x29, x30, [sp, -128]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	adrp	x22, .LANCHOR0
	mov	x0, x30
	bl	_mcount
	.p2align 3
.L4:
	add	x19, x22, :lo12:.LANCHOR0
	mov	w21, 16
.L9:
	ldr	w1, [x19]
	ldr	w0, [x19, 8]
	add	w3, w1, 1
	cmp	w1, w0
	blt	.L10
	mov	w3, 1
	mov	w1, 0
.L10:
	mov	x20, x19
	ldr	w2, [x19, 4]
	ldp	x23, x0, [x19, 16]
	str	w3, [x20], 24
	mul	w1, w1, w2
	ldr	x2, [x20, 8]
	add	x23, x23, x1
	ldr	w0, [x0]
	ldr	w1, [x2]
	sub	w0, w0, w1
	cmp	w0, 15
	bhi	.L34
.L3:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x29, x30, [sp], 128
	ret
	.p2align 2
.L34:
	ldr	w24, [x20, 16]
	add	x0, x29, 104
	ldr	x2, [x20, 24]
	and	w1, w1, w24
	add	w24, w24, 1
	sub	w24, w24, w1
	cmp	w24, 16
	add	x2, x2, x1, uxtw
	csel	w26, w24, w21, ls
	uxtw	x25, w26
	mov	x3, x25
	mov	x1, x25
	bl	memcpy_s
	cbnz	w0, .L3
	str	x27, [x29, 80]
	cmp	w24, 15
	bls	.L12
.L14:
	add	x24, x19, 24
	ldr	x0, [x19, 24]
	ldr	x3, [x24, 8]
	ldr	w1, [x29, 116]
	ldr	w2, [x0]
	ldr	w0, [x3]
	add	x1, x1, 16
	sub	w2, w2, w0
	cmp	x2, x1
	bcc	.L32
	add	w0, w0, 16
	str	w0, [x3]
	ldr	w20, [x29, 116]
	cmp	w20, 61440
	bhi	.L35
	ldr	x0, [x19, 24]
	mov	w3, 0
	ldr	x2, [x24, 8]
	ldr	w0, [x0]
	ldr	w1, [x2]
	sub	w0, w0, w1
	cmp	w20, w0
	bls	.L36
.L17:
	cmp	w3, w20
	bne	.L32
.L37:
	ldr	w0, [x29, 108]
	str	x23, [x29, 120]
	cmp	w0, 1
	beq	.L7
	cbz	w0, .L5
	cmp	w0, 2
	beq	.L8
	ldr	x27, [x29, 80]
	b	.L9
	.p2align 2
.L36:
	cbz	x23, .L24
	ldr	w25, [x24, 16]
	mov	x0, x23
	ldr	x2, [x24, 24]
	and	w1, w1, w25
	add	w25, w25, 1
	sub	w25, w25, w1
	cmp	w20, w25
	add	x2, x2, x1, uxtw
	csel	w27, w20, w25, ls
	uxtw	x26, w27
	mov	x3, x26
	mov	x1, x26
	bl	memcpy_s
	cbnz	w0, .L23
	cmp	w20, w25
	bhi	.L22
.L33:
	ldr	x2, [x24, 8]
	ldr	w1, [x2]
.L24:
	add	w1, w20, w1
	str	w1, [x2]
	mov	w3, w20
	ldr	w20, [x29, 116]
	cmp	w3, w20
	beq	.L37
	.p2align 3
.L32:
	ldr	x27, [x29, 80]
.L39:
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x29, x30, [sp], 128
	ret
	.p2align 2
.L8:
	add	x0, x29, 104
	bl	aop_proc_cmd
	ldr	x27, [x29, 80]
	b	.L4
	.p2align 2
.L5:
	add	x0, x29, 104
	bl	aip_proc_cmd
	ldr	x27, [x29, 80]
	b	.L4
	.p2align 2
.L7:
	add	x0, x29, 104
	bl	engine_proc_cmd
	ldr	x27, [x29, 80]
	b	.L4
	.p2align 2
.L22:
	ldr	x2, [x24, 24]
	sub	w3, w20, w27
	mov	x1, x3
	add	x0, x23, x26
	bl	memcpy_s
	cbz	w0, .L33
	.p2align 3
.L23:
	cbz	w20, .L38
	mov	w3, 0
	ldr	w20, [x29, 116]
	b	.L17
	.p2align 2
.L12:
	ldr	x2, [x20, 24]
	sub	w3, w21, w26
	add	x0, x29, 104
	mov	x1, x3
	add	x0, x0, x25
	bl	memcpy_s
	cbz	w0, .L14
	ldr	x27, [x29, 80]
	b	.L39
	.p2align 2
.L35:
	ldr	x1, [x24, 8]
	ldr	w0, [x1]
	add	w20, w0, w20
	str	w20, [x1]
	ldr	x27, [x29, 80]
	ldp	x19, x20, [sp, 16]
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x29, x30, [sp], 128
	ret
.L38:
	ldr	x2, [x19, 32]
	ldr	w1, [x2]
	b	.L24
	.size	aoe_proc_cmd, .-aoe_proc_cmd
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_reset
	.type	aoe_reset, %function
aoe_reset:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0+56
	ldp	x29, x30, [sp], 16
	str	wzr, [x0, #:lo12:.LANCHOR0+56]
	ret
	.size	aoe_reset, .-aoe_reset
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_init
	.type	aoe_init, %function
aoe_init:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	str	x21, [sp, 32]
	adrp	x19, .LANCHOR0
	add	x20, x19, :lo12:.LANCHOR0
	mov	x21, x0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x20, 56]
	cbz	w0, .L48
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L48:
	add	x0, x21, 832
	str	x21, [x20, 64]
	bl	aip_init
	add	x0, x21, 1600
	bl	aop_init
	add	x0, x21, 64
	bl	engine_init
	mov	x2, 24
	mov	w1, 0
	mov	x0, x20
	bl	adsp_memset
	mov	w2, 32
	mov	w1, 122880
	mov	w0, 70
	bl	hi_adsp_vmalloc_aligned
	cbz	x0, .L43
	mov	x1, 61440
	str	x0, [x20, 16]
	movk	x1, 0x2, lsl 32
	str	x1, [x20, 4]
.L43:
	add	x19, x19, :lo12:.LANCHOR0
	mov	x0, 0
	ldr	x2, [x19, 64]
	add	x1, x2, 16
	add	x3, x2, 28
	ldr	w4, [x2, 16]
	add	x5, x1, 16
	ldr	w2, [x2, 24]
	ldr	w1, [x1, 4]
	bfi	x0, x4, 0, 32
	cmp	w2, 1
	bfi	x0, x1, 32, 32
	bls	.L44
	sub	w4, w2, #1
	tst	w2, w4
	ccmp	x3, 0, 4, eq
	beq	.L44
	cmp	x5, 0
	ccmp	x0, 0, 4, ne
	beq	.L44
	mov	w2, 1
	stp	x3, x5, [x19, 24]
	str	w4, [x19, 40]
	str	x0, [x19, 48]
	str	w2, [x19, 56]
	ldp	x19, x20, [sp, 16]
	ldr	x21, [sp, 32]
	ldp	x29, x30, [sp], 48
	ret
	.p2align 2
.L44:
	bl	aoe_deinit_wk_buf
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	aoe_init, .-aoe_init
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_deinit
	.type	aoe_deinit, %function
aoe_deinit:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	adrp	x19, .LANCHOR0
	add	x19, x19, :lo12:.LANCHOR0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 56]
	cbz	w0, .L49
	bl	aip_deinit
	bl	aop_deinit
	bl	engine_deinit
	bl	aoe_deinit_wk_buf
	str	wzr, [x19, 56]
	str	xzr, [x19, 64]
.L49:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_deinit, .-aoe_deinit
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_flush_wk_buf
	.type	aoe_flush_wk_buf, %function
aoe_flush_wk_buf:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	ldp	x29, x30, [sp], 16
	str	wzr, [x0, #:lo12:.LANCHOR0]
	ret
	.size	aoe_flush_wk_buf, .-aoe_flush_wk_buf
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_get_wk_buf
	.type	aoe_get_wk_buf, %function
aoe_get_wk_buf:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x2, x0, :lo12:.LANCHOR0
	ldr	w1, [x0, #:lo12:.LANCHOR0]
	ldr	w2, [x2, 8]
	add	w3, w1, 1
	cmp	w1, w2
	blt	.L56
	mov	w3, 1
	mov	w1, 0
.L56:
	add	x2, x0, :lo12:.LANCHOR0
	str	w3, [x0, #:lo12:.LANCHOR0]
	ldp	x29, x30, [sp], 16
	ldr	w3, [x2, 4]
	ldr	x0, [x2, 16]
	mul	w1, w1, w3
	add	x0, x0, x1
	ret
	.size	aoe_get_wk_buf, .-aoe_get_wk_buf
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_release_wk_buf
	.type	aoe_release_wk_buf, %function
aoe_release_wk_buf:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x2, x0, :lo12:.LANCHOR0
	ldr	w1, [x0, #:lo12:.LANCHOR0]
	cbnz	w1, .L61
	ldr	w1, [x2, 8]
.L61:
	sub	w1, w1, #1
	str	w1, [x0, #:lo12:.LANCHOR0]
	ldp	x29, x30, [sp], 16
	ret
	.size	aoe_release_wk_buf, .-aoe_release_wk_buf
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_get_wk_buf_size
	.type	aoe_get_wk_buf_size, %function
aoe_get_wk_buf_size:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0+4
	ldp	x29, x30, [sp], 16
	ldr	w0, [x0, #:lo12:.LANCHOR0+4]
	ret
	.size	aoe_get_wk_buf_size, .-aoe_get_wk_buf_size
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_sched_statistics_init
	.type	aoe_sched_statistics_init, %function
aoe_sched_statistics_init:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	w1, [x0, 56]
	cbz	w1, .L63
	ldr	x0, [x0, 64]
	str	wzr, [x0, 12]
.L63:
	ldp	x29, x30, [sp], 16
	ret
	.size	aoe_sched_statistics_init, .-aoe_sched_statistics_init
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_sched_statistics_show
	.type	aoe_sched_statistics_show, %function
aoe_sched_statistics_show:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	stp	x19, x20, [sp, 16]
	and	w20, w1, 65535
	mov	w19, w0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0
	add	x0, x0, :lo12:.LANCHOR0
	ldr	w1, [x0, 56]
	cbz	w1, .L68
	cbz	w19, .L74
	cmp	w19, 1
	beq	.L75
.L68:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L74:
	ldr	x0, [x0, 64]
	ldr	w1, [x0, 12]
	bfi	w1, w20, 0, 16
	str	w1, [x0, 12]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L75:
	ldr	x0, [x0, 64]
	ldr	w1, [x0, 12]
	bfi	w1, w20, 16, 16
	str	w1, [x0, 12]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_sched_statistics_show, .-aoe_sched_statistics_show
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_proc_thread_sw
	.type	aoe_proc_thread_sw, %function
aoe_proc_thread_sw:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	adrp	x19, .LANCHOR0
	add	x19, x19, :lo12:.LANCHOR0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 56]
	str	wzr, [x19, 72]
	cbnz	w0, .L80
.L76:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L80:
	bl	aoe_proc_cmd
	bl	aip_process
	bl	aop_process
	bl	engine_process
	ldr	w0, [x19, 76]
	mov	w1, 1
	add	w0, w0, w1
	stp	w1, w0, [x19, 72]
	tst	x0, 3
	bne	.L76
	str	wzr, [x19, 72]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_proc_thread_sw, .-aoe_proc_thread_sw
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_proc_thread
	.type	aoe_proc_thread, %function
aoe_proc_thread:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	adrp	x19, .LANCHOR0
	add	x19, x19, :lo12:.LANCHOR0
	mov	x0, x30
	bl	_mcount
	ldr	w0, [x19, 56]
	str	wzr, [x19, 72]
	cbnz	w0, .L85
.L81:
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.p2align 2
.L85:
	bl	aoe_proc_cmd
	bl	aip_process
	bl	aop_process
	bl	engine_process
	ldr	w0, [x19, 76]
	mov	w1, 1
	add	w0, w0, w1
	stp	w1, w0, [x19, 72]
	tbnz	x0, 0, .L81
	str	wzr, [x19, 72]
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 32
	ret
	.size	aoe_proc_thread, .-aoe_proc_thread
	.global	_mcount
	.align	2
	.p2align 3,,7
	.global	aoe_get_thread_status
	.type	aoe_get_thread_status, %function
aoe_get_thread_status:
	stp	x29, x30, [sp, -16]!
	add	x29, sp, 0
	mov	x0, x30
	bl	_mcount
	adrp	x0, .LANCHOR0+72
	ldp	x29, x30, [sp], 16
	ldr	w0, [x0, #:lo12:.LANCHOR0+72]
	ret
	.size	aoe_get_thread_status, .-aoe_get_thread_status
	.bss
	.align	3
	.set	.LANCHOR0,. + 0
	.type	g_wkbuf, %object
	.size	g_wkbuf, 24
g_wkbuf:
	.zero	24
	.type	g_aoe_cmd_fifo, %object
	.size	g_aoe_cmd_fifo, 32
g_aoe_cmd_fifo:
	.zero	32
	.type	g_aoe_init_flag, %object
	.size	g_aoe_init_flag, 4
g_aoe_init_flag:
	.zero	4
	.zero	4
	.type	g_aoe_com_reg, %object
	.size	g_aoe_com_reg, 8
g_aoe_com_reg:
	.zero	8
	.type	g_aoe_task_status, %object
	.size	g_aoe_task_status, 4
g_aoe_task_status:
	.zero	4
	.type	g_aoe_task_ticks, %object
	.size	g_aoe_task_ticks, 4
g_aoe_task_ticks:
	.zero	4
	.ident	"GCC: (gcc-linaro-7.4-2019.02 glibc-2.30 czyong Fri Nov 15 09:16:55 CST 2019) 7.4.1 20181213"
	.section	.note.GNU-stack,"",@progbits
