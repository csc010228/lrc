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
	movw	r1,#2104
	sub	sp,sp,r1
	sub	sp,sp,#8
	bl	getch
	add	sp,sp,#8
.40:
	cmp	r0,#62
	beq	.41
	cmp	r0,#60
	beq	.41
	cmp	r0,#43
	beq	.41
	cmp	r0,#45
	beq	.41
	cmp	r0,#91
	beq	.41
	cmp	r0,#93
	beq	.41
	cmp	r0,#46
	beq	.41
	cmp	r0,#44
	beq	.41
	cmp	r0,#35
	beq	.41
	bl	getch
	b	.40
.41:
.42:
	cmp	r0,#35
	beq	.43
	mov	r4,r0
	bl	getch
	movw	r3,#:lower16:program
	movt	r3,#:upper16:program
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	ldr	fp,[r1]
	str	r4,[r3,fp,lsl 2]
.44:
	cmp	r0,#62
	beq	.45
	cmp	r0,#60
	beq	.45
	cmp	r0,#43
	beq	.45
	cmp	r0,#45
	beq	.45
	cmp	r0,#91
	beq	.45
	cmp	r0,#93
	beq	.45
	cmp	r0,#46
	beq	.45
	cmp	r0,#44
	beq	.45
	cmp	r0,#35
	beq	.45
	bl	getch
	b	.44
.45:
	add	fp,fp,#1
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	str	fp,[r1]
	b	.42
.43:
	bl	getch
	cmp	r0,#105
	it ne
	bne	.49
.46:
	bl	getint
	mov	r7,r0
	bl	getch
	movw	r6,#0
	mov	r4,r6
.47:
	cmp	r4,r7
	bge	.48
	bl	getch
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	str	r0,[r1,r4,lsl 2]
	add	r4,r4,#1
	b	.47
.48:
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	str	r7,[r1]
	b	.49
.49:
	sub	sp,sp,#0
	add	r10,sp,#52
	mov	r0,r10
	movw	r6,#0
	mov	r1,r6
	movw	r2,#2048
	bl	memset
	movw	r0,#116
	bl	_sysy_starttime
	mov	r0,r6
	mov	r8,r6
	mov	r9,r6
	mov	r4,r6
	mov	r5,r6
	add	sp,sp,#0
.50:
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	ldr	fp,[r1]
	cmp	r0,fp
	bge	.51
	movw	r3,#:lower16:program
	movt	r3,#:upper16:program
	ldr	r1,[r3,r0,lsl 2]
	mov	r2,r1
	cmp	r1,#62
	itt eq
	addeq	r8,r8,#1
	beq	.53
.52:
	cmp	r2,#60
	itt eq
	subeq	r8,r8,#1
	beq	.55
.54:
	cmp	r2,#43
	bne	.56
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	ldr	r2,[r1,r8,lsl 2]
	add	r2,r2,#1
	str	r2,[r1,r8,lsl 2]
	b	.57
.56:
	cmp	r2,#45
	bne	.58
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	ldr	r2,[r1,r8,lsl 2]
	sub	r2,r2,#1
	str	r2,[r1,r8,lsl 2]
	b	.59
.58:
	cmp	r2,#91
	bne	.60
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	ldr	r2,[r1,r8,lsl 2]
	cmp	r2,#0
	beq	.61
	mov	r1,r0
	str	r1,[r10,r4,lsl 2]
	add	r4,r4,#1
	b	.62
.61:
	movw	r1,#1
	mov	r2,r1
.63:
	cmp	r2,#0
	ble	.64
	add	r0,r0,#1
	ldr	r1,[r3,r0,lsl 2]
	cmp	r1,#93
	bne	.65
	sub	r2,r2,#1
.65:
	cmp	r1,#91
	bne	.66
	add	r2,r2,#1
.66:
	b	.63
.64:
.62:
	b	.67
.60:
	cmp	r2,#93
	bne	.68
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	ldr	r2,[r1,r8,lsl 2]
	cmp	r2,#0
	bne	.69
	sub	r4,r4,#1
	b	.70
.69:
	sub	r0,r4,#1
	ldr	r0,[r10,r0,lsl 2]
.70:
	b	.71
.68:
	cmp	r2,#46
	bne	.72
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	ldr	r2,[r1,r8,lsl 2]
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	str	r2,[r1,r5,lsl 2]
	add	r5,r5,#1
	b	.73
