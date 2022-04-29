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
	.global getMaxNum
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	getMaxNum, %function
getMaxNum:
	push	{fp,lr}
	add	fp,sp,#8
	push	{r4,r5,r6}
	nop
	push	{r0,r1}
	sub	sp,sp,#8
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.0:
	add	r1,sp,#0
	ldr	r0,[r1]
	add	r3,sp,#8
	ldr	r2,[r3]
	cmp	r0,r2
	bge	.2
	add	r1,sp,#0
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	add	r2,sp,#16
	ldr	r1,[r2]
	push	{r3}
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	add	r6,sp,#12
	ldr	r5,[r6]
	cmp	r0,r5
	add	sp,sp,#8
	ble	.1
	add	r1,sp,#0
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	add	r2,sp,#16
	ldr	r1,[r2]
	push	{r3}
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	mov	r5,r0
	add	r0,sp,#12
	str	r5,[r0]
	add	sp,sp,#8
.1:
	add	r1,sp,#0
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	b	.0
.2:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#16
	nop
	pop	{r4,r5,r6,fp,pc}
	.size	getMaxNum, .-getMaxNum
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
	add	fp,sp,#8
	push	{r4}
	nop
	sub	sp,sp,#8
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	bl	getarray
	mov	r1,r0
	push	{r0}
	add	r0,sp,#8
	str	r1,[r0]
	movw	r0,#90
	bl	_sysy_starttime
	movw	r0,#8
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	movw	r2,#0
	add	r4,sp,#8
	ldr	r3,[r4]
	bl	radixSort
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
	add	sp,sp,#4
.20:
	add	r1,sp,#0
	ldr	r0,[r1]
	add	r3,sp,#4
	ldr	r2,[r3]
	cmp	r0,r2
	bge	.21
	add	r1,sp,#0
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	add	r2,r0,#2
	push	{r2}
	push	{r3}
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#4
	ldr	r1,[r2]
	bl	__aeabi_idivmod
	push	{r1}
	add	r1,sp,#16
	ldr	r0,[r1]
	add	r3,sp,#0
	ldr	r2,[r3]
	mul	r4,r0,r2
	movw	r1,#:lower16:ans
	movt	r1,#:upper16:ans
	ldr	r0,[r1]
	add	r2,r0,r4
	mov	r0,r2
	str	r0,[r1]
	push	{r2}
	add	r2,sp,#20
	ldr	r0,[r2]
	add	r3,r0,#1
	mov	r0,r3
	str	r0,[r2]
	add	sp,sp,#20
	b	.20
.21:
	movw	r1,#:lower16:ans
	movt	r1,#:upper16:ans
	ldr	r0,[r1]
	cmp	r0,#0
	bge	.22
	movw	r1,#:lower16:ans
	movt	r1,#:upper16:ans
	ldr	r0,[r1]
	rsb	r2,r0,#0
	mov	r0,r2
	str	r0,[r1]
