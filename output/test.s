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
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#0
	sub	sp,sp,#8
	add	r1,sp,#4
	str	r0,[r1]
	str	r0,[r1]
	add	r2,sp,#4
	ldr	r1,[r2]
	ldr	r0,[r1,#4]
	bl	putint
	movw	r0,#10
	bl	putch
	add	r2,sp,#4
	ldr	r1,[r2]
	ldr	r0,[r1,#4]
	bl	putint
	movw	r0,#10
	bl	putch
	add	sp,sp,#8
	add	sp,sp,#0
	nop
	pop	{fp,pc}
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
	sub	sp,sp,#120
	add	r0,sp,#0
	movw	r1,#0
	movw	r2,#120
	bl	memset
	movw	r0,#1
	add	r1,sp,#0
	str	r0,[r1,#0]
	movw	r0,#2
	str	r0,[r1,#4]
	movw	r0,#3
	str	r0,[r1,#8]
	movw	r0,#4
	str	r0,[r1,#12]
	movw	r0,#5
	str	r0,[r1,#16]
	movw	r0,#6
	str	r0,[r1,#20]
	movw	r0,#7
	str	r0,[r1,#24]
	movw	r0,#8
	str	r0,[r1,#28]
	movw	r0,#9
	str	r0,[r1,#32]
	movw	r0,#11
	str	r0,[r1,#40]
	movw	r0,#12
	str	r0,[r1,#44]
	movw	r0,#13
	str	r0,[r1,#48]
	movw	r0,#14
	str	r0,[r1,#52]
	movw	r0,#15
	str	r0,[r1,#56]
	movw	r0,#16
	str	r0,[r1,#60]
	movw	r0,#17
	str	r0,[r1,#64]
	movw	r0,#18
	str	r0,[r1,#68]
	movw	r0,#19
	str	r0,[r1,#72]
	movw	r0,#10
	str	r0,[r1,#76]
	movw	r0,#31
	str	r0,[r1,#80]
	movw	r0,#32
	str	r0,[r1,#84]
	movw	r0,#33
	str	r0,[r1,#88]
	movw	r0,#34
	str	r0,[r1,#92]
	movw	r0,#35
	str	r0,[r1,#96]
	movw	r0,#36
	str	r0,[r1,#100]
	movw	r0,#37
	str	r0,[r1,#104]
	movw	r0,#38
	str	r0,[r1,#108]
	movw	r0,#39
	str	r0,[r1,#112]
	movw	r0,#30
	str	r0,[r1,#116]
	str	r0,[r1,#116]
	movw	r0,#2
	bl	putint
	movw	r0,#10
	bl	putch
	add	r0,sp,#0
	bl	test
	movw	r0,#0
	add	sp,sp,#120
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	.size	main, .-main
	.global x
	.data 
	.align 2
	.type x,%object
	.size x,4
x:
	.word 10
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
