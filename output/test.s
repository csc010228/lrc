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
	push	{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	sub	sp,sp,#52
	movw	r0,#:lower16:image_in
	movt	r0,#:upper16:image_in
	bl	getarray
	mov	r1,r0
	push	{r0}
	add	r0,sp,#44
	str	r1,[r0]
	ldr r0,=23
	bl	_sysy_starttime
	ldr r0,=1
	add	r1,sp,#48
	str	r0,[r1]
	add	sp,sp,#4
.3:
	add	r1,sp,#44
	ldr	r0,[r1]
	ldr r2,=511
	cmp	r0,r2
	bge	.6
	ldr r0,=1
	add	r1,sp,#48
	str	r0,[r1]
.4:
	add	r1,sp,#48
	ldr	r0,[r1]
	ldr r2,=1023
	cmp	r0,r2
	bge	.5
	add	r1,sp,#48
	ldr	r0,[r1]
	sub	r2,r0,#1
	ldr r0,=512
	mul	r1,r2,r0
	push	{r2}
	add	r2,sp,#48
	ldr	r0,[r2]
	add	r3,r1,r0
	sub	r0,r3,#1
	push	{r1}
	mov	r1,r0
	push	{r0}
	add	r2,sp,#48
	str	r1,[r2]
	add	r1,sp,#60
	ldr	r0,[r1]
	push	{r3}
	sub	r3,r0,#1
	ldr r0,=512
	mul	r1,r3,r0
	add	r2,sp,#60
	ldr	r0,[r2]
	push	{r3}
	add	r3,r1,r0
	mov	r0,r3
	push	{r1}
	add	r1,sp,#56
	str	r0,[r1]
	add	r2,sp,#72
	ldr	r0,[r2]
	push	{r3}
	sub	r3,r0,#1
	ldr r0,=512
	mul	r1,r3,r0
	add	r2,sp,#72
	ldr	r0,[r2]
	push	{r3}
	add	r3,r1,r0
	add	r0,r3,#1
	push	{r1}
	mov	r1,r0
	push	{r0}
	add	r2,sp,#68
	str	r1,[r2]
	add	r1,sp,#88
	ldr	r0,[r1]
	push	{r3}
	ldr r3,=512
	mul	r4,r0,r3
	add	r1,sp,#88
	ldr	r0,[r1]
	add	r2,r4,r0
	sub	r0,r2,#1
	mov	r1,r0
	push	{r0}
	push	{r2}
	add	r2,sp,#76
	str	r1,[r2]
	add	r1,sp,#100
	ldr	r0,[r1]
	push	{r4}
	mul	r4,r0,r3
	add	r1,sp,#100
	ldr	r0,[r1]
	add	r2,r4,r0
	mov	r0,r2
	add	r1,sp,#76
	str	r0,[r1]
	push	{r2}
	add	r2,sp,#108
	ldr	r0,[r2]
	push	{r4}
	mul	r4,r0,r3
	add	r1,sp,#108
	ldr	r0,[r1]
	add	r2,r4,r0
	add	r0,r2,#1
	mov	r1,r0
	push	{r0}
	push	{r2}
	add	r2,sp,#88
	str	r1,[r2]
	add	r1,sp,#120
	ldr	r0,[r1]
	add	r3,r0,#1
	ldr r0,=512
	mul	r1,r3,r0
	add	r2,sp,#116
	ldr	r0,[r2]
	push	{r3}
	add	r3,r1,r0
	sub	r0,r3,#1
	push	{r1}
	mov	r1,r0
	push	{r0}
	add	r2,sp,#96
	str	r1,[r2]
	add	r1,sp,#132
	ldr	r0,[r1]
	push	{r3}
	add	r3,r0,#1
	ldr r0,=512
	mul	r1,r3,r0
	add	r2,sp,#132
	ldr	r0,[r2]
	push	{r3}
	add	r3,r1,r0
	mov	r0,r3
	push	{r1}
	add	r1,sp,#104
	str	r0,[r1]
	add	r2,sp,#144
	ldr	r0,[r2]
	push	{r3}
	add	r3,r0,#1
	ldr r0,=512
	mul	r1,r3,r0
	add	r2,sp,#144
	ldr	r0,[r2]
	push	{r3}
	add	r3,r1,r0
	add	r0,r3,#1
	push	{r1}
	mov	r1,r0
	push	{r0}
	add	r2,sp,#116
	str	r1,[r2]
	add	r1,sp,#132
	ldr	r0,[r1]
	push	{r3}
	ldr r3,=1
	push	{r4}
	mul	r4,r0,r3
	add	r0,r4,#0
	push	{r0}
	ldr r0,=8
	movw	r2,#:lower16:image_in
	movt	r2,#:upper16:image_in
	push	{r4}
	add	r4,sp,#4
	ldr	r3,[r4]
	ldr	r1,[r2,r3,lsl 2]
	mul	r5,r0,r1
	add	r1,sp,#164
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	sub	r4,r5,r0
	add	r1,sp,#168
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r5}
	sub	r5,r4,r0
	add	r1,sp,#176
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r4}
	sub	r4,r5,r0
	add	r1,sp,#184
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r5}
	sub	r5,r4,r0
	add	r1,sp,#188
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r4}
	sub	r4,r5,r0
	add	r1,sp,#196
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r5}
	sub	r5,r4,r0
	add	r1,sp,#204
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r4}
	sub	r4,r5,r0
	add	r1,sp,#212
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	push	{r5}
	sub	r5,r4,r0
	mov	r0,r5
	add	r1,sp,#220
	str	r0,[r1]
	add	r2,sp,#268
	ldr	r0,[r2]
	ldr r3,=512
	push	{r4}
	mul	r4,r0,r3
	add	r1,sp,#268
	ldr	r0,[r1]
	add	r2,r4,r0
	ldr r0,=1
	mul	r1,r2,r0
	add	r0,r1,#0
	push	{r0}
	push	{r1}
	push	{r2}
	push	{r4}
	push	{r5}
	add	r1,sp,#244
	ldr	r0,[r1]
	bl	cutout
	mov	r1,r0
	push	{r0}
	movw	r2,#:lower16:image_out
	movt	r2,#:upper16:image_out
	add	r4,sp,#20
	ldr	r3,[r4]
	str	r1,[r2,r3,lsl 2]
	add	r1,sp,#296
	ldr	r0,[r1]
	add	r5,r0,#1
	mov	r0,r5
	str	r0,[r1]
	add	sp,sp,#248
	b	.4
