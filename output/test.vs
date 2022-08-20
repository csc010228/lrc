swap.void:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR51,r0
	mov	VR50,r1
	mov	VR52,r2
	ldr	VR55,[VR51,VR50,lsl 2]
	mov	VR54,VR55
	ldr	VR53,[VR51,VR52,lsl 2]
	mov	VR55,VR53
	str	VR55,[VR51,VR50,lsl 2]
	mov	VR53,VR54
	str	VR53,[VR51,VR52,lsl 2]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



meanless_calculation:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR56,r0
	mov	VR51,r1
	movw	VR57,#0
	mov	VR52,VR57
	mov	VR50,VR57
.0:
	cmp	VR52,VR56
	bge	.1
	cmp	VR52,VR51
	bge	.1
	add	VR52,VR52,#1
	add	VR55,VR50,VR56
	add	VR50,VR55,VR52
	b	.0
.1:
	mov	r0,VR50
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



swap:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR51,r0
	mov	VR60,r1
	mov	VR57,r2
	ldr	VR59,[VR51,VR60,lsl 2]
	mov	VR53,VR59
	ldr	VR50,[VR51,VR57,lsl 2]
	mov	VR59,VR50
	str	VR59,[VR51,VR60,lsl 2]
	mov	VR50,VR53
	movw	VR61,#0
	mov	VR52,VR61
	mov	VR56,VR61
	str	VR50,[VR51,VR57,lsl 2]
.14:
	cmp	VR52,VR60
	bge	.15
	cmp	VR52,VR57
	bge	.15
	add	VR52,VR52,#1
	add	VR55,VR56,VR60
	add	VR56,VR55,VR52
	b	.14
.15:
	mov	r0,VR56
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



median:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR61,r0
	mov	VR54,r1
	mov	VR68,r2
	mov	VR59,r3
	ldr	VR55,[VR61,VR54,lsl 2]
	mov	VR60,VR55
	mov	VR71,VR54
	add	VR62,VR68,#1
	mov	VR70,VR62
	movw	VR72,#0
	mov	VR63,VR72
.2:
.3:
	cmp	VR71,VR70
	bge	.5
	sub	VR70,VR70,#1
	ldr	VR67,[VR61,VR70,lsl 2]
	cmp	VR67,VR60
	bge	.4
	b	.5
.4:
	add	VR63,VR63,#1
	b	.3
.5:
.6:
	cmp	VR71,VR70
	bge	.8
	add	VR71,VR71,#1
	ldr	VR52,[VR61,VR71,lsl 2]
	cmp	VR52,VR60
	blt	.7
	b	.8
.7:
	sub	VR63,VR63,#1
	b	.6
.8:
	cmp	VR71,VR70
	bne	.9
	b	.11
	b	.10
.9:
	mov	r0,VR61
	mov	r1,VR71
	mov	r2,VR70
	bl	swap.void
.10:
	b	.2
.11:
	mov	VR55,VR60
	str	VR55,[VR61,VR54,lsl 2]
	mov	r0,VR61
	mov	r1,VR54
	mov	r2,VR71
	bl	swap.void
	cmp	VR71,VR59
	ble	.12
	mov	r0,VR61
	mov	r1,VR54
	mov	r2,VR71
	mov	r3,VR59
	bl	median
	mov	VR69,r0
	mov	r0,VR69
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.12:
	cmp	VR71,VR59
	bge	.13
	add	VR58,VR71,#1
	mov	r0,VR61
	mov	r1,VR58
	mov	r2,VR68
	mov	r3,VR59
	bl	median
	mov	VR65,r0
	mov	r0,VR65
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.13:
	mov	r0,VR63
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



main:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	movw	VR50,#:lower16:a
	movt	VR50,#:upper16:a
	mov	r0,VR50
	bl	getarray
	mov	VR54,r0
	movw	VR58,#59
	mov	r0,VR58
	bl	_sysy_starttime
	sub	VR51,VR54,#1
	add	VR56,VR54,VR54,lsr 31
	asr	VR56,VR54,#1
	mov	r0,VR50
	movw	VR59,#0
	mov	r1,VR59
	mov	r2,VR51
	mov	r3,VR56
	bl	median
	mov	VR55,r0
	movw	VR60,#61
	mov	r0,VR60
	bl	_sysy_stoptime
	mov	r0,VR54
	mov	r1,VR50
	bl	putarray
	ldr	VR57,[VR50,VR56,lsl 2]
	mov	r0,VR57
	movw	VR61,#256
	mov	r1,VR61
	bl	__aeabi_idivmod
	mov	VR52,r1
	movw	VR53,#:lower16:n
	movt	VR53,#:upper16:n
	str	VR54,[VR53]
	mov	r0,VR52
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



