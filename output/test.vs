search:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR53,r0
	mov	VR58,r1
	movw	VR63,#65535
	movt	VR63,#65535
	cmp	VR53,VR63
	beq	.0
	movw	VR61,#:lower16:value
	movt	VR61,#:upper16:value
	ldr	VR50,[VR61,VR53,lsl 2]
	cmp	VR50,VR58
	beq	.0
	b	.1
.0:
	mov	r0,VR53
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.1:
	cmp	VR58,VR50
	ble	.2
	movw	VR60,#:lower16:right_child
	movt	VR60,#:upper16:right_child
	ldr	VR51,[VR60,VR53,lsl 2]
	mov	r0,VR51
	mov	r1,VR58
	bl	search
	mov	VR56,r0
	mov	r0,VR56
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.2:
	movw	VR54,#:lower16:left_child
	movt	VR54,#:upper16:left_child
	ldr	VR52,[VR54,VR53,lsl 2]
	mov	r0,VR52
	mov	r1,VR58
	bl	search
	mov	VR62,r0
	mov	r0,VR62
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.3:
.4:



find_minimum:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR54,r0
	movw	VR56,#65535
	movt	VR56,#65535
	cmp	VR54,VR56
	bne	.5
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.5:
	movw	VR53,#:lower16:left_child
	movt	VR53,#:upper16:left_child
	ldr	VR50,[VR53,VR54,lsl 2]
	movw	VR56,#65535
	movt	VR56,#65535
	cmp	VR50,VR56
	beq	.6
	mov	r0,VR50
	bl	find_minimum
	mov	VR52,r0
	mov	r0,VR52
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.6:
.7:
	mov	r0,VR54
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



new_node:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR53,r0
	mov	VR50,VR53
	movw	VR60,#65535
	movt	VR60,#65535
	mov	VR51,VR60
	mov	VR52,VR60
	movw	VR55,#:lower16:now
	movt	VR55,#:upper16:now
	ldr	VR56,[VR55]
	movw	VR59,#:lower16:value
	movt	VR59,#:upper16:value
	str	VR50,[VR59,VR56,lsl 2]
	movw	VR57,#:lower16:left_child
	movt	VR57,#:upper16:left_child
	str	VR51,[VR57,VR56,lsl 2]
	movw	VR58,#:lower16:right_child
	movt	VR58,#:upper16:right_child
	str	VR52,[VR58,VR56,lsl 2]
	add	VR56,VR56,#1
	sub	VR54,VR56,#1
	str	VR56,[VR55]
	mov	r0,VR54
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



insert:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR54,r0
	mov	VR55,r1
	movw	VR66,#65535
	movt	VR66,#65535
	cmp	VR54,VR66
	bne	.8
	mov	VR51,VR55
	movw	VR66,#65535
	movt	VR66,#65535
	mov	VR52,VR66
	mov	VR53,VR66
	movw	VR61,#:lower16:now
	movt	VR61,#:upper16:now
	ldr	VR62,[VR61]
	movw	VR65,#:lower16:value
	movt	VR65,#:upper16:value
	str	VR51,[VR65,VR62,lsl 2]
	movw	VR63,#:lower16:left_child
	movt	VR63,#:upper16:left_child
	str	VR52,[VR63,VR62,lsl 2]
	movw	VR64,#:lower16:right_child
	movt	VR64,#:upper16:right_child
	str	VR53,[VR64,VR62,lsl 2]
	add	VR62,VR62,#1
	sub	VR50,VR62,#1
	str	VR62,[VR61]
	mov	r0,VR50
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.8:
	movw	VR65,#:lower16:value
	movt	VR65,#:upper16:value
	ldr	VR60,[VR65,VR54,lsl 2]
	cmp	VR55,VR60
	ble	.9
	movw	VR64,#:lower16:right_child
	movt	VR64,#:upper16:right_child
	ldr	VR58,[VR64,VR54,lsl 2]
	mov	r0,VR58
	mov	r1,VR55
	bl	insert
	mov	VR58,r0
	str	VR58,[VR64,VR54,lsl 2]
	b	.10
.9:
	movw	VR63,#:lower16:left_child
	movt	VR63,#:upper16:left_child
	ldr	VR59,[VR63,VR54,lsl 2]
	mov	r0,VR59
	mov	r1,VR55
	bl	insert
	mov	VR59,r0
	str	VR59,[VR63,VR54,lsl 2]
.10:
.11:
	mov	r0,VR54
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



delete:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR51,r0
	mov	VR52,r1
	movw	VR68,#65535
	movt	VR68,#65535
	cmp	VR51,VR68
	bne	.12
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.12:
	movw	VR67,#:lower16:value
	movt	VR67,#:upper16:value
	ldr	VR63,[VR67,VR51,lsl 2]
	cmp	VR52,VR63
	ble	.13
	movw	VR66,#:lower16:right_child
	movt	VR66,#:upper16:right_child
	ldr	VR64,[VR66,VR51,lsl 2]
	mov	r0,VR64
	mov	r1,VR52
	bl	delete
	mov	VR64,r0
	str	VR64,[VR66,VR51,lsl 2]
	b	.23
