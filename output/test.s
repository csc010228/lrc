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
	push	{r4,fp,lr}
	add	fp,sp,#12
	sub	sp,sp,#4
	mov	r4,r0
	mov	r3,r1
	movw	r0,#0
	mov	r1,r0
.38:
	cmp	r1,r2
	bge	.39
	ldr	r0,[r3,r1,lsl 2]
	str	r0,[r4,r1,lsl 2]
	add	r1,r1,#1
	b	.38
.39:
	add	sp,sp,#4
	pop	{r4,fp,pc}
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
	mov	r1,r5
	add	r0,sp,#32
	add	r0,sp,#32
	str	r1,[r0]
	mov	r10,r2
	movw	r5,#:lower16:a
	movt	r5,#:upper16:a
	mov	r0,r5
	bl	getarray
	movw	r9,#:lower16:kernelid
	movt	r9,#:upper16:kernelid
	mov	r0,r9
	bl	getarray
	mov	r8,r0
	movw	r0,#109
	bl	_sysy_starttime
	movw	r0,#0
	mov	r7,r0
	add	r0,sp,#32
	ldr	r0,[r0]
	mul	r6,r0,r10
	add	sp,sp,#24
.40:
	cmp	r7,r8
	bge	.41
	sub	sp,sp,#8
	add	r0,sp,#28
	ldr	r0,[r0]
	push	{r0}
	push	{r10}
	ldr	r0,[r9,r7,lsl 2]
	mov	r1,r5
	movw	r4,#:lower16:b
	movt	r4,#:upper16:b
	mov	r2,r4
	add	r3,sp,#24
	ldr	r3,[r3]
	bl	convn
	add	sp,sp,#8
	mov	r0,r5
	mov	r1,r4
	mov	r2,r6
	bl	memmove
	add	r7,r7,#1
	add	sp,sp,#8
	b	.40
.41:
	sub	sp,sp,#8
	movw	r0,#116
	bl	_sysy_stoptime
	add	r0,sp,#16
	ldr	r0,[r0]
	mul	r0,r0,r10
	mov	r1,r5
	bl	putarray
	movw	r0,#0
	add	sp,sp,#8
	add	sp,sp,#24
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0
	.size	main, .-main
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
	add	fp,sp,#8
	sub	sp,sp,#0
.0:
	cmp	r0,#1073741824
	ble	.1
	sub	r0,r0,#1073741824
	b	.0
.1:
.2:
	cmp	r0,#0
	bge	.3
	add	r0,r0,#1073741824
	b	.2
.3:
	add	sp,sp,#0
	pop	{fp,pc}
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
	add	fp,sp,#32
	sub	sp,sp,#0
	movs	r3,r0
	mov	r9,r1
	mov	r8,r2
	bne	.4
	add	r0,r9,r8
.42:
	cmp	r0,#1073741824
	ble	.43
	sub	r0,r0,#1073741824
	b	.42
.43:
.44:
	cmp	r0,#0
	bge	.45
	add	r0,r0,#1073741824
	b	.44
.45:
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
.4:
	cmp	r3,#1
	bne	.9
	movw	r0,#0
	mov	r6,r0
	movw	r0,#1
	mov	r7,r0
.5:
	cmp	r7,#1073741824
	bge	.8
	mov	r0,r9
	mov	r1,r7
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r4,r1
	mov	r0,r8
	mov	r1,r7
	bl	__aeabi_idiv
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r4,r0
	bne	.6
	movw	r5,#2
	mul	r6,r6,r5
	b	.7
.6:
	movw	r5,#2
	mul	r0,r6,r5
	add	r6,r0,#1
.7:
	movw	r5,#2
	mul	r7,r7,r5
	b	.5
.8:
	mov	r0,r6
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
.9:
	cmp	r3,#2
	bne	.12
	cmp	r9,r8
	ble	.10
	mov	r0,r9
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
.10:
	mov	r0,r8
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
.11:
.12:
	cmp	r3,#3
	bne	.18
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
.13:
	cmp	r6,#1073741824
	bge	.17
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	beq	.14
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	beq	.14
	b	.15
.14:
	movw	r5,#2
	mul	r0,r4,r5
	add	r4,r0,#1
	b	.16
.15:
	movw	r5,#2
	mul	r4,r4,r5
.16:
	movw	r5,#2
	mul	r6,r6,r5
	b	.13
