meanless_calculation:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR55,r0
	mov	VR56,r1
	movw	VR57,#0
	mov	VR50,VR57
	mov	VR52,VR57
.0:
	cmp	VR50,VR55
	bge	.1
	cmp	VR50,VR56
	bge	.1
	add	VR50,VR50,#1
	add	VR54,VR52,VR55
	add	VR52,VR54,VR50
	b	.0
.1:
	mov	r0,VR52
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



swap:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR59,r0
	mov	VR57,r1
	mov	VR58,r2
	ldr	VR60,[VR59,VR57,lsl 2]
	mov	VR56,VR60
	ldr	VR55,[VR59,VR58,lsl 2]
	mov	VR60,VR55
	str	VR60,[VR59,VR57,lsl 2]
	mov	VR55,VR56
	movw	VR61,#0
	mov	VR54,VR61
	mov	VR53,VR61
	str	VR55,[VR59,VR58,lsl 2]
.14:
	cmp	VR54,VR57
	bge	.15
	cmp	VR54,VR58
	bge	.15
	add	VR54,VR54,#1
	add	VR51,VR53,VR57
	add	VR53,VR51,VR54
	b	.14
.15:
	mov	r0,VR53
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



median:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR61,r0
	mov	VR62,r1
	mov	VR68,r2
	mov	VR59,r3
	ldr	VR56,[VR61,VR62,lsl 2]
	mov	VR65,VR56
	mov	VR60,VR62
	add	VR71,VR68,#1
	mov	VR58,VR71
	movw	VR74,#0
	mov	VR52,VR74
.2:
.3:
	cmp	VR60,VR58
	bge	.5
	sub	VR58,VR58,#1
	ldr	VR50,[VR61,VR58,lsl 2]
	cmp	VR50,VR65
	bge	.4
	b	.5
.4:
	add	VR52,VR52,#1
	b	.3
.5:
.6:
	cmp	VR60,VR58
	bge	.8
	add	VR60,VR60,#1
	ldr	VR70,[VR61,VR60,lsl 2]
	cmp	VR70,VR65
	blt	.7
	b	.8
.7:
	sub	VR52,VR52,#1
	b	.6
.8:
	cmp	VR60,VR58
	bne	.9
	b	.11
	b	.10
.9:
	mov	r0,VR61
	mov	r1,VR60
	mov	r2,VR58
	bl	swap
	mov	VR63,r0
.10:
	b	.2
.11:
	mov	VR56,VR65
	str	VR56,[VR61,VR62,lsl 2]
	mov	r0,VR61
	mov	r1,VR62
	mov	r2,VR60
	bl	swap
	mov	VR72,r0
	cmp	VR60,VR59
	ble	.12
	mov	r0,VR61
	mov	r1,VR62
	mov	r2,VR60
	mov	r3,VR59
	bl	median
	mov	VR57,r0
	mov	r0,VR57
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.12:
	cmp	VR60,VR59
	bge	.13
	add	VR55,VR60,#1
	mov	r0,VR61
	mov	r1,VR55
	mov	r2,VR68
	mov	r3,VR59
	bl	median
	mov	VR64,r0
	mov	r0,VR64
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.13:
	mov	r0,VR52
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



main:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	movw	VR53,#:lower16:a
	movt	VR53,#:upper16:a
	mov	r0,VR53
	bl	getarray
	mov	VR55,r0
	movw	VR58,#59
	mov	r0,VR58
	bl	_sysy_starttime
	sub	VR56,VR55,#1
	add	VR51,VR55,VR55,lsr 31
	asr	VR51,VR55,#1
	mov	r0,VR53
	movw	VR59,#0
	mov	r1,VR59
	mov	r2,VR56
	mov	r3,VR51
	bl	median
	mov	VR52,r0
	movw	VR60,#61
	mov	r0,VR60
	bl	_sysy_stoptime
	mov	r0,VR55
	mov	r1,VR53
	bl	putarray
	ldr	VR57,[VR53,VR51,lsl 2]
	mov	r0,VR57
	movw	VR61,#256
	mov	r1,VR61
	bl	__aeabi_idivmod
	mov	VR50,r1
	movw	VR54,#:lower16:n
	movt	VR54,#:upper16:n
	str	VR55,[VR54]
	mov	r0,VR50
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



