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
	nop
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#24
	sub	sp,sp,#8
	bl	getint
	nop
	mov	r6,r0
	bl	getint
	nop
	nop
	add	r9,sp,#24
	str	r0,[r9]
	movw	r0,#13
	nop
	bl	_sysy_starttime
	movw	r3,#0
	mov	r0,r3
	add	r8,sp,#20
	str	r0,[r8]
	mov	r4,r3
	mov	r0,r3
	add	r7,sp,#12
	str	r0,[r7]
	add	sp,sp,#8
.0:
	nop
	ldr	r0,[r8]
	cmp	r0,r6
	nop
	bge	.5
	nop
	movw	r3,#0
	mov	r4,r3
	mov	r0,r3
	str	r0,[r7]
	nop
.1:
	nop
	cmp	r4,r6
	nop
	bge	.4
	nop
	movw	r3,#0
	mov	r0,r3
	str	r0,[r7]
	nop
.2:
	nop
	ldr	r0,[r7]
	cmp	r0,r6
	nop
	bge	.3
	nop
	movw	r0,#600
	movw	r0,#600
	mul	r1,r4,r0
	ldr	r0,[r7]
	add	r1,r1,r0
	movw	r0,#32320
	movt	r0,#5
	movw	r2,#32320
	movt	r2,#5
	ldr	r0,[r8]
	mul	r0,r0,r2
	add	r2,r0,r1
	movw	r1,#1
	nop
	movw	r3,#0
	mov	r0,r3
	ldr	r3,[r7]
	add	r3,r3,#1
	nop
	str	r3,[r7]
	movw	r3,#:lower16:y
	movt	r3,#:upper16:y
	str	r0,[r3,r2,lsl 2]
	movw	r5,#:lower16:x
	movt	r5,#:upper16:x
	str	r1,[r5,r2,lsl 2]
	nop
	b	.2
.3:
	nop
	add	r0,r4,#1
	mov	r4,r0
	nop
	b	.1
.4:
	nop
	ldr	r0,[r8]
	add	r0,r0,#1
	nop
	str	r0,[r8]
	nop
	b	.0
.5:
	nop
	movw	r1,#1
	mov	r0,r1
	str	r0,[r8]
	mov	r4,r1
	mov	r0,r1
	str	r0,[r7]
	nop
.6:
	nop
	sub	r1,r6,#1
	ldr	r0,[r8]
	cmp	r0,r1
	nop
	bge	.11
	nop
	movw	r1,#1
	mov	r4,r1
	mov	r0,r1
	str	r0,[r7]
	nop
.7:
	nop
	sub	r0,r6,#1
	cmp	r4,r0
	nop
	bge	.10
	nop
	movw	r1,#1
	mov	r0,r1
	str	r0,[r7]
	nop
.8:
	nop
	sub	r0,r6,#1
	ldr	r1,[r7]
	cmp	r1,r0
	nop
	bge	.9
	sub	sp,sp,#120
	movw	r0,#600
	movw	r0,#600
	mul	r1,r4,r0
	ldr	r0,[r7]
	add	r0,r1,r0
	add	r5,sp,#112
	str	r0,[r5]
	movw	r0,#32320
	movt	r0,#5
	movw	r2,#32320
	movt	r2,#5
	ldr	r0,[r8]
	mul	r2,r0,r2
	ldr	r0,[r5]
	add	r3,r2,r0
	add	r0,sp,#104
	add	r0,sp,#104
	str	r3,[r0]
	ldr	r0,[r8]
	sub	r0,r0,#1
	movw	r3,#32320
	movt	r3,#5
	mul	r3,r0,r3
	ldr	r0,[r5]
	add	r0,r3,r0
	add	r3,sp,#92
	str	r0,[r3]
	ldr	r0,[r8]
	add	r0,r0,#1
	movw	r10,#32320
	movt	r10,#5
	mul	r10,r0,r10
	ldr	r0,[r5]
	add	r0,r10,r0
	movw	r5,#:lower16:x
	movt	r5,#:upper16:x
	ldr	r3,[r3]
	ldr	r3,[r5,r3,lsl 2]
	ldr	r0,[r5,r0,lsl 2]
	add	r0,r3,r0
	add	r3,sp,#76
	str	r0,[r3]
	sub	r0,r4,#1
	movw	r10,#600
	movw	r10,#600
	mul	r10,r0,r10
	ldr	r0,[r7]
	add	r0,r10,r0
	add	r0,r2,r0
	ldr	r0,[r5,r0,lsl 2]
	ldr	r3,[r3]
	add	r0,r3,r0
	add	r3,sp,#56
	str	r0,[r3]
	add	r0,r4,#1
	movw	r10,#600
	movw	r10,#600
	mul	r10,r0,r10
	ldr	r0,[r7]
	add	r0,r10,r0
	add	r0,r2,r0
	ldr	r10,[r5,r0,lsl 2]
	ldr	r0,[r3]
	add	r0,r0,r10
	ldr	r3,[r7]
	sub	r3,r3,#1
	add	r3,r1,r3
	add	r3,r2,r3
	ldr	r3,[r5,r3,lsl 2]
	add	r3,r0,r3
	ldr	r0,[r7]
	add	r0,r0,#1
	add	r10,sp,#16
	add	r10,sp,#16
	str	r0,[r10]
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r0,r1,r0
	add	r0,r2,r0
	ldr	r0,[r5,r0,lsl 2]
	add	r0,r3,r0
	nop
	ldr	r1,[r9]
	nop
	bl	__aeabi_idiv
	nop
	nop
	add	r1,sp,#16
	ldr	r1,[r1]
	nop
	str	r1,[r7]
	add	r1,sp,#104
	ldr	r1,[r1]
	str	r0,[r5,r1,lsl 2]
	add	sp,sp,#120
	b	.8
.9:
	nop
	add	r0,r4,#1
	mov	r4,r0
	nop
	b	.7
.10:
	nop
	ldr	r0,[r8]
	add	r0,r0,#1
	nop
	str	r0,[r8]
	nop
	b	.6
.11:
	nop
	movw	r0,#53
	nop
	bl	_sysy_stoptime
	mov	r0,r6
	movw	r5,#:lower16:x
	movt	r5,#:upper16:x
	add	r1,r5,#0
	nop
	bl	putarray
	mov	r0,r6
	movw	r1,#2
	nop
	bl	__aeabi_idiv
	mov	r1,r0
	movw	r0,#600
	movw	r0,#600
	mul	r2,r1,r0
	movw	r0,#32320
	movt	r0,#5
	movw	r0,#32320
	movt	r0,#5
	mul	r0,r1,r0
	add	r1,r0,r2
	mov	r0,r6
	add	r1,r5,r1,lsl 2
	nop
	bl	putarray
	ldr	r0,[r8]
	sub	r2,r0,#1
	sub	r1,r4,#1
	movw	r0,#600
	mul	r1,r1,r0
	movw	r0,#32320
	movt	r0,#5
	mul	r0,r2,r0
	add	r1,r0,r1
	mov	r0,r6
	add	r1,r5,r1,lsl 2
	nop
	bl	putarray
	movw	r0,#0
	nop
	add	sp,sp,#24
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	nop
	.size	main, .-main
	.comm x,864000000,4
	.comm y,864000000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
