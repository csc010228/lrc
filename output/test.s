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
	sub	sp,sp,#32
	sub	sp,sp,#0
	bl	getint
	mov	r1,r0
	add	r0,sp,#28
	add	r0,sp,#28
	str	r1,[r0]
	bl	getint
	mov	r10,r0
	mov	r0,#13
	bl	_sysy_starttime
	mov	r1,#0
	add	r0,sp,#20
	add	r0,sp,#20
	str	r1,[r0]
	mov	r0,#0
	mov	r0,#0
	add	sp,sp,#0
.0:
	sub	sp,sp,#8
	add	r0,sp,#36
	ldr	r1,[r0]
	add	r0,sp,#28
	ldr	r0,[r0]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.5
	sub	sp,sp,#0
	mov	r0,#0
	add	fp,sp,#16
	str	r0,[fp]
	mov	r0,#0
	add	sp,sp,#0
.1:
	sub	sp,sp,#8
	add	r0,sp,#36
	ldr	r0,[r0]
	ldr	r1,[fp]
	cmp	r1,r0
	add	sp,sp,#8
	bge	.4
	mov	r9,#0
.2:
	sub	sp,sp,#8
	add	r0,sp,#36
	ldr	r0,[r0]
	cmp	r9,r0
	add	sp,sp,#8
	bge	.3
	sub	sp,sp,#16
	movw	r0,#600
	ldr	r1,[fp]
	mul	r0,r1,r0
	add	r2,r0,r9
	movw	r0,#32320
	movt	r0,#5
	add	r1,sp,#36
	ldr	r1,[r1]
	mul	r0,r1,r0
	add	r1,r0,r2
	mov	r3,#1
	mov	r2,#0
	add	r9,r9,#1
	movw	r0,#:lower16:y
	movt	r0,#:upper16:y
	str	r2,[r0,r1,lsl 2]
	movw	r0,#:lower16:x
	movt	r0,#:upper16:x
	str	r3,[r0,r1,lsl 2]
	add	sp,sp,#16
	b	.2
.3:
	ldr	r0,[fp]
	add	r0,r0,#1
	str	r0,[fp]
	b	.1
.4:
	sub	sp,sp,#0
	add	r0,sp,#20
	ldr	r0,[r0]
	add	r0,r0,#1
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#0
	b	.0
.5:
	sub	sp,sp,#0
	mov	r1,#1
	add	r0,sp,#20
	str	r1,[r0]
	mov	r0,#1
	add	fp,sp,#16
	str	r0,[fp]
	mov	r9,#1
	add	r0,sp,#28
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#4
	add	r0,sp,#4
	str	r1,[r0]
	add	sp,sp,#0
.6:
	sub	sp,sp,#8
	add	r0,sp,#12
	ldr	r1,[r0]
	add	r0,sp,#28
	ldr	r0,[r0]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.11
	sub	sp,sp,#0
	mov	r0,#1
	str	r0,[fp]
	mov	r9,#1
	add	r0,sp,#28
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#8
	add	r0,sp,#8
	str	r1,[r0]
	add	sp,sp,#0
.7:
	sub	sp,sp,#8
	add	r0,sp,#16
	ldr	r0,[r0]
	ldr	r1,[fp]
	cmp	r1,r0
	add	sp,sp,#8
	bge	.10
	sub	sp,sp,#0
	mov	r9,#1
	add	r0,sp,#28
	ldr	r0,[r0]
	sub	r8,r0,#1
	add	sp,sp,#0
.8:
	cmp	r9,r8
	bge	.9
	sub	sp,sp,#120
	movw	r3,#600
	ldr	r0,[fp]
	mul	r5,r0,r3
	add	r2,r5,r9
	movw	r0,#32320
	movt	r0,#5
	add	r1,sp,#140
	ldr	r1,[r1]
	mul	r1,r1,r0
	add	r7,r1,r2
	add	r4,sp,#140
	ldr	r4,[r4]
	sub	r4,r4,#1
	mul	r4,r4,r0
	add	r6,r4,r2
	add	r4,sp,#140
	ldr	r4,[r4]
	add	r4,r4,#1
	mul	r0,r4,r0
	add	r0,r0,r2
	movw	r4,#:lower16:x
	movt	r4,#:upper16:x
	ldr	r2,[r4,r6,lsl 2]
	ldr	r0,[r4,r0,lsl 2]
	add	r2,r2,r0
	ldr	r0,[fp]
	sub	r0,r0,#1
	mul	r0,r0,r3
	add	r0,r0,r9
	add	r0,r1,r0
	ldr	r0,[r4,r0,lsl 2]
	add	r2,r2,r0
	ldr	r0,[fp]
	add	r0,r0,#1
	mul	r0,r0,r3
	add	r0,r0,r9
	add	r0,r1,r0
	ldr	r0,[r4,r0,lsl 2]
	add	r0,r2,r0
	sub	r2,r9,#1
	add	r2,r5,r2
	add	r2,r1,r2
	ldr	r2,[r4,r2,lsl 2]
	add	r0,r0,r2
	add	r6,r9,#1
	add	r2,r5,r6
	add	r1,r1,r2
	ldr	r1,[r4,r1,lsl 2]
	add	r0,r0,r1
	mov	r1,r10
	bl	__aeabi_idiv
	mov	r9,r6
	str	r0,[r4,r7,lsl 2]
	add	sp,sp,#120
	b	.8
.9:
	ldr	r0,[fp]
	add	r0,r0,#1
	str	r0,[fp]
	b	.7
.10:
	sub	sp,sp,#0
	add	r0,sp,#20
	ldr	r0,[r0]
	add	r0,r0,#1
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#0
	b	.6
.11:
	sub	sp,sp,#40
	mov	r0,#53
	bl	_sysy_stoptime
	add	r0,sp,#68
	ldr	r0,[r0]
	movw	r6,#:lower16:x
	movt	r6,#:upper16:x
	add	r1,r6,#0
	bl	putarray
	add	r0,sp,#68
	ldr	r0,[r0]
	add	r0,r0,r0,lsr 31
	add	r0,sp,#68
	ldr	r0,[r0]
	asr	r0,r0,#1
	movw	r4,#600
	mul	r1,r0,r4
	movw	r5,#32320
	movt	r5,#5
	mul	r0,r0,r5
	add	r1,r0,r1
	add	r0,sp,#68
	ldr	r0,[r0]
	add	r1,r6,r1,lsl 2
	bl	putarray
	add	r0,sp,#60
	ldr	r0,[r0]
	sub	r2,r0,#1
	ldr	r0,[fp]
	sub	r0,r0,#1
	mul	r1,r0,r4
	mul	r0,r2,r5
	add	r1,r0,r1
	add	r0,sp,#68
	ldr	r0,[r0]
	add	r1,r6,r1,lsl 2
	bl	putarray
	movw	r0,#0
	add	sp,sp,#40
	add	sp,sp,#32
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0
	.size	main, .-main
	.comm x,864000000,4
	.comm y,864000000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
