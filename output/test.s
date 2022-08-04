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
	.global memmove
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	memmove, %function
memmove:
	push	{r4,r5,fp,lr}
	nop
	add	fp,sp,#16
	sub	sp,sp,#0
	nop
	mov	r5,r0
	mov	r4,r1
	mov	r3,r2
	nop
	movw	r0,#0
	mov	r2,r0
	nop
.38:
	nop
	cmp	r2,r3
	nop
	bge	.39
	nop
	ldr	r0,[r4,r2,lsl 2]
	mov	r1,r0
	add	r0,r2,#1
	str	r1,[r5,r2,lsl 2]
	mov	r2,r0
	nop
	b	.38
.39:
	nop
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,fp,pc}
	nop
	.size	memmove, .-memmove
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
	sub	sp,sp,#24
	bl	getint
	mov	r4,r0
	bl	getint
	mov	r5,r0
	bl	getint
	mov	r2,r0
	mov	r1,r4
	add	r0,sp,#44
	add	r0,sp,#44
	str	r1,[r0]
	mov	r10,r5
	mov	r9,r2
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	nop
	bl	getarray
	nop
	movw	r8,#:lower16:kernelid
	movt	r8,#:upper16:kernelid
	mov	r0,r8
	bl	getarray
	nop
	mov	r7,r0
	movw	r0,#112
	nop
	bl	_sysy_starttime
	movw	r0,#0
	mov	r5,r0
	mul	r6,r10,r9
	add	sp,sp,#24
.40:
	nop
	cmp	r5,r7
	nop
	bge	.41
	sub	sp,sp,#8
	add	r0,sp,#28
	ldr	r0,[r0]
	push	{r0}
	push	{r9}
	ldr	r0,[r8,r5,lsl 2]
	nop
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	nop
	movw	r4,#:lower16:b
	movt	r4,#:upper16:b
	mov	r2,r4
	mov	r3,r10
	bl	convn
	nop
	add	sp,sp,#8
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	nop
	mov	r1,r4
	mov	r2,r6
	bl	memmove
	add	r0,r5,#1
	mov	r5,r0
	add	sp,sp,#8
	b	.40
.41:
	nop
	movw	r0,#119
	nop
	bl	_sysy_stoptime
	mul	r0,r10,r9
	nop
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
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
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global getvalue
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	getvalue, %function
getvalue:
	push	{r4,r5,r6,fp,lr}
	nop
	add	fp,sp,#20
	sub	sp,sp,#4
	nop
	mov	r4,r0
	mov	r5,r1
	mov	r6,r2
	movs	r1,r3
	nop
	nop
	nop
	blt	.24
	nop
	add	r0,fp,#0
	ldr	r2,[r0]
	cmp	r2,#0
	nop
	blt	.24
	nop
	cmp	r1,r5
	nop
	bge	.24
	nop
	cmp	r2,r6
	nop
	bge	.24
	nop
	nop
	b	.25
.24:
	nop
	movw	r0,#0
	nop
	nop
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,fp,pc}
	nop
.25:
	nop
	mul	r0,r1,r6
	add	r0,r0,r2
	ldr	r0,[r4,r0,lsl 2]
	nop
	nop
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,fp,pc}
	nop
	.size	getvalue, .-getvalue
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global convn
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	convn, %function
convn:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	nop
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#96
	mov	r4,r0
	add	r0,sp,#80
	str	r4,[r0]
	nop
	add	r0,sp,#84
	str	r1,[r0]
	mov	r1,r2
	add	r0,sp,#88
	str	r1,[r0]
	mov	r0,r3
	add	r1,sp,#92
	str	r0,[r1]
	sub	sp,sp,#8
	movw	r0,#0
	mov	r1,r0
	add	r5,sp,#80
	str	r1,[r5]
	nop
	add	r5,sp,#76
	str	r0,[r5]
	add	r0,fp,#4
	ldr	r0,[r0]
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idiv
	nop
	nop
	add	r2,sp,#72
	str	r0,[r2]
	add	sp,sp,#8
