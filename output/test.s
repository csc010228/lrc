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
	push	{r0}
	movw	r2,#:lower16:hashmod
	movt	r2,#:upper16:hashmod
	ldr	r1,[r2]
	bl	__aeabi_idivmod
	mov	r0,r1
	add	sp,sp,#4
	nop
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
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	push	{r0,r1}
	sub	sp,sp,#8
	movw	r2,#:lower16:hashmod
	movt	r2,#:upper16:hashmod
	ldr	r1,[r2]
	bl	__aeabi_idivmod
	add	r2,sp,#4
	str	r1,[r2]
	push	{r1}
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#4
	bne	.0
	movw	r1,#:lower16:cnt
	movt	r1,#:upper16:cnt
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	str	r1,[r2]
	push	{r1}
	movw	r2,#:lower16:head
	movt	r2,#:upper16:head
	add	r4,sp,#8
	ldr	r3,[r4]
	str	r1,[r2,r3,lsl 2]
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#16
	ldr	r0,[r1]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	str	r0,[r1,r2,lsl 2]
	movw	r0,#0
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#20
	nop
	pop	{r4,fp,pc}
	add	sp,sp,#4
.0:
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,sp,#0
	str	r0,[r1]
.1:
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#0
	beq	.3
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#8
	ldr	r1,[r2]
	cmp	r0,r1
	bne	.2
	movw	r1,#:lower16:cnt
	movt	r1,#:upper16:cnt
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	str	r1,[r2]
	push	{r1}
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r3,sp,#4
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#16
	ldr	r0,[r1]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r0,#1
	add	sp,sp,#20
	nop
	pop	{r4,fp,pc}
	add	sp,sp,#4
.2:
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	str	r0,[r3]
	b	.1
.3:
	movw	r1,#:lower16:cnt
	movt	r1,#:upper16:cnt
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	str	r1,[r2]
	push	{r1}
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#8
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#12
	ldr	r0,[r1]
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#16
	ldr	r0,[r1]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	str	r0,[r1,r2,lsl 2]
	movw	r0,#0
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#20
	nop
	pop	{r4,fp,pc}
	add	sp,sp,#4
	.size	insert, .-insert
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
	push	{r0}
	sub	sp,sp,#16
	movw	r2,#:lower16:hashmod
	movt	r2,#:upper16:hashmod
	ldr	r1,[r2]
	bl	__aeabi_idivmod
	push	{r1}
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,sp,#12
	str	r0,[r1]
	add	sp,sp,#4
.4:
	add	r1,sp,#8
	ldr	r0,[r1]
	cmp	r0,#0
	beq	.8
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r2,sp,#16
	ldr	r1,[r2]
	cmp	r0,r1
	bne	.7
	movw	r0,#0
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#8
	ldr	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
.5:
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#0
	beq	.6
	add	r1,sp,#4
	ldr	r0,[r1]
	movw	r2,#:lower16:value
	movt	r2,#:upper16:value
	add	r4,sp,#0
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	add	r2,r0,r1
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	ldr	r0,[r1,r3,lsl 2]
	str	r0,[r4]
	add	r0,sp,#4
	str	r2,[r0]
	b	.5
.6:
	add	r1,sp,#4
	ldr	r0,[r1]
	add	sp,sp,#20
	nop
	pop	{r4,fp,pc}
.7:
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	str	r0,[r3]
	b	.4
.8:
	movw	r0,#0
	add	sp,sp,#20
	nop
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
	push	{r4,fp,lr}
	nop
	add	fp,sp,#12
	sub	sp,sp,#60
	bl	getint
	push	{r0}
	movw	r1,#:lower16:hashmod
	movt	r1,#:upper16:hashmod
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#:lower16:keys
	movt	r0,#:upper16:keys
	bl	getarray
	push	{r0}
	add	r1,sp,#60
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#:lower16:values
	movt	r0,#:upper16:values
	bl	getarray
	push	{r0}
	movw	r0,#:lower16:requests
	movt	r0,#:upper16:requests
	bl	getarray
	push	{r0}
	add	r1,sp,#64
	str	r0,[r1]
	str	r0,[r1]
	movw	r0,#78
	bl	_sysy_starttime
	movw	r0,#0
	add	r1,sp,#72
	str	r0,[r1]
	add	sp,sp,#16
.9:
	add	r1,sp,#56
	ldr	r0,[r1]
	add	r2,sp,#52
	ldr	r1,[r2]
	cmp	r0,r1
	bge	.10
	add	r1,sp,#56
	ldr	r0,[r1]
	add	r1,sp,#16
	str	r0,[r1]
	add	r1,sp,#12
	str	r0,[r1]
	add	r1,sp,#16
	str	r0,[r1]
	add	r1,sp,#12
	str	r0,[r1]
	add	r1,sp,#16
	str	r0,[r1]
	add	r1,sp,#12
	str	r0,[r1]
	movw	r1,#:lower16:keys
	movt	r1,#:upper16:keys
	add	r3,sp,#56
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r2,#:lower16:hashmod
	movt	r2,#:upper16:hashmod
	ldr	r1,[r2]
	bl	__aeabi_idivmod
	push	{r1}
	add	r2,sp,#28
	str	r1,[r2]
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	cmp	r0,#0
	add	sp,sp,#4
	bne	.13
	movw	r1,#:lower16:cnt
	movt	r1,#:upper16:cnt
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	str	r1,[r2]
	movw	r2,#:lower16:head
	movt	r2,#:upper16:head
	add	r4,sp,#24
	ldr	r3,[r4]
	str	r1,[r2,r3,lsl 2]
	push	{r1}
	movw	r1,#:lower16:keys
	movt	r1,#:upper16:keys
	add	r3,sp,#20
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	str	r2,[r3]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:values
	movt	r1,#:upper16:values
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	str	r2,[r3]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r0,#0
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#4
	b	.17
