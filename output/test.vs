hash:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR52,r0
	mov	r0,VR52
	movw	VR50,#:lower16:hashmod
	movt	VR50,#:upper16:hashmod
	ldr	VR51,[VR50]
	mov	r1,VR51
	bl	__aeabi_idivmod
	mov	VR53,r1
	mov	r0,VR53
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



insert:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR55,r0
	mov	VR60,r1
	mov	r0,VR55
	movw	VR67,#:lower16:hashmod
	movt	VR67,#:upper16:hashmod
	ldr	VR68,[VR67]
	mov	r1,VR68
	bl	__aeabi_idivmod
	mov	VR72,r1
	mov	VR58,VR72
	movw	VR63,#:lower16:head
	movt	VR63,#:upper16:head
	ldr	VR71,[VR63,VR72,lsl 2]
	cmp	VR71,#0
	bne	.0
	movw	VR50,#:lower16:cnt
	movt	VR50,#:upper16:cnt
	ldr	VR51,[VR50]
	add	VR51,VR51,#1
	mov	VR73,VR51
	mov	VR74,VR55
	mov	VR75,VR60
	movw	VR76,#0
	mov	VR57,VR76
	mov	VR62,VR76
	str	VR51,[VR50]
	movw	VR65,#:lower16:next
	movt	VR65,#:upper16:next
	str	VR57,[VR65,VR51,lsl 2]
	movw	VR52,#:lower16:nextvalue
	movt	VR52,#:upper16:nextvalue
	str	VR62,[VR52,VR51,lsl 2]
	str	VR73,[VR63,VR58,lsl 2]
	movw	VR61,#:lower16:key
	movt	VR61,#:upper16:key
	str	VR74,[VR61,VR51,lsl 2]
	movw	VR70,#:lower16:value
	movt	VR70,#:upper16:value
	str	VR75,[VR70,VR51,lsl 2]
	mov	r0,VR76
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.0:
	ldr	VR73,[VR63,VR58,lsl 2]
	mov	VR64,VR73
.1:
	cmp	VR64,#0
	beq	.3
	movw	VR61,#:lower16:key
	movt	VR61,#:upper16:key
	ldr	VR69,[VR61,VR64,lsl 2]
	cmp	VR69,VR55
	bne	.2
	movw	VR50,#:lower16:cnt
	movt	VR50,#:upper16:cnt
	ldr	VR51,[VR50]
	add	VR51,VR51,#1
	movw	VR52,#:lower16:nextvalue
	movt	VR52,#:upper16:nextvalue
	ldr	VR54,[VR52,VR64,lsl 2]
	mov	VR62,VR54
	str	VR62,[VR52,VR51,lsl 2]
	mov	VR54,VR51
	mov	VR75,VR60
	str	VR51,[VR50]
	str	VR54,[VR52,VR64,lsl 2]
	movw	VR70,#:lower16:value
	movt	VR70,#:upper16:value
	str	VR75,[VR70,VR51,lsl 2]
	movw	r0,#1
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.2:
	movw	VR65,#:lower16:next
	movt	VR65,#:upper16:next
	ldr	VR66,[VR65,VR64,lsl 2]
	mov	VR64,VR66
	b	.1
.3:
	movw	VR50,#:lower16:cnt
	movt	VR50,#:upper16:cnt
	ldr	VR51,[VR50]
	add	VR51,VR51,#1
	mov	VR57,VR73
	mov	VR73,VR51
	mov	VR74,VR55
	mov	VR75,VR60
	movw	VR76,#0
	mov	VR62,VR76
	str	VR51,[VR50]
	movw	VR65,#:lower16:next
	movt	VR65,#:upper16:next
	str	VR57,[VR65,VR51,lsl 2]
	movw	VR52,#:lower16:nextvalue
	movt	VR52,#:upper16:nextvalue
	str	VR62,[VR52,VR51,lsl 2]
	str	VR73,[VR63,VR58,lsl 2]
	movw	VR61,#:lower16:key
	movt	VR61,#:upper16:key
	str	VR74,[VR61,VR51,lsl 2]
	movw	VR70,#:lower16:value
	movt	VR70,#:upper16:value
	str	VR75,[VR70,VR51,lsl 2]
	mov	r0,VR76
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



reduce:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR52,r0
	mov	r0,VR52
	movw	VR59,#:lower16:hashmod
	movt	VR59,#:upper16:hashmod
	ldr	VR60,[VR59]
	mov	r1,VR60
	bl	__aeabi_idivmod
	mov	VR65,r1
	movw	VR54,#:lower16:head
	movt	VR54,#:upper16:head
	ldr	VR66,[VR54,VR65,lsl 2]
	mov	VR57,VR66
