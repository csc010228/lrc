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
	movw	r0,#152
	movt	r0,#32
	sub	sp,sp,r0
	sub	sp,sp,#0
	add	r0,sp,#152
	mov	r1,#0
	movw	r2,#0
	mov	r2,#2097152
	bl	memset
	mov	r3,#0
	mov	r0,#0
	str	r0,[sp,#144]
	add	sp,sp,#0
.0:
	sub	sp,sp,#8
	ldr	r0,[sp,#152]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.37
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#140]
	ldr	r0,[sp,#144]
	lsl	r0,r0,#18
	str	r0,[sp,#48]
	add	sp,sp,#0
.1:
	sub	sp,sp,#8
	ldr	r0,[sp,#148]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.36
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#136]
	ldr	r0,[sp,#140]
	lsl	r0,r0,#17
	str	r0,[sp,#16]
	add	sp,sp,#0
.2:
	sub	sp,sp,#8
	ldr	r0,[sp,#144]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.35
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#132]
	ldr	r0,[sp,#136]
	lsl	r0,r0,#16
	str	r0,[sp,#0]
	add	sp,sp,#0
.3:
	sub	sp,sp,#8
	ldr	r0,[sp,#140]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.34
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#128]
	ldr	r0,[sp,#132]
	lsl	r0,r0,#15
	str	r0,[sp,#44]
	add	sp,sp,#0
.4:
	sub	sp,sp,#8
	ldr	r0,[sp,#136]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.33
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#124]
	ldr	r0,[sp,#128]
	lsl	r0,r0,#14
	str	r0,[sp,#8]
	add	sp,sp,#0
.5:
	sub	sp,sp,#8
	ldr	r0,[sp,#132]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.32
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#120]
	ldr	r0,[sp,#124]
	lsl	r0,r0,#13
	str	r0,[sp,#36]
	add	sp,sp,#0
.6:
	sub	sp,sp,#8
	ldr	r0,[sp,#128]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.31
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#116]
	ldr	r0,[sp,#120]
	lsl	r0,r0,#12
	str	r0,[sp,#12]
	add	sp,sp,#0
.7:
	sub	sp,sp,#8
	ldr	r0,[sp,#124]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.30
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#112]
	ldr	r0,[sp,#116]
	lsl	r0,r0,#11
	str	r0,[sp,#56]
	add	sp,sp,#0
.8:
	sub	sp,sp,#8
	ldr	r0,[sp,#120]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.29
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#108]
	ldr	r0,[sp,#112]
	lsl	r0,r0,#10
	str	r0,[sp,#4]
	add	sp,sp,#0
.9:
	sub	sp,sp,#8
	ldr	r0,[sp,#116]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.28
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#104]
	ldr	r0,[sp,#108]
	lsl	r0,r0,#9
	str	r0,[sp,#40]
	add	sp,sp,#0
.10:
	sub	sp,sp,#8
	ldr	r0,[sp,#112]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.27
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#100]
	ldr	r0,[sp,#104]
	lsl	r0,r0,#8
	str	r0,[sp,#20]
	add	sp,sp,#0
.11:
	sub	sp,sp,#8
	ldr	r0,[sp,#108]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.26
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#96]
	ldr	r0,[sp,#100]
	lsl	r0,r0,#7
	add	fp,sp,#64
	str	r0,[sp,#64]
	add	sp,sp,#0
.12:
	sub	sp,sp,#8
	ldr	r0,[sp,#104]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.25
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#92]
	ldr	r0,[sp,#96]
	lsl	r10,r0,#6
	add	sp,sp,#0
.13:
	sub	sp,sp,#8
	ldr	r0,[sp,#100]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.24
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#88]
	ldr	r0,[sp,#92]
	lsl	r0,r0,#5
	add	r9,sp,#24
	str	r0,[sp,#24]
	add	sp,sp,#0
.14:
	sub	sp,sp,#8
	ldr	r0,[sp,#96]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.23
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#84]
	ldr	r0,[sp,#88]
	lsl	r8,r0,#4
	add	sp,sp,#0
.15:
	sub	sp,sp,#8
	ldr	r0,[sp,#92]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.22
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#80]
	ldr	r0,[sp,#84]
	lsl	r0,r0,#3
	add	r7,sp,#68
	str	r0,[sp,#68]
	add	sp,sp,#0
.16:
	sub	sp,sp,#8
	ldr	r0,[sp,#88]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.21
	sub	sp,sp,#0
	mov	r0,#0
	str	r0,[sp,#76]
	ldr	r0,[sp,#80]
	lsl	r0,r0,#2
	add	r6,sp,#52
	str	r0,[sp,#52]
	add	sp,sp,#0
.17:
	sub	sp,sp,#8
	ldr	r0,[sp,#84]
	cmp	r0,#2
	add	sp,sp,#8
	bge	.20
	sub	sp,sp,#0
	mov	r0,#0
	add	r5,sp,#72
	str	r0,[sp,#72]
	ldr	r0,[sp,#76]
	lsl	r4,r0,#1
	add	sp,sp,#0
.18:
	ldr	r0,[r5]
	cmp	r0,#2
	bge	.19
	sub	sp,sp,#72
	ldr	r0,[r5]
	add	r0,r4,r0
	ldr	r1,[r6]
	add	r1,r1,r0
	ldr	r0,[r7]
	add	r0,r0,r1
	add	r1,r8,r0
	ldr	r0,[r9]
	add	r0,r0,r1
	add	r0,r10,r0
	ldr	r1,[fp]
	add	r1,r1,r0
	add	r0,sp,#92
	ldr	r0,[r0]
	add	r1,r0,r1
	add	r0,sp,#112
	ldr	r0,[r0]
	add	r1,r0,r1
	ldr	r0,[sp,#76]
	add	r1,r0,r1
	ldr	r0,[sp,#128]
	add	r1,r0,r1
	add	r0,sp,#84
	ldr	r0,[r0]
	add	r1,r0,r1
	ldr	r0,[sp,#108]
	add	r1,r0,r1
	ldr	r0,[sp,#80]
	add	r1,r0,r1
	add	r0,sp,#116
	ldr	r0,[r0]
	add	r0,r0,r1
	ldr	r1,[sp,#72]
	add	r1,r1,r0
	add	r0,sp,#88
	ldr	r0,[r0]
	add	r1,r0,r1
	add	r0,sp,#120
	ldr	r0,[r0]
	add	r0,r0,r1
	mov	r1,r3
	add	r3,r3,#1
	ldr	r2,[r5]
	add	r2,r2,#1
	str	r2,[r5]
	add	r2,sp,#224
	str	r1,[r2,r0,lsl 2]
	add	sp,sp,#72
	b	.18
.19:
	sub	sp,sp,#0
	ldr	r0,[sp,#76]
	add	r0,r0,#1
	str	r0,[sp,#76]
	add	sp,sp,#0
	b	.17
.20:
	sub	sp,sp,#0
	ldr	r0,[sp,#80]
	add	r0,r0,#1
	str	r0,[sp,#80]
	add	sp,sp,#0
	b	.16
.21:
	sub	sp,sp,#0
	ldr	r0,[sp,#84]
	add	r0,r0,#1
	str	r0,[sp,#84]
	add	sp,sp,#0
	b	.15
.22:
	sub	sp,sp,#0
	ldr	r0,[sp,#88]
	add	r0,r0,#1
	str	r0,[sp,#88]
	add	sp,sp,#0
	b	.14
.23:
	sub	sp,sp,#0
	ldr	r0,[sp,#92]
	add	r0,r0,#1
	str	r0,[sp,#92]
	add	sp,sp,#0
	b	.13
.24:
	sub	sp,sp,#0
	ldr	r0,[sp,#96]
	add	r0,r0,#1
	str	r0,[sp,#96]
	add	sp,sp,#0
	b	.12
.25:
	sub	sp,sp,#0
	ldr	r0,[sp,#100]
	add	r0,r0,#1
	str	r0,[sp,#100]
	add	sp,sp,#0
	b	.11
.26:
	sub	sp,sp,#0
	ldr	r0,[sp,#104]
	add	r0,r0,#1
	str	r0,[sp,#104]
	add	sp,sp,#0
	b	.10
.27:
	sub	sp,sp,#0
	ldr	r0,[sp,#108]
	add	r0,r0,#1
	str	r0,[sp,#108]
	add	sp,sp,#0
	b	.9
.28:
	sub	sp,sp,#0
	ldr	r0,[sp,#112]
	add	r0,r0,#1
	str	r0,[sp,#112]
	add	sp,sp,#0
	b	.8
.29:
	sub	sp,sp,#0
	ldr	r0,[sp,#116]
	add	r0,r0,#1
	str	r0,[sp,#116]
	add	sp,sp,#0
	b	.7
.30:
	sub	sp,sp,#0
	ldr	r0,[sp,#120]
	add	r0,r0,#1
	str	r0,[sp,#120]
	add	sp,sp,#0
	b	.6
.31:
	sub	sp,sp,#0
	ldr	r0,[sp,#124]
	add	r0,r0,#1
	str	r0,[sp,#124]
	add	sp,sp,#0
	b	.5
.32:
	sub	sp,sp,#0
	ldr	r0,[sp,#128]
	add	r0,r0,#1
	str	r0,[sp,#128]
	add	sp,sp,#0
	b	.4
.33:
	sub	sp,sp,#0
	ldr	r0,[sp,#132]
	add	r0,r0,#1
	str	r0,[sp,#132]
	add	sp,sp,#0
	b	.3
.34:
	sub	sp,sp,#0
	ldr	r0,[sp,#136]
	add	r0,r0,#1
	str	r0,[sp,#136]
	add	sp,sp,#0
	b	.2
.35:
	sub	sp,sp,#0
	ldr	r0,[sp,#140]
	add	r0,r0,#1
	str	r0,[sp,#140]
	add	sp,sp,#0
	b	.1
.36:
	sub	sp,sp,#0
	ldr	r0,[sp,#144]
	add	r0,r0,#1
	str	r0,[sp,#144]
	add	sp,sp,#0
	b	.0
.37:
	sub	sp,sp,#8
	sub	sp,sp,#4
	add	r0,sp,#164
	push	{r0}
	add	r0,sp,#168
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#172
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#176
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#180
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#184
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#188
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#192
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#196
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#200
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#204
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#208
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#212
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#216
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#220
	add	r0,r0,#0
	push	{r0}
	add	r0,sp,#224
	add	r0,r0,#0
	add	r1,sp,#224
	add	r1,r1,#0
	add	r2,sp,#224
	add	r2,r2,#0
	add	r3,sp,#224
	add	r3,r3,#0
	bl	sum
	add	sp,sp,#64
	bl	putint
	movw	r0,#0
	add	sp,sp,#8
	movw	r1,#152
	movt	r1,#32
	add	sp,sp,r1
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global sum
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	sum, %function
sum:
	push	{r4,fp,lr}
	add	fp,sp,#12
	sub	sp,sp,#4
	mov	r4,r1
	ldr	r1,[r0,#0]
	ldr	r0,[r4,#8]
	add	r1,r1,r0
	ldr	r0,[r2,#4]
	add	r1,r1,r0
	ldr	r0,[r3,#40]
	add	r1,r1,r0
	add	r0,fp,#0
	ldr	r0,[r0]
	ldr	r0,[r0,#24]
	add	r1,r1,r0
	add	r0,fp,#4
	ldr	r0,[r0]
	ldr	r0,[r0,#228]
	add	r1,r1,r0
	add	r0,fp,#8
	ldr	r0,[r0]
	ldr	r0,[r0,#56]
	add	r1,r1,r0
	add	r0,fp,#12
	ldr	r0,[r0]
	ldr	r0,[r0,#964]
	add	r1,r1,r0
	add	r0,fp,#16
	ldr	r0,[r0]
	ldr	r0,[r0,#224]
	add	r1,r1,r0
	add	r0,fp,#20
	ldr	r0,[r0]
	ldr	r0,[r0,#804]
	add	r1,r1,r0
	add	r0,fp,#24
	ldr	r0,[r0]
	ldr	r0,[r0,#1996]
	add	r1,r1,r0
	add	r0,fp,#28
	ldr	r0,[r0]
	ldr	r0,[r0,#3224]
	add	r2,r1,r0
	add	r0,fp,#32
	ldr	r1,[r0]
	movw	r0,#26400
	ldr	r0,[r1,r0]
	add	r1,r2,r0
	add	r0,fp,#36
	ldr	r0,[r0]
	ldr	r0,[r0,#508]
	add	r1,r1,r0
	add	r0,fp,#40
	ldr	r0,[r0]
	ldr	r0,[r0,#124]
	add	r2,r1,r0
	add	r0,fp,#44
	ldr	r1,[r0]
	movw	r0,#50064
	movt	r0,#3
	ldr	r0,[r1,r0]
	add	r2,r2,r0
	add	r0,fp,#48
	ldr	r1,[r0]
	movw	r0,#15956
	ldr	r0,[r1,r0]
	add	r2,r2,r0
	add	r0,fp,#52
	ldr	r1,[r0]
	movw	r0,#41932
	movt	r0,#12
	ldr	r0,[r1,r0]
	add	r2,r2,r0
	add	r0,fp,#56
	ldr	r1,[r0]
	movw	r0,#19456
	movt	r0,#4
	ldr	r0,[r1,r0]
	add	r0,r2,r0
	add	sp,sp,#4
	pop	{r4,fp,pc}
	.size	sum, .-sum
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