.17:
	mov	r0,r4
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
.18:
	cmp	r3,#4
	bne	.23
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
.19:
	cmp	r6,#1073741824
	bge	.22
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	bne	.20
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	bne	.20
	movw	r5,#2
	mul	r0,r4,r5
	add	r4,r0,#1
	b	.21
.20:
	movw	r5,#2
	mul	r4,r4,r5
.21:
	movw	r5,#2
	mul	r6,r6,r5
	b	.19
.22:
	mov	r0,r4
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
.23:
	movw	r0,#0
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	.size	reduce, .-reduce
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
	add	fp,sp,#20
	sub	sp,sp,#4
	mov	r4,r0
	mov	r5,r1
	mov	r6,r2
	movs	r1,r3
	blt	.24
	add	r0,fp,#0
	ldr	r2,[r0]
	cmp	r2,#0
	blt	.24
	cmp	r1,r5
	bge	.24
	cmp	r2,r6
	bge	.24
	b	.25
.24:
	movw	r0,#0
	add	sp,sp,#4
	pop	{r4,r5,r6,fp,pc}
.25:
	mul	r0,r1,r6
	add	r0,r0,r2
	ldr	r0,[r4,r0,lsl 2]
	add	sp,sp,#4
	pop	{r4,r5,r6,fp,pc}
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
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#96
	add	r4,sp,#80
	str	r0,[r4]
	add	r0,sp,#84
	str	r1,[r0]
	mov	r1,r2
	add	r0,sp,#88
	str	r1,[r0]
	mov	r0,r3
	add	r1,sp,#92
	str	r0,[r1]
	sub	sp,sp,#0
	movw	r0,#0
	mov	r1,r0
	add	r3,sp,#72
	str	r1,[r3]
	mov	r1,r0
	add	r0,sp,#68
	str	r1,[r0]
	add	r0,fp,#4
	ldr	r4,[r0]
	mov	r0,r4
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idiv
	mov	r1,r0
	add	r0,sp,#28
	str	r1,[r0]
	mov	r0,r4
	mov	r1,r5
	bl	__aeabi_idiv
	mov	r1,r0
	add	r0,sp,#24
	str	r1,[r0]
	mov	r0,r4
	mov	r1,r5
	bl	__aeabi_idiv
	mov	r1,r0
	add	r0,sp,#8
	str	r1,[r0]
	mov	r0,r4
	mov	r1,r5
	bl	__aeabi_idiv
	mov	r1,r0
	add	r0,sp,#16
	str	r1,[r0]
	add	sp,sp,#0
.26:
	sub	sp,sp,#0
	movw	r0,#0
	mov	r1,r0
	add	r0,sp,#68
	str	r1,[r0]
	add	r3,sp,#72
	ldr	r1,[r3]
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r1,r0
	add	r0,sp,#4
	str	r1,[r0]
	add	r2,fp,#0
	ldr	r2,[r2]
	ldr	r0,[r3]
	mul	r0,r0,r2
	add	r1,sp,#0
	str	r0,[r1]
	add	sp,sp,#0
.27:
	sub	sp,sp,#0
	movw	r0,#0
	mov	r9,r0
	add	r3,sp,#72
	ldr	r1,[r3]
	add	r0,sp,#28
	ldr	r0,[r0]
	sub	r10,r1,r0
	add	r0,sp,#68
	ldr	r1,[r0]
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r1,r1,r0
	add	r0,sp,#12
	str	r1,[r0]
	add	sp,sp,#0
.28:
	sub	sp,sp,#0
	add	r0,sp,#68
	ldr	r1,[r0]
	add	r0,sp,#24
	ldr	r0,[r0]
	sub	r0,r1,r0
	add	r1,sp,#60
	str	r0,[r1]
	add	sp,sp,#0
.29:
	sub	sp,sp,#8
	sub	sp,sp,#4
	add	r1,sp,#72
	ldr	r0,[r1]
	push	{r0}
	add	r0,sp,#100
	ldr	r0,[r0]
	add	r1,sp,#108
	ldr	r1,[r1]
	add	r2,fp,#0
	ldr	r2,[r2]
	mov	r3,r10
	bl	getvalue
	mov	r8,r0
	add	sp,sp,#8
	add	r4,sp,#88
	ldr	r0,[r4]
	cmp	r0,#0
	add	sp,sp,#8
	bne	.46
	add	r0,r9,r8
