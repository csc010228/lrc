isdigit:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR50,r0
	cmp	VR50,#48
	blt	.0
	cmp	VR50,#57
	bgt	.0
	movw	r0,#1
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.0:
	movw	r0,#0
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



power:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR51,r0
	mov	VR53,r1
	movw	VR54,#1
	mov	VR52,VR54
.1:
	cmp	VR53,#0
	beq	.2
	mul	VR52,VR52,VR51
	sub	VR53,VR53,#1
	b	.1
.2:
	mov	r0,VR52
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



getstr:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR55,r0
	bl	getch
	mov	VR54,r0
	mov	VR53,VR54
	movw	VR57,#0
	mov	VR50,VR57
.3:
	cmp	VR53,#13
	beq	.4
	cmp	VR53,#10
	beq	.4
	mov	VR56,VR53
	str	VR56,[VR55,VR50,lsl 2]
	add	VR50,VR50,#1
	bl	getch
	mov	VR53,r0
	b	.3
.4:
	mov	r0,VR50
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



intpush:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR53,r0
	movw	VR51,#:lower16:intt
	movt	VR51,#:upper16:intt
	ldr	VR52,[VR51]
	add	VR52,VR52,#1
	mov	VR54,VR53
	str	VR52,[VR51]
	movw	VR50,#:lower16:ints
	movt	VR50,#:upper16:ints
	str	VR54,[VR50,VR52,lsl 2]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



chapush:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR53,r0
	movw	VR50,#:lower16:chat
	movt	VR50,#:upper16:chat
	ldr	VR51,[VR50]
	add	VR51,VR51,#1
	mov	VR54,VR53
	str	VR51,[VR50]
	movw	VR52,#:lower16:chas
	movt	VR52,#:upper16:chas
	str	VR54,[VR52,VR51,lsl 2]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



intpop:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	movw	VR52,#:lower16:intt
	movt	VR52,#:upper16:intt
	ldr	VR53,[VR52]
	sub	VR53,VR53,#1
	add	VR54,VR53,#1
	str	VR53,[VR52]
	movw	VR50,#:lower16:ints
	movt	VR50,#:upper16:ints
	ldr	r0,[VR50,VR54,lsl 2]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



chapop:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	movw	VR50,#:lower16:chat
	movt	VR50,#:upper16:chat
	ldr	VR51,[VR50]
	sub	VR51,VR51,#1
	add	VR54,VR51,#1
	str	VR51,[VR50]
	movw	VR52,#:lower16:chas
	movt	VR52,#:upper16:chas
	ldr	r0,[VR52,VR54,lsl 2]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



intadd:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	mov	VR53,r0
	movw	VR50,#:lower16:ints
	movt	VR50,#:upper16:ints
	movw	VR51,#:lower16:intt
	movt	VR51,#:upper16:intt
	ldr	VR52,[VR51]
	ldr	VR54,[VR50,VR52,lsl 2]
	movw	VR55,#10
	mul	VR54,VR54,VR55
	add	VR54,VR54,VR53
	str	VR54,[VR50,VR52,lsl 2]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



find:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	movw	VR50,#:lower16:chat
	movt	VR50,#:upper16:chat
	ldr	VR51,[VR50]
	sub	VR51,VR51,#1
	add	VR62,VR51,#1
	movw	VR52,#:lower16:chas
	movt	VR52,#:upper16:chas
	ldr	VR63,[VR52,VR62,lsl 2]
	mov	VR55,VR63
	movw	VR64,#32
	mov	VR61,VR64
	movw	VR58,#:lower16:ii
	movt	VR58,#:upper16:ii
	ldr	VR59,[VR58]
	add	VR56,VR59,#1
	movw	VR57,#:lower16:get2
	movt	VR57,#:upper16:get2
	str	VR61,[VR57,VR59,lsl 2]
	mov	VR60,VR63
	add	VR59,VR59,#2
	cmp	VR51,#0
	str	VR60,[VR57,VR56,lsl 2]
	bne	.5
	movw	VR50,#:lower16:chat
	movt	VR50,#:upper16:chat
	str	VR51,[VR50]
	movw	VR54,#:lower16:c
	movt	VR54,#:upper16:c
	str	VR55,[VR54]
	movw	VR58,#:lower16:ii
	movt	VR58,#:upper16:ii
	str	VR59,[VR58]
	movw	r0,#0
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
.5:
	movw	VR50,#:lower16:chat
	movt	VR50,#:upper16:chat
	str	VR51,[VR50]
	movw	VR54,#:lower16:c
	movt	VR54,#:upper16:c
	str	VR55,[VR54]
	movw	VR58,#:lower16:ii
	movt	VR58,#:upper16:ii
	str	VR59,[VR58]
	movw	r0,#1
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



main:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add	fp,sp,#100
	sub	sp,sp,#4
	sub	sp,sp,#112
	sub	sp,sp,#8
	mov	VR87,#0
	mov	VR65,#0
	movw	VR92,#:lower16:get
	movt	VR92,#:upper16:get
	movw	VR297,#:lower16:get
	movt	VR297,#:upper16:get
	mov	r0,VR297
	bl	getstr
	mov	VR98,r0
	mov	VR278,VR98
	add	VR279,sp,#116
	add	VR315,sp,#116
	str	VR278,[VR315]
	add	sp,sp,#8
.6:
	sub	sp,sp,#8
	movw	VR99,#:lower16:i
	movt	VR99,#:upper16:i
	ldr	VR100,[VR99]
	add	VR316,sp,#116
	ldr	VR280,[VR316]
	cmp	VR100,VR280
	add	sp,sp,#8
	bge	.39
	movw	VR298,#:lower16:get
	movt	VR298,#:upper16:get
	ldr	VR153,[VR298,VR100,lsl 2]
	cmp	VR153,#48
	blt	.56
	cmp	VR153,#57
	bgt	.56
	mov	VR78,#1
	b	.57
.56:
	mov	VR78,#0
	b	.57
.57:
	cmp	VR78,#1
	bne	.7
	mov	VR152,VR153
	movw	VR94,#:lower16:ii
	movt	VR94,#:upper16:ii
	ldr	VR95,[VR94]
	movw	VR81,#:lower16:get2
	movt	VR81,#:upper16:get2
	str	VR152,[VR81,VR95,lsl 2]
	add	VR95,VR95,#1
	b	.38
.7:
	cmp	VR153,#40
	bne	.8
	add	VR65,VR65,#1
	mov	VR172,#40
	movw	VR68,#:lower16:chas
	movt	VR68,#:upper16:chas
	str	VR172,[VR68,VR65,lsl 2]
.8:
	cmp	VR153,#94
	bne	.9
	add	VR65,VR65,#1
	mov	VR255,#94
	movw	VR201,#:lower16:chas
	movt	VR201,#:upper16:chas
	str	VR255,[VR201,VR65,lsl 2]
.9:
	cmp	VR153,#41
	bne	.12
	sub	VR65,VR65,#1
	add	VR164,VR65,#1
	movw	VR202,#:lower16:chas
	movt	VR202,#:upper16:chas
	ldr	VR163,[VR202,VR164,lsl 2]
	mov	VR73,VR163
.10:
	cmp	VR73,#40
	beq	.11
	mov	VR245,#32
	movw	VR222,#:lower16:ii
	movt	VR222,#:upper16:ii
	ldr	VR95,[VR222]
	add	VR80,VR95,#1
	movw	VR213,#:lower16:get2
	movt	VR213,#:upper16:get2
	str	VR245,[VR213,VR95,lsl 2]
	mov	VR82,VR73
	add	VR95,VR95,#2
	sub	VR65,VR65,#1
	add	VR166,VR65,#1
	ldr	VR162,[VR202,VR166,lsl 2]
	mov	VR73,VR162
	str	VR82,[VR213,VR80,lsl 2]
	str	VR95,[VR222]
	b	.10
.11:
	movw	VR72,#:lower16:c
	movt	VR72,#:upper16:c
	str	VR73,[VR72]
.12:
	cmp	VR153,#43
	bne	.17
.13:
	movw	VR202,#:lower16:chas
	movt	VR202,#:upper16:chas
	ldr	VR256,[VR202,VR65,lsl 2]
	cmp	VR256,#43
	beq	.14
	cmp	VR256,#45
	beq	.14
	cmp	VR256,#42
	beq	.14
	cmp	VR256,#47
	beq	.14
	cmp	VR256,#37
	beq	.14
	cmp	VR256,#94
	beq	.14
	b	.16
