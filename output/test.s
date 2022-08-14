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
	bl	getint
	mov	r7,r0
	bl	getint
	mov	r9,r0
	bl	getint
	mov	r10,r0
	bl	getch
	movw	r0,#1
	mov	r5,r0
	mov	r8,r0
.29:
	cmp	r8,r9
	bgt	.30
	movw	r0,#1
	mov	r5,r0
	movw	r0,#500
	mul	r6,r8,r0
	mul	r4,r8,r0
.31:
	cmp	r5,r7
	bgt	.32
	bl	getch
	cmp	r0,#35
	bne	.33
	add	r2,r6,r5
	movw	r0,#1
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	str	r0,[r1,r2,lsl 2]
	b	.34
.33:
	add	r2,r4,r5
	movw	r0,#0
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	str	r0,[r1,r2,lsl 2]
.34:
	add	r5,r5,#1
	b	.31
.32:
	bl	getch
	add	r8,r8,#1
	b	.29
.30:
	movw	r0,#95
	bl	_sysy_starttime
.24:
	cmp	r10,#0
	ble	.27
	movw	r0,#:lower16:active
	movt	r0,#:upper16:active
	ldr	r6,[r0]
	cmp	r6,#1
	bne	.25
	movw	r0,#:lower16:width
	movt	r0,#:upper16:width
	str	r7,[r0]
	movw	r0,#:lower16:height
	movt	r0,#:upper16:height
	str	r9,[r0]
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	mov	r0,r1
	movw	r1,#:lower16:sheet2
	movt	r1,#:upper16:sheet2
	bl	step
	movw	r0,#2
	mov	r6,r0
	b	.26
.25:
	movw	r0,#:lower16:width
	movt	r0,#:upper16:width
	str	r7,[r0]
	movw	r0,#:lower16:height
	movt	r0,#:upper16:height
	str	r9,[r0]
	movw	r1,#:lower16:sheet2
	movt	r1,#:upper16:sheet2
	mov	r0,r1
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	bl	step
	movw	r0,#1
	mov	r6,r0
.26:
	sub	r10,r10,#1
	movw	r0,#:lower16:active
	movt	r0,#:upper16:active
	str	r6,[r0]
	b	.24
.27:
	movw	r0,#106
	bl	_sysy_stoptime
	movw	r0,#:lower16:active
	movt	r0,#:upper16:active
	ldr	r6,[r0]
	cmp	r6,#2
	bne	.28
	movw	r0,#1
	mov	r4,r0
	mov	r5,r0
.35:
	cmp	r5,r9
	bgt	.36
	movw	r0,#1
	mov	r4,r0
	movw	r0,#500
	mul	r3,r5,r0
.37:
	cmp	r4,r7
	bgt	.38
	add	r0,r3,r4
	movw	r1,#:lower16:sheet2
	movt	r1,#:upper16:sheet2
	ldr	r1,[r1,r0,lsl 2]
	mov	r2,r1
	add	r4,r4,#1
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	str	r2,[r1,r0,lsl 2]
	b	.37
.38:
	add	r5,r5,#1
	b	.35
.36:
.28:
	movw	r0,#1
	mov	r5,r0
	mov	r8,r0
.39:
	cmp	r8,r9
	bgt	.40
	movw	r0,#1
	mov	r5,r0
	movw	r0,#500
	mul	r4,r8,r0
.41:
	cmp	r5,r7
	bgt	.42
	add	r0,r4,r5
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	ldr	r0,[r1,r0,lsl 2]
	cmp	r0,#1
	bne	.43
	movw	r0,#35
	bl	putch
	b	.44
.43:
	movw	r0,#46
	bl	putch
.44:
	add	r5,r5,#1
	b	.41
.42:
	movw	r0,#10
	bl	putch
	add	r8,r8,#1
	b	.39
.40:
	movw	r0,#:lower16:width
	movt	r0,#:upper16:width
	str	r7,[r0]
	movw	r0,#:lower16:active
	movt	r0,#:upper16:active
	str	r6,[r0]
	movw	r0,#:lower16:height
	movt	r0,#:upper16:height
	str	r9,[r0]
	movw	r0,#:lower16:steps
	movt	r0,#:upper16:steps
	str	r10,[r0]
	movw	r0,#0
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global swap12
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	swap12, %function
swap12:
	push	{r4,r5,fp,lr}
	add	fp,sp,#16
	sub	sp,sp,#0
	movw	r0,#1
	mov	r2,r0
	mov	r5,r0
