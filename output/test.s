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
	mov	r2,r0
	movw	r0,#65535
	movt	r0,#65535
	cmp	r2,r0
	bne	.5
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#0
	pop	{fp,pc}
.5:
	movw	r0,#:lower16:left_child
	movt	r0,#:upper16:left_child
	ldr	r1,[r0,r2,lsl 2]
	movw	r0,#65535
	movt	r0,#65535
	cmp	r1,r0
	beq	.6
	mov	r0,r1
	bl	find_minimum
	add	sp,sp,#0
	pop	{fp,pc}
.6:
.7:
	mov	r0,r2
	add	sp,sp,#0
	pop	{fp,pc}
	.size	find_minimum, .-find_minimum
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
	push	{fp,lr}
	add	fp,sp,#8
	sub	sp,sp,#0
	mov	r2,r0
	movw	r0,#65535
	movt	r0,#65535
	cmp	r2,r0
	beq	.0
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	ldr	r0,[r0,r2,lsl 2]
	cmp	r0,r1
	it ne
	bne	.1
.0:
	mov	r0,r2
	add	sp,sp,#0
	pop	{fp,pc}
.1:
	cmp	r1,r0
	ble	.2
	movw	r0,#:lower16:right_child
	movt	r0,#:upper16:right_child
	ldr	r0,[r0,r2,lsl 2]
	bl	search
	add	sp,sp,#0
	pop	{fp,pc}
.2:
	movw	r0,#:lower16:left_child
	movt	r0,#:upper16:left_child
	ldr	r0,[r0,r2,lsl 2]
	bl	search
	add	sp,sp,#0
	pop	{fp,pc}
.3:
.4:
	.size	search, .-search
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
	push	{r4,r5,fp,lr}
	add	fp,sp,#16
	sub	sp,sp,#0
	mov	r1,r0
	movw	r0,#65535
	movt	r0,#65535
	mov	r3,r0
	mov	r2,r0
	movw	r4,#:lower16:now
	movt	r4,#:upper16:now
	ldr	r5,[r4]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r1,[r0,r5,lsl 2]
	movw	r0,#:lower16:left_child
	movt	r0,#:upper16:left_child
	str	r3,[r0,r5,lsl 2]
	movw	r0,#:lower16:right_child
	movt	r0,#:upper16:right_child
	str	r2,[r0,r5,lsl 2]
	add	r5,r5,#1
	sub	r0,r5,#1
	str	r5,[r4]
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
	.size	new_node, .-new_node
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
	push	{r4,r5,r6,fp,lr}
	add	fp,sp,#20
	sub	sp,sp,#4
	mov	r4,r0
	movw	r0,#65535
	movt	r0,#65535
	cmp	r4,r0
	bne	.8
	movw	r0,#65535
	movt	r0,#65535
	mov	r2,r0
	mov	r3,r0
	movw	r4,#:lower16:now
	movt	r4,#:upper16:now
	ldr	r5,[r4]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r1,[r0,r5,lsl 2]
	movw	r6,#:lower16:left_child
	movt	r6,#:upper16:left_child
	str	r2,[r6,r5,lsl 2]
	movw	r6,#:lower16:right_child
	movt	r6,#:upper16:right_child
	str	r3,[r6,r5,lsl 2]
	add	r5,r5,#1
	sub	r0,r5,#1
	str	r5,[r4]
	add	sp,sp,#4
	pop	{r4,r5,r6,fp,pc}
.8:
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	ldr	r0,[r0,r4,lsl 2]
	cmp	r1,r0
	ble	.9
	movw	r6,#:lower16:right_child
	movt	r6,#:upper16:right_child
	ldr	r0,[r6,r4,lsl 2]
	bl	insert
	str	r0,[r6,r4,lsl 2]
	b	.10
.9:
	movw	r6,#:lower16:left_child
	movt	r6,#:upper16:left_child
	ldr	r0,[r6,r4,lsl 2]
	bl	insert
	str	r0,[r6,r4,lsl 2]
.10:
.11:
	mov	r0,r4
	add	sp,sp,#4
	pop	{r4,r5,r6,fp,pc}
	.size	insert, .-insert
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
	push	{r4,r5,r6,r7,fp,lr}
	add	fp,sp,#24
	sub	sp,sp,#0
	movw	r0,#0
	mov	r7,r0
	bl	getint
	movs	r0,r0
	mov	r6,r0
	ite eq
	moveq	r0,#1
	movne	r0,#0
	bne	.25
	movw	r0,#:lower16:now
	movt	r0,#:upper16:now
	str	r7,[r0]
	movw	r0,#0
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,fp,pc}
.25:
	bl	getint
	mov	r1,r0
	movw	r0,#65535
	movt	r0,#65535
	mov	r3,r0
	mov	r2,r0
	movw	r4,#1
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r1,[r0,r7,lsl 2]
	movw	r0,#:lower16:left_child
	movt	r0,#:upper16:left_child
	str	r3,[r0,r7,lsl 2]
	movw	r0,#:lower16:right_child
	movt	r0,#:upper16:right_child
	str	r2,[r0,r7,lsl 2]
	mov	r7,r4
	sub	r0,r7,#1
	mov	r5,r0
	movw	r0,#:lower16:now
	movt	r0,#:upper16:now
	str	r7,[r0]
