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
	.global float_abs
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	float_abs, %function
float_abs:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	vpush	{s0}
	vcmp.f32	s0,#0
	vmrs	APSR_nzcv,FPSCR
	bge	.0
	vldr s0,=0
	add	r0,sp,#0
	vldr.f32	s1,[r0]
	vsub.f32	s2,s0,s1
	vmov	s0,s2
	add	sp,sp,#4
	nop
	pop	{fp,pc}
.0:
	add	r0,sp,#0
	vldr.f32	s0,[r0]
	add	sp,sp,#4
	nop
	pop	{fp,pc}
	.size	float_abs, .-float_abs
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global circle_area
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	circle_area, %function
circle_area:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	bl	__aeabi_i2f
	vldr s0,=1078530011
	vmov	s1,r0
	vmul.f32	s2,s0,s1
	push	{r0}
	vpush	{s2}
	add	r0,sp,#8
	ldr	r0,[r0]
	bl	__aeabi_i2f
	push	{r0}
	add	r0,sp,#4
	vldr.f32	s0,[r0]
	add	r1,sp,#0
	vldr.f32	s1,[r1]
	vmul.f32	s2,s0,s1
	add	r0,sp,#12
	ldr	r0,[r0]
	mul	r1,r0,r0
	push	{r1}
	vpush	{s2}
	mov	r0,r1
	bl	__aeabi_i2f
	vmov	s0,r0
	vldr s1,=1078530011
	vmul.f32	s2,s0,s1
	vpush	{s0}
	add	r0,sp,#4
	vldr.f32	s0,[r0]
	vadd.f32	s1,s0,s2
	vldr s0,=1073741824
	vpush	{s2}
	vdiv.f32	s2,s1,s0
	vmov	s0,s2
	add	sp,sp,#32
	nop
	pop	{fp,pc}
	add	sp,sp,#28
	.size	circle_area, .-circle_area
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global float_eq
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	float_eq, %function
float_eq:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	vpush	{s0,s1}
	vsub.f32	s2,s0,s1
	vpush	{s2}
	vmov	s0,s2
	bl	float_abs
	vldr s1,=897988541
	vcmp.f32	s0,s1
	vmrs	APSR_nzcv,FPSCR
	add	sp,sp,#4
	bge	.1
	movw	r0,#1
	add	sp,sp,#8
	nop
	pop	{fp,pc}
.1:
	movw	r0,#0
	add	sp,sp,#8
	nop
	pop	{fp,pc}
.2:
	movw	r0,#0
	add	sp,sp,#8
	nop
	pop	{fp,pc}
	.size	float_eq, .-float_eq
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global error
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	error, %function
error:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	movw	r0,#101
	bl	putch
	movw	r0,#114
	bl	putch
	movw	r0,#114
	bl	putch
	movw	r0,#111
	bl	putch
	movw	r0,#114
	bl	putch
	movw	r0,#10
	bl	putch
	nop
	pop	{fp,pc}
	.size	error, .-error
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global ok
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	ok, %function
ok:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	movw	r0,#111
	bl	putch
	movw	r0,#107
	bl	putch
	movw	r0,#10
	bl	putch
	nop
	pop	{fp,pc}
	.size	ok, .-ok
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global assert
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	assert, %function
assert:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	push	{r0}
	cmp	r0,#0
	ite eq
	moveq	r0,#1
	movne	r0,#0
	bne	.3
	movw	r0,#101
	bl	putch
	movw	r0,#114
	bl	putch
	movw	r0,#114
	bl	putch
	movw	r0,#111
	bl	putch
	movw	r0,#114
	bl	putch
	movw	r0,#10
	bl	putch
	b	.4
.3:
	movw	r0,#111
	bl	putch
	movw	r0,#107
	bl	putch
	movw	r0,#10
	bl	putch