.22:
	movw	r0,#102
	bl	_sysy_stoptime
	movw	r1,#:lower16:ans
	movt	r1,#:upper16:ans
	ldr	r0,[r1]
	bl	putint
	movw	r0,#10
	bl	putch
	movw	r0,#0
	add	sp,sp,#8
	nop
	pop	{r4,fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global getNumPos
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	getNumPos, %function
getNumPos:
	push	{fp,lr}
	add	fp,sp,#8
	nop
	nop
	push	{r0,r1}
	sub	sp,sp,#8
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.3:
	add	r1,sp,#0
	ldr	r0,[r1]
	add	r3,sp,#12
	ldr	r2,[r3]
	cmp	r0,r2
	bge	.4
	add	r1,sp,#8
	ldr	r0,[r1]
	movw	r1,#16
	bl	__aeabi_idiv
	mov	r1,r0
	push	{r0}
	add	r2,sp,#12
	str	r1,[r2]
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r3,r0,#1
	mov	r0,r3
	str	r0,[r1]
	add	sp,sp,#4
	b	.3
.4:
	add	r1,sp,#8
	ldr	r0,[r1]
	movw	r1,#16
	bl	__aeabi_idivmod
	mov	r0,r1
	add	sp,sp,#16
	nop
	pop	{fp,pc}
	.size	getNumPos, .-getNumPos
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global radixSort
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	radixSort, %function
radixSort:
	push	{fp,lr}
	add	fp,sp,#8
	push	{r4,r5,r6,r7,r8,r9,r10}
	nop
	push	{r0,r1,r2,r3}
	sub	sp,sp,#208
	add	r0,sp,#144
	movw	r1,#0
	movw	r2,#64
	bl	memset
	add	r0,sp,#80
	movw	r1,#0
	movw	r2,#64
	bl	memset
	add	r0,sp,#16
	movw	r1,#0
	movw	r2,#64
	bl	memset
	add	r1,sp,#208
	ldr	r0,[r1]
	movw	r2,#65535
	movt	r2,#65535
	cmp	r0,r2
	beq	.5
	add	r1,sp,#216
	ldr	r0,[r1]
	add	r2,r0,#1
	add	r1,sp,#220
	ldr	r0,[r1]
	cmp	r2,r0
	bge	.5
	b	.6
.5:
	add	sp,sp,#224
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.6:
	add	r1,sp,#216
	ldr	r0,[r1]
	mov	r2,r0
	add	r0,sp,#12
	str	r2,[r0]
.7:
	add	r1,sp,#12
	ldr	r0,[r1]
	add	r3,sp,#220
	ldr	r2,[r3]
	cmp	r0,r2
	bge	.8
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	push	{r3}
	add	r2,sp,#220
	ldr	r1,[r2]
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	add	r2,sp,#216
	ldr	r1,[r2]
	bl	getNumPos
	movw	r1,#1
	mul	r2,r0,r1
	push	{r0}
	add	r0,r2,#0
	push	{r0}
	add	r1,sp,#28
	ldr	r0,[r1]
	push	{r2}
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	push	{r3}
	add	r2,sp,#240
	ldr	r1,[r2]
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	add	r2,sp,#236
	ldr	r1,[r2]
	bl	getNumPos
	movw	r1,#1
	mul	r2,r0,r1
	push	{r0}
	add	r0,r2,#0
	push	{r0}
	add	r1,sp,#52
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r4,r0,#1
	mov	r0,r4
	add	r3,sp,#24
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	push	{r4}
	add	r4,sp,#56
	ldr	r0,[r4]
	add	r5,r0,#1
	mov	r0,r5
	str	r0,[r4]
	add	sp,sp,#44
	b	.7
.8:
	add	r1,sp,#216
	ldr	r0,[r1]
	mov	r2,r0
	add	r3,sp,#144
	str	r2,[r3,#0]
	add	r2,sp,#16
	ldr	r1,[r2,#0]
	add	r4,r0,r1
	mov	r0,r4
	add	r1,sp,#80
	str	r0,[r1,#0]
	movw	r0,#1
	add	r1,sp,#12
	str	r0,[r1]
.9:
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#16
	bge	.10
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	sub	r2,r0,#1
	movw	r0,#1
	mul	r1,r2,r0
	add	r0,r1,#0
	push	{r0}
	push	{r1}
	add	r1,sp,#92
	push	{r2}
	push	{r3}
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	mov	r4,r0
	add	r1,sp,#32
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	add	r1,sp,#184
	push	{r3}
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r6,sp,#40
	ldr	r5,[r6]
	str	r4,[r1,r5,lsl 2]
	add	r7,sp,#60
	add	r9,sp,#4
	ldr	r8,[r9]
	ldr	r4,[r7,r8,lsl 2]
	add	r10,r0,r4
	mov	r0,r10
	add	r1,sp,#124
	add	r3,sp,#20
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r4,sp,#56
	ldr	r0,[r4]
	add	r5,r0,#1
	mov	r0,r5
	str	r0,[r4]
	add	sp,sp,#44
	b	.9
.10:
	movw	r0,#0
	add	r1,sp,#12
	str	r0,[r1]
.11:
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#16
	bge	.16
.12:
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	add	r1,sp,#156
	push	{r3}
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r5,sp,#96
	add	r7,sp,#4
	ldr	r6,[r7]
	ldr	r4,[r5,r6,lsl 2]
	cmp	r0,r4
	add	sp,sp,#16
	bge	.15
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	add	r1,sp,#148
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r4,#1
	mul	r5,r0,r4
	add	r0,r5,#0
	push	{r0}
	add	r2,sp,#224
	ldr	r1,[r2]
	add	r4,sp,#0
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	push	{r5}
	mov	r5,r0
	add	r0,sp,#24
	str	r5,[r0]
	add	sp,sp,#16
.13:
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r2,sp,#208
	ldr	r1,[r2]
	bl	getNumPos
	add	r2,sp,#12
	ldr	r1,[r2]
	cmp	r0,r1
	beq	.14
	add	r1,sp,#8
	ldr	r0,[r1]
	mov	r2,r0
	add	r0,sp,#4
	str	r2,[r0]
	mov	r0,r2
	add	r2,sp,#208
	ldr	r1,[r2]
	bl	getNumPos
	movw	r1,#1
	mul	r2,r0,r1
	push	{r0}
	add	r0,r2,#0
	push	{r0}
	add	r1,sp,#152
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r4,#1
	mul	r5,r0,r4
	add	r0,r5,#0
	push	{r0}
	add	r2,sp,#228
	ldr	r1,[r2]
	add	r4,sp,#0
	ldr	r3,[r4]
	ldr	r0,[r1,r3,lsl 2]
	push	{r5}
	mov	r5,r0
	add	r0,sp,#28
	str	r5,[r0]
	add	r1,sp,#24
	ldr	r0,[r1]
	add	r2,sp,#228
	ldr	r1,[r2]
	bl	getNumPos
	movw	r1,#1
	mul	r2,r0,r1
	push	{r0}
	add	r0,r2,#0
	push	{r0}
	add	r1,sp,#172
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r4,#1
	mul	r5,r0,r4
	add	r0,r5,#0
	push	{r0}
	add	r1,sp,#40
	ldr	r0,[r1]
	mov	r2,r0
	add	r1,sp,#248
	ldr	r0,[r1]
	add	r4,sp,#0
	ldr	r3,[r4]
	str	r2,[r0,r3,lsl 2]
	push	{r5}
	add	r1,sp,#44
	ldr	r0,[r1]
	add	r2,sp,#248
	ldr	r1,[r2]
	bl	getNumPos
	movw	r1,#1
	mul	r2,r0,r1
	push	{r0}
	add	r0,r2,#0
	push	{r0}
	push	{r2}
	add	r1,sp,#56
	ldr	r0,[r1]
	add	r2,sp,#260
	ldr	r1,[r2]
	bl	getNumPos
	movw	r1,#1
	mul	r2,r0,r1
	push	{r0}
	add	r0,r2,#0
	push	{r0}
	add	r1,sp,#204
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r4,r0,#1
	mov	r0,r4
	add	r3,sp,#16
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#64
	b	.13
.14:
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	add	r1,sp,#148
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r4,#1
	mul	r5,r0,r4
	add	r0,r5,#0
	push	{r0}
	add	r1,sp,#20
	ldr	r0,[r1]
	mov	r2,r0
	add	r1,sp,#24
	ldr	r0,[r1]
	push	{r5}
	add	r5,sp,#228
	ldr	r3,[r5]
	add	r7,sp,#4
	ldr	r6,[r7]
	str	r2,[r3,r6,lsl 2]
	mul	r2,r0,r4
	add	r0,r2,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r2}
	mul	r2,r0,r4
	add	r0,r2,#0
	push	{r0}
	add	r1,sp,#172
	push	{r2}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r4,r0,#1
	mov	r0,r4
	add	r3,sp,#12
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#32
	b	.12
.15:
	add	r1,sp,#12
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	b	.11
.16:
	add	r1,sp,#216
	ldr	r0,[r1]
	mov	r2,r0
	mov	r1,r0
	add	r3,sp,#0
	str	r2,[r3]
	add	r2,sp,#144
	str	r1,[r2,#0]
	add	r4,sp,#16
	ldr	r1,[r4,#0]
	add	r5,r0,r1
	mov	r0,r5
	add	r1,sp,#80
	str	r0,[r1,#0]
	movw	r0,#0
	str	r0,[r3]
.17:
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#16
	bge	.19
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#0
	ble	.18
	add	r1,sp,#0
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	sub	r2,r0,#1
	movw	r0,#1
	mul	r1,r2,r0
	add	r0,r1,#0
	push	{r0}
	push	{r1}
	add	r1,sp,#92
	push	{r2}
	push	{r3}
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	mov	r4,r0
	add	r1,sp,#20
	ldr	r0,[r1]
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	add	r1,sp,#184
	push	{r3}
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r6,sp,#40
	ldr	r5,[r6]
	str	r4,[r1,r5,lsl 2]
	add	r7,sp,#60
	add	r9,sp,#4
	ldr	r8,[r9]
	ldr	r4,[r7,r8,lsl 2]
	add	r10,r0,r4
	mov	r0,r10
	add	r1,sp,#124
	add	r3,sp,#20
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#44
.18:
	add	r1,sp,#208
	ldr	r0,[r1]
	sub	r2,r0,#1
	add	r1,sp,#0
	ldr	r0,[r1]
	push	{r2}
	movw	r2,#1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	push	{r3}
	add	r1,sp,#16
	ldr	r0,[r1]
	add	r2,sp,#232
	ldr	r1,[r2]
	add	r3,sp,#164
	add	r5,sp,#12
	ldr	r4,[r5]
	ldr	r2,[r3,r4,lsl 2]
	add	r4,sp,#100
	add	r6,sp,#4
	ldr	r5,[r6]
	ldr	r3,[r4,r5,lsl 2]
	bl	radixSort
	add	r1,sp,#20
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	add	sp,sp,#20
	b	.17
.19:
	add	sp,sp,#224
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#224
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	radixSort, .-radixSort
	.comm a,120000040,4
	.comm ans,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
