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
	bl	if_ifElse_
	nop
	pop	{fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global if_ifElse_
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	if_ifElse_, %function
if_ifElse_:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#8
	movw	r0,#5
	add	r1,sp,#4
	str	r0,[r1]
	movw	r0,#10
	add	r1,sp,#0
	str	r0,[r1]
	movw	r0,#1
	movw	r0,#1
	movw	r0,#25
	add	r1,sp,#4
	str	r0,[r1]
	b	.1
.0:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	r2,r0,#15
	mov	r0,r2
	str	r0,[r1]
.1:
.2:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	.size	if_ifElse_, .-if_ifElse_
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