.13:
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#24
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,sp,#20
	str	r0,[r1]
.14:
	add	r1,sp,#20
	ldr	r0,[r1]
	cmp	r0,#0
	beq	.15
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	add	r3,sp,#20
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r2,#:lower16:keys
	movt	r2,#:upper16:keys
	add	r4,sp,#16
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	str	r3,[r4]
	bne	.16
	movw	r1,#:lower16:cnt
	movt	r1,#:upper16:cnt
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	str	r1,[r2]
	push	{r1}
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	add	r3,sp,#24
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	add	r3,sp,#24
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:values
	movt	r1,#:upper16:values
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	str	r2,[r3]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#4
	b	.17
.16:
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	add	r3,sp,#20
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	str	r0,[r3]
	b	.14
.15:
	movw	r1,#:lower16:cnt
	movt	r1,#:upper16:cnt
	ldr	r0,[r1]
	add	r1,r0,#1
	movw	r2,#:lower16:cnt
	movt	r2,#:upper16:cnt
	str	r1,[r2]
	push	{r1}
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#28
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	ldr	r0,[r3]
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#28
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:keys
	movt	r1,#:upper16:keys
	add	r3,sp,#20
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	str	r2,[r3]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:values
	movt	r1,#:upper16:values
	add	r3,sp,#16
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r1,#:lower16:value
	movt	r1,#:upper16:value
	str	r2,[r3]
	add	r3,sp,#0
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	movw	r0,#0
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	str	r0,[r1,r2,lsl 2]
	add	sp,sp,#4
	b	.17
.17:
	add	r1,sp,#56
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#56
	str	r1,[r0]
	b	.9
.10:
	movw	r0,#0
	add	r1,sp,#56
	str	r0,[r1]
.11:
	add	r1,sp,#56
	ldr	r0,[r1]
	add	r2,sp,#48
	ldr	r1,[r2]
	cmp	r0,r1
	bge	.12
	add	r1,sp,#56
	ldr	r0,[r1]
	add	r1,sp,#8
	str	r0,[r1]
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#8
	str	r0,[r1]
	add	r1,sp,#4
	str	r0,[r1]
	add	r1,sp,#8
	str	r0,[r1]
	add	r1,sp,#4
	str	r0,[r1]
	movw	r1,#:lower16:requests
	movt	r1,#:upper16:requests
	add	r3,sp,#56
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r2,#:lower16:hashmod
	movt	r2,#:upper16:hashmod
	ldr	r1,[r2]
	bl	__aeabi_idivmod
	push	{r1}
	movw	r1,#:lower16:head
	movt	r1,#:upper16:head
	add	r3,sp,#0
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	add	r1,sp,#40
	str	r0,[r1]
	add	sp,sp,#4
.18:
	add	r1,sp,#36
	ldr	r0,[r1]
	cmp	r0,#0
	beq	.19
	movw	r1,#:lower16:key
	movt	r1,#:upper16:key
	add	r3,sp,#36
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r2,#:lower16:requests
	movt	r2,#:upper16:requests
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	cmp	r0,r1
	str	r3,[r4]
	bne	.20
	movw	r0,#0
	add	r1,sp,#32
	str	r0,[r1]
	add	r1,sp,#36
	ldr	r0,[r1]
	add	r1,sp,#28
	str	r0,[r1]
.21:
	add	r1,sp,#28
	ldr	r0,[r1]
	cmp	r0,#0
	beq	.22
	add	r1,sp,#32
	ldr	r0,[r1]
	movw	r2,#:lower16:value
	movt	r2,#:upper16:value
	add	r4,sp,#28
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	add	r2,r0,r1
	movw	r1,#:lower16:nextvalue
	movt	r1,#:upper16:nextvalue
	ldr	r0,[r1,r3,lsl 2]
	str	r0,[r4]
	add	r0,sp,#32
	str	r2,[r0]
	b	.21
.22:
	add	r1,sp,#32
	ldr	r0,[r1]
	add	r1,sp,#0
	str	r0,[r1]
	b	.23
.20:
	movw	r1,#:lower16:next
	movt	r1,#:upper16:next
	add	r3,sp,#36
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	str	r0,[r3]
	b	.18
.19:
	movw	r0,#0
	add	r1,sp,#0
	str	r0,[r1]
	b	.23
.23:
	add	r1,sp,#0
	ldr	r0,[r1]
	movw	r1,#:lower16:ans
	movt	r1,#:upper16:ans
	add	r3,sp,#8
	ldr	r2,[r3]
	str	r0,[r1,r2,lsl 2]
	add	r1,sp,#0
	str	r0,[r1]
	add	r1,sp,#56
	ldr	r0,[r1]
	add	r1,r0,#1
	add	r0,sp,#56
	str	r1,[r0]
	str	r2,[r3]
	b	.11
.12:
	movw	r0,#90
	bl	_sysy_stoptime
	add	r1,sp,#48
	ldr	r0,[r1]
	movw	r1,#:lower16:ans
	movt	r1,#:upper16:ans
	bl	putarray
	movw	r0,#0
	add	sp,sp,#60
	nop
	pop	{r4,fp,pc}
	.size	main, .-main
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
