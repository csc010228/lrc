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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#8
	sub	sp,sp,#40
	bl	getint
	add	r1,sp,#36
	str	r0,[r1]
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	str	r0,[r1]
	bl	getint
	add	r1,sp,#8
	str	r0,[r1]
	movw	r1,#:lower16:b
	movt	r1,#:upper16:b
	str	r0,[r1]
	bl	getint
	add	r1,sp,#24
	str	r0,[r1]
	movw	r1,#:lower16:c
	movt	r1,#:upper16:c
	str	r0,[r1]
	bl	getint
	add	r1,sp,#32
	str	r0,[r1]
	movw	r1,#:lower16:d
	movt	r1,#:upper16:d
	str	r0,[r1]
	bl	getint
	add	r1,sp,#28
	str	r0,[r1]
	movw	r1,#:lower16:e
	movt	r1,#:upper16:e
	str	r0,[r1]
	movw	r0,#0
	add	r1,sp,#44
	str	r0,[r1]
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r2,sp,#24
	ldr	r1,[r2]
	mul	r2,r0,r1
	add	r1,sp,#36
	ldr	r0,[r1]
	sub	r1,r0,r2
	add	r0,sp,#20
	str	r1,[r0]
	add	r0,sp,#4
	str	r2,[r0]
	add	r3,sp,#4
	str	r2,[r3]
	add	r2,sp,#20
	str	r1,[r2]
	add	r1,sp,#36
	ldr	r0,[r1]
	add	r2,sp,#24
	ldr	r1,[r2]
	bl	__aeabi_idiv
	add	r1,sp,#16
	str	r0,[r1]
	add	r1,sp,#32
	ldr	r0,[r1]
	add	r2,sp,#16
	ldr	r1,[r2]
	sub	r2,r0,r1
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,r2
	add	sp,sp,#40
	bne	.0
	sub	sp,sp,#16
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	ldr	r0,[r1]
	movw	r2,#:lower16:b
	movt	r2,#:upper16:b
	ldr	r1,[r2]
	mul	r2,r0,r1
	add	r0,sp,#8
	str	r2,[r0]
	mov	r0,r2
	add	r3,sp,#8
	str	r2,[r3]
	movw	r2,#:lower16:c
	movt	r2,#:upper16:c
	ldr	r1,[r2]
	bl	__aeabi_idiv
	add	r1,sp,#0
	str	r0,[r1]
	movw	r1,#:lower16:e
	movt	r1,#:upper16:e
	ldr	r0,[r1]
	movw	r2,#:lower16:d
	movt	r2,#:upper16:d
	ldr	r1,[r2]
	add	r2,r0,r1
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,r2
	add	sp,sp,#16
	beq	.0
	sub	sp,sp,#16
	movw	r1,#:lower16:a
	movt	r1,#:upper16:a
	ldr	r0,[r1]
	movw	r2,#:lower16:b
	movt	r2,#:upper16:b
	ldr	r1,[r2]
	add	r2,r0,r1
	movw	r1,#:lower16:c
	movt	r1,#:upper16:c
	ldr	r0,[r1]
	add	r1,r2,r0
	add	r3,sp,#0
	str	r2,[r3]
	add	r2,sp,#4
	str	r1,[r2]
	movw	r1,#:lower16:d
	movt	r1,#:upper16:d
	ldr	r0,[r1]
	movw	r2,#:lower16:e
	movt	r2,#:upper16:e
	ldr	r1,[r2]
	add	r2,r0,r1
	add	r1,sp,#4
	ldr	r0,[r1]
	cmp	r0,r2
	add	sp,sp,#16
	beq	.0
	b	.1
.0:
	movw	r0,#1
	add	r1,sp,#4
	str	r0,[r1]
.1:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#8
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	.size	main, .-main
	.comm a,4,4
	.comm b,4,4
	.comm c,4,4
	.comm d,4,4
	.comm e,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