.26:
	sub	sp,sp,#16
	movw	r0,#0
	nop
	add	r5,sp,#84
	str	r0,[r5]
	add	r5,sp,#88
	ldr	r0,[r5]
	add	r2,sp,#80
	ldr	r1,[r2]
	sub	r4,r0,r1
	ldr	r1,[r5]
	ldr	r0,[r2]
	add	r3,r1,r0
	add	r2,fp,#0
	ldr	r2,[r2]
	ldr	r0,[r5]
	mul	r2,r0,r2
	mov	r1,r4
	add	r0,sp,#40
	str	r1,[r0]
	mov	r1,r3
	add	r0,sp,#36
	str	r1,[r0]
	mov	r1,r2
	add	r0,sp,#32
	str	r1,[r0]
	add	sp,sp,#16
.27:
	sub	sp,sp,#8
	movw	r0,#0
	mov	r9,r0
	add	r0,sp,#32
	ldr	r0,[r0]
	nop
	add	r3,sp,#68
	str	r0,[r3]
	add	r5,sp,#76
	ldr	r1,[r5]
	add	r2,sp,#72
	ldr	r0,[r2]
	sub	r3,r1,r0
	ldr	r1,[r5]
	ldr	r0,[r2]
	add	r2,r1,r0
	mov	r1,r3
	add	r0,sp,#20
	str	r1,[r0]
	mov	r1,r2
	add	r0,sp,#16
	str	r1,[r0]
	add	sp,sp,#8
.28:
	sub	sp,sp,#0
	add	r0,sp,#12
	ldr	r0,[r0]
	mov	r10,r0
	add	sp,sp,#0
.29:
	sub	sp,sp,#8
	sub	sp,sp,#4
	push	{r10}
	add	r0,sp,#100
	ldr	r0,[r0]
	nop
	add	r1,sp,#108
	ldr	r1,[r1]
	nop
	add	r2,fp,#0
	ldr	r2,[r2]
	nop
	add	r3,sp,#76
	ldr	r3,[r3]
	nop
	bl	getvalue
	mov	r8,r0
	add	sp,sp,#8
	add	r0,sp,#88
	ldr	r1,[r0]
	cmp	r1,#0
	add	sp,sp,#8
	bne	.46
	nop
	add	r0,r9,r8
	nop
	nop
.47:
	nop
	cmp	r0,#1073741824
	nop
	ble	.48
	nop
	sub	r0,r0,#1073741824
	nop
	nop
	b	.47
.48:
	nop
	nop
.49:
	nop
	cmp	r0,#0
	nop
	bge	.50
	nop
	add	r0,r0,#1073741824
	nop
	nop
	b	.49
.50:
	nop
	mov	r1,r0
	nop
	b	.70
.46:
	nop
	ldr	r1,[r0]
	cmp	r1,#1
	nop
	bne	.51
	nop
	movw	r0,#0
	mov	r6,r0
	movw	r0,#1
	mov	r7,r0
	nop
.52:
	nop
	cmp	r7,#1073741824
	nop
	bge	.53
	nop
	mov	r0,r9
	mov	r1,r7
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r4,r1
	mov	r0,r8
	mov	r1,r7
	bl	__aeabi_idiv
	nop
	nop
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r4,r0
	nop
	bne	.54
	nop
	movw	r5,#2
	mul	r0,r6,r5
	mov	r6,r0
	nop
	b	.55
.54:
	nop
	movw	r5,#2
	mul	r0,r6,r5
	add	r0,r0,#1
	mov	r6,r0
	nop
.55:
	nop
	movw	r5,#2
	mul	r0,r7,r5
	mov	r7,r0
	nop
	b	.52
.53:
	nop
	mov	r1,r6
	nop
	b	.70
.51:
	nop
	ldr	r0,[r0]
	cmp	r0,#2
	nop
	bne	.56
	nop
	cmp	r9,r8
	nop
	ble	.57
	nop
	mov	r1,r9
	nop
	b	.70
