	.arch armv7
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global main
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	main, %function
main:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#4
	bl	getint
	push	{r0}
	add	r1,sp,#4
	str	r0,[r1]
	bl	getch
	push	{r0}
	bl	next_token
	add	sp,sp,#8
.29:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#0
	beq	.30
	bl	eval
	push	{r0}
	bl	putint
	movw	r0,#10
	bl	putch
	add	r0,sp,#4
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#4
	str	r1,[r0]
	add	sp,sp,#4
	b	.29
.30:
	movw	r0,#0
	add	sp,sp,#4
	nop
	pop	{fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global eval
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	eval, %function
eval:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	movw	r0,#2076
	sub	sp,sp,r0
	movw	r0,#1052
	add	r0,sp,r0
	movw	r1,#0
	movw	r2,#1024
	bl	memset
	add	r0,sp,#28
	movw	r1,#0
	movw	r2,#1024
	bl	memset
	movw	r0,#:lower16:cur_token
	movt	r0,#:upper16:cur_token
	ldr	r0,[r0]
	cmp	r0,#0
	beq	.20
	bl	panic
	movw	r1,#2076
	add	sp,sp,r1
	nop
	pop	{fp,pc}
.20:
	movw	r0,#1052
	add	r0,sp,r0
	movw	r1,#:lower16:num
	movt	r1,#:upper16:num
	ldr	r1,[r1]
	bl	stack_push
	bl	next_token
.21:
	movw	r0,#:lower16:cur_token
	movt	r0,#:upper16:cur_token
	ldr	r0,[r0]
	cmp	r0,#1
	bne	.26
	movw	r0,#:lower16:other
	movt	r0,#:upper16:other
	ldr	r0,[r0]
	add	r1,sp,#12
	str	r0,[r1]
	bl	get_op_prec
	cmp	r0,#0
	push	{r0}
	ite eq
	moveq	r0,#1
	movne	r0,#0
	add	sp,sp,#4
	bne	.22
	b	.26
.22:
	bl	next_token
.23:
	add	r0,sp,#28
	bl	stack_size
	cmp	r0,#0
	beq	.24
	add	r0,sp,#28
	bl	stack_peek
	push	{r0}
	bl	get_op_prec
	push	{r0}
	add	r0,sp,#20
	ldr	r0,[r0]
	bl	get_op_prec
	push	{r0}
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	cmp	r0,r1
	add	sp,sp,#12
	blt	.24
	add	r0,sp,#28
	bl	stack_pop
	push	{r0}
	add	r0,sp,#1056
	bl	stack_pop
	push	{r0}
	movw	r0,#1060
	add	r0,sp,r0
	bl	stack_pop
	push	{r0}
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	add	r2,sp,#4
	ldr	r2,[r2]
	bl	eval_op
	push	{r0}
	movw	r0,#1068
	add	r0,sp,r0
	add	r1,sp,#0
	ldr	r1,[r1]
	bl	stack_push
	add	sp,sp,#16
	b	.23
.24:
	add	r0,sp,#28
	add	r1,sp,#12
	ldr	r1,[r1]
	bl	stack_push
	movw	r0,#:lower16:cur_token
	movt	r0,#:upper16:cur_token
	ldr	r0,[r0]
	cmp	r0,#0
	beq	.25
	bl	panic
	movw	r1,#2076
	add	sp,sp,r1
	nop
	pop	{fp,pc}
.25:
	movw	r0,#1052
	add	r0,sp,r0
	movw	r1,#:lower16:num
	movt	r1,#:upper16:num
	ldr	r1,[r1]
	bl	stack_push
	bl	next_token
	b	.21
.26:
	bl	next_token
.27:
	add	r0,sp,#28
	bl	stack_size
	cmp	r0,#0
	beq	.28
	add	r0,sp,#28
	bl	stack_pop
	push	{r0}
	add	r0,sp,#1056
	bl	stack_pop
	push	{r0}
	movw	r0,#1060
	add	r0,sp,r0
	bl	stack_pop
	push	{r0}
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	add	r2,sp,#4
	ldr	r2,[r2]
	bl	eval_op
	push	{r0}
	movw	r0,#1068
	add	r0,sp,r0
	add	r1,sp,#0
	ldr	r1,[r1]
	bl	stack_push
	add	sp,sp,#16
	b	.27
.28:
	movw	r0,#1052
	add	r0,sp,r0
	bl	stack_peek
	movw	r1,#2076
	add	sp,sp,r1
	nop
	pop	{fp,pc}
	.size	eval, .-eval
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global next_char
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	next_char, %function
next_char:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	bl	getch
	movw	r1,#:lower16:last_char
	movt	r1,#:upper16:last_char
	str	r0,[r1]
	nop
	pop	{fp,pc}
	.size	next_char, .-next_char
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global is_space
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	is_space, %function
is_space:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	cmp	r0,#32
	beq	.0
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#10
	beq	.0
	b	.1
.0:
	movw	r0,#1
	add	sp,sp,#4
	nop
	pop	{fp,pc}
.1:
	movw	r0,#0
	add	sp,sp,#4
	nop
	pop	{fp,pc}
.2:
	.size	is_space, .-is_space
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global get_op_prec
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	get_op_prec, %function
get_op_prec:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	cmp	r0,#43
	beq	.11
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#45
	beq	.11
	b	.12
.11:
	movw	r0,#10
	add	sp,sp,#4
	nop
	pop	{fp,pc}
.12:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#42
	beq	.13
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#47
	beq	.13
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#37
	beq	.13
	b	.14
.13:
	movw	r0,#20
	add	sp,sp,#4
	nop
	pop	{fp,pc}
.14:
	movw	r0,#0
	add	sp,sp,#4
	nop
	pop	{fp,pc}
	.size	get_op_prec, .-get_op_prec
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global stack_push
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	stack_push, %function
stack_push:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1}
	add	r0,sp,#0
	ldr	r0,[r0]
	ldr	r0,[r0,#0]
	add	r1,r0,#1
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r1,[r2,#0]
	str	r0,[r2,r1,lsl 2]
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	.size	stack_push, .-stack_push
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global stack_pop
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	stack_pop, %function
stack_pop:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	sub	sp,sp,#4
	add	r0,sp,#4
	ldr	r0,[r0]
	ldr	r0,[r0,#0]
	push	{r0}
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	add	r1,sp,#4
	str	r0,[r1]
	add	r0,sp,#8
	ldr	r0,[r0]
	ldr	r0,[r0,#0]
	sub	r2,r0,#1
	add	r0,sp,#8
	ldr	r0,[r0]
	str	r2,[r0,#0]
	ldr	r0,[r1]
	add	sp,sp,#12
	nop
	pop	{fp,pc}
	add	sp,sp,#4
	.size	stack_pop, .-stack_pop
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global eval_op
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	eval_op, %function
eval_op:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1,r2}
	cmp	r0,#43
	bne	.15
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	add	r2,r0,r1
	mov	r0,r2
	add	sp,sp,#12
	nop
	pop	{fp,pc}
.15:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#45
	bne	.16
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	sub	r2,r0,r1
	mov	r0,r2
	add	sp,sp,#12
	nop
	pop	{fp,pc}
.16:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#42
	bne	.17
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	mul	r2,r0,r1
	mov	r0,r2
	add	sp,sp,#12
	nop
	pop	{fp,pc}
.17:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#47
	bne	.18
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	bl	__aeabi_idiv
	add	sp,sp,#12
	nop
	pop	{fp,pc}
.18:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#37
	bne	.19
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	bl	__aeabi_idivmod
	mov	r0,r1
	add	sp,sp,#12
	nop
	pop	{fp,pc}
.19:
	movw	r0,#0
	add	sp,sp,#12
	nop
	pop	{fp,pc}
	.size	eval_op, .-eval_op
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global stack_peek
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	stack_peek, %function
stack_peek:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	add	r0,sp,#0
	ldr	r0,[r0]
	ldr	r0,[r0,#0]
	push	{r0}
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	add	sp,sp,#4
	.size	stack_peek, .-stack_peek
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global stack_size
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	stack_size, %function
stack_size:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	add	r0,sp,#0
	ldr	r0,[r0]
	ldr	r0,[r0,#0]
	add	sp,sp,#4
	nop
	pop	{fp,pc}
	.size	stack_size, .-stack_size
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global is_num
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	is_num, %function
is_num:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	cmp	r0,#48
	blt	.3
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#57
	bgt	.3
	movw	r0,#1
	add	sp,sp,#4
	nop
	pop	{fp,pc}
.3:
	movw	r0,#0
	add	sp,sp,#4
	nop
	pop	{fp,pc}
.4:
	.size	is_num, .-is_num
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global next_token
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	next_token, %function
next_token:
	push	{fp,lr}
	nop
	add	fp,sp,#8
.5:
	movw	r0,#:lower16:last_char
	movt	r0,#:upper16:last_char
	ldr	r0,[r0]
	bl	is_space
	cmp	r0,#0
	beq	.6
	bl	next_char
	b	.5
.6:
	movw	r0,#:lower16:last_char
	movt	r0,#:upper16:last_char
	ldr	r0,[r0]
	bl	is_num
	cmp	r0,#0
	beq	.9
	movw	r0,#:lower16:last_char
	movt	r0,#:upper16:last_char
	ldr	r0,[r0]
	sub	r1,r0,#48
	movw	r0,#:lower16:num
	movt	r0,#:upper16:num
	str	r1,[r0]
.7:
	bl	next_char
	push	{r0}
	bl	is_num
	cmp	r0,#0
	add	sp,sp,#4
	beq	.8
	movw	r0,#:lower16:num
	movt	r0,#:upper16:num
	ldr	r0,[r0]
	movw	r1,#10
	mul	r2,r0,r1
	movw	r0,#:lower16:last_char
	movt	r0,#:upper16:last_char
	ldr	r0,[r0]
	add	r1,r2,r0
	sub	r0,r1,#48
	push	{r1}
	movw	r1,#:lower16:num
	movt	r1,#:upper16:num
	str	r0,[r1]
	add	sp,sp,#4
	b	.7
.8:
	movw	r0,#0
	movw	r1,#:lower16:cur_token
	movt	r1,#:upper16:cur_token
	str	r0,[r1]
	b	.10
.9:
	movw	r0,#:lower16:last_char
	movt	r0,#:upper16:last_char
	ldr	r0,[r0]
	movw	r1,#:lower16:other
	movt	r1,#:upper16:other
	str	r0,[r1]
	bl	next_char
	push	{r0}
	movw	r0,#1
	movw	r1,#:lower16:cur_token
	movt	r1,#:upper16:cur_token
	str	r0,[r1]
	add	sp,sp,#4
.10:
	movw	r0,#:lower16:cur_token
	movt	r0,#:upper16:cur_token
	ldr	r0,[r0]
	nop
	pop	{fp,pc}
	.size	next_token, .-next_token
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global panic
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	panic, %function
panic:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	movw	r0,#112
	bl	putch
	movw	r0,#97
	bl	putch
	movw	r0,#110
	bl	putch
	movw	r0,#105
	bl	putch
	movw	r0,#99
	bl	putch
	movw	r0,#33
	bl	putch
	movw	r0,#10
	bl	putch
	movw	r0,#65535
	movt	r0,#65535
	nop
	pop	{fp,pc}
	.size	panic, .-panic
	.global last_char
	.data 
	.align 2
	.type last_char,%object
	.size last_char,4
last_char:
	.word 32
	.comm num,4,4
	.comm other,4,4
	.comm cur_token,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
