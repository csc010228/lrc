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
	.global output_
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	output_, %function
output_:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#8
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
.34:
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r2,#:lower16:output_length
	movt	r2,#:upper16:output_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.35
	sub	sp,sp,#8
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	putch
	add	r1,sp,#12
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#12
	str	r1,[r0]
	add	sp,sp,#8
	b	.34
.35:
	add	sp,sp,#8
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	.size	output_, .-output_
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
	push	{r4,r5,fp,lr}
	nop
	add	fp,sp,#16
	sub	sp,sp,#0
	movw	r0,#2104
	sub	sp,sp,r0
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#24
	str	r0,[r1]
	add	sp,sp,#8
.40:
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#62
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#60
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#43
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#45
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#91
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#93
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#46
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#44
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	cmp	r0,#35
	add	sp,sp,#8
	beq	.41
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#24
	str	r0,[r1]
	add	sp,sp,#8
	b	.40
.41:
	add	r1,sp,#16
	ldr	r0,[r1]
	add	r1,sp,#12
	str	r0,[r1]
.42:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#35
	add	sp,sp,#8
	beq	.43
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	movw	r3,#:lower16:program_length
	movt	r3,#:upper16:program_length
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	bl	getch
	add	r1,sp,#16
	str	r0,[r1]
	add	sp,sp,#8
.44:
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#62
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#60
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#43
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#45
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#91
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#93
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#46
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#44
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#35
	add	sp,sp,#8
	beq	.45
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#16
	str	r0,[r1]
	add	sp,sp,#8
	b	.44
.45:
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	add	r1,sp,#20
	str	r0,[r1]
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r0,#:lower16:program_length
	movt	r0,#:upper16:program_length
	str	r1,[r0]
	add	sp,sp,#8
	b	.42
.43:
	sub	sp,sp,#8
	bl	getch
	cmp	r0,#105
	add	sp,sp,#8
	beq	.46
	b	.49
.46:
	sub	sp,sp,#8
	bl	getint
	add	r1,sp,#4
	str	r0,[r1]
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	str	r0,[r1]
	bl	getch
	add	r1,sp,#0
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#8
	str	r0,[r1]
	add	sp,sp,#8
.47:
	sub	sp,sp,#8
	add	r1,sp,#8
	ldr	r0,[r1]
	movw	r2,#:lower16:input_length
	movt	r2,#:upper16:input_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.48
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#4
	str	r0,[r1]
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	add	r3,sp,#8
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r1,r0,#1
	str	r1,[r3]
	add	sp,sp,#8
	b	.47
.48:
	b	.49
.49:
	add	r0,sp,#40
	movw	r1,#0
	movw	r2,#2048
	bl	memset
	movw	r0,#116
	bl	_sysy_starttime
	movw	r0,#0
	add	r1,sp,#2096
	str	r0,[r1]
	movw	r2,#2092
	add	r1,sp,r2
	str	r0,[r1]
	movw	r2,#2088
	add	r1,sp,r2
	str	r0,[r1]
	add	r1,sp,#36
	str	r0,[r1]
	movw	r1,#:lower16:output_length
	movt	r1,#:upper16:output_length
	str	r0,[r1]
.50:
	sub	sp,sp,#8
	movw	r2,#2104
	add	r1,sp,r2
	ldr	r0,[r1]
	movw	r2,#:lower16:program_length
	movt	r2,#:upper16:program_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.51
	sub	sp,sp,#8
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	movw	r4,#2104
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#62
	add	r1,sp,#40
	str	r0,[r1]
	add	sp,sp,#8
	bne	.52
	sub	sp,sp,#8
	movw	r2,#2100
	add	r1,sp,r2
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,r2
	str	r1,[r0]
	add	sp,sp,#8
	b	.53
.52:
	sub	sp,sp,#8
	add	r1,sp,#40
	ldr	r0,[r1]
	cmp	r0,#60
	add	sp,sp,#8
	bne	.54
	sub	sp,sp,#8
	movw	r2,#2100
	add	r1,sp,r2
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r0,sp,r2
	str	r1,[r0]
	add	sp,sp,#8
	b	.55
.54:
	sub	sp,sp,#8
	add	r1,sp,#40
	ldr	r0,[r1]
	cmp	r0,#43
	add	sp,sp,#8
	bne	.56
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2100
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r1,[r0,r2,lsl 2]
	add	sp,sp,#8
	b	.57