.57:
	nop
	mov	r1,r8
	nop
	b	.70
.58:
	nop
	nop
.56:
	sub	sp,sp,#8
	add	r0,sp,#88
	ldr	r1,[r0]
	cmp	r1,#3
	add	sp,sp,#8
	bne	.59
	nop
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
	nop
.60:
	nop
	cmp	r6,#1073741824
	nop
	bge	.61
	nop
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	beq	.62
	nop
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	beq	.62
	nop
	nop
	b	.63
.62:
	nop
	movw	r5,#2
	mul	r0,r4,r5
	add	r0,r0,#1
	mov	r4,r0
	nop
	b	.64
.63:
	nop
	movw	r5,#2
	mul	r0,r4,r5
	mov	r4,r0
	nop
.64:
	nop
	movw	r5,#2
	mul	r0,r6,r5
	mov	r6,r0
	nop
	b	.60
.61:
	nop
	mov	r1,r4
	nop
	b	.70
.59:
	nop
	ldr	r0,[r0]
	cmp	r0,#4
	nop
	bne	.65
	nop
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
	nop
.66:
	nop
	cmp	r6,#1073741824
	nop
	bge	.67
	nop
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	bne	.68
	nop
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	bne	.68
	nop
	movw	r5,#2
	mul	r0,r4,r5
	add	r0,r0,#1
	mov	r4,r0
	nop
	b	.69
.68:
	nop
	movw	r5,#2
	mul	r0,r4,r5
	mov	r4,r0
	nop
.69:
	nop
	movw	r5,#2
	mul	r0,r6,r5
	mov	r6,r0
	nop
	b	.66
.67:
	nop
	mov	r1,r4
	nop
	b	.70
.65:
	nop
	movw	r0,#0
	mov	r1,r0
	nop
	b	.70
.70:
	sub	sp,sp,#8
	mov	r9,r1
	add	r1,r10,#1
	mov	r10,r1
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r1,r0
	add	sp,sp,#8
	blt	.30
	nop
	nop
	b	.31
.30:
	nop
	nop
	b	.29
.31:
	sub	sp,sp,#8
	add	r3,sp,#68
	ldr	r0,[r3]
	add	r1,r0,#1
	mov	r0,r1
	str	r0,[r3]
	add	r0,sp,#28
	ldr	r0,[r0]
	cmp	r1,r0
	add	sp,sp,#8
	blt	.32
	nop
	nop
	b	.33
.32:
	nop
	nop
	b	.28
.33:
	sub	sp,sp,#16
	add	r5,sp,#84
	ldr	r1,[r5]
	add	r0,sp,#32
	ldr	r0,[r0]
	add	r4,r0,r1
	mov	r3,r9
	ldr	r0,[r5]
	add	r1,r0,#1
	mov	r0,r1
	str	r0,[r5]
	add	r2,fp,#0
	ldr	r2,[r2]
	cmp	r1,r2
	add	r0,sp,#104
	ldr	r0,[r0]
	str	r3,[r0,r4,lsl 2]
	add	sp,sp,#16
	blt	.34
	nop
	nop
	b	.35
.34:
	nop
	nop
	b	.27
.35:
	sub	sp,sp,#8
	add	r5,sp,#80
	ldr	r0,[r5]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r5]
	add	r1,sp,#100
	ldr	r0,[r1]
	cmp	r2,r0
	add	sp,sp,#8
	blt	.36
	nop
	nop
	b	.37
.36:
	nop
	nop
	b	.26
.37:
	nop
	movw	r0,#0
	nop
	add	sp,sp,#96
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	nop
	.size	convn, .-convn
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global checkrange
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	checkrange, %function
checkrange:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#0
	nop
	nop
	nop
	nop
.0:
	nop
	cmp	r0,#1073741824
	nop
	ble	.1
	nop
	sub	r0,r0,#1073741824
	nop
	nop
	b	.0
