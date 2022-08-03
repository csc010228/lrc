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
	.global reduce
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	reduce, %function
reduce:
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	sub	sp,sp,#4
	nop
	mov	r4,r0
	nop
	mov	r0,r4
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	nop
	bl	__aeabi_idivmod
	nop
	movw	r0,#:lower16:head
	movt	r0,#:upper16:head
	ldr	r0,[r0,r1,lsl 2]
	mov	r1,r0
	nop
.4:
	nop
	cmp	r1,#0
	nop
	beq	.8
	nop
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	ldr	r0,[r0,r1,lsl 2]
	cmp	r0,r4
	nop
	bne	.7
	nop
	movw	r0,#0
	mov	r2,r0
	nop
	nop
.5:
	nop
	cmp	r1,#0
	nop
	beq	.6
	nop
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	ldr	r0,[r0,r1,lsl 2]
	add	r0,r2,r0
	mov	r2,r0
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	ldr	r0,[r0,r1,lsl 2]
	mov	r1,r0
	nop
	b	.5
.6:
	nop
	mov	r0,r2
	nop
	nop
	add	sp,sp,#4
	nop
	pop	{r4,fp,pc}
	nop
.7:
	nop
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	ldr	r0,[r0,r1,lsl 2]
	mov	r1,r0
	nop
	b	.4
.8:
	nop
	movw	r0,#0
	nop
	nop
	add	sp,sp,#4
	nop
	pop	{r4,fp,pc}
	nop
	.size	reduce, .-reduce
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
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	nop
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#48
	sub	sp,sp,#16
	bl	getint
	nop
	nop
	movw	r5,#:lower16:hashmod
	movt	r5,#:upper16:hashmod
	str	r0,[r5]
	movw	r0,#:lower16:keys
	movt	r0,#:upper16:keys
	movw	r0,#:lower16:keys
	movt	r0,#:upper16:keys
	nop
	bl	getarray
	nop
	mov	r1,r0
	add	r0,sp,#56
	add	r0,sp,#56
	str	r1,[r0]
	movw	r0,#:lower16:values
	movt	r0,#:upper16:values
	movw	r0,#:lower16:values
	movt	r0,#:upper16:values
	nop
	bl	getarray
	nop
	movw	r0,#:lower16:requests
	movt	r0,#:upper16:requests
	movw	r0,#:lower16:requests
	movt	r0,#:upper16:requests
	nop
	bl	getarray
	nop
	nop
	add	r4,sp,#52
	str	r0,[r4]
	movw	r0,#78
	nop
	bl	_sysy_starttime
	movw	r8,#0
	mov	r0,r8
	add	r6,sp,#60
	str	r0,[r6]
	add	sp,sp,#16
.9:
	sub	sp,sp,#8
	add	r0,sp,#48
	ldr	r1,[r0]
	ldr	r0,[r6]
	cmp	r0,r1
	add	sp,sp,#8
	bge	.10
	sub	sp,sp,#8
	ldr	r1,[r6]
	movw	r0,#:lower16:keys
	movt	r0,#:upper16:keys
	ldr	r7,[r0,r1,lsl 2]
	mov	r0,r7
	ldr	r1,[r5]
	nop
	bl	__aeabi_idivmod
	mov	r3,r1
	mov	r0,r3
	add	r1,sp,#20
	str	r0,[r1]
	movw	r2,#:lower16:head
	movt	r2,#:upper16:head
	ldr	r3,[r2,r3,lsl 2]
	cmp	r3,#0
	add	sp,sp,#8
	bne	.13
	sub	sp,sp,#8
	movw	r8,#:lower16:cnt
	movt	r8,#:upper16:cnt
	ldr	r3,[r8]
	str	r3,[r8]
	ldr	r3,[r8]
	add	r9,r3,#1
	add	r3,sp,#4
	str	r9,[r3]
	ldr	r9,[r3]
	nop
	str	r9,[r8]
	ldr	r10,[r3]
	mov	r8,r10
	str	r8,[r2,r0,lsl 2]
	mov	r0,r7
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	str	r0,[r8,r10,lsl 2]
	ldr	r7,[r6]
	movw	r0,#:lower16:values
	movt	r0,#:upper16:values
	ldr	r7,[r0,r7,lsl 2]
	nop
	movw	r8,#0
	mov	r9,r8
	mov	r0,r8
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	str	r0,[r8,r10,lsl 2]
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	ldr	r0,[r3]
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	ldr	r10,[r8,r0,lsl 2]
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	str	r10,[r8,r0,lsl 2]
	ldr	r1,[r1]
	ldr	r0,[r2,r1,lsl 2]
	str	r0,[r2,r1,lsl 2]
	movw	r8,#:lower16:next
	movt	r8,#:upper16:next
	ldr	r1,[r3]
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	str	r9,[r0,r1,lsl 2]
	movw	r8,#:lower16:value
	movt	r8,#:upper16:value
	ldr	r0,[r3]
	str	r7,[r8,r0,lsl 2]
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	ldr	r1,[r3]
	ldr	r0,[r8,r1,lsl 2]
	str	r0,[r8,r1,lsl 2]
	add	sp,sp,#8
	b	.17