.14:
	sub	VR65,VR65,#1
	add	VR159,VR65,#1
	ldr	VR155,[VR202,VR159,lsl 2]
	mov	VR73,VR155
	mov	VR246,#32
	movw	VR223,#:lower16:ii
	movt	VR223,#:upper16:ii
	ldr	VR95,[VR223]
	add	VR160,VR95,#1
	movw	VR214,#:lower16:get2
	movt	VR214,#:upper16:get2
	str	VR246,[VR214,VR95,lsl 2]
	mov	VR170,VR155
	add	VR95,VR95,#2
	cmp	VR65,#0
	str	VR170,[VR214,VR160,lsl 2]
	bne	.58
	mov	VR89,#0
	b	.59
.58:
	mov	VR89,#1
	b	.59
.59:
	cmp	VR89,#0
	bne	.15
	movw	VR203,#:lower16:c
	movt	VR203,#:upper16:c
	str	VR73,[VR203]
	movw	VR224,#:lower16:ii
	movt	VR224,#:upper16:ii
	str	VR95,[VR224]
	b	.16
.15:
	movw	VR204,#:lower16:c
	movt	VR204,#:upper16:c
	str	VR73,[VR204]
	movw	VR225,#:lower16:ii
	movt	VR225,#:upper16:ii
	str	VR95,[VR225]
	b	.13
.16:
	add	VR65,VR65,#1
	mov	VR256,#43
	str	VR256,[VR202,VR65,lsl 2]
.17:
	cmp	VR153,#45
	bne	.22
.18:
	movw	VR202,#:lower16:chas
	movt	VR202,#:upper16:chas
	ldr	VR256,[VR202,VR65,lsl 2]
	cmp	VR256,#43
	beq	.19
	cmp	VR256,#45
	beq	.19
	cmp	VR256,#42
	beq	.19
	cmp	VR256,#47
	beq	.19
	cmp	VR256,#37
	beq	.19
	cmp	VR256,#94
	beq	.19
	b	.21
.19:
	sub	VR65,VR65,#1
	add	VR161,VR65,#1
	ldr	VR171,[VR202,VR161,lsl 2]
	mov	VR73,VR171
	mov	VR247,#32
	movw	VR226,#:lower16:ii
	movt	VR226,#:upper16:ii
	ldr	VR95,[VR226]
	add	VR169,VR95,#1
	movw	VR215,#:lower16:get2
	movt	VR215,#:upper16:get2
	str	VR247,[VR215,VR95,lsl 2]
	mov	VR168,VR171
	add	VR95,VR95,#2
	cmp	VR65,#0
	str	VR168,[VR215,VR169,lsl 2]
	bne	.60
	mov	VR74,#0
	b	.61
.60:
	mov	VR74,#1
	b	.61
.61:
	cmp	VR74,#0
	bne	.20
	movw	VR205,#:lower16:c
	movt	VR205,#:upper16:c
	str	VR73,[VR205]
	movw	VR227,#:lower16:ii
	movt	VR227,#:upper16:ii
	str	VR95,[VR227]
	b	.21
.20:
	movw	VR206,#:lower16:c
	movt	VR206,#:upper16:c
	str	VR73,[VR206]
	movw	VR228,#:lower16:ii
	movt	VR228,#:upper16:ii
	str	VR95,[VR228]
	b	.18
.21:
	add	VR65,VR65,#1
	mov	VR256,#45
	str	VR256,[VR202,VR65,lsl 2]
.22:
	cmp	VR153,#42
	bne	.27
.23:
	movw	VR202,#:lower16:chas
	movt	VR202,#:upper16:chas
	ldr	VR256,[VR202,VR65,lsl 2]
	cmp	VR256,#42
	beq	.24
	cmp	VR256,#47
	beq	.24
	cmp	VR256,#37
	beq	.24
	cmp	VR256,#94
	beq	.24
	b	.26