.1:
	nop
	nop
.2:
	nop
	cmp	r0,#0
	nop
	bge	.3
	nop
	add	r0,r0,#1073741824
	nop
	nop
	b	.2
.3:
	nop
	nop
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	nop
	.size	checkrange, .-checkrange
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global reduce
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	reduce, %function
reduce:
	push	{r4,r5,r6,r7,r8,r9,fp,lr}
	nop
	add	fp,sp,#32
	sub	sp,sp,#0
	nop
	movs	r3,r0
	mov	r8,r1
	mov	r9,r2
	nop
	nop
	nop
	bne	.4
	nop
	add	r0,r8,r9
	nop
	nop
.42:
	nop
	cmp	r0,#1073741824
	nop
	ble	.43
	nop
	sub	r0,r0,#1073741824
	nop
	nop
	b	.42
.43:
	nop
	nop
.44:
	nop
	cmp	r0,#0
	nop
	bge	.45
	nop
	add	r0,r0,#1073741824
	nop
	nop
	b	.44
.45:
	nop
	nop
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	nop
.4:
	nop
	cmp	r3,#1
	nop
	bne	.9
	nop
	movw	r0,#0
	mov	r6,r0
	movw	r0,#1
	mov	r7,r0
	nop
.5:
	nop
	cmp	r7,#1073741824
	nop
	bge	.8
	nop
	mov	r0,r8
	mov	r1,r7
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r4,r1
	mov	r0,r9
	mov	r1,r7
	bl	__aeabi_idiv
	nop
	nop
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r4,r0
	nop
	bne	.6
	nop
	movw	r5,#2
	mul	r0,r6,r5
	mov	r6,r0
	nop
	b	.7
.6:
	nop
	movw	r5,#2
	mul	r0,r6,r5
	add	r0,r0,#1
	mov	r6,r0
	nop
.7:
	nop
	movw	r5,#2
	mul	r0,r7,r5
	mov	r7,r0
	nop
	b	.5
.8:
	nop
	mov	r0,r6
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	nop
.9:
	nop
	cmp	r3,#2
	nop
	bne	.12
	nop
	cmp	r8,r9
	nop
	ble	.10
	nop
	mov	r0,r8
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	nop
.10:
	nop
	mov	r0,r9
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	nop
.11:
	nop
	nop
.12:
	nop
	cmp	r3,#3
	nop
	bne	.18
	nop
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
	nop
.13:
	nop
	cmp	r6,#1073741824
	nop
	bge	.17
	nop
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	beq	.14
	nop
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	beq	.14
	nop
	nop
	b	.15
.14:
	nop
	movw	r5,#2
	mul	r0,r4,r5
	add	r0,r0,#1
	mov	r4,r0
	nop
	b	.16
.15:
	nop
	movw	r5,#2
	mul	r0,r4,r5
	mov	r4,r0
	nop
.16:
	nop
	movw	r5,#2
	mul	r0,r6,r5
	mov	r6,r0
	nop
	b	.13
.17:
	nop
	mov	r0,r4
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	nop
.18:
	nop
	cmp	r3,#4
	nop
	bne	.23
	nop
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
	nop
.19:
	nop
	cmp	r6,#1073741824
	nop
	bge	.22
	nop
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	bne	.20
	nop
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	nop
	nop
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	nop
	bne	.20
	nop
	movw	r5,#2
	mul	r0,r4,r5
	add	r0,r0,#1
	mov	r4,r0
	nop
	b	.21
.20:
	nop
	movw	r5,#2
	mul	r0,r4,r5
	mov	r4,r0
	nop
.21:
	nop
	movw	r5,#2
	mul	r0,r6,r5
	mov	r6,r0
	nop
	b	.19
.22:
	nop
	mov	r0,r4
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	nop
.23:
	nop
	movw	r0,#0
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	nop
	.size	reduce, .-reduce
	.comm a,40000000,4
	.comm b,40000000,4
	.comm kernelid,40000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
