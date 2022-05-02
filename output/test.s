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
	.global insertsort
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	insertsort, %function
insertsort:
	push	{r4,r5,r6,fp,lr}
	nop
	add	fp,sp,#20
	push	{r0}
	sub	sp,sp,#12
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
.0:
	add	r1,sp,#4
	ldr	r0,[r1]
	movw	r3,#:lower16:n
	movt	r3,#:upper16:n
	ldr	r2,[r3]
	cmp	r0,r2
	bge	.3
	add	r1,sp,#4
	ldr	r0,[r1]
	mov	r2,r0
	mov	r0,r2
	push	{r0}
	push	{r2}
	add	r2,sp,#20
	ldr	r1,[r2]
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	mov	r5,r0
	add	r1,sp,#12
	ldr	r0,[r1]
	sub	r2,r0,#1
	mov	r0,r2
	add	r1,sp,#16
	str	r0,[r1]
	add	r0,sp,#8
	str	r5,[r0]
	add	sp,sp,#8
.1:
	add	r1,sp,#8
	ldr	r0,[r1]
	movw	r2,#65535
	movt	r2,#65535
	cmp	r0,r2
	ble	.2
	add	r1,sp,#8
	ldr	r0,[r1]
	mov	r2,r0
	mov	r0,r2
	push	{r0}
	add	r1,sp,#4
	ldr	r0,[r1]
	push	{r2}
	add	r4,sp,#20
	ldr	r3,[r4]
	add	r6,sp,#4
	ldr	r5,[r6]
	ldr	r2,[r3,r5,lsl 2]
	cmp	r0,r2
	add	sp,sp,#8
	bge	.2
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	mov	r1,r0
	push	{r0}
	push	{r1}
	add	r1,sp,#16
	ldr	r0,[r1]
	push	{r2}
	mov	r2,r0
	mov	r0,r2
	push	{r0}
	push	{r2}
	add	r2,sp,#32
	ldr	r1,[r2]
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	mov	r5,r0
	add	r1,sp,#28
	ldr	r0,[r1]
	sub	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	add	r1,sp,#32
	ldr	r0,[r1]
	add	r3,sp,#12
	ldr	r2,[r3]
	str	r5,[r0,r2,lsl 2]
	add	sp,sp,#20
	b	.1
.2:
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	mov	r1,r0
	push	{r0}
	push	{r1}
	add	r1,sp,#8
	ldr	r0,[r1]
	push	{r2}
	mov	r2,r0
	add	r1,sp,#16
	ldr	r0,[r1]
	add	r4,sp,#24
	ldr	r3,[r4]
	add	r6,sp,#4
	ldr	r5,[r6]
	str	r2,[r3,r5,lsl 2]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	add	sp,sp,#12
	b	.0
.3:
	movw	r0,#0
	add	sp,sp,#16
	nop
	pop	{r4,r5,r6,fp,pc}
	.size	insertsort, .-insertsort
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
	sub	sp,sp,#48
	movw	r0,#10
	movw	r1,#:lower16:n
	movt	r1,#:upper16:n
	str	r0,[r1]
	movw	r0,#4
	add	r1,sp,#8
	str	r0,[r1,#0]
	movw	r0,#3
	str	r0,[r1,#4]
	movw	r0,#9
	str	r0,[r1,#8]
	movw	r0,#2
	str	r0,[r1,#12]
	movw	r0,#0
	str	r0,[r1,#16]
	movw	r0,#1
	str	r0,[r1,#20]
	movw	r0,#6
	str	r0,[r1,#24]
	movw	r0,#5
	str	r0,[r1,#28]
	movw	r0,#7
	str	r0,[r1,#32]
	movw	r0,#8
	str	r0,[r1,#36]
	mov	r0,r1
	bl	insertsort
	mov	r1,r0
	add	r0,sp,#0
	str	r1,[r0]
.4:
	add	r1,sp,#0
	ldr	r0,[r1]
	movw	r3,#:lower16:n
	movt	r3,#:upper16:n
	ldr	r2,[r3]
	cmp	r0,r2
	bge	.5
	add	r1,sp,#0
	ldr	r0,[r1]
	mov	r2,r0
	mov	r0,r2
	push	{r0}
	add	r1,sp,#12
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	mov	r4,r0
	add	r0,sp,#12
	str	r4,[r0]
	mov	r0,r4
	bl	putint
	movw	r4,#10
	add	r0,sp,#12
	str	r4,[r0]
	mov	r0,r4
	bl	putch
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	add	sp,sp,#8
	b	.4
.5:
	movw	r0,#0
	add	sp,sp,#48
	nop
	pop	{r4,fp,pc}
	.size	main, .-main
	.comm n,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
