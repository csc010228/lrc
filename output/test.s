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
	push	{r0}
.0:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#1073741824
	ble	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	sub	r1,r0,#1073741824
	add	r0,sp,#0
	str	r1,[r0]
	b	.0
.1:
.2:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#0
	bge	.3
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,r0,#1073741824
	add	r0,sp,#0
	str	r1,[r0]
	b	.2
.3:
	add	r0,sp,#0
	ldr	r0,[r0]
	add	sp,sp,#4
	nop
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1,r2}
	sub	sp,sp,#24
	cmp	r0,#0
	bne	.4
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,sp,#32
	ldr	r1,[r1]
	add	r2,r0,r1
	push	{r2}
	mov	r0,r2
	bl	checkrange
	add	sp,sp,#40
	nop
	pop	{fp,pc}
	add	sp,sp,#4
.4:
	add	r0,sp,#24
	ldr	r0,[r0]
	cmp	r0,#1
	bne	.9
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
.5:
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#1073741824
	bge	.8
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,sp,#4
	ldr	r1,[r1]
	bl	__aeabi_idiv
	push	{r0}
	movw	r1,#2
	bl	__aeabi_idivmod
	push	{r1}
	add	r0,sp,#40
	ldr	r0,[r0]
	add	r1,sp,#12
	ldr	r1,[r1]
	bl	__aeabi_idiv
	push	{r0}
	movw	r1,#2
	bl	__aeabi_idivmod
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,r1
	add	sp,sp,#12
	bne	.6
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,sp,#0
	str	r2,[r0]
	b	.7
.6:
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,r2,#1
	add	r1,sp,#0
	str	r0,[r1]
.7:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,sp,#4
	str	r2,[r0]
	b	.5
.8:
	add	r0,sp,#0
	ldr	r0,[r0]
	add	sp,sp,#36
	nop
	pop	{fp,pc}
.9:
	add	r0,sp,#24
	ldr	r0,[r0]
	cmp	r0,#2
	bne	.12
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,sp,#32
	ldr	r1,[r1]
	cmp	r0,r1
	ble	.10
	add	r0,sp,#28
	ldr	r0,[r0]
	add	sp,sp,#36
	nop
	pop	{fp,pc}
.10:
	add	r0,sp,#32
	ldr	r0,[r0]
	add	sp,sp,#36
	nop
	pop	{fp,pc}
.11:
.12:
	add	r0,sp,#24
	ldr	r0,[r0]
	cmp	r0,#3
	bne	.18
	movw	r0,#0
	add	r1,sp,#8
	str	r0,[r1]
	movw	r0,#1
	add	r1,sp,#12
	str	r0,[r1]
.13:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#1073741824
	bge	.17
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,sp,#12
	ldr	r1,[r1]
	bl	__aeabi_idiv
	push	{r0}
	movw	r1,#2
	bl	__aeabi_idivmod
	cmp	r1,#1
	add	sp,sp,#4
	beq	.14
	add	r0,sp,#32
	ldr	r0,[r0]
	add	r1,sp,#12
	ldr	r1,[r1]
	bl	__aeabi_idiv
	push	{r0}
	movw	r1,#2
	bl	__aeabi_idivmod
	cmp	r1,#1
	add	sp,sp,#4
	beq	.14
	b	.15
.14:
	add	r0,sp,#8
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,r2,#1
	add	r1,sp,#8
	str	r0,[r1]
	b	.16
.15:
	add	r0,sp,#8
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,sp,#8
	str	r2,[r0]
.16:
	add	r0,sp,#12
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,sp,#12
	str	r2,[r0]
	b	.13
.17:
	add	r0,sp,#8
	ldr	r0,[r0]
	add	sp,sp,#36
	nop
	pop	{fp,pc}
.18:
	add	r0,sp,#24
	ldr	r0,[r0]
	cmp	r0,#4
	bne	.23
	movw	r0,#0
	add	r1,sp,#16
	str	r0,[r1]
	movw	r0,#1
	add	r1,sp,#20
	str	r0,[r1]
