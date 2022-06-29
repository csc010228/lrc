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
	sub	sp,sp,#128
	movw	r0,#3
	movw	r1,#:lower16:L
	movt	r1,#:upper16:L
	str	r0,[r1]
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	str	r0,[r1]
	movw	r1,#:lower16:M
	movt	r1,#:upper16:M
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
.2:
	add	r1,sp,#4
	ldr	r0,[r1]
	cmp	r0,#3
	bge	.3
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r2,sp,#116
	str	r0,[r2,r0,lsl 2]
	add	r2,sp,#104
	str	r0,[r2,r0,lsl 2]
	add	r2,sp,#92
	str	r0,[r2,r0,lsl 2]
	add	r2,sp,#80
	str	r0,[r2,r0,lsl 2]
	add	r2,sp,#68
	str	r0,[r2,r0,lsl 2]
	add	r2,sp,#56
	str	r0,[r2,r0,lsl 2]
	add	r0,sp,#4
	str	r1,[r0]
	b	.2
.3:
	add	r0,sp,#8
	push	{r0}
	add	r0,sp,#24
	push	{r0}
	add	r0,sp,#40
	push	{r0}
	add	r0,sp,#68
	push	{r0}
	add	r0,sp,#84
	push	{r0}
	add	r0,sp,#136
	add	r1,sp,#124
	add	r2,sp,#112
	add	r3,sp,#100
	bl	sub
	add	sp,sp,#20
	add	r1,sp,#4
	str	r0,[r1]
.4:
	add	r1,sp,#4
	ldr	r0,[r1]
	cmp	r0,#3
	bge	.5
	add	r1,sp,#32
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	putint
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#4
	str	r1,[r0]
	b	.4
.5:
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#10
	bl	putch
.6:
	add	r1,sp,#4
	ldr	r0,[r1]
	cmp	r0,#3
	bge	.7
	add	r1,sp,#20
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	putint
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#4
	str	r1,[r0]
	b	.6
.7:
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#10
	bl	putch
.8:
	add	r1,sp,#4
	ldr	r0,[r1]
	cmp	r0,#3
	bge	.9
	add	r1,sp,#8
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	putint
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#4
	str	r1,[r0]
	b	.8
.9:
	movw	r0,#10
	bl	putch
	movw	r0,#0
	add	sp,sp,#128
	nop
	pop	{fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global sub
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	sub, %function
sub:
	push	{r4,r5,fp,lr}
	nop
	add	fp,sp,#16
	push	{r0,r1,r2,r3}
	sub	sp,sp,#4
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.0:
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#3
	bge	.1
	add	r2,sp,#4
	ldr	r1,[r2]
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r3,sp,#16
	ldr	r2,[r3]
	add	r4,sp,#0
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	sub	r2,r0,r1
	push	{r2}
	add	r4,fp,#8
	ldr	r3,[r4]
	add	r5,sp,#4
	ldr	r4,[r5]
	str	r2,[r3,r4,lsl 2]
	add	r2,sp,#12
	ldr	r1,[r2]
	ldr	r0,[r1,r4,lsl 2]
	add	r3,fp,#0
	ldr	r2,[r3]
	ldr	r1,[r2,r4,lsl 2]
	sub	r2,r0,r1
	push	{r2}
	add	r4,fp,#12
	ldr	r3,[r4]
	add	r5,sp,#8
	ldr	r4,[r5]
	str	r2,[r3,r4,lsl 2]
	add	r2,sp,#20
	ldr	r1,[r2]
	ldr	r0,[r1,r4,lsl 2]
	add	r3,fp,#4
	ldr	r2,[r3]
	ldr	r1,[r2,r4,lsl 2]
	sub	r2,r0,r1
	add	r0,r4,#1
	add	r3,fp,#16
	ldr	r1,[r3]
	str	r2,[r1,r4,lsl 2]
	str	r0,[r5]
	add	sp,sp,#8
	b	.0
.1:
	movw	r0,#0
	add	sp,sp,#20
	nop
	pop	{r4,r5,fp,pc}
	.size	sub, .-sub
	.comm N,4,4
	.comm M,4,4
	.comm L,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
