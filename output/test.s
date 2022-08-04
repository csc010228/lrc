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
	sub	sp,sp,#64
	sub	sp,sp,#8
	bl	getint
	nop
	mov	r1,r0
	add	r0,sp,#68
	add	r0,sp,#68
	str	r1,[r0]
	bl	getint
	nop
	mov	r1,r0
	add	r0,sp,#64
	add	r0,sp,#64
	str	r1,[r0]
	movw	r0,#13
	nop
	bl	_sysy_starttime
	movw	r0,#0
	mov	r1,r0
	add	r4,sp,#60
	str	r1,[r4]
	mov	r1,r0
	add	r5,sp,#56
	str	r1,[r5]
	mov	r2,r0
	add	sp,sp,#8
.0:
	sub	sp,sp,#8
	ldr	r0,[r4]
	add	r1,sp,#68
	ldr	r1,[r1]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.5
	nop
	movw	r0,#0
	mov	r1,r0
	str	r1,[r5]
	mov	r2,r0
	movw	r1,#32320
	movt	r1,#5
	ldr	r0,[r4]
	movw	r1,#32320
	movt	r1,#5
	mul	r8,r0,r1
	nop
.1:
	sub	sp,sp,#8
	ldr	r0,[r5]
	add	r1,sp,#68
	ldr	r1,[r1]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.4
	nop
	movw	r0,#0
	mov	r2,r0
	movw	r1,#600
	ldr	r0,[r5]
	movw	r1,#600
	mul	r6,r0,r1
	nop
.2:
	sub	sp,sp,#8
	add	r0,sp,#68
	ldr	r0,[r0]
	cmp	r2,r0
	add	sp,sp,#8
	bge	.3
	nop
	add	r0,r6,r2
	add	r7,r8,r0
	movw	r0,#1
	mov	r1,r0
	movw	r0,#0
	nop
	add	r2,r2,#1
	nop
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	str	r1,[r3,r7,lsl 2]
	movw	r1,#:lower16:y
	movt	r1,#:upper16:y
	str	r0,[r1,r7,lsl 2]
	nop
	b	.2
.3:
	nop
	ldr	r0,[r5]
	add	r0,r0,#1
	nop
	str	r0,[r5]
	nop
	b	.1
.4:
	nop
	ldr	r0,[r4]
	add	r0,r0,#1
	nop
	str	r0,[r4]
	nop
	b	.0
.5:
	sub	sp,sp,#0
	movw	r0,#1
	mov	r1,r0
	str	r1,[r4]
	mov	r1,r0
	str	r1,[r5]
	mov	r2,r0
	add	r0,sp,#60
	ldr	r0,[r0]
	sub	r0,r0,#1
	add	r8,sp,#32
	str	r0,[r8]
	add	r0,sp,#60
	ldr	r0,[r0]
	sub	r0,r0,#1
	add	r6,sp,#28
	str	r0,[r6]
	add	r0,sp,#60
	ldr	r0,[r0]
	sub	r0,r0,#1
	add	r7,sp,#24
	str	r0,[r7]
	add	sp,sp,#0
.6:
	nop
	ldr	r0,[r4]
	ldr	r1,[r8]
	cmp	r0,r1
	nop
	bge	.11
	sub	sp,sp,#0
	movw	r0,#1
	mov	r1,r0
	str	r1,[r5]
	mov	r2,r0
	movw	r1,#32320
	movt	r1,#5
	ldr	r1,[r4]
	movw	r0,#32320
	movt	r0,#5
	mul	r1,r1,r0
	add	r0,sp,#16
	add	r0,sp,#16
	str	r1,[r0]
	ldr	r0,[r4]
	sub	r1,r0,#1
	add	r0,sp,#12
	add	r0,sp,#12
	str	r1,[r0]
	ldr	r0,[r4]
	add	r0,r0,#1
	add	r1,sp,#8
	add	r1,sp,#8
	str	r0,[r1]
	add	sp,sp,#0
.7:
	nop
	ldr	r1,[r5]
	ldr	r0,[r6]
	cmp	r1,r0
	nop
	bge	.10
	sub	sp,sp,#0
	movw	r0,#1
	mov	r2,r0
	movw	r1,#600
	ldr	r1,[r5]
	movw	r0,#600
	mul	r1,r1,r0
	add	r0,sp,#20
	add	r0,sp,#20
	str	r1,[r0]
	ldr	r0,[r5]
	sub	r1,r0,#1
	add	r0,sp,#4
	add	r0,sp,#4
	str	r1,[r0]
	ldr	r0,[r5]
	add	r1,r0,#1
	add	r0,sp,#0
	add	r0,sp,#0
	str	r1,[r0]
	add	sp,sp,#0
