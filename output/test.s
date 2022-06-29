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
	.global add_node
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	add_node, %function
add_node:
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	push	{r0,r1,r2}
	movw	r1,#10
	mul	r2,r0,r1
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	add	r4,sp,#0
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	add	r1,r2,r0
	push	{r1}
	push	{r1}
	push	{r1}
	add	r1,sp,#16
	ldr	r0,[r1]
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#24
	ldr	r0,[r1]
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	add	r3,sp,#12
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	add	r3,sp,#20
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:rev
	movt	r1,#:upper16:rev
	add	r4,sp,#8
	ldr	r3,[r4]
	str	r0,[r1,r3,lsl 2]
	movw	r0,#10
	mul	r1,r2,r0
	movw	r2,#:lower16:size
	movt	r2,#:upper16:size
	add	r4,sp,#20
	ldr	r3,[r4]
	ldr	r0,[r2,r3,lsl 2]
	add	r2,r1,r0
	push	{r1}
	add	r1,sp,#20
	ldr	r0,[r1]
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	str	r0,[r1,r2,lsl 2]
	movw	r0,#0
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	push	{r2}
	push	{r2}
	push	{r2}
	add	r3,sp,#32
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,r0,#1
	movw	r2,#:lower16:rev
	movt	r2,#:upper16:rev
	add	r4,sp,#0
	ldr	r3,[r4]
	str	r0,[r2,r3,lsl 2]
	movw	r0,#:lower16:size
	movt	r0,#:upper16:size
	add	r3,sp,#32
	ldr	r2,[r3]
	str	r1,[r0,r2,lsl 2]
	movw	r2,#:lower16:size
	movt	r2,#:upper16:size
	add	r4,sp,#32
	ldr	r3,[r4]
	str	r1,[r2,r3,lsl 2]
	push	{r1}
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	add	r3,sp,#40
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,r0,#1
	movw	r0,#:lower16:size
	movt	r0,#:upper16:size
	str	r1,[r0,r2,lsl 2]
	add	sp,sp,#48
	nop
	pop	{r4,fp,pc}
	add	sp,sp,#36
	.size	add_node, .-add_node
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global dfs
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	dfs, %function
dfs:
	push	{r4,r5,fp,lr}
	nop
	add	fp,sp,#16
	push	{r0,r1,r2}
	sub	sp,sp,#12
	cmp	r0,r1
	bne	.2
	add	r1,sp,#20
	ldr	r0,[r1]
	add	sp,sp,#24
	nop
	pop	{r4,r5,fp,pc}
.2:
	movw	r0,#1
	movw	r1,#:lower16:used
	movt	r1,#:upper16:used
	add	r3,sp,#12
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r0,#0
	add	r1,sp,#8
	str	r0,[r1]
.3:
	add	r1,sp,#8
	ldr	r0,[r1]
	movw	r2,#:lower16:size
	movt	r2,#:upper16:size
	add	r4,sp,#12
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	bge	.9
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#10
	mul	r2,r0,r1
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r2,r0
	push	{r1}
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r0}
	movw	r1,#:lower16:used
	movt	r1,#:upper16:used
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#12
	beq	.4
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	b	.3
.4:
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#10
	mul	r2,r0,r1
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r2,r0
	push	{r1}
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#8
	bgt	.5
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	b	.3
.5:
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#10
	mul	r2,r0,r1
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r2,r0
	push	{r1}
	add	r1,sp,#24
	ldr	r0,[r1]
	push	{r2}
	movw	r2,#:lower16:cap
	movt	r2,#:upper16:cap
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.6
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
	b	.7
.6:
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#10
	mul	r2,r0,r1
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r2,r0
	push	{r1}
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,sp,#8
	str	r0,[r1]
	add	sp,sp,#8
.7:
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#10
	mul	r2,r0,r1
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r2,r0
	push	{r1}
	push	{r2}
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#24
	ldr	r1,[r2]
	add	r3,sp,#8
	ldr	r2,[r3]
	bl	dfs
	cmp	r0,#0
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#8
	ble	.8
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#10
	mul	r2,r0,r1
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r2,r0
	push	{r1}
	push	{r1}
	push	{r1}
	push	{r1}
	push	{r1}
	push	{r1}
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#32
	ldr	r1,[r2]
	sub	r2,r0,r1
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	movw	r3,#:lower16:cap
	movt	r3,#:upper16:cap
	add	r5,sp,#8
	ldr	r4,[r5]
	str	r2,[r3,r4,lsl 2]
	push	{r2}
	add	r3,sp,#24
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#10
	mul	r2,r0,r1
	movw	r1,#:lower16:rev
	movt	r1,#:upper16:rev
	add	r4,sp,#28
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	add	r1,r2,r0
	push	{r1}
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	push	{r2}
	add	r3,sp,#28
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#10
	mul	r2,r0,r1
	movw	r1,#:lower16:rev
	movt	r1,#:upper16:rev
	add	r4,sp,#24
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	add	r1,r2,r0
	push	{r1}
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#52
	ldr	r1,[r2]
	add	r2,r0,r1
	movw	r0,#:lower16:cap
	movt	r0,#:upper16:cap
	add	r3,sp,#12
	ldr	r1,[r3]
	str	r2,[r0,r1,lsl 2]
	add	r1,sp,#52
	ldr	r0,[r1]
	add	sp,sp,#72
	nop
	pop	{r4,r5,fp,pc}
	add	sp,sp,#48