.26:
	cmp	r4,r6
	bge	.27
	bl	getint
	mov	r1,r0
	mov	r0,r5
	bl	insert
	add	r4,r4,#1
	b	.26
.27:
	mov	r0,r5
	bl	inorder
	movw	r0,#10
	bl	putch
	bl	getint
	mov	r6,r0
	movw	r0,#0
	mov	r4,r0
.28:
	cmp	r4,r6
	bge	.29
	bl	getint
	mov	r1,r0
	mov	r0,r5
	bl	delete
	mov	r5,r0
	add	r4,r4,#1
	b	.28
.29:
	mov	r0,r5
	bl	inorder
	movw	r0,#10
	bl	putch
	movw	r0,#0
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,fp,pc}
	.size	main, .-main
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
	push	{r4,fp,lr}
	add	fp,sp,#12
	sub	sp,sp,#4
	mov	r4,r0
	movw	r0,#65535
	movt	r0,#65535
	cmp	r4,r0
	beq	.24
	movw	r0,#:lower16:left_child
	movt	r0,#:upper16:left_child
	ldr	r0,[r0,r4,lsl 2]
	bl	inorder
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	ldr	r0,[r0,r4,lsl 2]
	bl	putint
	movw	r0,#32
	bl	putch
	movw	r0,#:lower16:right_child
	movt	r0,#:upper16:right_child
	ldr	r0,[r0,r4,lsl 2]
	bl	inorder
.24:
	add	sp,sp,#4
	pop	{r4,fp,pc}
	.size	inorder, .-inorder
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
	push	{r4,r5,r6,r7,fp,lr}
	add	fp,sp,#24
	sub	sp,sp,#0
	mov	r7,r0
	mov	r2,r1
	movw	r0,#65535
	movt	r0,#65535
	cmp	r7,r0
	bne	.12
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,fp,pc}
.12:
	movw	r6,#:lower16:value
	movt	r6,#:upper16:value
	ldr	r0,[r6,r7,lsl 2]
	cmp	r2,r0
	ble	.13
	movw	r4,#:lower16:right_child
	movt	r4,#:upper16:right_child
	ldr	r5,[r4,r7,lsl 2]
	mov	r0,r5
	mov	r1,r2
	bl	delete
	mov	r5,r0
	str	r5,[r4,r7,lsl 2]
	b	.23
.13:
	cmp	r2,r0
	bge	.14
	movw	r4,#:lower16:left_child
	movt	r4,#:upper16:left_child
	ldr	r1,[r4,r7,lsl 2]
	mov	r0,r1
	mov	r1,r2
	bl	delete
	mov	r1,r0
	str	r1,[r4,r7,lsl 2]
	b	.22
.14:
	movw	r4,#:lower16:left_child
	movt	r4,#:upper16:left_child
	ldr	r1,[r4,r7,lsl 2]
	movw	r0,#65535
	movt	r0,#65535
	cmp	r1,r0
	bne	.15
	movw	r4,#:lower16:right_child
	movt	r4,#:upper16:right_child
	ldr	r5,[r4,r7,lsl 2]
	movw	r0,#65535
	movt	r0,#65535
	cmp	r5,r0
	bne	.15
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,fp,pc}
.15:
	movw	r0,#65535
	movt	r0,#65535
	cmp	r1,r0
	beq	.16
	movw	r4,#:lower16:right_child
	movt	r4,#:upper16:right_child
	ldr	r5,[r4,r7,lsl 2]
	movw	r0,#65535
	movt	r0,#65535
	cmp	r5,r0
	beq	.16
	b	.19
.16:
	movw	r0,#65535
	movt	r0,#65535
	cmp	r1,r0
	bne	.17
	movw	r4,#:lower16:right_child
	movt	r4,#:upper16:right_child
	ldr	r0,[r4,r7,lsl 2]
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,fp,pc}
.17:
	mov	r0,r1
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,fp,pc}
.18:
	b	.20
.19:
	mov	r0,r5
	bl	find_minimum
	mov	r2,r0
	ldr	r1,[r6,r2,lsl 2]
	mov	r0,r1
	str	r0,[r6,r7,lsl 2]
	mov	r0,r5
	ldr	r1,[r6,r2,lsl 2]
	bl	delete
	mov	r5,r0
	str	r5,[r4,r7,lsl 2]
.20:
.21:
.22:
.23:
	mov	r0,r7
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,fp,pc}
	.size	delete, .-delete
	.comm value,40000,4
	.comm left_child,40000,4
	.comm right_child,40000,4
	.comm now,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
