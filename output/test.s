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
	.global read_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	read_map, %function
read_map:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#12
	bl	getint
	push	{r0}
	movw	r1,#:lower16:width
	movt	r1,#:upper16:width
	str	r0,[r1]
	bl	getint
	push	{r0}
	movw	r1,#:lower16:height
	movt	r1,#:upper16:height
	str	r0,[r1]
	bl	getint
	push	{r0}
	movw	r1,#:lower16:steps
	movt	r1,#:upper16:steps
	str	r0,[r1]
	bl	getch
	push	{r0}
	movw	r0,#1
	add	r1,sp,#24
	str	r0,[r1]
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#16
.0:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:height
	movt	r1,#:upper16:height
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.5
	movw	r0,#1
	add	r1,sp,#8
	str	r0,[r1]
.1:
	add	r0,sp,#8
	ldr	r0,[r0]
	movw	r1,#:lower16:width
	movt	r1,#:upper16:width
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.4
	bl	getch
	cmp	r0,#35
	bne	.2
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r2,r0
	movw	r0,#1
	push	{r1}
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
	b	.3
.2:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r2,r0
	movw	r0,#0
	push	{r1}
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
.3:
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	b	.1
.4:
	bl	getch
	push	{r0}
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	add	sp,sp,#4
	b	.0
.5:
	add	sp,sp,#12
	nop
	pop	{fp,pc}
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#8
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
.6:
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#:lower16:height
	movt	r1,#:upper16:height
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.11
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
.7:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:width
	movt	r1,#:upper16:width
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.10
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,r2,r0
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
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#4
	str	r1,[r0]
	b	.7
.10:
	movw	r0,#10
	bl	putch
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#0
	str	r1,[r0]
	b	.6
.11:
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	.size	put_map, .-put_map
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#8
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
.12:
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#:lower16:height
	movt	r1,#:upper16:height
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.15
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
.13:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:width
	movt	r1,#:upper16:width
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.14
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,r2,r0
	movw	r0,#:lower16:sheet2
	movt	r0,#:upper16:sheet2
	ldr	r0,[r0,r1,lsl 2]
	push	{r1}
	push	{r1}
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,sp,#16
	str	r3,[r0]
	add	sp,sp,#12
	b	.13
.14:
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#0
	str	r1,[r0]
	b	.12
.15:
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	.size	swap12, .-swap12
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
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	push	{r0,r1}
	sub	sp,sp,#12
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#8
	str	r0,[r1]
.16:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:height
	movt	r1,#:upper16:height
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.23
	movw	r0,#1
	add	r1,sp,#8
	str	r0,[r1]