.8:
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	b	.3
.9:
	movw	r0,#0
	add	sp,sp,#24
	nop
	pop	{r4,r5,fp,pc}
	.size	dfs, .-dfs
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global my_memset
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	my_memset, %function
my_memset:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1,r2}
	sub	sp,sp,#4
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.0:
	add	r1,sp,#0
	ldr	r0,[r1]
	add	r2,sp,#12
	ldr	r1,[r2]
	cmp	r0,r1
	bge	.1
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r2,sp,#4
	ldr	r1,[r2]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r1,r0,#1
	str	r1,[r3]
	b	.0
.1:
	add	sp,sp,#16
	nop
	pop	{fp,pc}
	.size	my_memset, .-my_memset
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global max_flow
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	max_flow, %function
max_flow:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1}
	sub	sp,sp,#8
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
.10:
	movw	r0,#:lower16:used
	movt	r0,#:upper16:used
	movw	r1,#0
	movw	r2,#10
	bl	my_memset
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r2,sp,#12
	ldr	r1,[r2]
	movw	r2,#0
	movt	r2,#28672
	bl	dfs
	cmp	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
	bne	.11
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#16
	nop
	pop	{fp,pc}
.11:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r2,sp,#0
	ldr	r1,[r2]
	add	r2,r0,r1
	add	r0,sp,#4
	str	r2,[r0]
	b	.10
.12:
	.size	max_flow, .-max_flow
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
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	sub	sp,sp,#32
	bl	getint
	push	{r0}
	add	r1,sp,#28
	str	r0,[r1]
	bl	getint
	push	{r0}
	add	r1,sp,#36
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#:lower16:size
	movt	r0,#:upper16:size
	movw	r1,#0
	movw	r2,#10
	bl	my_memset
	add	sp,sp,#8
.13:
	add	r1,sp,#28
	ldr	r0,[r1]
	cmp	r0,#0
	ble	.14
	bl	getint
	push	{r0}
	bl	getint
	push	{r0}
	bl	getint
	push	{r0}
	add	r1,sp,#8
	ldr	r0,[r1]
	movw	r1,#10
	mul	r2,r0,r1
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	add	r4,sp,#8
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	add	r1,r2,r0
	push	{r1}
	push	{r1}
	push	{r1}
	add	r1,sp,#16
	ldr	r0,[r1]
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	push	{r2}
	add	r3,sp,#12
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#16
	ldr	r0,[r1]
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	add	r3,sp,#8
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	add	r3,sp,#20
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:rev
	movt	r1,#:upper16:rev
	add	r4,sp,#4
	ldr	r3,[r4]
	str	r0,[r1,r3,lsl 2]
	movw	r0,#10
	mul	r1,r2,r0
	movw	r2,#:lower16:size
	movt	r2,#:upper16:size
	add	r4,sp,#20
	ldr	r3,[r4]
	ldr	r0,[r2,r3,lsl 2]
	add	r2,r1,r0
	push	{r1}
	add	r1,sp,#28
	ldr	r0,[r1]
	movw	r1,#:lower16:to
	movt	r1,#:upper16:to
	str	r0,[r1,r2,lsl 2]
	movw	r0,#0
	movw	r1,#:lower16:cap
	movt	r1,#:upper16:cap
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	push	{r2}
	push	{r2}
	push	{r2}
	add	r3,sp,#40
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,r0,#1
	movw	r2,#:lower16:rev
	movt	r2,#:upper16:rev
	add	r4,sp,#0
	ldr	r3,[r4]
	str	r0,[r2,r3,lsl 2]
	movw	r0,#:lower16:size
	movt	r0,#:upper16:size
	add	r3,sp,#40
	ldr	r2,[r3]
	str	r1,[r0,r2,lsl 2]
	push	{r1}
	movw	r2,#:lower16:size
	movt	r2,#:upper16:size
	add	r4,sp,#44
	ldr	r3,[r4]
	str	r1,[r2,r3,lsl 2]
	movw	r1,#:lower16:size
	movt	r1,#:upper16:size
	add	r3,sp,#40
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,r0,#1
	push	{r1}
	movw	r2,#:lower16:size
	movt	r2,#:upper16:size
	add	r4,sp,#44
	ldr	r3,[r4]
	str	r1,[r2,r3,lsl 2]
	add	r1,sp,#80
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r0,sp,#80
	str	r1,[r0]
	add	sp,sp,#52
	b	.13
.14:
	movw	r0,#0
	add	r1,sp,#20
	str	r0,[r1]
.15:
	movw	r0,#:lower16:used
	movt	r0,#:upper16:used
	movw	r1,#0
	movw	r2,#10
	bl	my_memset
	movw	r0,#1
	add	r2,sp,#24
	ldr	r1,[r2]
	movw	r2,#0
	movt	r2,#28672
	bl	dfs
	cmp	r0,#0
	add	r1,sp,#16
	str	r0,[r1]
	bne	.16
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
	b	.18
.16:
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r2,sp,#16
	ldr	r1,[r2]
	add	r2,r0,r1
	add	r0,sp,#20
	str	r2,[r0]
	b	.15
.17:
.18:
	add	r1,sp,#0
	ldr	r0,[r1]
	bl	putint
	movw	r0,#10
	bl	putch
	movw	r0,#0
	add	sp,sp,#32
	nop
	pop	{r4,fp,pc}
	.size	main, .-main
	.comm size,40,4
	.comm to,400,4
	.comm cap,400,4
	.comm rev,400,4
	.comm used,40,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
