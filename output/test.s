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
	.global power
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	power, %function
power:
	push	{r4,r5,fp,lr}
	add	fp,sp,#16
	sub	sp,sp,#0
	mov	r5,r0
	movs	r4,r1
	bne	.4
	movw	r0,#1
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
.4:
	add	r1,r4,r4,lsr 31
	asr	r1,r4,#1
	mov	r0,r5
	bl	power
	mov	r1,r0
	mov	r0,r1
	bl	multiply
	mov	r1,r0
	cmp	r4,#0
	and	r0,r4,#1
	it lt
	rsblt	r0,r0,#0
	cmp	r0,#1
	bne	.5
	mov	r0,r1
	mov	r1,r5
	bl	multiply
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
.5:
	mov	r0,r1
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
.6:
	.size	power, .-power
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global multiply
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	multiply, %function
multiply:
	push	{r4,r5,fp,lr}
	add	fp,sp,#16
	sub	sp,sp,#0
	mov	r5,r0
	movs	r4,r1
	bne	.0
	movw	r0,#0
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
.0:
	cmp	r4,#1
	bne	.1
	mov	r0,r5
	movw	r1,#1
	movt	r1,#15232
	bl	__aeabi_idivmod
	mov	r0,r1
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
.1:
	add	r1,r4,r4,lsr 31
	asr	r1,r4,#1
	mov	r0,r5
	bl	multiply
	lsl	r0,r0,#1
	movw	r1,#1
	movt	r1,#15232
	bl	__aeabi_idivmod
	cmp	r4,#0
	and	r0,r4,#1
	it lt
	rsblt	r0,r0,#0
	cmp	r0,#1
	bne	.2
	add	r0,r1,r5
	movw	r1,#1
	movt	r1,#15232
	bl	__aeabi_idivmod
	mov	r0,r1
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
.2:
	mov	r0,r1
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
.3:
	.size	multiply, .-multiply
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
	movw	r5,#:lower16:a
	movt	r5,#:upper16:a
	mov	r0,r5
	bl	getarray
	mov	r6,r0
	movw	r7,#:lower16:b
	movt	r7,#:upper16:b
	mov	r0,r7
	bl	getarray
	mov	r4,r0
	movw	r0,#60
	bl	_sysy_starttime
	movw	r0,#1
	mov	r8,r0
	add	r0,r6,r4
.16:
	sub	r4,r0,#1
	cmp	r8,r4
	itt lt
	lsllt	r8,r8,#1
	blt	.16
.17:
	movw	r0,#0
	movt	r0,#15232
	mov	r1,r8
	bl	__aeabi_idiv
	mov	r10,r0
	movw	r0,#3
	mov	r1,r10
	bl	power
	mov	r6,r0
	mov	r0,r5
	movw	r9,#0
	mov	r1,r9
	mov	r2,r8
	mov	r3,r6
	bl	fft
	mov	r0,r7
	mov	r1,r9
	mov	r2,r8
	mov	r3,r6
	bl	fft
	mov	r6,r9
.18:
	cmp	r6,r8
	bge	.19
	ldr	r0,[r5,r6,lsl 2]
	ldr	r1,[r7,r6,lsl 2]
	bl	multiply
	str	r0,[r5,r6,lsl 2]
	add	r6,r6,#1
	b	.18
.19:
	mov	r0,r10
	rsb	r1,r0,#998244352
	movw	r0,#3
	bl	power
	mov	r3,r0
	mov	r0,r5
	movw	r9,#0
	mov	r1,r9
	mov	r2,r8
	bl	fft
	mov	r6,r9
.20:
	cmp	r6,r8
	bge	.21
	mov	r0,r8
	movw	r1,#65535
	movt	r1,#15231
	bl	power
	mov	r1,r0
	ldr	r0,[r5,r6,lsl 2]
	bl	multiply
	str	r0,[r5,r6,lsl 2]
	add	r6,r6,#1
	b	.20
.21:
	movw	r0,#79
	bl	_sysy_stoptime
	mov	r0,r4
	mov	r1,r5
	bl	putarray
	movw	r0,#:lower16:d
	movt	r0,#:upper16:d
	str	r8,[r0]
	movw	r0,#0
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global fft
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	fft, %function
fft:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#40
	mov	r4,r0
	add	r0,sp,#24
	add	r0,sp,#24
	str	r4,[r0]
	add	r0,sp,#28
	add	r0,sp,#28
	str	r1,[r0]
	mov	r4,r2
	mov	r1,r3
	add	r0,sp,#36
	add	r0,sp,#36
	str	r1,[r0]
	sub	sp,sp,#8
	cmp	r4,#1
	add	sp,sp,#8
	bne	.9
	movw	r0,#1
	add	sp,sp,#40
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.9:
	sub	sp,sp,#0
	mov	r1,#0
	add	r0,sp,#20
	add	r0,sp,#20
	str	r1,[r0]
	add	r3,r4,r4,lsr 31
	asr	r3,r4,#1
	add	sp,sp,#0
