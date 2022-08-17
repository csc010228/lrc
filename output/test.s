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
	.global insert
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	insert, %function
insert:
	push	{r4,fp,lr}
	add	fp,sp,#12
	sub	sp,sp,#4
	sub	sp,sp,#8
	sub	sp,sp,#8
	add	r2,sp,#12
	str	r1,[r2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	r1,sp,#8
	str	r0,[r1]
	add	sp,sp,#8
	bne	.8
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	bl	new_node
	add	sp,sp,#8
	add	sp,sp,#8
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.8:
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#:lower16:value
	movt	r2,#:upper16:value
	add	r4,sp,#8
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	add	sp,sp,#8
	ble	.9
	sub	sp,sp,#8
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#12
	ldr	r1,[r2]
	bl	insert
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#8
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
	b	.10
.9:
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#12
	ldr	r1,[r2]
	bl	insert
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#8
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
.10:
	sub	sp,sp,#0
	add	sp,sp,#0
.11:
	sub	sp,sp,#0
	add	r1,sp,#0
	ldr	r0,[r1]
	add	sp,sp,#0
	add	sp,sp,#8
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
	.size	insert, .-insert
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global delete
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	delete, %function
delete:
	push	{r4,fp,lr}
	add	fp,sp,#12
	sub	sp,sp,#4
	sub	sp,sp,#16
	sub	sp,sp,#8
	add	r2,sp,#20
	str	r1,[r2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	r1,sp,#16
	str	r0,[r1]
	add	sp,sp,#8
	bne	.12
	sub	sp,sp,#0
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#0
	add	sp,sp,#16
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.12:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	movw	r2,#:lower16:value
	movt	r2,#:upper16:value
	add	r4,sp,#16
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	add	sp,sp,#8
	ble	.13
	sub	sp,sp,#8
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#20
	ldr	r1,[r2]
	bl	delete
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#16
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
	b	.23
.13:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	movw	r2,#:lower16:value
	movt	r2,#:upper16:value
	add	r4,sp,#16
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.14
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#20
	ldr	r1,[r2]
	bl	delete
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#16
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
	b	.22
.14:
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	sp,sp,#8
	bne	.15
	sub	sp,sp,#8
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	sp,sp,#8
	bne	.15
	sub	sp,sp,#0
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#0
	add	sp,sp,#16
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.15:
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	sp,sp,#8
	beq	.16
	sub	sp,sp,#8
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	sp,sp,#8
	beq	.16
	sub	sp,sp,#0
	add	sp,sp,#0
	b	.19
.16:
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	sp,sp,#8
	bne	.17
	sub	sp,sp,#0
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	sp,sp,#0
	add	sp,sp,#16
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.17:
	sub	sp,sp,#0
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	sp,sp,#0
	add	sp,sp,#16
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.18:
	sub	sp,sp,#0
	add	sp,sp,#0
	b	.20
.19:
	sub	sp,sp,#8
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	find_minimum
	add	r1,sp,#0
	str	r0,[r1]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r3,sp,#16
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	ldr	r0,[r1,r2,lsl 2]
	movw	r2,#:lower16:value
	movt	r2,#:upper16:value
	add	r4,sp,#0
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	bl	delete
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#16
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
.20:
	sub	sp,sp,#0
	add	sp,sp,#0
.21:
	sub	sp,sp,#0
	add	sp,sp,#0
.22:
	sub	sp,sp,#0
	add	sp,sp,#0
.23:
	sub	sp,sp,#0
	add	r1,sp,#8
	ldr	r0,[r1]
	add	sp,sp,#0
	add	sp,sp,#16
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
	.size	delete, .-delete
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
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#16
	sub	sp,sp,#8
	movw	r0,#0
	movw	r1,#:lower16:now
	movt	r1,#:upper16:now
	str	r0,[r1]
	bl	getint
	cmp	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
	add	r1,sp,#16
	str	r0,[r1]
	ite eq
	moveq	r0,#1
	movne	r0,#0
	add	sp,sp,#8
	bne	.25
	sub	sp,sp,#0
	movw	r0,#0
	add	sp,sp,#0
	add	sp,sp,#16
	add	sp,sp,#0
	pop	{fp,pc}
	add	sp,sp,#0
.25:
	sub	sp,sp,#8
	bl	getint
	add	r1,sp,#4
	str	r0,[r1]
	bl	new_node
	add	r1,sp,#12
	str	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
	movw	r0,#1
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#8
.26:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r2,sp,#16
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.27
	sub	sp,sp,#16
	bl	getint
	add	r1,sp,#8
	str	r0,[r1]
	str	r0,[r1]
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r2,sp,#8
	ldr	r1,[r2]
	bl	insert
	add	r1,sp,#12
	str	r0,[r1]
	add	r1,sp,#28
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#28
	str	r1,[r0]
	add	sp,sp,#16
	b	.26
.27:
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	bl	inorder
	movw	r0,#10
	bl	putch
	bl	getint
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#16
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#8
.28:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r2,sp,#16
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.29
	sub	sp,sp,#16
	bl	getint
	add	r1,sp,#4
	str	r0,[r1]
	str	r0,[r1]
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r2,sp,#4
	ldr	r1,[r2]
	bl	delete
	add	r1,sp,#8
	str	r0,[r1]
	add	r1,sp,#20
	str	r0,[r1]
	add	r1,sp,#28
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#28
	str	r1,[r0]
	add	sp,sp,#16
	b	.28
.29:
	sub	sp,sp,#0
	add	r1,sp,#4
	ldr	r0,[r1]
	bl	inorder
	movw	r0,#10
	bl	putch
	movw	r0,#0
	add	sp,sp,#0
	add	sp,sp,#16
	add	sp,sp,#0
	pop	{fp,pc}
	add	sp,sp,#0
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global new_node
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	new_node, %function
new_node:
	push	{fp,lr}
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#8
	sub	sp,sp,#8
	add	r1,sp,#12
	str	r0,[r1]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	movw	r3,#:lower16:now
	movt	r3,#:upper16:now
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r0,#65535
	movt	r0,#65535
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	str	r0,[r1,r2,lsl 2]
	add	r0,r2,#1
	sub	r1,r0,#1
	str	r0,[r3]
	add	r2,sp,#0
	str	r0,[r2]
	str	r0,[r3]
	mov	r0,r1
	add	sp,sp,#8
	add	sp,sp,#8
	add	sp,sp,#0
	pop	{fp,pc}
	add	sp,sp,#0
	.size	new_node, .-new_node
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global find_minimum
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	find_minimum, %function
find_minimum:
	push	{fp,lr}
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#8
	sub	sp,sp,#8
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#8
	bne	.5
	sub	sp,sp,#0
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#0
	add	sp,sp,#8
	add	sp,sp,#0
	pop	{fp,pc}
	add	sp,sp,#0
.5:
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	sp,sp,#8
	beq	.6
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	find_minimum
	add	sp,sp,#8
	add	sp,sp,#8
	add	sp,sp,#0
	pop	{fp,pc}
	add	sp,sp,#0
.6:
	sub	sp,sp,#0
	add	sp,sp,#0
.7:
	sub	sp,sp,#0
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#0
	add	sp,sp,#8
	add	sp,sp,#0
	pop	{fp,pc}
	add	sp,sp,#0
	.size	find_minimum, .-find_minimum
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global inorder
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	inorder, %function
inorder:
	push	{fp,lr}
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#8
	sub	sp,sp,#8
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#8
	beq	.24
	sub	sp,sp,#0
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	inorder
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	putint
	movw	r0,#32
	bl	putch
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	inorder
	add	sp,sp,#0
.24:
	sub	sp,sp,#0
	add	sp,sp,#0
	add	sp,sp,#8
	add	sp,sp,#0
	pop	{fp,pc}
	add	sp,sp,#0
	.size	inorder, .-inorder
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global search
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	search, %function
search:
	push	{r4,fp,lr}
	add	fp,sp,#12
	sub	sp,sp,#4
	sub	sp,sp,#8
	sub	sp,sp,#8
	add	r2,sp,#12
	str	r1,[r2]
	movw	r1,#65535
	movt	r1,#65535
	cmp	r0,r1
	add	r1,sp,#8
	str	r0,[r1]
	add	sp,sp,#8
	beq	.0
	sub	sp,sp,#8
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#12
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	beq	.0
	sub	sp,sp,#0
	add	sp,sp,#0
	b	.1
.0:
	sub	sp,sp,#0
	add	r1,sp,#0
	ldr	r0,[r1]
	add	sp,sp,#0
	add	sp,sp,#8
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.1:
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#:lower16:value
	movt	r2,#:upper16:value
	add	r4,sp,#8
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	add	sp,sp,#8
	ble	.2
	sub	sp,sp,#8
	movw	r1,#:lower16:right_child
	movt	r1,#:upper16:right_child
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#12
	ldr	r1,[r2]
	bl	search
	add	sp,sp,#8
	add	sp,sp,#8
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.2:
	sub	sp,sp,#8
	movw	r1,#:lower16:left_child
	movt	r1,#:upper16:left_child
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#12
	ldr	r1,[r2]
	bl	search
	add	sp,sp,#8
	add	sp,sp,#8
	add	sp,sp,#4
	pop	{r4,fp,pc}
	add	sp,sp,#0
.3:
	sub	sp,sp,#0
	add	sp,sp,#0
.4:
	sub	sp,sp,#0
	add	sp,sp,#0
	.size	search, .-search
	.comm value,40000,4
	.comm left_child,40000,4
	.comm right_child,40000,4
	.comm now,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