.13:
	nop
	ldr	r9,[r1]
	ldr	r0,[r2,r9,lsl 2]
	str	r0,[r2,r9,lsl 2]
	ldr	r0,[r2,r9,lsl 2]
	mov	r3,r0
	nop
.14:
	nop
	cmp	r3,#0
	nop
	beq	.15
	nop
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	ldr	r0,[r8,r3,lsl 2]
	cmp	r0,r7
	nop
	bne	.16
	nop
	movw	r8,#:lower16:cnt
	movt	r8,#:upper16:cnt
	ldr	r0,[r8]
	str	r0,[r8]
	ldr	r0,[r8]
	add	r1,r0,#1
	mov	r0,r1
	str	r0,[r8]
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	ldr	r0,[r0,r3,lsl 2]
	mov	r2,r0
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	str	r2,[r0,r1,lsl 2]
	mov	r0,r1
	ldr	r7,[r6]
	movw	r2,#:lower16:values
	movt	r2,#:upper16:values
	ldr	r7,[r2,r7,lsl 2]
	mov	r2,r7
	movw	r8,#:lower16:value
	movt	r8,#:upper16:value
	str	r2,[r8,r1,lsl 2]
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r0,[r1,r3,lsl 2]
	nop
	b	.17
.16:
	nop
	movw	r8,#:lower16:next
	movt	r8,#:upper16:next
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	ldr	r0,[r0,r3,lsl 2]
	mov	r3,r0
	nop
	b	.14
.15:
	nop
	movw	r8,#:lower16:cnt
	movt	r8,#:upper16:cnt
	ldr	r0,[r8]
	str	r0,[r8]
	ldr	r0,[r8]
	add	r0,r0,#1
	mov	r3,r0
	str	r3,[r8]
	ldr	r3,[r2,r9,lsl 2]
	mov	r10,r3
	movw	r8,#:lower16:next
	movt	r8,#:upper16:next
	movw	r3,#:lower16:next
	movt	r3,#:upper16:next
	str	r10,[r3,r0,lsl 2]
	mov	r3,r0
	str	r3,[r2,r9,lsl 2]
	mov	r3,r7
	ldr	r8,[r6]
	movw	r7,#:lower16:values
	movt	r7,#:upper16:values
	ldr	r7,[r7,r8,lsl 2]
	nop
	movw	r8,#:lower16:value
	movt	r8,#:upper16:value
	str	r7,[r8,r0,lsl 2]
	movw	r8,#0
	mov	r7,r8
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	str	r3,[r8,r0,lsl 2]
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	movw	r3,#:lower16:nextvalue
	movt	r3,#:upper16:nextvalue
	str	r7,[r3,r0,lsl 2]
	movw	r8,#:lower16:next
	movt	r8,#:upper16:next
	movw	r3,#:lower16:next
	movt	r3,#:upper16:next
	ldr	r7,[r3,r0,lsl 2]
	movw	r3,#:lower16:next
	movt	r3,#:upper16:next
	str	r7,[r3,r0,lsl 2]
	movw	r8,#:lower16:value
	movt	r8,#:upper16:value
	ldr	r3,[r8,r0,lsl 2]
	str	r3,[r8,r0,lsl 2]
	ldr	r1,[r1]
	ldr	r0,[r2,r1,lsl 2]
	str	r0,[r2,r1,lsl 2]
	nop
	b	.17
