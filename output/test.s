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
	.global test
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	test, %function
test:
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	sub	sp,sp,#4
	sub	sp,sp,#16
	add	r4,sp,#12
	str	r3,[r4]
	add	r3,sp,#8
	str	r2,[r3]
	add	r2,sp,#4
	str	r1,[r2]
	add	r1,sp,#0
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#62915
	movt	r0,#16456
	vmov	s0,r0
	bl	putfloat
	add	sp,sp,#16
	add	sp,sp,#4
	nop
	pop	{r4,fp,pc}
	.size	test, .-test
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
	sub	sp,sp,#4
	movw	r0,#5
	push	{r0}
	movw	r0,#1
	movw	r1,#2
	movw	r2,#3
	movw	r3,#4
	bl	test
	add	sp,sp,#8
	movw	r0,#0
	add	sp,sp,#8
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	.size	main, .-main
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
