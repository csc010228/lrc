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
	add	fp,sp,#8
	nop
	nop
	bl	ifElseIf
	push	{r0}
	bl	putint
	movw	r0,#0
	add	sp,sp,#4
	nop
	pop	{fp,pc}
	add	sp,sp,#4
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global ifElseIf
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	ifElseIf, %function
ifElseIf:
	push	{fp,lr}
	add	fp,sp,#8
	nop
	nop
	sub	sp,sp,#8
	movw	r0,#5
	add	r1,sp,#4
	str	r0,[r1]
	movw	r0,#10
	add	r1,sp,#0
	str	r0,[r1]
	add	r2,sp,#4
	ldr	r0,[r2]
	cmp	r0,#6
	beq	.0
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#11
	beq	.0
	b	.1
.0:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	b	.6
.1:
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#10
	bne	.2
	add	r1,sp,#4
	ldr	r0,[r1]
	cmp	r0,#1
	bne	.2
	movw	r0,#25
	add	r1,sp,#4
	str	r0,[r1]
	b	.5
.2:
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#10
	bne	.3
	add	r1,sp,#4
	ldr	r0,[r1]
	movw	r2,#65531
	movt	r2,#65535
	cmp	r0,r2
	bne	.3
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r2,r0,#15
	mov	r0,r2
	str	r0,[r1]
	b	.4
.3:
	add	r1,sp,#4
	ldr	r0,[r1]
	rsb	r2,r0,#0
	mov	r0,r2
	str	r0,[r1]
.4:
.5:
.6:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	.size	ifElseIf, .-ifElseIf
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
