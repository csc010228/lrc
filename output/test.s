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
	sub	sp,sp,#20
	movw	r0,#1
	add	r1,sp,#8
	str	r0,[r1]
	movw	r0,#2
	add	r1,sp,#0
	str	r0,[r1]
	movw	r0,#3
	push	{r0}
	add	r1,sp,#8
	str	r0,[r1]
	movw	r0,#5
	push	{r0}
	add	r1,sp,#20
	str	r0,[r1]
	movw	r0,#8
	add	sp,sp,#28
	nop
	pop	{fp,pc}
	add	r1,sp,#24
	str	r0,[r1]
	add	sp,sp,#8
	.size	main, .-main
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