.19:
	add	r0,sp,#20
	ldr	r0,[r0]
	cmp	r0,#1073741824
	bge	.22
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,sp,#20
	ldr	r1,[r1]
	bl	__aeabi_idiv
	push	{r0}
	movw	r1,#2
	bl	__aeabi_idivmod
	cmp	r1,#1
	add	sp,sp,#4
	bne	.20
	add	r0,sp,#32
	ldr	r0,[r0]
	add	r1,sp,#20
	ldr	r1,[r1]
	bl	__aeabi_idiv
	push	{r0}
	movw	r1,#2
	bl	__aeabi_idivmod
	cmp	r1,#1
	add	sp,sp,#4
	bne	.20
	add	r0,sp,#16
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,r2,#1
	add	r1,sp,#16
	str	r0,[r1]
	b	.21
.20:
	add	r0,sp,#16
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,sp,#16
	str	r2,[r0]
.21:
	add	r0,sp,#20
	ldr	r0,[r0]
	movw	r1,#2
	mul	r2,r0,r1
	add	r0,sp,#20
	str	r2,[r0]
	b	.19
.22:
	add	r0,sp,#16
	ldr	r0,[r0]
	add	sp,sp,#36
	nop
	pop	{fp,pc}
.23:
	movw	r0,#0
	add	sp,sp,#36
	nop
	pop	{fp,pc}
	.size	reduce, .-reduce
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1,r2}
	sub	sp,sp,#4
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.38:
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,sp,#12
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.39
	add	r0,sp,#0
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	add	r0,sp,#24
	ldr	r0,[r0]
	add	r1,sp,#4
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	add	r1,sp,#20
	ldr	r1,[r1]
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,sp,#16
	str	r3,[r0]
	add	sp,sp,#16
	b	.38
.39:
	add	sp,sp,#16
	nop
	pop	{fp,pc}
	.size	memmove, .-memmove
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1,r2,r3}
	cmp	r3,#0
	blt	.24
	add	r0,fp,#0
	ldr	r0,[r0]
	cmp	r0,#0
	blt	.24
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,sp,#4
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.24
	add	r0,fp,#0
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.24
	b	.25
.24:
	movw	r0,#0
	add	sp,sp,#16
	nop
	pop	{fp,pc}
.25:
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	mul	r2,r0,r1
	add	r0,fp,#0
	ldr	r0,[r0]
	add	r1,r2,r0
	add	r0,sp,#0
	ldr	r0,[r0]
	ldr	r0,[r0,r1,lsl 2]
	add	sp,sp,#16
	nop
	pop	{fp,pc}
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0,r1,r2,r3}
	sub	sp,sp,#20
	movw	r0,#0
	add	r1,sp,#8
	str	r0,[r1]
	add	r1,sp,#12
	str	r0,[r1]
.26:
	movw	r0,#0
	add	r1,sp,#8
	str	r0,[r1]
.27:
	movw	r0,#0
	add	r1,sp,#16
	str	r0,[r1]
	str	r0,[r1]
	add	r0,fp,#4
	ldr	r0,[r0]
	movw	r1,#2
	bl	__aeabi_idiv
	push	{r0}
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	sub	r2,r0,r1
	add	r0,sp,#8
	str	r2,[r0]
	add	sp,sp,#4
.28:
	add	r0,fp,#4
	ldr	r0,[r0]
	movw	r1,#2
	bl	__aeabi_idiv
	push	{r0}
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	sub	r2,r0,r1
	add	r0,sp,#4
	str	r2,[r0]
	add	sp,sp,#4