.12:
	movw	r0,#:lower16:height
	movt	r0,#:upper16:height
	ldr	r0,[r0]
	cmp	r5,r0
	bgt	.15
	movw	r0,#1
	mov	r2,r0
	movw	r0,#500
	mul	r4,r5,r0
.13:
	movw	r0,#:lower16:width
	movt	r0,#:upper16:width
	ldr	r0,[r0]
	cmp	r2,r0
	bgt	.14
	add	r1,r4,r2
	movw	r0,#:lower16:sheet2
	movt	r0,#:upper16:sheet2
	ldr	r0,[r0,r1,lsl 2]
	mov	r3,r0
	add	r2,r2,#1
	movw	r0,#:lower16:sheet1
	movt	r0,#:upper16:sheet1
	str	r3,[r0,r1,lsl 2]
	b	.13
.14:
	add	r5,r5,#1
	b	.12
.15:
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
	.size	swap12, .-swap12
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global read_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	read_map, %function
read_map:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	add	fp,sp,#36
	sub	sp,sp,#4
	bl	getint
	mov	r7,r0
	bl	getint
	mov	r9,r0
	bl	getint
	mov	r10,r0
	bl	getch
	movw	r0,#1
	mov	r5,r0
	mov	r8,r0
.0:
	cmp	r8,r9
	bgt	.5
	movw	r0,#1
	mov	r5,r0
	movw	r0,#500
	mul	r6,r8,r0
	mul	r4,r8,r0
.1:
	cmp	r5,r7
	bgt	.4
	bl	getch
	cmp	r0,#35
	bne	.2
	add	r2,r6,r5
	movw	r0,#1
	mov	r1,r0
	movw	r0,#:lower16:sheet1
	movt	r0,#:upper16:sheet1
	str	r1,[r0,r2,lsl 2]
	b	.3
.2:
	add	r2,r4,r5
	movw	r0,#0
	mov	r1,r0
	movw	r0,#:lower16:sheet1
	movt	r0,#:upper16:sheet1
	str	r1,[r0,r2,lsl 2]
.3:
	add	r5,r5,#1
	b	.1
.4:
	bl	getch
	add	r8,r8,#1
	b	.0
.5:
	movw	r0,#:lower16:width
	movt	r0,#:upper16:width
	str	r7,[r0]
	movw	r0,#:lower16:height
	movt	r0,#:upper16:height
	str	r9,[r0]
	movw	r0,#:lower16:steps
	movt	r0,#:upper16:steps
	str	r10,[r0]
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	read_map, .-read_map
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global put_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	put_map, %function
put_map:
	push	{r4,r5,r6,fp,lr}
	add	fp,sp,#20
	sub	sp,sp,#4
	movw	r0,#1
	mov	r4,r0
	mov	r6,r0
.6:
	movw	r0,#:lower16:height
	movt	r0,#:upper16:height
	ldr	r0,[r0]
	cmp	r6,r0
	bgt	.11
	movw	r0,#1
	mov	r4,r0
	movw	r0,#500
	mul	r5,r6,r0
.7:
	movw	r0,#:lower16:width
	movt	r0,#:upper16:width
	ldr	r0,[r0]
	cmp	r4,r0
	bgt	.10
	add	r1,r5,r4
	movw	r0,#:lower16:sheet1
	movt	r0,#:upper16:sheet1
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#1
	bne	.8
	movw	r0,#35
	bl	putch
	b	.9
.8:
	movw	r0,#46
	bl	putch
.9:
	add	r4,r4,#1
	b	.7
.10:
	movw	r0,#10
	bl	putch
	add	r6,r6,#1
	b	.6
.11:
	add	sp,sp,#4
	pop	{r4,r5,r6,fp,pc}
	.size	put_map, .-put_map
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global step
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	step, %function
step:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#48
	mov	r2,r0
	add	r0,sp,#44
	add	r0,sp,#44
	str	r1,[r0]
	sub	sp,sp,#0
	mov	r0,#1
	mov	r1,#1
	add	r0,sp,#32
	add	r0,sp,#32
	str	r1,[r0]
	add	sp,sp,#0
