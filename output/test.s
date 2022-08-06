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
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#56
	sub	sp,sp,#0
	movw	r0,#10
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	str	r0,[r1]
	movw	r0,#1
	add	r5,sp,#12
	str	r0,[r5,#0]
	movw	r0,#3
	str	r0,[r5,#4]
	movw	r0,#4
	str	r0,[r5,#8]
	movw	r0,#7
	str	r0,[r5,#12]
	movw	r0,#8
	str	r0,[r5,#16]
	movw	r0,#11
	mov	r10,r0
	movw	r0,#13
	mov	r9,r0
	movw	r0,#18
	mov	r8,r0
	movw	r0,#56
	mov	r7,r0
	movw	r0,#78
	mov	r6,r0
	movw	r0,#0
	mov	r4,r0
	bl	getint
	mov	r2,r0
	add	r5,sp,#12
	ldr	r0,[r5,#0]
	str	r0,[r5,#0]
	ldr	r0,[r5,#4]
	str	r0,[r5,#4]
	ldr	r0,[r5,#8]
	str	r0,[r5,#8]
	ldr	r0,[r5,#12]
	str	r0,[r5,#12]
	ldr	r0,[r5,#16]
	str	r0,[r5,#16]
	str	r10,[r5,#20]
	str	r9,[r5,#24]
	str	r8,[r5,#28]
	str	r7,[r5,#32]
	str	r6,[r5,#36]
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	ldr	r0,[r1]
	str	r0,[r1]
	mov	r0,r5
	mov	r1,r2
	bl	insert
	mov	r2,r0
	add	sp,sp,#0
.4:
	cmp	r4,#10
	bge	.5
	ldr	r0,[r5,r4,lsl 2]
	bl	putint
	movw	r0,#10
	bl	putch
	add	r4,r4,#1
	b	.4
.5:
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	ldr	r0,[r1]
	str	r0,[r1]
	movw	r0,#0
	add	sp,sp,#56
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	main, .-main
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
	push	{r4,r5,fp,lr}
	add	fp,sp,#16
	sub	sp,sp,#0
	mov	r5,r1
	movw	r1,#0
	mov	r3,r1
.0:
	ldr	r2,[r0,r3,lsl 2]
	cmp	r5,r2
	ble	.1
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	ldr	r1,[r1]
	cmp	r3,r1
	bge	.1
	add	r3,r3,#1
	b	.0
.1:
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	ldr	r1,[r1]
	mov	r4,r1
.2:
	cmp	r4,r3
	ble	.3
	sub	r1,r4,#1
	ldr	r2,[r0,r1,lsl 2]
	str	r2,[r0,r4,lsl 2]
	mov	r2,r5
	mov	r4,r1
	str	r2,[r0,r3,lsl 2]
	b	.2
.3:
	movw	r0,#0
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
	.size	insert, .-insert
	.comm N,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
