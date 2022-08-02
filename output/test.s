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
	nop
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#8
	sub	sp,sp,#40
	bl	getint
	mov	r7,r0
	mov	r1,r7
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	str	r1,[r0]
	bl	getint
	mov	r1,r0
	add	r0,sp,#32
	add	r0,sp,#32
	str	r1,[r0]
	add	r0,sp,#32
	ldr	r0,[r0]
	nop
	movw	r4,#:lower16:b
	movt	r4,#:upper16:b
	str	r0,[r4]
	bl	getint
	nop
	add	r8,sp,#28
	str	r0,[r8]
	ldr	r0,[r8]
	nop
	movw	r5,#:lower16:c
	movt	r5,#:upper16:c
	str	r0,[r5]
	bl	getint
	nop
	add	r9,sp,#24
	str	r0,[r9]
	ldr	r0,[r9]
	nop
	movw	r6,#:lower16:d
	movt	r6,#:upper16:d
	str	r0,[r6]
	bl	getint
	nop
	mov	r10,r0
	movw	r0,#0
	movt	r0,#0
	nop
	add	r1,sp,#44
	add	r1,sp,#44
	str	r0,[r1]
	ldr	r0,[r8]
	add	r1,sp,#32
	ldr	r1,[r1]
	mul	r0,r1,r0
	sub	r0,r7,r0
	add	r1,sp,#12
	add	r1,sp,#12
	str	r0,[r1]
	mov	r0,r7
	ldr	r1,[r8]
	nop
	bl	__aeabi_idiv
	nop
	ldr	r1,[r9]
	sub	r0,r1,r0
	add	r1,sp,#12
	ldr	r1,[r1]
	cmp	r1,r0
	add	sp,sp,#40
	bne	.0
	nop
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	ldr	r1,[r0]
	ldr	r0,[r4]
	mul	r0,r1,r0
	nop
	ldr	r1,[r5]
	nop
	bl	__aeabi_idiv
	mov	r1,r0
	ldr	r0,[r6]
	add	r0,r10,r0
	cmp	r1,r0
	nop
	beq	.0
	nop
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	ldr	r0,[r0]
	ldr	r1,[r4]
	add	r0,r0,r1
	ldr	r1,[r5]
	add	r0,r0,r1
	ldr	r1,[r6]
	add	r1,r1,r10
	cmp	r0,r1
	nop
	beq	.0
	nop
	nop
	b	.1
.0:
	sub	sp,sp,#0
	movw	r0,#1
	movt	r0,#0
	nop
	add	r1,sp,#4
	str	r0,[r1]
	add	sp,sp,#0
.1:
	sub	sp,sp,#0
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	ldr	r1,[r0]
	movw	r0,#:lower16:a
	movt	r0,#:upper16:a
	str	r1,[r0]
	movw	r4,#:lower16:b
	movt	r4,#:upper16:b
	ldr	r0,[r4]
	str	r0,[r4]
	movw	r5,#:lower16:c
	movt	r5,#:upper16:c
	ldr	r0,[r5]
	str	r0,[r5]
	movw	r6,#:lower16:d
	movt	r6,#:upper16:d
	ldr	r0,[r6]
	str	r0,[r6]
	movw	r0,#:lower16:e
	movt	r0,#:upper16:e
	str	r10,[r0]
	add	r0,sp,#4
	ldr	r0,[r0]
	nop
	add	sp,sp,#0
	add	sp,sp,#8
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0
	.size	main, .-main
	.comm a,4,4
	.comm b,4,4
	.comm c,4,4
	.comm d,4,4
	.comm e,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