.16:
	sub	sp,sp,#8
	movw	r0,#:lower16:height
	movt	r0,#:upper16:height
	ldr	r1,[r0]
	add	r0,sp,#40
	ldr	r0,[r0]
	cmp	r0,r1
	add	sp,sp,#8
	bgt	.23
	sub	sp,sp,#0
	mov	r1,#1
	add	r0,sp,#36
	add	r0,sp,#36
	str	r1,[r0]
	add	r0,sp,#32
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#12
	add	r0,sp,#12
	str	r1,[r0]
	movw	r0,#500
	add	r1,sp,#32
	ldr	r1,[r1]
	mul	r7,r1,r0
	add	r1,sp,#32
	ldr	r1,[r1]
	add	fp,r1,#1
	add	r1,sp,#32
	ldr	r1,[r1]
	mul	r1,r1,r0
	add	r10,sp,#24
	str	r1,[r10]
	add	r1,sp,#32
	ldr	r1,[r1]
	mul	r1,r1,r0
	add	r9,sp,#20
	str	r1,[r9]
	add	r1,sp,#32
	ldr	r1,[r1]
	mul	r8,r1,r0
	add	sp,sp,#0
.17:
	sub	sp,sp,#8
	movw	r0,#:lower16:width
	movt	r0,#:upper16:width
	ldr	r1,[r0]
	add	r0,sp,#44
	ldr	r0,[r0]
	cmp	r0,r1
	add	sp,sp,#8
	bgt	.22
	sub	sp,sp,#88
	add	r0,sp,#124
	ldr	r0,[r0]
	sub	r4,r0,#1
	movw	r5,#500
	add	r0,sp,#100
	ldr	r0,[r0]
	mul	r1,r0,r5
	add	r0,r1,r4
	add	r3,sp,#124
	ldr	r3,[r3]
	add	r3,r1,r3
	ldr	r6,[r2,r0,lsl 2]
	ldr	r0,[r2,r3,lsl 2]
	add	r6,r6,r0
	add	r0,sp,#124
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,r1,r3
	ldr	r0,[r2,r0,lsl 2]
	add	r1,r6,r0
	add	r0,r7,r4
	ldr	r0,[r2,r0,lsl 2]
	add	r1,r1,r0
	add	r0,r7,r3
	ldr	r0,[r2,r0,lsl 2]
	add	r0,r1,r0
	mul	r1,fp,r5
	add	r4,r1,r4
	ldr	r4,[r2,r4,lsl 2]
	add	r0,r0,r4
	add	r4,sp,#124
	ldr	r4,[r4]
	add	r4,r1,r4
	ldr	r4,[r2,r4,lsl 2]
	add	r4,r0,r4
	add	r0,r1,r3
	ldr	r0,[r2,r0,lsl 2]
	add	r0,r4,r0
	mov	r1,r0
	add	r0,sp,#124
	ldr	r0,[r0]
	add	r0,r7,r0
	ldr	r0,[r2,r0,lsl 2]
	cmp	r0,#1
	add	sp,sp,#88
	bne	.18
	cmp	r1,#2
	bne	.18
	sub	sp,sp,#8
	ldr	r1,[r10]
	add	r0,sp,#44
	ldr	r0,[r0]
	add	r1,r1,r0
	mov	r3,#1
	add	r0,sp,#52
	ldr	r0,[r0]
	str	r3,[r0,r1,lsl 2]
	add	sp,sp,#8
	b	.21
.18:
	cmp	r1,#3
	bne	.19
	sub	sp,sp,#8
	ldr	r1,[r9]
	add	r0,sp,#44
	ldr	r0,[r0]
	add	r1,r1,r0
	mov	r3,#1
	add	r0,sp,#52
	ldr	r0,[r0]
	str	r3,[r0,r1,lsl 2]
	add	sp,sp,#8
	b	.20
.19:
	sub	sp,sp,#8
	add	r0,sp,#44
	ldr	r0,[r0]
	add	r1,r8,r0
	mov	r3,#0
	add	r0,sp,#52
	ldr	r0,[r0]
	str	r3,[r0,r1,lsl 2]
	add	sp,sp,#8
.20:
.21:
	sub	sp,sp,#0
	add	r0,sp,#36
	ldr	r0,[r0]
	add	r0,r0,#1
	add	r1,sp,#36
	str	r0,[r1]
	add	sp,sp,#0
	b	.17
.22:
	sub	sp,sp,#0
	add	r0,sp,#32
	ldr	r0,[r0]
	add	r0,r0,#1
	add	r1,sp,#32
	str	r0,[r1]
	add	sp,sp,#0
	b	.16
.23:
	add	sp,sp,#48
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	step, .-step
	.global sheet1
	.data 
	.align 2
	.type sheet1,%object
	.size sheet1,1000000
sheet1:
	.space 1000000
	.global sheet2
	.data 
	.align 2
	.type sheet2,%object
	.size sheet2,1000000
sheet2:
	.space 1000000
	.global active
	.data 
	.align 2
	.type active,%object
	.size active,4
active:
	.word 1
	.comm width,4,4
	.comm height,4,4
	.comm steps,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