.8:
	nop
	ldr	r0,[r7]
	cmp	r2,r0
	nop
	bge	.9
	sub	sp,sp,#96
	add	r0,sp,#116
	ldr	r0,[r0]
	add	r0,r0,r2
	add	r1,sp,#112
	ldr	r1,[r1]
	add	r1,r1,r0
	add	r9,sp,#88
	str	r1,[r9]
	movw	r1,#32320
	movt	r1,#5
	add	r1,sp,#108
	ldr	r1,[r1]
	movw	r3,#32320
	movt	r3,#5
	mul	r1,r1,r3
	add	r1,r1,r0
	add	r3,sp,#104
	ldr	r3,[r3]
	movw	r10,#32320
	movt	r10,#5
	mul	r3,r3,r10
	add	r0,r3,r0
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	ldr	r1,[r3,r1,lsl 2]
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	ldr	r0,[r3,r0,lsl 2]
	add	r0,r1,r0
	movw	r1,#600
	add	r1,sp,#100
	ldr	r1,[r1]
	movw	r3,#600
	mul	r1,r1,r3
	add	r1,r1,r2
	add	r3,sp,#112
	ldr	r3,[r3]
	add	r1,r3,r1
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	ldr	r1,[r3,r1,lsl 2]
	add	r0,r0,r1
	add	r1,sp,#96
	ldr	r1,[r1]
	movw	r3,#600
	mul	r1,r1,r3
	add	r1,r1,r2
	add	r3,sp,#112
	ldr	r3,[r3]
	add	r1,r3,r1
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	ldr	r1,[r3,r1,lsl 2]
	add	r0,r0,r1
	sub	r1,r2,#1
	add	r3,sp,#116
	ldr	r3,[r3]
	add	r1,r3,r1
	add	r3,sp,#112
	ldr	r3,[r3]
	add	r1,r3,r1
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	ldr	r1,[r3,r1,lsl 2]
	add	r0,r0,r1
	add	r1,r2,#1
	add	r10,sp,#16
	str	r1,[r10]
	add	r1,sp,#116
	ldr	r2,[r1]
	ldr	r1,[r10]
	add	r1,r2,r1
	add	r2,sp,#112
	ldr	r2,[r2]
	add	r1,r2,r1
	movw	r2,#:lower16:x
	movt	r2,#:upper16:x
	ldr	r1,[r2,r1,lsl 2]
	add	r0,r0,r1
	nop
	add	r1,sp,#152
	ldr	r1,[r1]
	nop
	bl	__aeabi_idiv
	nop
	nop
	ldr	r1,[r10]
	mov	r2,r1
	movw	r1,#:lower16:x
	movt	r1,#:upper16:x
	ldr	r3,[r9]
	str	r0,[r1,r3,lsl 2]
	add	sp,sp,#96
	b	.8
.9:
	nop
	ldr	r0,[r5]
	add	r0,r0,#1
	nop
	str	r0,[r5]
	nop
	b	.7
.10:
	nop
	ldr	r0,[r4]
	add	r0,r0,#1
	nop
	str	r0,[r4]
	nop
	b	.6
.11:
	sub	sp,sp,#40
	movw	r0,#53
	nop
	bl	_sysy_stoptime
	add	r0,sp,#100
	ldr	r0,[r0]
	nop
	movw	r3,#:lower16:x
	movt	r3,#:upper16:x
	movw	r1,#:lower16:x
	movt	r1,#:upper16:x
	add	r1,r1,#0
	nop
	bl	putarray
	add	r0,sp,#100
	ldr	r0,[r0]
	nop
	movw	r1,#2
	nop
	bl	__aeabi_idiv
	nop
	movw	r1,#600
	movw	r1,#600
	mul	r2,r0,r1
	movw	r1,#32320
	movt	r1,#5
	movw	r1,#32320
	movt	r1,#5
	mul	r0,r0,r1
	add	r1,r0,r2
	add	r0,sp,#100
	ldr	r0,[r0]
	nop
	movw	r2,#:lower16:x
	movt	r2,#:upper16:x
	add	r1,r2,r1,lsl 2
	nop
	bl	putarray
	ldr	r0,[r4]
	sub	r0,r0,#1
	ldr	r1,[r5]
	sub	r1,r1,#1
	movw	r2,#600
	mul	r1,r1,r2
	movw	r2,#32320
	movt	r2,#5
	mul	r0,r0,r2
	add	r1,r0,r1
	add	r0,sp,#100
	ldr	r0,[r0]
	nop
	movw	r2,#:lower16:x
	movt	r2,#:upper16:x
	add	r1,r2,r1,lsl 2
	nop
	bl	putarray
	movw	r0,#0
	add	sp,sp,#40
	add	sp,sp,#64
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0
	.size	main, .-main
	.comm x,864000000,4
	.comm y,864000000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