.17:
	add	r0,sp,#8
	ldr	r0,[r0]
	movw	r1,#:lower16:width
	movt	r1,#:upper16:width
	ldr	r1,[r1]
	cmp	r0,r1
	bgt	.22
	add	r0,sp,#4
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#8
	ldr	r0,[r0]
	push	{r1}
	sub	r1,r0,#1
	add	r0,sp,#0
	ldr	r0,[r0]
	push	{r1}
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,r2,r0
	add	r0,sp,#16
	ldr	r0,[r0]
	push	{r1}
	add	r1,r2,r0
	add	r0,sp,#24
	ldr	r0,[r0]
	push	{r1}
	add	r1,sp,#4
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	push	{r2}
	add	r2,sp,#32
	ldr	r2,[r2]
	add	r3,sp,#4
	ldr	r3,[r3]
	ldr	r2,[r2,r3,lsl 2]
	add	r4,r0,r2
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r2,r0,r1
	add	r0,sp,#32
	ldr	r0,[r0]
	ldr	r0,[r0,r2,lsl 2]
	push	{r1}
	add	r1,r4,r0
	add	r0,sp,#28
	ldr	r0,[r0]
	push	{r1}
	movw	r1,#500
	push	{r2}
	mul	r2,r0,r1
	add	r0,sp,#24
	ldr	r0,[r0]
	add	r1,r2,r0
	add	r0,sp,#4
	ldr	r0,[r0]
	push	{r1}
	add	r1,sp,#48
	ldr	r1,[r1]
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	ldr	r1,[r1,r2,lsl 2]
	add	r3,r0,r1
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,sp,#16
	ldr	r1,[r1]
	add	r2,r0,r1
	add	r0,sp,#52
	ldr	r0,[r0]
	ldr	r0,[r0,r2,lsl 2]
	add	r1,r3,r0
	add	r0,sp,#44
	ldr	r0,[r0]
	push	{r1}
	add	r1,r0,#1
	movw	r0,#500
	push	{r2}
	mul	r2,r1,r0
	add	r0,sp,#40
	ldr	r0,[r0]
	push	{r1}
	add	r1,r2,r0
	add	r0,sp,#8
	ldr	r0,[r0]
	push	{r1}
	add	r1,sp,#68
	ldr	r1,[r1]
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	ldr	r1,[r1,r2,lsl 2]
	push	{r3}
	add	r3,r0,r1
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#72
	ldr	r1,[r1]
	add	r2,r0,r1
	add	r0,sp,#76
	ldr	r0,[r0]
	ldr	r0,[r0,r2,lsl 2]
	add	r1,r3,r0
	add	r0,sp,#4
	ldr	r0,[r0]
	push	{r1}
	add	r1,sp,#44
	ldr	r1,[r1]
	push	{r2}
	add	r2,r0,r1
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#84
	ldr	r1,[r1]
	ldr	r1,[r1,r2,lsl 2]
	push	{r3}
	add	r3,r0,r1
	add	r0,sp,#36
	ldr	r0,[r0]
	add	r1,sp,#84
	ldr	r1,[r1]
	push	{r2}
	add	r2,r0,r1
	add	r0,sp,#92
	ldr	r0,[r0]
	ldr	r0,[r0,r2,lsl 2]
	cmp	r0,#1
	add	r0,sp,#80
	str	r3,[r0]
	add	sp,sp,#80
	bne	.18
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#2
	bne	.18
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r2,r0
	movw	r0,#1
	push	{r1}
	add	r1,sp,#20
	ldr	r1,[r1]
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
	b	.21
.18:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#3
	bne	.19
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r2,r0
	movw	r0,#1
	push	{r1}
	add	r1,sp,#20
	ldr	r1,[r1]
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
	b	.20
.19:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#500
	mul	r2,r0,r1
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r2,r0
	movw	r0,#0
	push	{r1}
	add	r1,sp,#20
	ldr	r1,[r1]
	push	{r2}
	add	r2,sp,#4
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#8
.20:
.21:
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	b	.17
.22:
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#4
	str	r1,[r0]
	b	.16
.23:
	add	sp,sp,#20
	nop
	pop	{r4,fp,pc}
	.size	step, .-step
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
	bl	read_map
	movw	r0,#95
	bl	_sysy_starttime
.24:
	movw	r0,#:lower16:steps
	movt	r0,#:upper16:steps
	ldr	r0,[r0]
	cmp	r0,#0
	ble	.27
	movw	r0,#:lower16:active
	movt	r0,#:upper16:active
	ldr	r0,[r0]
	cmp	r0,#1
	bne	.25
	movw	r0,#:lower16:sheet1
	movt	r0,#:upper16:sheet1
	movw	r1,#:lower16:sheet2
	movt	r1,#:upper16:sheet2
	bl	step
	movw	r0,#2
	movw	r1,#:lower16:active
	movt	r1,#:upper16:active
	str	r0,[r1]
	b	.26
.25:
	movw	r0,#:lower16:sheet2
	movt	r0,#:upper16:sheet2
	movw	r1,#:lower16:sheet1
	movt	r1,#:upper16:sheet1
	bl	step
	movw	r0,#1
	movw	r1,#:lower16:active
	movt	r1,#:upper16:active
	str	r0,[r1]
.26:
	movw	r0,#:lower16:steps
	movt	r0,#:upper16:steps
	ldr	r0,[r0]
	sub	r1,r0,#1
	movw	r0,#:lower16:steps
	movt	r0,#:upper16:steps
	str	r1,[r0]
	b	.24
.27:
	movw	r0,#106
	bl	_sysy_stoptime
	movw	r0,#:lower16:active
	movt	r0,#:upper16:active
	ldr	r0,[r0]
	cmp	r0,#2
	bne	.28
	bl	swap12
.28:
	bl	put_map
	movw	r0,#0
	nop
	pop	{fp,pc}
	.size	main, .-main
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
