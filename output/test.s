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
	push	{r4,r5,r6,r7,r8,r9,fp,lr}
	add	fp,sp,#32
	sub	sp,sp,#0
	bl	getint
	mov	r7,r0
	bl	getint
	movw	r0,#13
	bl	_sysy_starttime
	movw	r1,#0
	mov	r9,r1
.0:
	cmp	r9,r7
	bge	.5
	movw	r1,#0
	mov	r8,r1
	movw	r0,#32320
	movt	r0,#5
	mul	r6,r9,r0
.1:
	cmp	r8,r7
	bge	.4
	movw	r1,#0
	mov	r5,r1
	movw	r0,#600
	mul	r4,r8,r0
.2:
	cmp	r5,r7
	bge	.3
	add	r0,r4,r5
	add	r0,r6,r0
	movw	r1,#1
	mov	r2,r1
	movw	r1,#0
	mov	r3,r1
	add	r5,r5,#1
	movw	r1,#:lower16:x
	movt	r1,#:upper16:x
	str	r2,[r1,r0,lsl 2]
	movw	r1,#:lower16:y
	movt	r1,#:upper16:y
	str	r3,[r1,r0,lsl 2]
	b	.2
.3:
	add	r8,r8,#1
	b	.1
.4:
	add	r9,r9,#1
	b	.0
.5:
	movw	r0,#0
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global main.thread.0
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	main.thread.0, %function
main.thread.0:
	push	{r4,r5,r6,r7,r8,r9,fp,lr}
	add	fp,sp,#32
	sub	sp,sp,#0
	mov	r9,r0
	mov	r7,r1
.6:
	cmp	r9,r7
	bge	.7
	movw	r1,#0
	mov	r8,r1
	movw	r0,#32320
	movt	r0,#5
	mul	r5,r9,r0
.8:
	cmp	r8,r7
	bge	.9
	movw	r1,#0
	mov	r6,r1
	movw	r0,#600
	mul	r4,r8,r0
.10:
	cmp	r6,r7
	bge	.11
	add	r0,r4,r6
	add	r0,r5,r0
	movw	r1,#1
	mov	r3,r1
	movw	r1,#0
	mov	r2,r1
	add	r6,r6,#1
	movw	r1,#:lower16:y
	movt	r1,#:upper16:y
	str	r2,[r1,r0,lsl 2]
	movw	r1,#:lower16:x
	movt	r1,#:upper16:x
	str	r3,[r1,r0,lsl 2]
	b	.10
.11:
	add	r8,r8,#1
	b	.8
.9:
	add	r9,r9,#1
	b	.6
.7:
	add	sp,sp,#0
	pop	{r4,r5,r6,r7,r8,r9,fp,pc}
	.size	main.thread.0, .-main.thread.0
	.comm x,864000000,4
	.comm y,864000000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
