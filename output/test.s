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
	push	{r4,r5,r6,fp,lr}
	nop
	add	fp,sp,#20
	sub	sp,sp,#12
	bl	getint
	mov	r1,r0
	cmp	r1,#10
	movw	r0,#:lower16:n
	movt	r0,#:upper16:n
	str	r1,[r0]
	ble	.0
	movw	r0,#1
	add	sp,sp,#12
	nop
	pop	{r4,r5,r6,fp,pc}
.0:
	movw	r0,#0
	add	r1,sp,#8
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.1:
	add	r1,sp,#8
	ldr	r0,[r1]
	movw	r3,#:lower16:n
	movt	r3,#:upper16:n
	ldr	r2,[r3]
	cmp	r0,r2
	bge	.2
	add	r1,sp,#8
	ldr	r0,[r1]
	mov	r2,r0
	mov	r0,r2
	push	{r0}
	push	{r2}
	bl	getint
	mov	r1,r0
	push	{r0}
	movw	r2,#:lower16:a
	movt	r2,#:upper16:a
	add	r4,sp,#8
	ldr	r3,[r4]
	str	r1,[r2,r3,lsl 2]
	add	r1,sp,#20
	ldr	r0,[r1]
	mov	r5,r0
	mov	r0,r5
	push	{r0}
	add	r1,sp,#16
	ldr	r0,[r1]
	movw	r3,#:lower16:a
	movt	r3,#:upper16:a
	push	{r5}
	add	r5,sp,#4
	ldr	r4,[r5]
	ldr	r2,[r3,r4,lsl 2]
	add	r6,r0,r2
	mov	r0,r6
	str	r0,[r1]
	add	r2,sp,#28
	ldr	r0,[r2]
	add	r3,r0,#1
	mov	r0,r3
	str	r0,[r2]
	add	sp,sp,#20
	b	.1
.2:
	add	r1,sp,#0
	ldr	r0,[r1]
	bl	putint
	movw	r0,#10
	add	r1,sp,#4
	str	r0,[r1]
	movw	r0,#10
	bl	putch
	add	r1,sp,#0
	ldr	r0,[r1]
	add	sp,sp,#12
	nop
	pop	{r4,r5,r6,fp,pc}
	.size	main, .-main
	.comm n,4,4
	.comm a,40,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