.17:
	nop
	ldr	r0,[r6]
	add	r0,r0,#1
	nop
	str	r0,[r6]
	movw	r8,#:lower16:cnt
	movt	r8,#:upper16:cnt
	ldr	r0,[r8]
	str	r0,[r8]
	nop
	b	.9
.10:
	nop
	movw	r8,#0
	mov	r0,r8
	str	r0,[r6]
	nop
.11:
	nop
	ldr	r1,[r6]
	ldr	r0,[r4]
	cmp	r1,r0
	nop
	bge	.12
	nop
	ldr	r1,[r6]
	movw	r0,#:lower16:requests
	movt	r0,#:upper16:requests
	ldr	r7,[r0,r1,lsl 2]
	mov	r0,r7
	ldr	r1,[r5]
	nop
	bl	__aeabi_idivmod
	mov	r0,r1
	movw	r2,#:lower16:head
	movt	r2,#:upper16:head
	ldr	r0,[r2,r0,lsl 2]
	nop
	nop
.18:
	nop
	cmp	r0,#0
	nop
	beq	.19
	nop
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	ldr	r1,[r8,r0,lsl 2]
	cmp	r1,r7
	nop
	bne	.20
	nop
	movw	r8,#0
	mov	r1,r8
	mov	r2,r0
	nop
.21:
	nop
	cmp	r2,#0
	nop
	beq	.22
	nop
	movw	r8,#:lower16:value
	movt	r8,#:upper16:value
	ldr	r0,[r8,r2,lsl 2]
	add	r0,r1,r0
	mov	r1,r0
	movw	r8,#:lower16:nextvalue
	movt	r8,#:upper16:nextvalue
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	ldr	r0,[r0,r2,lsl 2]
	mov	r2,r0
	nop
	b	.21
.22:
	nop
	nop
	nop
	b	.23
.20:
	nop
	movw	r8,#:lower16:next
	movt	r8,#:upper16:next
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	ldr	r0,[r1,r0,lsl 2]
	nop
	nop
	b	.18
.19:
	nop
	movw	r8,#0
	mov	r1,r8
	nop
	b	.23
.23:
	nop
	nop
	ldr	r0,[r6]
	add	r2,r0,#1
	movw	r3,#:lower16:ans
	movt	r3,#:upper16:ans
	ldr	r0,[r6]
	str	r1,[r3,r0,lsl 2]
	mov	r0,r2
	str	r0,[r6]
	nop
	b	.11
