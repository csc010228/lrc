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
	.global add
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	add, %function
add:
	push	{r4,r5,r6,fp,lr}
	nop
	add	fp,sp,#20
	push	{r0,r1,r2,r3}
	sub	sp,sp,#4
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
.0:
	add	r0,sp,#0
	ldr	r0,[r0]
	movw	r1,#:lower16:M
	movt	r1,#:upper16:M
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.1
	add	r0,sp,#0
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	add	r0,sp,#28
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	ldr	r1,[r0,r1,lsl 2]
	vmov	s0,r1
	add	r2,sp,#40
	ldr	r2,[r2]
	add	r3,sp,#0
	ldr	r3,[r3]
	ldr	r3,[r2,r3,lsl 2]
	vmov	s1,r3
	vadd.f32	s2,s0,s1
	add	r0,sp,#24
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	add	r0,sp,#56
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	ldr	r1,[r0,r1,lsl 2]
	vmov	s0,r1
	add	r2,fp,#0
	ldr	r2,[r2]
	add	r3,sp,#0
	ldr	r3,[r3]
	ldr	r3,[r2,r3,lsl 2]
	vmov	s1,r3
	vpush	{s2}
	add	r4,fp,#8
	ldr	r4,[r4]
	vmov	r5,s2
	add	r6,sp,#44
	ldr	r6,[r6]
	str	r5,[r4,r6,lsl 2]
	vadd.f32	s2,s0,s1
	add	r0,sp,#52
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	push	{r0}
	add	r0,sp,#88
	ldr	r0,[r0]
	add	r1,sp,#8
	ldr	r1,[r1]
	ldr	r1,[r0,r1,lsl 2]
	vmov	s0,r1
	add	r2,fp,#4
	ldr	r2,[r2]
	add	r3,sp,#0
	ldr	r3,[r3]
	ldr	r3,[r2,r3,lsl 2]
	vmov	s1,r3
	vpush	{s2}
	add	r4,fp,#12
	ldr	r4,[r4]
	vmov	r5,s2
	add	r6,sp,#48
	ldr	r6,[r6]
	str	r5,[r4,r6,lsl 2]
	vadd.f32	s2,s0,s1
	add	r0,sp,#80
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#80
	str	r1,[r0]
	add	r0,fp,#16
	ldr	r0,[r0]
	push	{r1}
	vmov	r1,s2
	add	r2,sp,#24
	ldr	r2,[r2]
	str	r1,[r0,r2,lsl 2]
	add	sp,sp,#84
	b	.0
.1:
	movw	r0,#0
	add	sp,sp,#20
	nop
	pop	{r4,r5,r6,fp,pc}
	.size	add, .-add
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
	sub	sp,sp,#128
	movw	r0,#3
	movw	r1,#:lower16:M
	movt	r1,#:upper16:M
	str	r0,[r1]
	movw	r2,#:lower16:L
	movt	r2,#:upper16:L
	str	r0,[r2]
	movw	r3,#:lower16:N
	movt	r3,#:upper16:N
	str	r0,[r3]
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
.2:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:M
	movt	r1,#:upper16:M
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.3
	add	r0,sp,#4
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	bl	__aeabi_i2f
	add	r1,sp,#124
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#12
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	bl	__aeabi_i2f
	add	r1,sp,#120
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#20
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	bl	__aeabi_i2f
	add	r1,sp,#116
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#28
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	bl	__aeabi_i2f
	add	r1,sp,#112
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#36
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	bl	__aeabi_i2f
	add	r1,sp,#108
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#44
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	bl	__aeabi_i2f
	add	r1,sp,#104
	add	r2,sp,#0
	ldr	r2,[r2]
	str	r0,[r1,r2,lsl 2]
	add	r0,sp,#52
	ldr	r0,[r0]
	add	r3,r0,#1
	add	r0,sp,#52
	str	r3,[r0]
	add	sp,sp,#48
	b	.2
.3:
	add	r0,sp,#8
	push	{r0}
	add	r0,sp,#24
	push	{r0}
	add	r0,sp,#40
	push	{r0}
	add	r0,sp,#68
	push	{r0}
	add	r0,sp,#84
	push	{r0}
	add	r0,sp,#136
	add	r1,sp,#124
	add	r2,sp,#112
	add	r3,sp,#100
	bl	add
	add	sp,sp,#20
	add	r1,sp,#4
	str	r0,[r1]
.4:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.5
	add	r0,sp,#4
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	add	r0,sp,#40
	add	r1,sp,#0
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	bl	__aeabi_f2iz
	add	r1,sp,#8
	str	r0,[r1]
	bl	putint
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#12
	str	r1,[r0]
	add	sp,sp,#8
	b	.4
.5:
	movw	r0,#10
	add	r1,sp,#0
	str	r0,[r1]
	bl	putch
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
.6:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.7
	add	r0,sp,#4
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	add	r0,sp,#28
	add	r1,sp,#0
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	bl	__aeabi_f2iz
	add	r1,sp,#8
	str	r0,[r1]
	bl	putint
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#12
	str	r1,[r0]
	add	sp,sp,#8
	b	.6
.7:
	movw	r0,#10
	add	r1,sp,#0
	str	r0,[r1]
	bl	putch
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
.8:
	add	r0,sp,#4
	ldr	r0,[r0]
	movw	r1,#:lower16:N
	movt	r1,#:upper16:N
	ldr	r1,[r1]
	cmp	r0,r1
	bge	.9
	add	r0,sp,#4
	ldr	r0,[r0]
	push	{r0}
	push	{r0}
	add	r0,sp,#16
	add	r1,sp,#0
	ldr	r1,[r1]
	ldr	r0,[r0,r1,lsl 2]
	bl	__aeabi_f2iz
	add	r1,sp,#8
	str	r0,[r1]
	bl	putint
	add	r0,sp,#12
	ldr	r0,[r0]
	add	r1,r0,#1
	add	r0,sp,#12
	str	r1,[r0]
	add	sp,sp,#8
	b	.8
.9:
	movw	r0,#10
	add	r1,sp,#0
	str	r0,[r1]
	bl	putch
	movw	r0,#0
	add	sp,sp,#128
	nop
	pop	{fp,pc}
	.size	main, .-main
	.comm M,4,4
	.comm L,4,4
	.comm N,4,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