.24:
	sub	VR65,VR65,#1
	add	VR150,VR65,#1
	ldr	VR146,[VR202,VR150,lsl 2]
	mov	VR73,VR146
	mov	VR248,#32
	movw	VR229,#:lower16:ii
	movt	VR229,#:upper16:ii
	ldr	VR95,[VR229]
	add	VR148,VR95,#1
	movw	VR216,#:lower16:get2
	movt	VR216,#:upper16:get2
	str	VR248,[VR216,VR95,lsl 2]
	mov	VR144,VR146
	add	VR95,VR95,#2
	cmp	VR65,#0
	str	VR144,[VR216,VR148,lsl 2]
	bne	.62
	mov	VR51,#0
	b	.63
.62:
	mov	VR51,#1
	b	.63
.63:
	cmp	VR51,#0
	bne	.25
	movw	VR207,#:lower16:c
	movt	VR207,#:upper16:c
	str	VR73,[VR207]
	movw	VR230,#:lower16:ii
	movt	VR230,#:upper16:ii
	str	VR95,[VR230]
	b	.26
.25:
	movw	VR208,#:lower16:c
	movt	VR208,#:upper16:c
	str	VR73,[VR208]
	movw	VR231,#:lower16:ii
	movt	VR231,#:upper16:ii
	str	VR95,[VR231]
	b	.23
.26:
	add	VR65,VR65,#1
	mov	VR256,#42
	str	VR256,[VR202,VR65,lsl 2]
.27:
	cmp	VR153,#47
	bne	.32
.28:
	movw	VR202,#:lower16:chas
	movt	VR202,#:upper16:chas
	ldr	VR256,[VR202,VR65,lsl 2]
	cmp	VR256,#42
	beq	.29
	cmp	VR256,#47
	beq	.29
	cmp	VR256,#37
	beq	.29
	cmp	VR256,#94
	beq	.29
	b	.31
.29:
	sub	VR65,VR65,#1
	add	VR143,VR65,#1
	ldr	VR154,[VR202,VR143,lsl 2]
	mov	VR73,VR154
	mov	VR249,#32
	movw	VR232,#:lower16:ii
	movt	VR232,#:upper16:ii
	ldr	VR95,[VR232]
	add	VR145,VR95,#1
	movw	VR217,#:lower16:get2
	movt	VR217,#:upper16:get2
	str	VR249,[VR217,VR95,lsl 2]
	mov	VR173,VR154
	add	VR95,VR95,#2
	cmp	VR65,#0
	str	VR173,[VR217,VR145,lsl 2]
	bne	.64
	mov	VR52,#0
	b	.65
.64:
	mov	VR52,#1
	b	.65
.65:
	cmp	VR52,#0
	bne	.30
	movw	VR209,#:lower16:c
	movt	VR209,#:upper16:c
	str	VR73,[VR209]
	movw	VR233,#:lower16:ii
	movt	VR233,#:upper16:ii
	str	VR95,[VR233]
	b	.31
.30:
	movw	VR210,#:lower16:c
	movt	VR210,#:upper16:c
	str	VR73,[VR210]
	movw	VR234,#:lower16:ii
	movt	VR234,#:upper16:ii
	str	VR95,[VR234]
	b	.28
.31:
	add	VR65,VR65,#1
	mov	VR256,#47
	str	VR256,[VR202,VR65,lsl 2]
.32:
	cmp	VR153,#37
	bne	.37
.33:
	movw	VR202,#:lower16:chas
	movt	VR202,#:upper16:chas
	ldr	VR256,[VR202,VR65,lsl 2]
	cmp	VR256,#42
	beq	.34
	cmp	VR256,#47
	beq	.34
	cmp	VR256,#37
	beq	.34
	cmp	VR256,#94
	beq	.34
	b	.36
.34:
	sub	VR65,VR65,#1
	add	VR178,VR65,#1
	ldr	VR179,[VR202,VR178,lsl 2]
	mov	VR73,VR179
	mov	VR250,#32
	movw	VR235,#:lower16:ii
	movt	VR235,#:upper16:ii
	ldr	VR95,[VR235]
	add	VR175,VR95,#1
	movw	VR218,#:lower16:get2
	movt	VR218,#:upper16:get2
	str	VR250,[VR218,VR95,lsl 2]
	mov	VR180,VR179
	add	VR95,VR95,#2
	cmp	VR65,#0
	str	VR180,[VR218,VR175,lsl 2]
	bne	.66
	mov	VR112,#0
	b	.67