.56:
	sub	sp,sp,#8
	add	r1,sp,#40
	ldr	r0,[r1]
	cmp	r0,#45
	add	sp,sp,#8
	bne	.58
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2100
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	sub	r1,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r1,[r0,r2,lsl 2]
	add	sp,sp,#8
	b	.59
.58:
	sub	sp,sp,#8
	add	r1,sp,#40
	ldr	r0,[r1]
	cmp	r0,#91
	add	sp,sp,#8
	bne	.60
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2100
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#8
	beq	.61
	sub	sp,sp,#8
	movw	r2,#2104
	add	r1,sp,r2
	ldr	r0,[r1]
	add	r1,sp,#48
	add	r3,sp,#44
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r1,r0,#1
	str	r1,[r3]
	add	sp,sp,#8
	b	.62
.61:
	movw	r0,#1
	add	r1,sp,#24
	str	r0,[r1]
.63:
	sub	sp,sp,#8
	add	r1,sp,#32
	ldr	r0,[r1]
	cmp	r0,#0
	add	sp,sp,#8
	ble	.64
	sub	sp,sp,#8
	movw	r2,#2104
	add	r1,sp,r2
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r3,#2104
	add	r2,sp,r3
	str	r1,[r2]
	add	r2,sp,#4
	str	r1,[r2]
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#93
	add	sp,sp,#8
	bne	.65
	sub	sp,sp,#8
	add	r1,sp,#32
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r0,sp,#32
	str	r1,[r0]
	add	sp,sp,#8
.65:
	sub	sp,sp,#8
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	movw	r4,#2104
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#91
	add	sp,sp,#8
	bne	.66
	sub	sp,sp,#8
	add	r1,sp,#32
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#32
	str	r1,[r0]
	add	sp,sp,#8
.66:
	b	.63
.64:
.62:
	b	.67
.60:
	sub	sp,sp,#8
	add	r1,sp,#40
	ldr	r0,[r1]
	cmp	r0,#93
	add	sp,sp,#8
	bne	.68
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2100
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#8
	bne	.69
	sub	sp,sp,#8
	add	r1,sp,#44
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r0,sp,#44
	str	r1,[r0]
	add	sp,sp,#8
	b	.70
.69:
	sub	sp,sp,#8
	add	r1,sp,#44
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r2,sp,#4
	str	r1,[r2]
	add	r1,sp,#48
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	str	r2,[r3]
	movw	r2,#2104
	add	r1,sp,r2
	str	r0,[r1]
	add	sp,sp,#8
.70:
	b	.71
.68:
	sub	sp,sp,#8
	add	r1,sp,#40
	ldr	r0,[r1]
	cmp	r0,#46
	add	sp,sp,#8
	bne	.72
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2100
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	movw	r3,#:lower16:output_length
	movt	r3,#:upper16:output_length
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r1,r0,#1
	str	r1,[r3]
	add	sp,sp,#8
	b	.73
.72:
	sub	sp,sp,#8
	add	r1,sp,#40
	ldr	r0,[r1]
	cmp	r0,#44
	add	sp,sp,#8
	bne	.74
	sub	sp,sp,#8
	add	r1,sp,#2096
	ldr	r0,[r1]
	movw	r2,#:lower16:input_length
	movt	r2,#:upper16:input_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	blt	.75
	movw	r0,#0
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2092
	add	r3,sp,r4
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	b	.76
.75:
	sub	sp,sp,#8
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	add	r3,sp,#2096
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r5,#2100
	add	r4,sp,r5
	ldr	r3,[r4]
	str	r0,[r1,r3,lsl 2]
	add	r0,r2,#1
	add	r1,sp,#2096
	str	r0,[r1]
	add	sp,sp,#8
.76:
.74:
.73:
.71:
.67:
.59:
.57:
.55:
.53:
	sub	sp,sp,#8
	movw	r2,#2104
	add	r1,sp,r2
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,r2
	str	r1,[r0]
	add	sp,sp,#8
	b	.50
.51:
	movw	r0,#118
	bl	_sysy_stoptime
	movw	r0,#0
	movw	r2,#2100
	add	r1,sp,r2
	str	r0,[r1]
.77:
	sub	sp,sp,#8
	movw	r2,#2108
	add	r1,sp,r2
	ldr	r0,[r1]
	movw	r2,#:lower16:output_length
	movt	r2,#:upper16:output_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.78
	sub	sp,sp,#8
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	movw	r4,#2108
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	bl	putch
	add	r1,sp,r4
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,r4
	str	r1,[r0]
	add	sp,sp,#8
	b	.77
