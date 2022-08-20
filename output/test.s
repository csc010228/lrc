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
	push	{fp,lr}
	add	fp,sp,#8
	sub	sp,sp,#0
	mov	r3,r0
	movw	r0,#0
	mov	r2,r0
.0:
	cmp	r2,r3
	bge	.1
	cmp	r2,r1
	bge	.1
	add	r2,r2,#1
	add	r0,r0,r3
	add	r0,r0,r2
	b	.0
.1:
	add	sp,sp,#0
	pop	{fp,pc}
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
	push	{r4,r5,fp,lr}
	add	fp,sp,#16
	sub	sp,sp,#0
	mov	r5,r0
	mov	r3,r1
	ldr	r0,[r5,r3,lsl 2]
	mov	r1,r0
	ldr	r4,[r5,r2,lsl 2]
	mov	r0,r4
	str	r0,[r5,r3,lsl 2]
	mov	r4,r1
	movw	r0,#0
	mov	r1,r0
	str	r4,[r5,r2,lsl 2]
.14:
	cmp	r1,r3
	bge	.15
	cmp	r1,r2
	bge	.15
	add	r1,r1,#1
	add	r0,r0,r3
	add	r0,r0,r1
	b	.14
.15:
	add	sp,sp,#0
	pop	{r4,r5,fp,pc}
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
	add	fp,sp,#36
	sub	sp,sp,#4
	mov	r8,r0
	mov	r9,r1
	mov	r10,r2
	mov	fp,r3
	ldr	r0,[r8,r9,lsl 2]
	mov	r6,r0
	mov	r7,r9
	add	r0,r10,#1
	mov	r5,r0
	movw	r0,#0
	mov	r4,r0
.2:
.3:
	cmp	r7,r5
	bge	.5
	sub	r5,r5,#1
	ldr	r0,[r8,r5,lsl 2]
	cmp	r0,r6
	it lt
	blt	.5
.4:
	add	r4,r4,#1
	b	.3
.5:
.6:
	cmp	r7,r5
	bge	.8
	add	r7,r7,#1
	ldr	r0,[r8,r7,lsl 2]
	cmp	r0,r6
	it ge
	bge	.8
.7:
	sub	r4,r4,#1
	b	.6
.8:
	cmp	r7,r5
	bne	.9
	b	.11
	b	.10
.9:
	mov	r0,r8
	mov	r1,r7
	mov	r2,r5
	bl	swap
.10:
	b	.2
.11:
	mov	r0,r6
	str	r0,[r8,r9,lsl 2]
	mov	r0,r8
	mov	r1,r9
	mov	r2,r7
	bl	swap
	cmp	r7,fp
	ble	.12
	mov	r0,r8
	mov	r1,r9
	mov	r2,r7
	mov	r3,fp
	bl	median
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.12:
	cmp	r7,fp
	bge	.13
	add	r1,r7,#1
	mov	r0,r8
	mov	r2,r10
	mov	r3,fp
	bl	median
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.13:
	mov	r0,r4
	add	sp,sp,#4
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
	push	{r4,r5,r6,fp,lr}
	add	fp,sp,#20
	sub	sp,sp,#4
	movw	r5,#:lower16:a
	movt	r5,#:upper16:a
	mov	r0,r5
	bl	getarray
	mov	r6,r0
	movw	r0,#59
	bl	_sysy_starttime
	sub	r2,r6,#1
	add	r4,r6,r6,lsr 31
	asr	r4,r6,#1
	mov	r0,r5
	movw	r1,#0
	mov	r3,r4
	bl	median
	movw	r0,#61
	bl	_sysy_stoptime
	mov	r0,r6
	mov	r1,r5
	bl	putarray
	ldr	r0,[r5,r4,lsl 2]
	movw	r1,#256
	bl	__aeabi_idivmod
	movw	r0,#:lower16:n
	movt	r0,#:upper16:n
	str	r6,[r0]
	mov	r0,r1
	add	sp,sp,#4
	pop	{r4,r5,r6,fp,pc}
	.size	main, .-main
	.comm n,4,4
	.comm a,40000000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