.72:
	cmp	r2,#44
	bne	.74
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	ldr	r7,[r1]
	cmp	r9,r7
	blt	.75
	movw	r6,#0
	mov	r2,r6
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	str	r2,[r1,r8,lsl 2]
	b	.76
.75:
	movw	r1,#:lower16:input
	movt	r1,#:upper16:input
	ldr	r1,[r1,r9,lsl 2]
	mov	r2,r1
	add	r9,r9,#1
	movw	r1,#:lower16:tape
	movt	r1,#:upper16:tape
	str	r2,[r1,r8,lsl 2]
.76:
	movw	r1,#:lower16:input_length
	movt	r1,#:upper16:input_length
	str	r7,[r1]
.74:
.73:
.71:
.67:
.59:
.57:
.55:
.53:
	add	r0,r0,#1
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	str	fp,[r1]
	b	.50
.51:
	movw	r0,#118
	bl	_sysy_stoptime
	movw	r6,#0
	mov	r4,r6
.77:
	cmp	r4,r5
	bge	.78
	movw	r1,#:lower16:output
	movt	r1,#:upper16:output
	ldr	r0,[r1,r4,lsl 2]
	bl	putch
	add	r4,r4,#1
	b	.77
.78:
	sub	sp,sp,#0
	movw	r0,#:lower16:output_length
	movt	r0,#:upper16:output_length
	str	r5,[r0]
	movw	r1,#:lower16:program_length
	movt	r1,#:upper16:program_length
	str	fp,[r1]
	movw	r0,#0
	add	sp,sp,#0
	movw	r1,#2104
	add	sp,sp,r1
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0
	.size	main, .-main
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
	push	{r4,fp,lr}
	add	fp,sp,#12
	sub	sp,sp,#4
	movw	r0,#0
	mov	r4,r0
.34:
	movw	r0,#:lower16:output_length
	movt	r0,#:upper16:output_length
	ldr	r0,[r0]
	cmp	r4,r0
	bge	.35
	movw	r0,#:lower16:output
	movt	r0,#:upper16:output
	ldr	r0,[r0,r4,lsl 2]
	bl	putch
	add	r4,r4,#1
	b	.34
.35:
	add	sp,sp,#4
	pop	{r4,fp,pc}
	.size	output_, .-output_
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
	push	{r4,r5,r6,r7,r8,r9,fp,lr}
	add	fp,sp,#32
	sub	sp,sp,#0
	sub	sp,sp,#2080
	sub	sp,sp,#0
	add	r9,sp,#20
	mov	r0,r9
	movw	r6,#0
	mov	r1,r6
	movw	r2,#2048
	bl	memset
	mov	r1,r6
	mov	r7,r6
	mov	r8,r6
	mov	r4,r6
	mov	r5,r6
	add	sp,sp,#0
.7:
	movw	r0,#:lower16:program_length
	movt	r0,#:upper16:program_length
	ldr	r0,[r0]
	cmp	r1,r0
	bge	.33
	movw	r0,#:lower16:program
	movt	r0,#:upper16:program
	ldr	r2,[r0,r1,lsl 2]
	mov	r3,r2
	cmp	r2,#62
	itt eq
	addeq	r7,r7,#1
	beq	.32
.8:
	cmp	r3,#60
	itt eq
	subeq	r7,r7,#1
	beq	.31
.9:
	cmp	r3,#43
	bne	.10
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	ldr	r3,[r2,r7,lsl 2]
	add	r3,r3,#1
	str	r3,[r2,r7,lsl 2]
	b	.30
.10:
	cmp	r3,#45
	bne	.11
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	ldr	r3,[r2,r7,lsl 2]
	sub	r3,r3,#1
	str	r3,[r2,r7,lsl 2]
	b	.29
.11:
	cmp	r3,#91
	bne	.18
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	ldr	r3,[r2,r7,lsl 2]
	cmp	r3,#0
	beq	.12
	mov	r0,r1
	str	r0,[r9,r4,lsl 2]
	add	r4,r4,#1
	b	.17
.12:
	movw	r2,#1
	mov	r3,r2
.13:
	cmp	r3,#0
	ble	.16
	add	r1,r1,#1
	ldr	r2,[r0,r1,lsl 2]
	cmp	r2,#93
	bne	.14
	sub	r3,r3,#1
.14:
	cmp	r2,#91
	bne	.15
	add	r3,r3,#1
.15:
	b	.13
.16:
.17:
	b	.28
