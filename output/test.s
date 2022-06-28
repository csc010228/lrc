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
	sub	sp,sp,#20
	bl	getch
	add	r1,sp,#0
	str	r0,[r1]
.36:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#62
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#60
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#43
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#45
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#91
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#93
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#46
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#44
	beq	.37
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#35
	beq	.37
	bl	getch
	add	r1,sp,#0
	str	r0,[r1]
	b	.36
.37:
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,sp,#16
	str	r0,[r1]
.2:
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#35
	beq	.3
	add	r0,sp,#16
	ldr	r0,[r0]
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	movw	r2,#:lower16:program_length
	movt	r2,#:upper16:program_length
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	bl	getch
	add	r1,sp,#4
	str	r0,[r1]
.38:
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#62
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#60
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#43
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#45
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#91
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#93
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#46
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#44
	beq	.39
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#35
	beq	.39
	bl	getch
	add	r1,sp,#4
	str	r0,[r1]
	b	.38
.39:
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r1,sp,#16
	str	r0,[r1]
	movw	r0,#:lower16:program_length
	movt	r0,#:upper16:program_length
	ldr	r0,[r0]
	add	r2,r0,#1
	movw	r0,#:lower16:program_length
	movt	r0,#:upper16:program_length
	str	r2,[r0]
	b	.2
.3:
	bl	getch
	cmp	r0,#105
	beq	.4
	add	sp,sp,#20
	nop
	pop	{fp,pc}
.4:
	bl	getint
	push	{r0}
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	str	r0,[r1]
	bl	getch
	push	{r0}
	movw	r0,#0
	add	r1,sp,#20
	str	r0,[r1]
	add	sp,sp,#8
.5:
	add	r0,sp,#12
	ldr	r0,[r0]
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.6
	bl	getch
	push	{r0}
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	add	r2,sp,#16
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,sp,#16
	str	r3,[r0]
	add	sp,sp,#4
	b	.5
.6:
	add	sp,sp,#20
	nop
	pop	{fp,pc}
	.size	read_program, .-read_program
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#2080
	add	r0,sp,#20
	movw	r1,#0
	movw	r2,#2048
	bl	memset
	movw	r0,#0
	movw	r1,#:lower16:output_length
	movt	r1,#:upper16:output_length
	str	r0,[r1]
	movw	r1,#2072
	add	r1,sp,r1
	str	r0,[r1]
	movw	r1,#2068
	add	r1,sp,r1
	str	r0,[r1]
	movw	r1,#2076
	add	r1,sp,r1
	str	r0,[r1]
	add	r1,sp,#16
	str	r0,[r1]
.7:
	movw	r0,#2072
	add	r0,sp,r0
	ldr	r0,[r0]
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.33
	movw	r0,#:lower16:program
	movt	r0,#:upper16:program
	movw	r1,#2072
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#62
	add	r1,sp,#12
	str	r0,[r1]
	bne	.8
	movw	r0,#2068
	add	r0,sp,r0
	ldr	r0,[r0]
	add	r1,r0,#1
	movw	r0,#2068
	add	r0,sp,r0
	str	r1,[r0]
	b	.32
.8:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#60
	bne	.9
	movw	r0,#2068
	add	r0,sp,r0
	ldr	r0,[r0]
	sub	r1,r0,#1
	movw	r0,#2068
	add	r0,sp,r0
	str	r1,[r0]
	b	.31
.9:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#43
	bne	.10
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2068
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	add	r2,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r2,[r0,r1,lsl 2]
	b	.30
.10:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#45
	bne	.11
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2068
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	sub	r2,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r2,[r0,r1,lsl 2]
	b	.29
.11:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#91
	bne	.18
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2068
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#0
	beq	.12
	movw	r0,#2072
	add	r0,sp,r0
	ldr	r0,[r0]
	add	r1,sp,#20
	add	r2,sp,#16
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,sp,#16
	str	r3,[r0]
	b	.17
.12:
	movw	r0,#1
	add	r1,sp,#0
	str	r0,[r1]
.13:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#0
	ble	.16
	movw	r0,#2072
	add	r0,sp,r0
	ldr	r0,[r0]
	add	r1,r0,#1
	movw	r0,#:lower16:program
	movt	r0,#:upper16:program
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#93
	movw	r0,#2072
	add	r0,sp,r0
	str	r1,[r0]
	bne	.14
	add	r0,sp,#0
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#0
	str	r1,[r0]
.14:
	movw	r0,#:lower16:program
	movt	r0,#:upper16:program
	movw	r1,#2072
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#91
	bne	.15
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#0
	str	r1,[r0]
.15:
	b	.13
.16:
.17:
	b	.28
.18:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#93
	bne	.21
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2068
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#0
	bne	.19
	add	r0,sp,#16
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#16
	str	r1,[r0]
	b	.20
.19:
	add	r0,sp,#16
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#20
	ldr	r0,[r0,r1,lsl 2]
	push	{r1}
	movw	r1,#2076
	add	r1,sp,r1
	str	r0,[r1]
	add	sp,sp,#4