.47:
	cmp	r0,#1073741824
	ble	.48
	sub	r0,r0,#1073741824
	b	.47
.48:
.49:
	cmp	r0,#0
	bge	.50
	add	r0,r0,#1073741824
	b	.49
.50:
	mov	r9,r0
	b	.70
.46:
	ldr	r0,[r4]
	cmp	r0,#1
	bne	.51
	movw	r0,#0
	mov	r6,r0
	movw	r0,#1
	mov	r7,r0
.52:
	cmp	r7,#1073741824
	bge	.53
	mov	r0,r9
	mov	r1,r7
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r4,r1
	mov	r0,r8
	mov	r1,r7
	bl	__aeabi_idiv
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r4,r0
	bne	.54
	movw	r5,#2
	mul	r6,r6,r5
	b	.55
.54:
	movw	r5,#2
	mul	r0,r6,r5
	add	r6,r0,#1
.55:
	movw	r5,#2
	mul	r7,r7,r5
	b	.52
.53:
	mov	r9,r6
	b	.70
.51:
	ldr	r0,[r4]
	cmp	r0,#2
	bne	.56
	cmp	r9,r8
	ble	.57
	b	.70
.57:
	mov	r9,r8
	b	.70
.58:
.56:
	sub	sp,sp,#8
	add	r4,sp,#88
	ldr	r0,[r4]
	cmp	r0,#3
	add	sp,sp,#8
	bne	.59
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
.60:
	cmp	r6,#1073741824
	bge	.61
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	beq	.62
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	beq	.62
	b	.63
.62:
	movw	r5,#2
	mul	r0,r4,r5
	add	r4,r0,#1
	b	.64
.63:
	movw	r5,#2
	mul	r4,r4,r5
.64:
	movw	r5,#2
	mul	r6,r6,r5
	b	.60
.61:
	mov	r9,r4
	b	.70
.59:
	ldr	r0,[r4]
	cmp	r0,#4
	bne	.65
	movw	r0,#0
	mov	r4,r0
	movw	r0,#1
	mov	r6,r0
.66:
	cmp	r6,#1073741824
	bge	.67
	mov	r0,r9
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	bne	.68
	mov	r0,r8
	mov	r1,r6
	bl	__aeabi_idiv
	movw	r5,#2
	mov	r1,r5
	bl	__aeabi_idivmod
	mov	r0,r1
	cmp	r0,#1
	bne	.68
	movw	r5,#2
	mul	r0,r4,r5
	add	r4,r0,#1
	b	.69
.68:
	movw	r5,#2
	mul	r4,r4,r5
.69:
	movw	r5,#2
	mul	r6,r6,r5
	b	.66
.67:
	mov	r9,r4
	b	.70
.65:
	movw	r0,#0
	mov	r9,r0
	b	.70
.70:
	sub	sp,sp,#8
	add	r1,sp,#68
	ldr	r0,[r1]
	add	r0,r0,#1
	str	r0,[r1]
	ldr	r1,[r1]
	add	r0,sp,#20
	ldr	r0,[r0]
	cmp	r1,r0
	add	sp,sp,#8
	blt	.30
	b	.31
.30:
	b	.29
.31:
	sub	sp,sp,#8
	add	r10,r10,#1
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r10,r0
	add	sp,sp,#8
	blt	.32
	b	.33
.32:
	b	.28
.33:
	sub	sp,sp,#8
	add	r0,sp,#76
	ldr	r2,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	add	r4,r1,r2
	mov	r3,r9
	ldr	r1,[r0]
	add	r1,r1,#1
	str	r1,[r0]
	add	r2,fp,#0
	ldr	r2,[r2]
	ldr	r0,[r0]
	cmp	r0,r2
	add	r0,sp,#96
	ldr	r0,[r0]
	str	r3,[r0,r4,lsl 2]
	add	sp,sp,#8
	blt	.34
	b	.35
.34:
	b	.27
.35:
	sub	sp,sp,#8
	add	r3,sp,#80
	ldr	r0,[r3]
	add	r0,r0,#1
	str	r0,[r3]
	add	r1,sp,#100
	ldr	r1,[r1]
	ldr	r0,[r3]
	cmp	r0,r1
	add	sp,sp,#8
	blt	.36
	b	.37
.36:
	b	.26
.37:
	movw	r0,#0
	add	sp,sp,#96
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	convn, .-convn
	.comm a,40000000,4
	.comm b,40000000,4
	.comm kernelid,40000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