.13:
	cmp	VR52,VR63
	bge	.14
	movw	VR65,#:lower16:left_child
	movt	VR65,#:upper16:left_child
	ldr	VR62,[VR65,VR51,lsl 2]
	mov	r0,VR62
	mov	r1,VR52
	bl	delete
	mov	VR62,r0
	str	VR62,[VR65,VR51,lsl 2]
	b	.22
.14:
	movw	VR65,#:lower16:left_child
	movt	VR65,#:upper16:left_child
	ldr	VR62,[VR65,VR51,lsl 2]
	movw	VR68,#65535
	movt	VR68,#65535
	cmp	VR62,VR68
	bne	.15
	movw	VR66,#:lower16:right_child
	movt	VR66,#:upper16:right_child
	ldr	VR64,[VR66,VR51,lsl 2]
	movw	VR68,#65535
	movt	VR68,#65535
	cmp	VR64,VR68
	bne	.15
	movw	r0,#65535
	movt	r0,#65535
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.15:
	movw	VR68,#65535
	movt	VR68,#65535
	cmp	VR62,VR68
	beq	.16
	movw	VR66,#:lower16:right_child
	movt	VR66,#:upper16:right_child
	ldr	VR64,[VR66,VR51,lsl 2]
	movw	VR68,#65535
	movt	VR68,#65535
	cmp	VR64,VR68
	beq	.16
	b	.19
.16:
	movw	VR68,#65535
	movt	VR68,#65535
	cmp	VR62,VR68
	bne	.17
	movw	VR66,#:lower16:right_child
	movt	VR66,#:upper16:right_child
	ldr	r0,[VR66,VR51,lsl 2]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.17:
	mov	r0,VR62
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.18:
	b	.20
.19:
	mov	r0,VR64
	bl	find_minimum
	mov	VR61,r0
	ldr	VR50,[VR67,VR61,lsl 2]
	mov	VR63,VR50
	str	VR63,[VR67,VR51,lsl 2]
	mov	r0,VR64
	ldr	VR50,[VR67,VR61,lsl 2]
	mov	r1,VR50
	bl	delete
	mov	VR64,r0
	str	VR64,[VR66,VR51,lsl 2]
.20:
.21:
.22:
.23:
	mov	r0,VR51
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



inorder:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR53,r0
	movw	VR58,#65535
	movt	VR58,#65535
	cmp	VR53,VR58
	beq	.24
	movw	VR55,#:lower16:left_child
	movt	VR55,#:upper16:left_child
	ldr	VR52,[VR55,VR53,lsl 2]
	mov	r0,VR52
	bl	inorder
	movw	VR57,#:lower16:value
	movt	VR57,#:upper16:value
	ldr	VR51,[VR57,VR53,lsl 2]
	mov	r0,VR51
	bl	putint
	movw	VR59,#32
	mov	r0,VR59
	bl	putch
	movw	VR56,#:lower16:right_child
	movt	VR56,#:upper16:right_child
	ldr	VR50,[VR56,VR53,lsl 2]
	mov	r0,VR50
	bl	inorder
.24:
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



main:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	movw	VR70,#0
	mov	VR66,VR70
	bl	getint
	mov	VR53,r0
	mov	VR54,VR53
	cmp	VR53,#0
	ite eq
	moveq	VR55,#1
	movne	VR55,#0
	bne	.25
	movw	VR65,#:lower16:now
	movt	VR65,#:upper16:now
	str	VR66,[VR65]
	movw	r0,#0
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.25:
	bl	getint
	mov	VR56,r0
	mov	VR50,VR56
	movw	VR71,#65535
	movt	VR71,#65535
	mov	VR51,VR71
	mov	VR52,VR71
	movw	VR72,#1
	movw	VR69,#:lower16:value
	movt	VR69,#:upper16:value
	str	VR50,[VR69,VR66,lsl 2]
	movw	VR67,#:lower16:left_child
	movt	VR67,#:upper16:left_child
	str	VR51,[VR67,VR66,lsl 2]
	movw	VR68,#:lower16:right_child
	movt	VR68,#:upper16:right_child
	str	VR52,[VR68,VR66,lsl 2]
	mov	VR66,VR72
	sub	VR64,VR66,#1
	mov	VR57,VR64
	mov	VR58,VR72
	movw	VR65,#:lower16:now
	movt	VR65,#:upper16:now
	str	VR66,[VR65]
.26:
	cmp	VR58,VR54
	bge	.27
	bl	getint
	mov	VR60,r0
	mov	r0,VR57
	mov	r1,VR60
	bl	insert
	mov	VR61,r0
	add	VR58,VR58,#1
	b	.26
.27:
	mov	r0,VR57
	bl	inorder
	movw	VR73,#10
	mov	r0,VR73
	bl	putch
	bl	getint
	mov	VR54,r0
	movw	VR70,#0
	mov	VR58,VR70
.28:
	cmp	VR58,VR54
	bge	.29
	bl	getint
	mov	VR63,r0
	mov	r0,VR57
	mov	r1,VR63
	bl	delete
	mov	VR57,r0
	add	VR58,VR58,#1
	b	.28
.29:
	mov	r0,VR57
	bl	inorder
	movw	VR73,#10
	mov	r0,VR73
	bl	putch
	movw	r0,#0
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