.20:
	b	.27
.21:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#46
	bne	.22
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2068
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	movw	r2,#:lower16:output_length
	movt	r2,#:upper16:output_length
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	movw	r0,#:lower16:output_length
	movt	r0,#:upper16:output_length
	ldr	r0,[r0]
	add	r3,r0,#1
	movw	r0,#:lower16:output_length
	movt	r0,#:upper16:output_length
	str	r3,[r0]
	b	.26
.22:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#44
	bne	.25
	movw	r0,#2076
	add	r0,sp,r0
	ldr	r0,[r0]
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	ldr	r1,[r1]
	cmp	r0,r1
	blt	.23
	movw	r0,#0
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r2,#2068
	add	r2,sp,r2
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	b	.24
.23:
	movw	r0,#:lower16:input
	movt	r0,#:upper16:input
	movw	r1,#2076
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	movw	r3,#2068
	add	r3,sp,r3
	ldr	r3,[r3]
	str	r0,[r2,r3,lsl 2]
	add	r0,r1,#1
	movw	r1,#2076
	add	r1,sp,r1
	str	r0,[r1]
.24:
.25:
.26:
.27:
.28:
.29:
.30:
.31:
.32:
	movw	r0,#2072
	add	r0,sp,r0
	ldr	r0,[r0]
	add	r1,r0,#1
	movw	r0,#2072
	add	r0,sp,r0
	str	r1,[r0]
	b	.7
.33:
	add	sp,sp,#2080
	nop
	pop	{fp,pc}
	.size	run_program, .-run_program
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
	sub	sp,sp,#4
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.34:
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#:lower16:output_length
	movt	r1,#:upper16:output_length
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.35
	movw	r0,#:lower16:output
	movt	r0,#:upper16:output
	add	r1,sp,#0
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	bl	putch
	add	r0,sp,#0
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#0
	str	r1,[r0]
	b	.34
.35:
	add	sp,sp,#4
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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	movw	r0,#2104
	sub	sp,sp,r0
	bl	getch
	add	r1,sp,#16
	str	r0,[r1]
.40:
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#62
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#60
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#43
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#45
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#91
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#93
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#46
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#44
	beq	.41
	add	r0,sp,#16
	ldr	r0,[r0]
	cmp	r0,#35
	beq	.41
	bl	getch
	add	r1,sp,#16
	str	r0,[r1]
	b	.40
.41:
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r1,sp,#12
	str	r0,[r1]
.42:
	add	r0,sp,#12
	ldr	r0,[r0]
	cmp	r0,#35
	beq	.43
	add	r0,sp,#12
	ldr	r0,[r0]
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	movw	r2,#:lower16:program_length
	movt	r2,#:upper16:program_length
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	bl	getch
	add	r1,sp,#8
	str	r0,[r1]
.44:
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#62
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#60
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#43
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#45
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#91
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#93
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#46
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#44
	beq	.45
	add	r0,sp,#8
	ldr	r0,[r0]
	cmp	r0,#35
	beq	.45
	bl	getch
	add	r1,sp,#8
	str	r0,[r1]
	b	.44
.45:
	add	r0,sp,#8
	ldr	r0,[r0]
	add	r1,sp,#12
	str	r0,[r1]
	movw	r0,#:lower16:program_length
	movt	r0,#:upper16:program_length
	ldr	r0,[r0]
	add	r2,r0,#1
	movw	r0,#:lower16:program_length
	movt	r0,#:upper16:program_length
	str	r2,[r0]
	b	.42
.43:
	bl	getch
	cmp	r0,#105
	beq	.46
	b	.49
.46:
	bl	getint
	push	{r0}
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	str	r0,[r1]
	bl	getch
	push	{r0}
	movw	r0,#0
	add	r1,sp,#8
	str	r0,[r1]
	add	sp,sp,#8
.47:
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.48
	bl	getch
	push	{r0}
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	add	r2,sp,#4
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#4
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,sp,#4
	str	r3,[r0]
	add	sp,sp,#4
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
	movw	r1,#2092
	add	r1,sp,r1
	str	r0,[r1]
	add	r1,sp,#2096
	str	r0,[r1]
	movw	r1,#2088
	add	r1,sp,r1
	str	r0,[r1]
	add	r1,sp,#36
	str	r0,[r1]
	movw	r1,#:lower16:output_length
	movt	r1,#:upper16:output_length
	str	r0,[r1]
.50:
	add	r0,sp,#2096
	ldr	r0,[r0]
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.51
	movw	r0,#:lower16:program
	movt	r0,#:upper16:program
	add	r1,sp,#2096
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#62
	add	r1,sp,#32
	str	r0,[r1]
	bne	.52
	movw	r0,#2092
	add	r0,sp,r0
	ldr	r0,[r0]
	add	r1,r0,#1
	movw	r0,#2092
	add	r0,sp,r0
	str	r1,[r0]
	b	.53