.18:
	cmp	r3,#93
	bne	.21
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	ldr	r3,[r2,r7,lsl 2]
	cmp	r3,#0
	bne	.19
	sub	r4,r4,#1
	b	.20
.19:
	sub	r0,r4,#1
	ldr	r0,[r9,r0,lsl 2]
	mov	r1,r0
.20:
	b	.27
.21:
	cmp	r3,#46
	bne	.22
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	ldr	r3,[r2,r7,lsl 2]
	mov	r2,r3
	movw	r0,#:lower16:output
	movt	r0,#:upper16:output
	str	r2,[r0,r5,lsl 2]
	add	r5,r5,#1
	b	.26
.22:
	cmp	r3,#44
	bne	.25
	movw	r0,#:lower16:input_length
	movt	r0,#:upper16:input_length
	ldr	r0,[r0]
	cmp	r8,r0
	blt	.23
	movw	r6,#0
	mov	r3,r6
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	str	r3,[r2,r7,lsl 2]
	b	.24
.23:
	movw	r0,#:lower16:input
	movt	r0,#:upper16:input
	ldr	r0,[r0,r8,lsl 2]
	mov	r3,r0
	add	r8,r8,#1
	movw	r2,#:lower16:tape
	movt	r2,#:upper16:tape
	str	r3,[r2,r7,lsl 2]
.24:
.25:
.26:
.27:
.28:
.29:
.30:
.31:
.32:
	add	r1,r1,#1
	b	.7
.33:
	movw	r0,#:lower16:output_length
	movt	r0,#:upper16:output_length
	str	r5,[r0]
	add	sp,sp,#2080
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	.size	run_program, .-run_program
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
	push	{r4,r5,fp,lr}
	add	fp,sp,#16
	sub	sp,sp,#0
	bl	getch
.36:
	cmp	r0,#62
	beq	.37
	cmp	r0,#60
	beq	.37
	cmp	r0,#43
	beq	.37
	cmp	r0,#45
	beq	.37
	cmp	r0,#91
	beq	.37
	cmp	r0,#93
	beq	.37
	cmp	r0,#46
	beq	.37
	cmp	r0,#44
	beq	.37
	cmp	r0,#35
	beq	.37
	bl	getch
	b	.36
.37:
.2:
	cmp	r0,#35
	beq	.3
	mov	r5,r0
	bl	getch
	movw	r1,#:lower16:program
	movt	r1,#:upper16:program
	movw	r2,#:lower16:program_length
	movt	r2,#:upper16:program_length
	ldr	r4,[r2]
	str	r5,[r1,r4,lsl 2]
.38:
	cmp	r0,#62
	beq	.39
	cmp	r0,#60
	beq	.39
	cmp	r0,#43
	beq	.39
	cmp	r0,#45
	beq	.39
	cmp	r0,#91
	beq	.39
	cmp	r0,#93
	beq	.39
	cmp	r0,#46
	beq	.39
	cmp	r0,#44
	beq	.39
	cmp	r0,#35
	beq	.39
	bl	getch
	b	.38
.39:
	add	r4,r4,#1
	movw	r2,#:lower16:program_length
	movt	r2,#:upper16:program_length
	str	r4,[r2]
	b	.2
.3:
	bl	getch
	cmp	r0,#105
	itt ne
	addne	sp,sp,#0
	popne	{r4,r5,fp,pc}
.4:
	bl	getint
	mov	r5,r0
	bl	getch
	movw	r0,#0
	mov	r4,r0
.5:
	cmp	r4,r5
	bge	.6
	bl	getch
	mov	r1,r0
	movw	r0,#:lower16:input
	movt	r0,#:upper16:input
	str	r1,[r0,r4,lsl 2]
	add	r4,r4,#1
	b	.5
.6:
	movw	r0,#:lower16:input_length
	movt	r0,#:upper16:input_length
	str	r5,[r0]
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
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
	add	fp,sp,#8
	sub	sp,sp,#0
	bl	getch
.0:
	cmp	r0,#62
	beq	.1
	cmp	r0,#60
	beq	.1
	cmp	r0,#43
	beq	.1
	cmp	r0,#45
	beq	.1
	cmp	r0,#91
	beq	.1
	cmp	r0,#93
	beq	.1
	cmp	r0,#46
	beq	.1
	cmp	r0,#44
	beq	.1
	cmp	r0,#35
	beq	.1
	bl	getch
	b	.0
.1:
	add	sp,sp,#0
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
