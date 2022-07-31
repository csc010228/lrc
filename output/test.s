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
	.global meanless_calculation
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	meanless_calculation, %function
meanless_calculation:
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	sub	sp,sp,#4
	sub	sp,sp,#16
	mov	r4,r0
	mov	r3,r1
	movw	r0,#0
	mov	r2,r0
	nop
.0:
	sub	sp,sp,#8
	cmp	r2,r4
	add	sp,sp,#8
	bge	.1
	sub	sp,sp,#8
	cmp	r2,r3
	add	sp,sp,#8
	bge	.1
	sub	sp,sp,#16
	add	r1,r2,#1
	mov	r2,r1
	add	r0,r0,r4
	add	r0,r0,r1
	nop
	add	sp,sp,#16
	b	.0
.1:
	nop
	add	sp,sp,#16
	add	sp,sp,#4
	nop
	pop	{r4,fp,pc}
	.size	meanless_calculation, .-meanless_calculation
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global swap
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	swap, %function
swap:
	push	{r4,r5,r6,fp,lr}
	nop
	add	fp,sp,#20
	sub	sp,sp,#4
	sub	sp,sp,#24
	mov	r6,r0
	nop
	mov	r0,r2
	ldr	r2,[r6,r1,lsl 2]
	mov	r3,r2
	ldr	r5,[r6,r0,lsl 2]
	mov	r2,r5
	str	r2,[r6,r1,lsl 2]
	mov	r5,r3
	movw	r2,#0
	mov	r4,r2
	nop
	str	r5,[r6,r0,lsl 2]
.14:
	sub	sp,sp,#8
	cmp	r4,r1
	add	sp,sp,#8
	bge	.15
	sub	sp,sp,#8
	cmp	r4,r0
	add	sp,sp,#8
	bge	.15
	sub	sp,sp,#16
	add	r3,r4,#1
	mov	r4,r3
	add	r2,r2,r1
	add	r2,r2,r3
	nop
	add	sp,sp,#16
	b	.14
.15:
	mov	r0,r2
	add	sp,sp,#24
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,fp,pc}
	.size	swap, .-swap
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global median
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	median, %function
median:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	nop
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#32
	mov	r10,r0
	mov	r5,r1
	mov	r9,r2
	mov	r6,r3
	sub	sp,sp,#8
	ldr	r0,[r10,r5,lsl 2]
	mov	r4,r0
	mov	r7,r5
	add	r0,r9,#1
	mov	r8,r0
	movw	r0,#0
	mov	r1,r0
	add	r0,sp,#8
	str	r1,[r0]
	add	sp,sp,#8
.2:
.3:
	sub	sp,sp,#8
	cmp	r7,r8
	add	sp,sp,#8
	bge	.5
	sub	sp,sp,#8
	sub	r0,r8,#1
	mov	r8,r0
	ldr	r0,[r10,r0,lsl 2]
	cmp	r0,r4
	add	sp,sp,#8
	bge	.4
	b	.5
.4:
	sub	sp,sp,#8
	add	r0,sp,#8
	ldr	r1,[r0]
	add	r1,r1,#1
	nop
	str	r1,[r0]
	add	sp,sp,#8
	b	.3
.5:
.6:
	sub	sp,sp,#8
	cmp	r7,r8
	add	sp,sp,#8
	bge	.8
	sub	sp,sp,#8
	add	r0,r7,#1
	mov	r7,r0
	ldr	r0,[r10,r0,lsl 2]
	cmp	r0,r4
	add	sp,sp,#8
	blt	.7
	b	.8
.7:
	sub	sp,sp,#8
	add	r0,sp,#8
	ldr	r1,[r0]
	sub	r1,r1,#1
	nop
	str	r1,[r0]
	add	sp,sp,#8
	b	.6
.8:
	sub	sp,sp,#8
	cmp	r7,r8
	add	sp,sp,#8
	bne	.9
	b	.11
	b	.10
.9:
	sub	sp,sp,#8
	mov	r0,r10
	mov	r1,r7
	mov	r2,r8
	bl	swap
	nop
	add	sp,sp,#8
.10:
	b	.2
.11:
	sub	sp,sp,#8
	mov	r0,r4
	str	r0,[r10,r5,lsl 2]
	mov	r0,r10
	mov	r1,r5
	mov	r2,r7
	bl	swap
	nop
	cmp	r7,r6
	add	sp,sp,#8
	ble	.12
	sub	sp,sp,#8
	mov	r0,r10
	mov	r1,r5
	mov	r2,r7
	mov	r3,r6
	bl	median
	nop
	nop
	add	sp,sp,#40
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#8
.12:
	sub	sp,sp,#8
	cmp	r7,r6
	add	sp,sp,#8
	bge	.13
	sub	sp,sp,#8
	add	r1,r7,#1
	mov	r0,r10
	nop
	mov	r2,r9
	mov	r3,r6
	bl	median
	nop
	nop
	add	sp,sp,#40
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#8
.13:
	add	r0,sp,#0
	ldr	r0,[r0]
	nop
	add	sp,sp,#32
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	median, .-median
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
	push	{r4,r5,r6,r7,r8,fp,lr}
	nop
	add	fp,sp,#28
	sub	sp,sp,#4
	sub	sp,sp,#24
	movw	r7,#:lower16:a
	movt	r7,#:upper16:a
	mov	r0,r7
	bl	getarray
	mov	r6,r0
	mov	r5,r6
	movw	r0,#59
	nop
	bl	_sysy_starttime
	sub	r8,r6,#1
	mov	r0,r6
	movw	r1,#2
	nop
	bl	__aeabi_idiv
	mov	r4,r0
	mov	r0,r7
	movw	r1,#0
	nop
	mov	r2,r8
	mov	r3,r4
	bl	median
	nop
	movw	r0,#61
	nop
	bl	_sysy_stoptime
	mov	r0,r6
	mov	r1,r7
	bl	putarray
	ldr	r0,[r7,r4,lsl 2]
	nop
	movw	r1,#256
	nop
	bl	__aeabi_idivmod
	mov	r0,r1
	movw	r1,#:lower16:n
	movt	r1,#:upper16:n
	str	r5,[r1]
	nop
	add	sp,sp,#24
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,fp,pc}
	add	sp,sp,#24
	.size	main, .-main
	.comm n,4,4
	.comm a,40000000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