.10:
	sub	sp,sp,#8
	add	r0,sp,#28
	ldr	r0,[r0]
	cmp	r0,r4
	add	sp,sp,#8
	bge	.13
	sub	sp,sp,#8
	add	r0,sp,#28
	ldr	r0,[r0]
	cmp	r0,#0
	add	r0,sp,#28
	ldr	r0,[r0]
	and	r0,r0,#1
	it lt
	rsblt	r0,r0,#0
	cmp	r0,#0
	add	sp,sp,#8
	bne	.11
	sub	sp,sp,#8
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r0,r0,r0,lsr 31
	add	r0,sp,#28
	ldr	r0,[r0]
	asr	r0,r0,#1
	add	r1,sp,#36
	ldr	r2,[r1]
	add	r1,sp,#28
	ldr	r1,[r1]
	add	r2,r1,r2
	add	r1,sp,#32
	ldr	r1,[r1]
	ldr	r1,[r1,r2,lsl 2]
	mov	r2,r1
	movw	r1,#:lower16:temp
	movt	r1,#:upper16:temp
	str	r2,[r1,r0,lsl 2]
	add	sp,sp,#8
	b	.12
.11:
	sub	sp,sp,#16
	add	r0,sp,#36
	ldr	r0,[r0]
	add	r0,r0,r0,lsr 31
	add	r0,sp,#36
	ldr	r0,[r0]
	asr	r0,r0,#1
	add	r0,r3,r0
	add	r1,sp,#44
	ldr	r2,[r1]
	add	r1,sp,#36
	ldr	r1,[r1]
	add	r2,r1,r2
	add	r1,sp,#40
	ldr	r1,[r1]
	ldr	r1,[r1,r2,lsl 2]
	mov	r2,r1
	movw	r1,#:lower16:temp
	movt	r1,#:upper16:temp
	str	r2,[r1,r0,lsl 2]
	add	sp,sp,#16
.12:
	sub	sp,sp,#0
	add	r0,sp,#20
	ldr	r0,[r0]
	add	r0,r0,#1
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#0
	b	.10
.13:
	sub	sp,sp,#32
	add	r0,sp,#56
	ldr	r0,[r0]
	add	r1,sp,#60
	ldr	r1,[r1]
	movw	r2,#:lower16:temp
	movt	r2,#:upper16:temp
	mov	r3,r4
	bl	memmove
	add	r6,r4,r4,lsr 31
	asr	r6,r4,#1
	add	r0,sp,#68
	ldr	r0,[r0]
	add	r1,sp,#68
	ldr	r1,[r1]
	bl	multiply
	mov	r5,r0
	add	r0,sp,#56
	ldr	r0,[r0]
	add	r1,sp,#60
	ldr	r1,[r1]
	mov	r2,r6
	mov	r3,r5
	bl	fft
	add	r0,sp,#60
	ldr	r0,[r0]
	add	r1,r0,r6
	mov	r4,r6
	add	r0,sp,#56
	ldr	r0,[r0]
	mov	r2,r6
	mov	r3,r5
	bl	fft
	mov	r1,#0
	add	r0,sp,#52
	str	r1,[r0]
	mov	r6,#1
	mov	fp,r4
	add	sp,sp,#32
.14:
	sub	sp,sp,#8
	add	r0,sp,#28
	ldr	r0,[r0]
	cmp	r0,fp
	add	sp,sp,#8
	bge	.15
	sub	sp,sp,#32
	add	r0,sp,#60
	ldr	r1,[r0]
	add	r0,sp,#52
	ldr	r0,[r0]
	add	r5,r1,r0
	add	r0,sp,#56
	ldr	r0,[r0]
	ldr	r4,[r0,r5,lsl 2]
	mov	r10,r4
	mov	r0,fp
	add	r7,r5,r0
	add	r0,sp,#56
	ldr	r0,[r0]
	ldr	r1,[r0,r7,lsl 2]
	mov	r8,r1
	mov	r0,r6
	bl	multiply
	add	r0,r4,r0
	movw	r4,#1
	movt	r4,#15232
	mov	r1,r4
	bl	__aeabi_idivmod
	mov	r9,r1
	mov	r0,r6
	mov	r1,r8
	bl	multiply
	sub	r0,r10,r0
	add	r0,r0,r4
	mov	r1,r4
	add	r2,sp,#56
	ldr	r2,[r2]
	str	r9,[r2,r5,lsl 2]
	bl	__aeabi_idivmod
	mov	r4,r1
	mov	r0,r6
	add	r1,sp,#68
	ldr	r1,[r1]
	bl	multiply
	mov	r6,r0
	add	r0,sp,#52
	ldr	r0,[r0]
	add	r0,r0,#1
	add	r1,sp,#52
	str	r0,[r1]
	add	r0,sp,#56
	ldr	r0,[r0]
	str	r4,[r0,r7,lsl 2]
	add	sp,sp,#32
	b	.14
.15:
	movw	r0,#0
	add	sp,sp,#40
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	fft, .-fft
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global memmove
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	memmove, %function
memmove:
	push	{r4,r5,r6,fp,lr}
	add	fp,sp,#20
	sub	sp,sp,#4
	mov	r6,r0
	mov	r5,r1
	mov	r4,r2
	movw	r0,#0
	mov	r2,r0
.7:
	cmp	r2,r3
	bge	.8
	add	r1,r5,r2
	ldr	r0,[r4,r2,lsl 2]
	add	r2,r2,#1
	str	r0,[r6,r1,lsl 2]
	b	.7
.8:
	mov	r0,r2
	add	sp,sp,#4
	pop	{r4,r5,r6,fp,pc}
	.size	memmove, .-memmove
	.comm d,4,4
	.comm temp,8388608,4
	.comm a,8388608,4
	.comm b,8388608,4
	.comm c,8388608,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