.4:
	cmp	VR57,#0
	beq	.8
	movw	VR53,#:lower16:key
	movt	VR53,#:upper16:key
	ldr	VR69,[VR53,VR57,lsl 2]
	cmp	VR69,VR52
	bne	.7
	movw	VR70,#0
	mov	VR63,VR70
	mov	VR51,VR57
.5:
	cmp	VR51,#0
	beq	.6
	movw	VR62,#:lower16:value
	movt	VR62,#:upper16:value
	ldr	VR67,[VR62,VR51,lsl 2]
	add	VR63,VR63,VR67
	movw	VR50,#:lower16:nextvalue
	movt	VR50,#:upper16:nextvalue
	ldr	VR64,[VR50,VR51,lsl 2]
	mov	VR51,VR64
	b	.5
.6:
	mov	r0,VR63
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.7:
	movw	VR56,#:lower16:next
	movt	VR56,#:upper16:next
	ldr	VR68,[VR56,VR57,lsl 2]
	mov	VR57,VR68
	b	.4
.8:
	movw	r0,#0
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



main:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	sub	sp,sp,#56
	sub	sp,sp,#16
	bl	getint
	mov	VR138,r0
	movw	VR88,#:lower16:hashmod
	movt	VR88,#:upper16:hashmod
	movw	VR150,#:lower16:hashmod
	movt	VR150,#:upper16:hashmod
	str	VR138,[VR150]
	movw	VR90,#:lower16:keys
	movt	VR90,#:upper16:keys
	movw	VR146,#:lower16:keys
	movt	VR146,#:upper16:keys
	mov	r0,VR146
	bl	getarray
	mov	VR84,r0
	mov	VR75,VR84
	movw	VR78,#:lower16:values
	movt	VR78,#:upper16:values
	mov	r0,VR78
	bl	getarray
	mov	VR82,r0
	movw	VR72,#:lower16:requests
	movt	VR72,#:upper16:requests
	movw	VR147,#:lower16:requests
	movt	VR147,#:upper16:requests
	mov	r0,VR147
	bl	getarray
	mov	VR68,r0
	mov	VR139,VR68
	str	VR139,[sp,#60]
	mov	r0,#78
	bl	_sysy_starttime
	mov	VR73,#0
	add	sp,sp,#16
.9:
	cmp	VR73,VR75
	bge	.10
	movw	VR148,#:lower16:keys
	movt	VR148,#:upper16:keys
	ldr	VR98,[VR148,VR73,lsl 2]
	mov	r0,VR98
	movw	VR151,#:lower16:hashmod
	movt	VR151,#:upper16:hashmod
	ldr	VR141,[VR151]
	mov	r1,VR141
	bl	__aeabi_idivmod
	mov	VR94,r1
	mov	VR95,VR94
	movw	VR81,#:lower16:head
	movt	VR81,#:upper16:head
	ldr	VR52,[VR81,VR94,lsl 2]
	cmp	VR52,#0
	bne	.13
	movw	VR69,#:lower16:cnt
	movt	VR69,#:upper16:cnt
	ldr	VR70,[VR69]
	add	VR70,VR70,#1
	mov	VR53,VR70
	mov	VR100,VR98
	ldr	VR97,[VR78,VR73,lsl 2]
	mov	VR101,VR97
	mov	VR74,#0
	mov	VR80,#0
	str	VR53,[VR81,VR95,lsl 2]
	movw	VR85,#:lower16:next
	movt	VR85,#:upper16:next
	str	VR74,[VR85,VR70,lsl 2]
	movw	VR71,#:lower16:nextvalue
	movt	VR71,#:upper16:nextvalue
	str	VR80,[VR71,VR70,lsl 2]
	movw	VR79,#:lower16:key
	movt	VR79,#:upper16:key
	str	VR100,[VR79,VR70,lsl 2]
	movw	VR92,#:lower16:value
	movt	VR92,#:upper16:value
	str	VR101,[VR92,VR70,lsl 2]
	b	.17
.13:
	ldr	VR105,[VR81,VR95,lsl 2]
	mov	VR86,VR105
.14:
	cmp	VR86,#0
	beq	.15
	movw	VR116,#:lower16:key
	movt	VR116,#:upper16:key
	ldr	VR55,[VR116,VR86,lsl 2]
	cmp	VR55,VR98
	bne	.16
	movw	VR107,#:lower16:cnt
	movt	VR107,#:upper16:cnt
	ldr	VR70,[VR107]
	add	VR70,VR70,#1
	movw	VR110,#:lower16:nextvalue
	movt	VR110,#:upper16:nextvalue
	ldr	VR56,[VR110,VR86,lsl 2]
	mov	VR119,VR56
	str	VR119,[VR110,VR70,lsl 2]
	mov	VR106,VR70
	ldr	VR128,[VR78,VR73,lsl 2]
	mov	VR131,VR128
	str	VR106,[VR110,VR86,lsl 2]
	movw	VR124,#:lower16:value
	movt	VR124,#:upper16:value
	str	VR131,[VR124,VR70,lsl 2]
	b	.17
.16:
	movw	VR121,#:lower16:next
	movt	VR121,#:upper16:next
	ldr	VR57,[VR121,VR86,lsl 2]
	mov	VR86,VR57
	b	.14
.15:
	movw	VR108,#:lower16:cnt
	movt	VR108,#:upper16:cnt
	ldr	VR70,[VR108]
	add	VR70,VR70,#1
	mov	VR113,VR105
	mov	VR105,VR70
	mov	VR130,VR98
	ldr	VR129,[VR78,VR73,lsl 2]
	mov	VR132,VR129
	mov	VR120,#0
	str	VR105,[VR81,VR95,lsl 2]
	movw	VR122,#:lower16:next
	movt	VR122,#:upper16:next
	str	VR113,[VR122,VR70,lsl 2]
	movw	VR111,#:lower16:nextvalue
	movt	VR111,#:upper16:nextvalue
	str	VR120,[VR111,VR70,lsl 2]
	movw	VR117,#:lower16:key
	movt	VR117,#:upper16:key
	str	VR130,[VR117,VR70,lsl 2]
	movw	VR125,#:lower16:value
	movt	VR125,#:upper16:value
	str	VR132,[VR125,VR70,lsl 2]
	b	.17
.17:
	add	VR73,VR73,#1
	movw	VR109,#:lower16:cnt
	movt	VR109,#:upper16:cnt
	str	VR70,[VR109]
	b	.9
.10:
	mov	VR73,#0
.11:
	sub	sp,sp,#8
	add	VR152,sp,#52
	ldr	VR142,[VR152]
	cmp	VR73,VR142
	add	sp,sp,#8
	bge	.12
	movw	VR149,#:lower16:requests
	movt	VR149,#:upper16:requests
	ldr	VR93,[VR149,VR73,lsl 2]
	mov	r0,VR93
	movw	VR153,#:lower16:hashmod
	movt	VR153,#:upper16:hashmod
	ldr	VR143,[VR153]
	mov	r1,VR143
	bl	__aeabi_idivmod
	mov	VR51,r1
	movw	VR81,#:lower16:head
	movt	VR81,#:upper16:head
	ldr	VR62,[VR81,VR51,lsl 2]
	mov	VR50,VR62
.18:
	cmp	VR50,#0
	beq	.19
	movw	VR118,#:lower16:key
	movt	VR118,#:upper16:key
	ldr	VR63,[VR118,VR50,lsl 2]
	cmp	VR63,VR93
	bne	.20
	mov	VR59,#0
	mov	VR60,VR50
.21:
	cmp	VR60,#0
	beq	.22
	movw	VR126,#:lower16:value
	movt	VR126,#:upper16:value
	ldr	VR64,[VR126,VR60,lsl 2]
	add	VR59,VR59,VR64
	movw	VR112,#:lower16:nextvalue
	movt	VR112,#:upper16:nextvalue
	ldr	VR65,[VR112,VR60,lsl 2]
	mov	VR60,VR65
	b	.21
.22:
	mov	VR96,VR59
	movw	VR77,#:lower16:ans
	movt	VR77,#:upper16:ans
	str	VR96,[VR77,VR73,lsl 2]
	b	.23
.20:
	movw	VR123,#:lower16:next
	movt	VR123,#:upper16:next
	ldr	VR66,[VR123,VR50,lsl 2]
	mov	VR50,VR66
	b	.18
.19:
	mov	VR127,#0
	movw	VR114,#:lower16:ans
	movt	VR114,#:upper16:ans
	str	VR127,[VR114,VR73,lsl 2]
	b	.23
.23:
	add	VR73,VR73,#1
	b	.11
.12:
	sub	sp,sp,#0
	mov	r0,#90
	bl	_sysy_stoptime
	add	VR154,sp,#44
	ldr	VR144,[VR154]
	mov	r0,VR144
	movw	VR115,#:lower16:ans
	movt	VR115,#:upper16:ans
	mov	r1,VR115
	bl	putarray
	movw	VR88,#:lower16:hashmod
	movt	VR88,#:upper16:hashmod
	movw	VR155,#:lower16:hashmod
	movt	VR155,#:upper16:hashmod
	ldr	VR145,[VR155]
	movw	VR156,#:lower16:hashmod
	movt	VR156,#:upper16:hashmod
	str	VR145,[VR156]
	movw	r0,#0
	add	sp,sp,#0
	add	sp,sp,#56
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	add	sp,sp,#0