.12:
	nop
	movw	r0,#90
	nop
	bl	_sysy_stoptime
	ldr	r0,[r4]
	nop
	movw	r3,#:lower16:ans
	movt	r3,#:upper16:ans
	mov	r1,r3
	bl	putarray
	movw	r5,#:lower16:hashmod
	movt	r5,#:upper16:hashmod
	ldr	r0,[r5]
	str	r0,[r5]
	movw	r0,#0
	nop
	add	sp,sp,#48
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	nop
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global hash
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	hash, %function
hash:
	push	{fp,lr}
	nop
	add	fp,sp,#8
	sub	sp,sp,#0
	nop
	nop
	nop
	nop
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	nop
	bl	__aeabi_idivmod
	mov	r0,r1
	nop
	nop
	nop
	add	sp,sp,#0
	nop
	pop	{fp,pc}
	nop
	.size	hash, .-hash
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global insert
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	insert, %function
insert:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	nop
	add	fp,sp,#36
	sub	sp,sp,#4
	nop
	mov	r5,r0
	mov	r4,r1
	nop
	mov	r0,r5
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	nop
	bl	__aeabi_idivmod
	mov	r0,r1
	mov	r3,r0
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	ldr	r0,[r1,r0,lsl 2]
	cmp	r0,#0
	nop
	bne	.0
	nop
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	ldr	r0,[r2]
	add	r6,r0,#1
	mov	r0,r6
	mov	r7,r6
	nop
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	str	r5,[r8,r6,lsl 2]
	nop
	movw	r9,#:lower16:value
	movt	r9,#:upper16:value
	str	r4,[r9,r6,lsl 2]
	movw	r4,#0
	mov	r10,r4
	mov	r5,r4
	str	r7,[r1,r3,lsl 2]
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	ldr	r1,[r8,r6,lsl 2]
	str	r1,[r8,r6,lsl 2]
	movw	r9,#:lower16:value
	movt	r9,#:upper16:value
	ldr	r1,[r9,r6,lsl 2]
	str	r1,[r9,r6,lsl 2]
	movw	r9,#:lower16:next
	movt	r9,#:upper16:next
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	str	r10,[r1,r6,lsl 2]
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r5,[r1,r6,lsl 2]
	str	r0,[r2]
	mov	r0,r4
	nop
	nop
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	nop
.0:
	nop
	ldr	r7,[r1,r3,lsl 2]
	mov	r6,r7
	nop
.1:
	nop
	cmp	r6,#0
	nop
	beq	.3
	nop
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	ldr	r0,[r8,r6,lsl 2]
	cmp	r0,r5
	nop
	bne	.2
	nop
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	ldr	r0,[r2]
	add	r3,r0,#1
	mov	r0,r3
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	ldr	r5,[r1,r6,lsl 2]
	nop
	str	r5,[r1,r3,lsl 2]
	mov	r5,r3
	nop
	str	r5,[r1,r6,lsl 2]
	movw	r9,#:lower16:value
	movt	r9,#:upper16:value
	str	r4,[r9,r3,lsl 2]
	str	r0,[r2]
	movw	r0,#1
	nop
	nop
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	nop
.2:
	nop
	movw	r9,#:lower16:next
	movt	r9,#:upper16:next
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	ldr	r0,[r0,r6,lsl 2]
	mov	r6,r0
	nop
	b	.1
.3:
	nop
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	ldr	r0,[r2]
	add	r6,r0,#1
	mov	r0,r6
	mov	r8,r7
	mov	r7,r6
	nop
	nop
	movw	r9,#:lower16:value
	movt	r9,#:upper16:value
	str	r4,[r9,r6,lsl 2]
	movw	r4,#0
	mov	r10,r4
	str	r7,[r1,r3,lsl 2]
	movw	r9,#:lower16:next
	movt	r9,#:upper16:next
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	str	r8,[r1,r6,lsl 2]
	movw	r8,#:lower16:key
	movt	r8,#:upper16:key
	str	r5,[r8,r6,lsl 2]
	movw	r9,#:lower16:value
	movt	r9,#:upper16:value
	ldr	r1,[r9,r6,lsl 2]
	str	r1,[r9,r6,lsl 2]
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r10,[r1,r6,lsl 2]
	str	r0,[r2]
	mov	r0,r4
	nop
	nop
	add	sp,sp,#4
	nop
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	nop
	.size	insert, .-insert
	.comm hashmod,4,4
	.comm bucket,40000000,4
	.comm head,40000000,4
	.comm next,40000000,4
	.comm nextvalue,40000000,4
	.comm key,40000000,4
	.comm value,40000000,4
	.comm cnt,4,4
	.comm keys,40000000,4
	.comm values,40000000,4
	.comm requests,40000000,4
	.comm ans,40000000,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