.29:
	add	r0,sp,#0
	ldr	r0,[r0]
	push	{r0}
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,sp,#36
	ldr	r1,[r1]
	add	r2,fp,#0
	ldr	r2,[r2]
	add	r3,sp,#8
	ldr	r3,[r3]
	bl	getvalue
	add	sp,sp,#4
	push	{r0}
	add	r0,sp,#24
	ldr	r0,[r0]
	add	r1,sp,#20
	ldr	r1,[r1]
	add	r2,sp,#0
	ldr	r2,[r2]
	bl	reduce
	push	{r0}
	add	r1,sp,#24
	str	r0,[r1]
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r2,r0,#1
	push	{r2}
	add	r0,sp,#12
	str	r2,[r0]
	add	r0,fp,#4
	ldr	r0,[r0]
	movw	r1,#2
	bl	__aeabi_idiv
	push	{r0}
	add	r0,sp,#24
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	add	r2,r0,r1
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,r2
	add	sp,sp,#16
	blt	.30
	b	.31
.30:
	b	.29
.31:
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,r0,#1
	push	{r1}
	add	r0,sp,#8
	str	r1,[r0]
	add	r0,fp,#4
	ldr	r0,[r0]
	add	r2,sp,#8
	str	r1,[r2]
	movw	r1,#2
	bl	__aeabi_idiv
	push	{r0}
	add	r0,sp,#20
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	add	r2,r0,r1
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,r2
	add	sp,sp,#8
	blt	.32
	b	.33
.32:
	b	.28
.33:
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,fp,#0
	ldr	r1,[r1]
	mul	r2,r0,r1
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r2,r0
	add	r0,sp,#16
	ldr	r0,[r0]
	push	{r1}
	push	{r1}
	push	{r1}
	add	r1,sp,#40
	ldr	r1,[r1]
	push	{r2}
	add	r2,sp,#8
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#24
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,fp,#0
	ldr	r0,[r0]
	cmp	r3,r0
	add	r0,sp,#24
	str	r3,[r0]
	add	sp,sp,#16
	blt	.34
	b	.35
.34:
	b	.27
.35:
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r1,r0
	add	r0,sp,#12
	str	r1,[r0]
	blt	.36
	b	.37
.36:
	b	.26
.37:
	movw	r0,#0
	add	sp,sp,#36
	nop
	pop	{fp,pc}
	.size	convn, .-convn
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
	sub	sp,sp,#20
	bl	getint
	push	{r0}
	bl	getint
	push	{r0}
	bl	getint
	push	{r0}
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,sp,#28
	str	r0,[r1]
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#16
	str	r0,[r1]
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,sp,#20
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	bl	getarray
	push	{r0}
	movw	r0,#:lower16:kernelid
	movt	r0,#:upper16:kernelid
	bl	getarray
	push	{r0}
	add	r1,sp,#20
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#111
	bl	_sysy_starttime
	movw	r0,#0
	add	r1,sp,#32
	str	r0,[r1]
	add	sp,sp,#20
.40:
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,sp,#0
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.41
	add	r0,sp,#12
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	add	r0,sp,#24
	ldr	r0,[r0]
	push	{r0}
	add	r0,sp,#20
	ldr	r0,[r0]
	push	{r0}
	movw	r0,#:lower16:kernelid
	movt	r0,#:upper16:kernelid
	add	r1,sp,#8
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	movw	r2,#:lower16:b
	movt	r2,#:upper16:b
	add	r3,sp,#20
	ldr	r3,[r3]
	bl	convn
	add	sp,sp,#8
	push	{r0}
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r1,sp,#20
	ldr	r1,[r1]
	mul	r2,r0,r1
	push	{r2}
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	movw	r1,#:lower16:b
	movt	r1,#:upper16:b
	bl	memmove
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#28
	str	r1,[r0]
	add	sp,sp,#16
	b	.40
.41:
	movw	r0,#118
	bl	_sysy_stoptime
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	mul	r2,r0,r1
	push	{r2}
	mov	r0,r2
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	bl	putarray
	movw	r0,#0
	add	sp,sp,#24
	nop
	pop	{fp,pc}
	add	sp,sp,#4
	.size	main, .-main
	.comm a,40000000,4
	.comm b,40000000,4
	.comm kernelid,40000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
