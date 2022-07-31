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
	sub	sp,sp,#8
	movw	r0,#1
	nop
	add	sp,sp,#8
	movw	r0,#1
	add	sp,sp,#8
	add	sp,sp,#0
	nop
	pop	{fp,pc}
.0:
	movw	r0,#0
	add	sp,sp,#8
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	.size	main, .-main
	.comm a,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