.52:
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r0,#60
	bne	.54
	movw	r0,#2092
	add	r0,sp,r0
	ldr	r0,[r0]
	sub	r1,r0,#1
	movw	r0,#2092
	add	r0,sp,r0
	str	r1,[r0]
	b	.55
.54:
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r0,#43
	bne	.56
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2092
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	add	r2,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r2,[r0,r1,lsl 2]
	b	.57
.56:
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r0,#45
	bne	.58
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2092
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	sub	r2,r0,#1
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	str	r2,[r0,r1,lsl 2]
	b	.59
.58:
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r0,#91
	bne	.60
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2092
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#0
	beq	.61
	add	r0,sp,#2096
	ldr	r0,[r0]
	add	r1,sp,#40
	add	r2,sp,#36
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#36
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,sp,#36
	str	r3,[r0]
	b	.62
.61:
	movw	r0,#1
	add	r1,sp,#24
	str	r0,[r1]
.63:
	add	r0,sp,#24
	ldr	r0,[r0]
	cmp	r0,#0
	ble	.64
	add	r0,sp,#2096
	ldr	r0,[r0]
	add	r1,r0,#1
	movw	r0,#:lower16:program
	movt	r0,#:upper16:program
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#93
	add	r0,sp,#2096
	str	r1,[r0]
	bne	.65
	add	r0,sp,#24
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#24
	str	r1,[r0]
.65:
	movw	r0,#:lower16:program
	movt	r0,#:upper16:program
	add	r1,sp,#2096
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#91
	bne	.66
	add	r0,sp,#24
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#24
	str	r1,[r0]
.66:
	b	.63
.64:
.62:
	b	.67
.60:
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r0,#93
	bne	.68
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2092
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,#0
	bne	.69
	add	r0,sp,#36
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#36
	str	r1,[r0]
	b	.70
.69:
	add	r0,sp,#36
	ldr	r0,[r0]
	sub	r1,r0,#1
	add	r0,sp,#40
	ldr	r0,[r0,r1,lsl 2]
	push	{r1}
	movw	r1,#2100
	add	r1,sp,r1
	str	r0,[r1]
	add	sp,sp,#4
.70:
	b	.71
.68:
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r0,#46
	bne	.72
	movw	r0,#:lower16:tape
	movt	r0,#:upper16:tape
	movw	r1,#2092
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	movw	r2,#:lower16:output_length
	movt	r2,#:upper16:output_length
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	movw	r0,#:lower16:output_length
	movt	r0,#:upper16:output_length
	ldr	r0,[r0]
	add	r3,r0,#1
	movw	r0,#:lower16:output_length
	movt	r0,#:upper16:output_length
	str	r3,[r0]
	b	.73
.72:
	add	r0,sp,#32
	ldr	r0,[r0]
	cmp	r0,#44
	bne	.74
	movw	r0,#2088
	add	r0,sp,r0
	ldr	r0,[r0]
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	ldr	r1,[r1]
	cmp	r0,r1
	blt	.75
	movw	r0,#0
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	movw	r2,#2092
	add	r2,sp,r2
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	b	.76
.75:
	movw	r0,#:lower16:input
	movt	r0,#:upper16:input
	movw	r1,#2088
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	movw	r3,#2092
	add	r3,sp,r3
	ldr	r3,[r3]
	str	r0,[r2,r3,lsl 2]
	add	r0,r1,#1
	movw	r1,#2088
	add	r1,sp,r1
	str	r0,[r1]
.76:
.74:
.73:
.71:
.67:
.59:
.57:
.55:
.53:
	add	r0,sp,#2096
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#2096
	str	r1,[r0]
	b	.50
.51:
	movw	r0,#118
	bl	_sysy_stoptime
	movw	r0,#0
	movw	r1,#2100
	add	r1,sp,r1
	str	r0,[r1]
.77:
	movw	r0,#2100
	add	r0,sp,r0
	ldr	r0,[r0]
	movw	r1,#:lower16:output_length
	movt	r1,#:upper16:output_length
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.78
	movw	r0,#:lower16:output
	movt	r0,#:upper16:output
	movw	r1,#2100
	add	r1,sp,r1
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	bl	putch
	movw	r0,#2100
	add	r0,sp,r0
	ldr	r0,[r0]
	add	r1,r0,#1
	movw	r0,#2100
	add	r0,sp,r0
	str	r1,[r0]
	b	.77
.78:
	movw	r0,#0
	movw	r1,#2104
	add	sp,sp,r1
	nop
	pop	{fp,pc}
	.size	main, .-main
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
	sub	sp,sp,#4
	bl	getch
	add	r1,sp,#0
	str	r0,[r1]
.0:
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#62
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#60
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#43
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#45
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#91
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#93
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#46
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#44
	beq	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	cmp	r0,#35
	beq	.1
	bl	getch
	add	r1,sp,#0
	str	r0,[r1]
	b	.0
.1:
	add	r0,sp,#0
	ldr	r0,[r0]
	add	sp,sp,#4
	nop
	pop	{fp,pc}
	.size	get_bf_char, .-get_bf_char
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