.4:
	add	sp,sp,#4
	nop
	pop	{fp,pc}
	.size	assert, .-assert
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
	sub	sp,sp,#64
	vldr s0,=1033895936
	vldr s1,=-956241920
	bl	float_eq
	cmp	r0,#0
	push	{r0}
	ite eq
	moveq	r0,#1
	movne	r0,#0
	push	{r0}
	bl	assert
	vldr s0,=1119752446
	vldr s1,=1107966695
	bl	float_eq
	cmp	r0,#0
	push	{r0}
	ite eq
	moveq	r0,#1
	movne	r0,#0
	push	{r0}
	bl	assert
	vldr s0,=1107966695
	vmov	s1,s0
	bl	float_eq
	push	{r0}
	bl	assert
	movw	r0,#5
	bl	circle_area
	vpush	{s0}
	movw	r0,#5
	bl	circle_area
	vpush	{s0}
	add	r0,sp,#4
	vldr.f32	s0,[r0]
	add	r0,sp,#0
	vldr.f32	s1,[r0]
	bl	float_eq
	push	{r0}
	bl	assert
	vldr s0,=1130954752
	vldr s1,=1166012416
	bl	float_eq
	cmp	r0,#0
	push	{r0}
	ite eq
	moveq	r0,#1
	movne	r0,#0
	push	{r0}
	bl	assert
	bl	ok
	add	sp,sp,#40
.5:
	bl	ok
.6:
	b	.7
	bl	error
.7:
	b	.8
	b	.9
.8:
	bl	ok
.9:
	add	r0,sp,#24
	movw	r1,#0
	movw	r2,#40
	bl	memset
	movw	r0,#1
	add	r1,sp,#20
	str	r0,[r1]
	movw	r0,#0
	vldr s0,=1065353216
	add	r1,sp,#12
	str	r0,[r1]
	add	r0,sp,#24
	vstr.f32	s0,[r0,#0]
	vldr s0,=1073741824
	vstr.f32	s0,[r0,#4]
	bl	getfarray
	add	r1,sp,#16
	str	r0,[r1]
.10:
	add	r0,sp,#20
	ldr	r0,[r0]
	movw	r1,#51712
	movt	r1,#15258
	cmp	r0,r1
	bge	.11
	bl	getfloat
	vpush	{s0}
	vldr s0,=1078530011
	add	r0,sp,#0
	vldr.f32	s1,[r0]
	vmul.f32	s2,s0,s1
	vmul.f32	s0,s2,s1
	vpush	{s0}
	vpush	{s2}
	vmov	r0,s1
	bl	__aeabi_f2iz
	push	{r0}
	bl	circle_area
	vpush	{s0}
	add	r0,sp,#44
	add	r1,sp,#32
	ldr	r1,[r1]
	ldr	r2,[r0,r1,lsl 2]
	vmov	s0,r2
	add	r2,sp,#16
	vldr.f32	s1,[r2]
	vadd.f32	s2,s0,s1
	vpush	{s2}
	vmov	r1,s2
	add	r2,sp,#36
	ldr	r2,[r2]
	str	r1,[r0,r2,lsl 2]
	add	r0,sp,#16
	vldr.f32	s0,[r0]
	bl	putfloat
	movw	r0,#32
	bl	putch
	add	r0,sp,#4
	ldr	r0,[r0]
	bl	__aeabi_f2iz
	push	{r0}
	bl	putint
	movw	r0,#10
	bl	putch
	add	r0,sp,#48
	ldr	r0,[r0]
	bl	__aeabi_i2f
	vmov	s0,r0
	vldr s1,=1092616192
	vmul.f32	s2,s0,s1
	push	{r0}
	vpush	{s2}
	vmov	r0,s2
	bl	__aeabi_f2iz
	add	r1,sp,#56
	str	r0,[r1]
	add	r0,sp,#48
	ldr	r0,[r0]
	add	r2,r0,#1
	add	r0,sp,#48
	str	r2,[r0]
	add	sp,sp,#36
	b	.10
.11:
	add	r0,sp,#16
	ldr	r0,[r0]
	add	r1,sp,#24
	bl	putfarray
	movw	r0,#0
	add	sp,sp,#64
	nop
	pop	{fp,pc}
	.size	main, .-main
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
