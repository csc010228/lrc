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
	sub	sp,sp,#8
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
.0:
	add	r0,sp,#4
	ldr	r0,[r0]
	cmp	r0,#6
	bge	.1
	add	r0,sp,#4
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	add	r0,sp,#8
	ldr	r0,[r0]
	movw	r1,#:lower16:arr
	movt	r1,#:upper16:arr
	add	r2,sp,#12
	ldr	r2,[r2]
	ldr	r1,[r1,r2,lsl 2]
	add	r3,r0,r1
	add	r0,r2,#1
	add	r1,sp,#12
	str	r0,[r1]
	push	{r0}
	add	r0,sp,#12
	str	r3,[r0]
	add	sp,sp,#12
	b	.0
.1:
	add	r0,sp,#0
	ldr	r0,[r0]
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	.size	main, .-main
	.global arr
	.data 
	.align 2
	.type arr,%object
	.size arr,24
arr:
	.word 1
	.word 2
	.word 33
	.word 4
	.word 5
	.word 6
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