.78:
	movw	r0,#0
	movw	r1,#2104
	add	sp,sp,r1
	add	sp,sp,#0
	nop
	pop	{r4,r5,fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global read_program
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	read_program, %function
read_program:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#24
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#8
.36:
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#62
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#60
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#43
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#45
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#91
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#93
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#46
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#44
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#35
	add	sp,sp,#8
	beq	.37
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#8
	b	.36
.37:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r1,sp,#20
	str	r0,[r1]
.2:
	sub	sp,sp,#8
	add	r1,sp,#28
	ldr	r0,[r1]
	cmp	r0,#35
	add	sp,sp,#8
	beq	.3
	sub	sp,sp,#8
	add	r1,sp,#28
	ldr	r0,[r1]
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	movw	r3,#:lower16:program_length
	movt	r3,#:upper16:program_length
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	bl	getch
	add	r1,sp,#16
	str	r0,[r1]
	add	sp,sp,#8
.38:
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#62
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#60
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#43
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#45
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#91
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#93
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#46
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#44
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	cmp	r0,#35
	add	sp,sp,#8
	beq	.39
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#16
	str	r0,[r1]
	add	sp,sp,#8
	b	.38
.39:
	sub	sp,sp,#8
	add	r1,sp,#16
	ldr	r0,[r1]
	add	r1,sp,#28
	str	r0,[r1]
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r0,#:lower16:program_length
	movt	r0,#:upper16:program_length
	str	r1,[r0]
	add	sp,sp,#8
	b	.2
.3:
	sub	sp,sp,#8
	bl	getch
	cmp	r0,#105
	add	sp,sp,#8
	beq	.4
	add	sp,sp,#24
	add	sp,sp,#0
	nop
	pop	{fp,pc}
.4:
	sub	sp,sp,#8
	bl	getint
	add	r1,sp,#0
	str	r0,[r1]
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	str	r0,[r1]
	bl	getch
	add	r1,sp,#4
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#24
	str	r0,[r1]
	add	sp,sp,#8
.5:
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	movw	r2,#:lower16:input_length
	movt	r2,#:upper16:input_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.6
	sub	sp,sp,#8
	bl	getch
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	add	r3,sp,#24
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#0
	str	r0,[r1]
	ldr	r0,[r3]
	add	r1,r0,#1
	str	r1,[r3]
	add	sp,sp,#8
	b	.5
.6:
	add	sp,sp,#24
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	.size	read_program, .-read_program
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global get_bf_char
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	get_bf_char, %function
get_bf_char:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#8
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#8
.0:
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#62
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#60
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#43
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#45
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#91
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#93
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#46
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#44
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	add	r1,sp,#12
	ldr	r0,[r1]
	cmp	r0,#35
	add	sp,sp,#8
	beq	.1
	sub	sp,sp,#8
	bl	getch
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#8
	b	.0
.1:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#8
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	.size	get_bf_char, .-get_bf_char
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global run_program
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	run_program, %function
run_program:
	push	{r4,r5,fp,lr}
	nop
	add	fp,sp,#16
	sub	sp,sp,#0
	sub	sp,sp,#2080
	add	r0,sp,#20
	movw	r1,#0
	movw	r2,#2048
	bl	memset
	movw	r0,#0
	movw	r1,#:lower16:output_length
	movt	r1,#:upper16:output_length
	str	r0,[r1]
	add	r1,sp,#16
	str	r0,[r1]
	movw	r2,#2072
	add	r1,sp,r2
	str	r0,[r1]
	movw	r2,#2068
	add	r1,sp,r2
	str	r0,[r1]
	movw	r2,#2076
	add	r1,sp,r2
	str	r0,[r1]
.7:
	sub	sp,sp,#8
	add	r1,sp,#2080
	ldr	r0,[r1]
	movw	r2,#:lower16:program_length
	movt	r2,#:upper16:program_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.33
	sub	sp,sp,#8
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	add	r3,sp,#2080
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#62
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#8
	bne	.8
	sub	sp,sp,#8
	movw	r2,#2076
	add	r1,sp,r2
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,r2
	str	r1,[r0]
	add	sp,sp,#8
	b	.32
.8:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#60
	add	sp,sp,#8
	bne	.9
	sub	sp,sp,#8
	movw	r2,#2076
	add	r1,sp,r2
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r0,sp,r2
	str	r1,[r0]
	add	sp,sp,#8
	b	.31
.9:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#43
	add	sp,sp,#8
	bne	.10
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2076
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r1,[r0,r2,lsl 2]
	add	sp,sp,#8
	b	.30
.10:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#45
	add	sp,sp,#8
	bne	.11
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2076
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	sub	r1,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r1,[r0,r2,lsl 2]
	add	sp,sp,#8
	b	.29
.11:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#91
	add	sp,sp,#8
	bne	.18
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2076
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#8
	beq	.12
	sub	sp,sp,#8
	add	r1,sp,#2080
	ldr	r0,[r1]
	add	r1,sp,#28
	add	r3,sp,#24
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r1,r0,#1
	str	r1,[r3]
	add	sp,sp,#8
	b	.17
.12:
	movw	r0,#1
	add	r1,sp,#0
	str	r0,[r1]
.13:
	sub	sp,sp,#8
	add	r1,sp,#8
	ldr	r0,[r1]
	cmp	r0,#0
	add	sp,sp,#8
	ble	.16
	sub	sp,sp,#8
	add	r1,sp,#2080
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r2,sp,#0
	str	r1,[r2]
	add	r2,sp,#2080
	str	r1,[r2]
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#93
	add	sp,sp,#8
	bne	.14
	sub	sp,sp,#8
	add	r1,sp,#8
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	add	sp,sp,#8
.14:
	sub	sp,sp,#8
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	add	r3,sp,#2080
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#91
	add	sp,sp,#8
	bne	.15
	sub	sp,sp,#8
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#8
	str	r1,[r0]
	add	sp,sp,#8
.15:
	b	.13
.16:
.17:
	b	.28
.18:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#93
	add	sp,sp,#8
	bne	.21
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2076
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#8
	bne	.19
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r0,sp,#24
	str	r1,[r0]
	add	sp,sp,#8
	b	.20
.19:
	sub	sp,sp,#8
	add	r1,sp,#24
	ldr	r0,[r1]
	sub	r1,r0,#1
	add	r2,sp,#4
	str	r1,[r2]
	add	r1,sp,#28
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,sp,#2080
	str	r0,[r1]
	add	sp,sp,#8
.20:
	b	.27
.21:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#46
	add	sp,sp,#8
	bne	.22
	sub	sp,sp,#8
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2076
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	movw	r3,#:lower16:output_length
	movt	r3,#:upper16:output_length
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r1,r0,#1
	str	r1,[r3]
	add	sp,sp,#8
	b	.26
.22:
	sub	sp,sp,#8
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#44
	add	sp,sp,#8
	bne	.25
	sub	sp,sp,#8
	movw	r2,#2084
	add	r1,sp,r2
	ldr	r0,[r1]
	movw	r2,#:lower16:input_length
	movt	r2,#:upper16:input_length
	ldr	r1,[r2]
	cmp	r0,r1
	add	sp,sp,#8
	blt	.23
	movw	r0,#0
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r4,#2068
	add	r3,sp,r4
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	b	.24
.23:
	sub	sp,sp,#8
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	movw	r4,#2084
	add	r3,sp,r4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r5,#2076
	add	r4,sp,r5
	ldr	r3,[r4]
	str	r0,[r1,r3,lsl 2]
	add	r0,r2,#1
	movw	r2,#2084
	add	r1,sp,r2
	str	r0,[r1]
	add	sp,sp,#8
.24:
.25:
.26:
.27:
.28:
.29:
.30:
.31:
.32:
	sub	sp,sp,#8
	add	r1,sp,#2080
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#2080
	str	r1,[r0]
	add	sp,sp,#8
	b	.7
.33:
	add	sp,sp,#2080
	add	sp,sp,#0
	nop
	pop	{r4,r5,fp,pc}
	.size	run_program, .-run_program
	.global program_length
	.data 
	.align 2
	.type program_length,%object
	.size program_length,4
program_length:
	.word 0
	.global program
	.data 
	.align 2
	.type program,%object
	.size program,262144
program:
	.space 262144
	.global tape
	.data 
	.align 2
	.type tape,%object
	.size tape,262144
tape:
	.space 262144
	.global input
	.data 
	.align 2
	.type input,%object
	.size input,262144
input:
	.space 262144
	.global input_length
	.data 
	.align 2
	.type input_length,%object
	.size input_length,4
input_length:
	.word 0
	.global output
	.data 
	.align 2
	.type output,%object
	.size output,262144
output:
	.space 262144
	.global output_length
	.data 
	.align 2
	.type output_length,%object
	.size output_length,4
output_length:
	.word 0
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