.66:
	mov	VR112,#1
	b	.67
.67:
	cmp	VR112,#0
	bne	.35
	movw	VR211,#:lower16:c
	movt	VR211,#:upper16:c
	str	VR73,[VR211]
	movw	VR236,#:lower16:ii
	movt	VR236,#:upper16:ii
	str	VR95,[VR236]
	b	.36
.35:
	movw	VR212,#:lower16:c
	movt	VR212,#:upper16:c
	str	VR73,[VR212]
	movw	VR237,#:lower16:ii
	movt	VR237,#:upper16:ii
	str	VR95,[VR237]
	b	.33
.36:
	add	VR65,VR65,#1
	mov	VR256,#37
	str	VR256,[VR202,VR65,lsl 2]
.37:
	mov	VR251,#32
	movw	VR238,#:lower16:ii
	movt	VR238,#:upper16:ii
	ldr	VR95,[VR238]
	movw	VR219,#:lower16:get2
	movt	VR219,#:upper16:get2
	str	VR251,[VR219,VR95,lsl 2]
	add	VR95,VR95,#1
.38:
	add	VR100,VR100,#1
	movw	VR239,#:lower16:ii
	movt	VR239,#:upper16:ii
	str	VR95,[VR239]
	movw	VR243,#:lower16:i
	movt	VR243,#:upper16:i
	str	VR100,[VR243]
	b	.6
.39:
.40:
	cmp	VR65,#0
	ble	.41
	sub	VR65,VR65,#1
	add	VR183,VR65,#1
	mov	VR252,#32
	movw	VR240,#:lower16:ii
	movt	VR240,#:upper16:ii
	ldr	VR95,[VR240]
	add	VR115,VR95,#1
	movw	VR202,#:lower16:chas
	movt	VR202,#:upper16:chas
	ldr	VR174,[VR202,VR183,lsl 2]
	movw	VR220,#:lower16:get2
	movt	VR220,#:upper16:get2
	str	VR252,[VR220,VR95,lsl 2]
	mov	VR116,VR174
	add	VR95,VR95,#2
	str	VR95,[VR240]
	str	VR116,[VR220,VR115,lsl 2]
	b	.40
.41:
	mov	VR253,#64
	mov	VR100,#1
	movw	VR221,#:lower16:get2
	movt	VR221,#:upper16:get2
	movw	VR241,#:lower16:ii
	movt	VR241,#:upper16:ii
	ldr	VR95,[VR241]
	str	VR253,[VR221,VR95,lsl 2]
.42:
	ldr	VR299,[VR221,VR100,lsl 2]
	str	VR299,[VR221,VR100,lsl 2]
	ldr	VR300,[VR221,VR100,lsl 2]
	cmp	VR300,#64
	beq	.55
	ldr	VR301,[VR221,VR100,lsl 2]
	cmp	VR301,#43
	beq	.43
	ldr	VR302,[VR221,VR100,lsl 2]
	cmp	VR302,#45
	beq	.43
	ldr	VR303,[VR221,VR100,lsl 2]
	cmp	VR303,#42
	beq	.43
	ldr	VR304,[VR221,VR100,lsl 2]
	cmp	VR304,#47
	beq	.43
	ldr	VR305,[VR221,VR100,lsl 2]
	cmp	VR305,#37
	beq	.43
	ldr	VR306,[VR221,VR100,lsl 2]
	cmp	VR306,#94
	beq	.43
	b	.50
.43:
	sub	sp,sp,#8
	sub	VR87,VR87,#1
	add	VR181,VR87,#1
	movw	VR66,#:lower16:ints
	movt	VR66,#:upper16:ints
	ldr	VR182,[VR66,VR181,lsl 2]
	mov	VR281,VR182
	add	VR282,sp,#56
	str	VR281,[VR282]
	sub	VR87,VR87,#1
	add	VR177,VR87,#1
	ldr	VR185,[VR66,VR177,lsl 2]
	mov	VR125,VR185
	ldr	VR307,[VR221,VR100,lsl 2]
	cmp	VR307,#43
	add	sp,sp,#8
	bne	.44
	sub	sp,sp,#0
	ldr	VR283,[VR282]
	add	VR284,VR283,VR125
	add	VR285,sp,#40
	str	VR284,[VR285]
	add	sp,sp,#0
