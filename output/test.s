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
	add	fp,sp,#12
	sub	sp,sp,#4
	mov	r4,r0
	mov	r0,r4
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	bl	__aeabi_idivmod
	movw	r0,#:lower16:head
	movt	r0,#:upper16:head
	ldr	r0,[r0,r1,lsl 2]
	mov	r2,r0
.4:
	cmp	r2,#0
	beq	.8
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	ldr	r0,[r0,r2,lsl 2]
	cmp	r0,r4
	bne	.7
	movw	r0,#0
	mov	r1,r0
.5:
	cmp	r2,#0
	beq	.6
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	ldr	r0,[r0,r2,lsl 2]
	add	r1,r1,r0
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	ldr	r0,[r0,r2,lsl 2]
	mov	r2,r0
	b	.5
.6:
	mov	r0,r1
	add	sp,sp,#4
	pop	{r4,fp,pc}
.7:
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	ldr	r0,[r0,r2,lsl 2]
	mov	r2,r0
	b	.4
.8:
	movw	r0,#0
	add	sp,sp,#4
	pop	{r4,fp,pc}
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
	add	fp,sp,#36
	sub	sp,sp,#4
	sub	sp,sp,#56
	sub	sp,sp,#16
	bl	getint
	mov	r1,r0
	movw	r0,#:lower16:hashmod
	movt	r0,#:upper16:hashmod
	movw	r0,#:lower16:hashmod
	movt	r0,#:upper16:hashmod
	str	r1,[r0]
	movw	r0,#:lower16:keys
	movt	r0,#:upper16:keys
	movw	r0,#:lower16:keys
	movt	r0,#:upper16:keys
	bl	getarray
	mov	r10,r0
	movw	r6,#:lower16:values
	movt	r6,#:upper16:values
	mov	r0,r6
	bl	getarray
	movw	r0,#:lower16:requests
	movt	r0,#:upper16:requests
	movw	r0,#:lower16:requests
	movt	r0,#:upper16:requests
	bl	getarray
	mov	r1,r0
	str	r1,[sp,#60]
	mov	r0,#78
	bl	_sysy_starttime
	mov	r7,#0
	add	sp,sp,#16
.9:
	cmp	r7,r10
	bge	.10
	movw	r0,#:lower16:keys
	movt	r0,#:upper16:keys
	ldr	r5,[r0,r7,lsl 2]
	mov	r0,r5
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	bl	__aeabi_idivmod
	mov	r0,r1
	mov	r9,r0
	movw	r3,#:lower16:head
	movt	r3,#:upper16:head
	ldr	r0,[r3,r0,lsl 2]
	cmp	r0,#0
	bne	.13
	movw	r0,#:lower16:cnt
	movt	r0,#:upper16:cnt
	ldr	r2,[r0]
	add	r2,r2,#1
	mov	r0,r2
	mov	r8,r5
	ldr	r1,[r6,r7,lsl 2]
	mov	r5,r1
	mov	r4,#0
	mov	r1,#0
	str	r0,[r3,r9,lsl 2]
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	str	r4,[r0,r2,lsl 2]
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	str	r1,[r0,r2,lsl 2]
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	str	r8,[r0,r2,lsl 2]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r5,[r0,r2,lsl 2]
	b	.17
.13:
	ldr	r1,[r3,r9,lsl 2]
	mov	r4,r1
.14:
	cmp	r4,#0
	beq	.15
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	ldr	r0,[r0,r4,lsl 2]
	cmp	r0,r5
	bne	.16
	movw	r0,#:lower16:cnt
	movt	r0,#:upper16:cnt
	ldr	r2,[r0]
	add	r2,r2,#1
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	ldr	r1,[r0,r4,lsl 2]
	str	r1,[r0,r2,lsl 2]
	mov	r3,r2
	ldr	r1,[r6,r7,lsl 2]
	str	r3,[r0,r4,lsl 2]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r1,[r0,r2,lsl 2]
	b	.17
.16:
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	ldr	r0,[r0,r4,lsl 2]
	mov	r4,r0
	b	.14
.15:
	movw	r0,#:lower16:cnt
	movt	r0,#:upper16:cnt
	ldr	r2,[r0]
	add	r2,r2,#1
	mov	r4,r1
	mov	r1,r2
	mov	r8,r5
	ldr	r0,[r6,r7,lsl 2]
	mov	r5,r0
	mov	r0,#0
	str	r1,[r3,r9,lsl 2]
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	str	r4,[r1,r2,lsl 2]
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r0,[r1,r2,lsl 2]
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	str	r8,[r0,r2,lsl 2]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r5,[r0,r2,lsl 2]
	b	.17
.17:
	add	r7,r7,#1
	movw	r0,#:lower16:cnt
	movt	r0,#:upper16:cnt
	str	r2,[r0]
	b	.9
.10:
	mov	r7,#0
.11:
	sub	sp,sp,#8
	add	r0,sp,#52
	ldr	r0,[r0]
	cmp	r7,r0
	add	sp,sp,#8
	bge	.12
	movw	r0,#:lower16:requests
	movt	r0,#:upper16:requests
	ldr	r4,[r0,r7,lsl 2]
	mov	r0,r4
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	bl	__aeabi_idivmod
	mov	r0,r1
	movw	r3,#:lower16:head
	movt	r3,#:upper16:head
	ldr	r0,[r3,r0,lsl 2]
	mov	r2,r0
.18:
	cmp	r2,#0
	beq	.19
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	ldr	r0,[r0,r2,lsl 2]
	cmp	r0,r4
	bne	.20
	mov	r1,#0
.21:
	cmp	r2,#0
	beq	.22
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	ldr	r0,[r0,r2,lsl 2]
	add	r1,r1,r0
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	ldr	r0,[r0,r2,lsl 2]
	mov	r2,r0
	b	.21
.22:
	movw	r0,#:lower16:ans
	movt	r0,#:upper16:ans
	str	r1,[r0,r7,lsl 2]
	b	.23
.20:
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	ldr	r0,[r0,r2,lsl 2]
	mov	r2,r0
	b	.18
.19:
	mov	r1,#0
	movw	r0,#:lower16:ans
	movt	r0,#:upper16:ans
	str	r1,[r0,r7,lsl 2]
	b	.23
.23:
	add	r7,r7,#1
	b	.11
.12:
	sub	sp,sp,#0
	mov	r0,#90
	bl	_sysy_stoptime
	add	r0,sp,#44
	ldr	r0,[r0]
	movw	r1,#:lower16:ans
	movt	r1,#:upper16:ans
	bl	putarray
	movw	r0,#:lower16:hashmod
	movt	r0,#:upper16:hashmod
	movw	r0,#:lower16:hashmod
	movt	r0,#:upper16:hashmod
	ldr	r1,[r0]
	movw	r0,#:lower16:hashmod
	movt	r0,#:upper16:hashmod
	str	r1,[r0]
	movw	r0,#0
	add	sp,sp,#0
	add	sp,sp,#56
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0
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
	add	fp,sp,#8
	sub	sp,sp,#0
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	bl	__aeabi_idivmod
	mov	r0,r1
	add	sp,sp,#0
	pop	{fp,pc}
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
	add	fp,sp,#36
	sub	sp,sp,#4
	mov	r4,r0
	mov	r5,r1
	mov	r0,r4
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	ldr	r1,[r1]
	bl	__aeabi_idivmod
	mov	r0,r1
	mov	r10,r0
	movw	r9,#:lower16:head
	movt	r9,#:upper16:head
	ldr	r0,[r9,r0,lsl 2]
	cmp	r0,#0
	bne	.0
	movw	r3,#:lower16:cnt
	movt	r3,#:upper16:cnt
	ldr	r2,[r3]
	add	r2,r2,#1
	mov	r8,r2
	mov	r7,r4
	movw	r6,#0
	mov	r1,r6
	mov	r4,r6
	str	r2,[r3]
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	str	r1,[r0,r2,lsl 2]
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	str	r4,[r0,r2,lsl 2]
	str	r8,[r9,r10,lsl 2]
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	str	r7,[r0,r2,lsl 2]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r5,[r0,r2,lsl 2]
	mov	r0,r6
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.0:
	ldr	r8,[r9,r10,lsl 2]
	mov	r6,r8
.1:
	cmp	r6,#0
	beq	.3
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	ldr	r0,[r0,r6,lsl 2]
	cmp	r0,r4
	bne	.2
	movw	r3,#:lower16:cnt
	movt	r3,#:upper16:cnt
	ldr	r2,[r3]
	add	r2,r2,#1
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	ldr	r1,[r0,r6,lsl 2]
	mov	r4,r1
	str	r4,[r0,r2,lsl 2]
	mov	r1,r2
	str	r2,[r3]
	str	r1,[r0,r6,lsl 2]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r5,[r0,r2,lsl 2]
	movw	r0,#1
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.2:
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	ldr	r0,[r0,r6,lsl 2]
	mov	r6,r0
	b	.1
.3:
	movw	r3,#:lower16:cnt
	movt	r3,#:upper16:cnt
	ldr	r2,[r3]
	add	r2,r2,#1
	mov	r1,r8
	mov	r8,r2
	mov	r7,r4
	movw	r6,#0
	mov	r4,r6
	str	r2,[r3]
	movw	r0,#:lower16:next
	movt	r0,#:upper16:next
	str	r1,[r0,r2,lsl 2]
	movw	r0,#:lower16:nextvalue
	movt	r0,#:upper16:nextvalue
	str	r4,[r0,r2,lsl 2]
	str	r8,[r9,r10,lsl 2]
	movw	r0,#:lower16:key
	movt	r0,#:upper16:key
	str	r7,[r0,r2,lsl 2]
	movw	r0,#:lower16:value
	movt	r0,#:upper16:value
	str	r5,[r0,r2,lsl 2]
	mov	r0,r6
	add	sp,sp,#4
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
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