.5:
	add	r1,sp,#44
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	b	.3
.6:
	ldr r0,=0
	add	r1,sp,#48
	str	r0,[r1]
.7:
	add	r1,sp,#48
	ldr	r0,[r1]
	cmp	r0,#1024
	bge	.8
	add	r1,sp,#48
	ldr	r0,[r1]
	ldr r2,=512
	mul	r3,r0,r2
	ldr r0,=1
	mul	r1,r3,r0
	add	r0,r1,#0
	push	{r0}
	push	{r1}
	add	r1,sp,#56
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	ldr r0,=1
	mul	r1,r3,r0
	add	r0,r1,#0
	push	{r0}
	push	{r1}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	mov	r4,r0
	add	r1,sp,#72
	ldr	r0,[r1]
	ldr r2,=512
	mul	r3,r0,r2
	add	r0,r3,#512
	sub	r1,r0,#1
	push	{r0}
	ldr r0,=1
	mul	r2,r1,r0
	add	r0,r2,#0
	push	{r0}
	push	{r1}
	add	r1,sp,#84
	ldr	r0,[r1]
	push	{r2}
	ldr r2,=512
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#512
	sub	r1,r0,#1
	push	{r0}
	ldr r0,=1
	mul	r2,r1,r0
	add	r0,r2,#0
	push	{r0}
	push	{r1}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r2}
	push	{r3}
	add	r3,sp,#12
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r5,#:lower16:image_out
	movt	r5,#:upper16:image_out
	add	r7,sp,#60
	ldr	r6,[r7]
	str	r4,[r5,r6,lsl 2]
	mov	r4,r0
	add	r1,sp,#112
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	add	r1,sp,#32
	ldr	r0,[r1]
	str	r4,[r5,r0,lsl 2]
	add	sp,sp,#64
	b	.7
.8:
	ldr r0,=0
	add	r1,sp,#44
	str	r0,[r1]
.9:
	add	r1,sp,#44
	ldr	r0,[r1]
	cmp	r0,#512
	bge	.10
	add	r1,sp,#44
	ldr	r0,[r1]
	ldr r2,=1
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	ldr	r0,[r1]
	push	{r3}
	mul	r3,r0,r2
	add	r0,r3,#0
	push	{r0}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#4
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	mov	r4,r0
	add	r1,sp,#60
	ldr	r0,[r1]
	ldr r2,=523776
	add	r3,r0,r2
	ldr r0,=1
	mul	r1,r3,r0
	add	r0,r1,#0
	push	{r0}
	push	{r1}
	add	r1,sp,#68
	ldr	r0,[r1]
	push	{r3}
	add	r3,r0,r2
	ldr r0,=1
	mul	r1,r3,r0
	add	r0,r1,#0
	push	{r0}
	push	{r1}
	movw	r1,#:lower16:image_in
	movt	r1,#:upper16:image_in
	push	{r3}
	add	r3,sp,#8
	ldr	r2,[r3]
	ldr	r0,[r1,r2,lsl 2]
	movw	r5,#:lower16:image_out
	movt	r5,#:upper16:image_out
	add	r7,sp,#36
	ldr	r6,[r7]
	str	r4,[r5,r6,lsl 2]
	mov	r4,r0
	add	r1,sp,#84
	ldr	r0,[r1]
	add	r2,r0,#1
	mov	r0,r2
	str	r0,[r1]
	add	r1,sp,#20
	ldr	r0,[r1]
	str	r4,[r5,r0,lsl 2]
	add	sp,sp,#40
	b	.9
.10:
	ldr r0,=59
	bl	_sysy_stoptime
	ldr r0,=524288
	movw	r1,#:lower16:image_out
	movt	r1,#:upper16:image_out
	bl	putarray
	add	r1,sp,#40
	ldr	r0,[r1]
	add	sp,sp,#52
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
	.size	main, .-main
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global cutout
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	cutout, %function
cutout:
	push	{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	push	{r0}
	cmp	r0,#0
	bge	.0
	ldr r0,=0
	add	r1,sp,#0
	str	r0,[r1]
	b	.2
.0:
	add	r1,sp,#0
	ldr	r0,[r1]
	cmp	r0,#255
	ble	.1
	ldr r0,=255
	add	r1,sp,#0
	str	r0,[r1]
.1:
.2:
	add	r1,sp,#0
	ldr	r0,[r1]
	add	sp,sp,#4
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
	.size	cutout, .-cutout
	.comm image_in,2097152,4
	.comm image_out,2097152,4
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