.44:
	ldr	VR308,[VR221,VR100,lsl 2]
	cmp	VR308,#45
	bne	.45
	sub	sp,sp,#0
	ldr	VR286,[VR282]
	sub	VR287,VR125,VR286
	add	VR285,sp,#40
	str	VR287,[VR285]
	add	sp,sp,#0
.45:
	ldr	VR309,[VR221,VR100,lsl 2]
	cmp	VR309,#42
	bne	.46
	sub	sp,sp,#0
	ldr	VR288,[VR282]
	mul	VR289,VR288,VR125
	add	VR285,sp,#40
	str	VR289,[VR285]
	add	sp,sp,#0
.46:
	ldr	VR310,[VR221,VR100,lsl 2]
	cmp	VR310,#47
	bne	.47
	sub	sp,sp,#0
	mov	r0,VR125
	ldr	VR290,[VR282]
	mov	r1,VR290
	bl	__aeabi_idiv
	mov	VR291,r0
	add	VR285,sp,#40
	str	VR291,[VR285]
	add	sp,sp,#0
.47:
	ldr	VR311,[VR221,VR100,lsl 2]
	cmp	VR311,#37
	bne	.48
	sub	sp,sp,#0
	mov	r0,VR125
	ldr	VR292,[VR282]
	mov	r1,VR292
	bl	__aeabi_idivmod
	mov	VR293,r1
	add	VR285,sp,#40
	str	VR293,[VR285]
	add	sp,sp,#0
.48:
	ldr	VR312,[VR221,VR100,lsl 2]
	cmp	VR312,#94
	bne	.49
	ldr	VR294,[VR282]
	mov	VR187,VR294
	mov	VR186,#1
.68:
	cmp	VR187,#0
	beq	.69
	mul	VR186,VR186,VR125
	sub	VR187,VR187,#1
	b	.68
.69:
	sub	sp,sp,#0
	mov	VR295,VR186
	add	VR285,sp,#40
	str	VR295,[VR285]
	add	sp,sp,#0
.49:
	sub	sp,sp,#0
	mov	VR87,VR177
	add	VR285,sp,#40
	ldr	VR296,[VR285]
	mov	VR158,VR296
	str	VR158,[VR66,VR87,lsl 2]
	add	sp,sp,#0
	b	.54
.50:
	ldr	VR313,[VR221,VR100,lsl 2]
	cmp	VR313,#32
	beq	.53
	ldr	VR314,[VR221,VR100,lsl 2]
	sub	VR134,VR314,#48
	add	VR87,VR87,#1
	mov	VR254,VR134
	mov	VR95,#1
	movw	VR66,#:lower16:ints
	movt	VR66,#:upper16:ints
	str	VR254,[VR66,VR87,lsl 2]
.51:
	add	VR135,VR100,VR95
	ldr	VR136,[VR221,VR135,lsl 2]
	cmp	VR136,#32
	beq	.52
	mov	VR138,VR135
	ldr	VR139,[VR221,VR138,lsl 2]
	sub	VR140,VR139,#48
	movw	VR199,#10
	mul	VR254,VR254,VR199
	add	VR254,VR254,VR140
	add	VR95,VR95,#1
	str	VR254,[VR66,VR87,lsl 2]
	b	.51
.52:
	mov	VR141,VR135
	sub	VR100,VR141,#1
.53:
.54:
	add	VR100,VR100,#1
	b	.42
.55:
	movw	VR200,#:lower16:ints
	movt	VR200,#:upper16:ints
	ldr	VR142,[VR200,#4]
	mov	r0,VR142
	bl	putint
	movw	VR64,#:lower16:chat
	movt	VR64,#:upper16:chat
	str	VR65,[VR64]
	movw	VR86,#:lower16:intt
	movt	VR86,#:upper16:intt
	str	VR87,[VR86]
	movw	VR242,#:lower16:ii
	movt	VR242,#:upper16:ii
	str	VR95,[VR242]
	movw	VR244,#:lower16:i
	movt	VR244,#:upper16:i
	str	VR100,[VR244]
	movw	r0,#0
	add	sp,sp,#112
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}



