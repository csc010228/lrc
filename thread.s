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
	.global thread_func
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	thread_func, %function
thread_func:
	@ thread_func do not need to save context registers,and do not need to assign fp
	@ but thread_func still need to move sp
	sub	sp,sp,#...
	...
	add	sp,sp,#...
	.size	thread_func, .-thread_func
	.text
	.align 1
	.global	__aeabi_idiv
	.global	__aeabi_idivmod
	.global multi_threads
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfp
	.type	multi_threads, %function
SYS_clone = 120
CLONE_VM = 256
SIGCHLD = 17
SYS_waitid = 280
SYS_exit = 1
P_ALL = 0
WEXITED = 4
multi_threads:
	push	{r4,r5,r6,r7,r8,r9,r10,fp,lr}
	vpush	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	add fp,sp,#100
	push	{r0,r1,r2,r3}
	mov r9,r0
	sub r0,r1,r0
	mov r1,#thread_num
	bl __aeabi_idiv
	mov r8,r0
	mov r7, #SYS_clone
	mov r0, #(CLONE_VM | SIGCHLD)
    sub r1, sp, #0
    mov r2, #0
    mov r3, #0
    mov r4, #0
	mov r5, #0
	mov r6, #0
    swi #0
	cmp r0,#0
	bne .L1
	@child thread do start
	ldm sp,{r0,r1,r2,r3}
	add r9,r9,r8
	mov r1,r9
	bl thread_func
	@child thread do end
	mov r0, #0
    mov r7, #SYS_exit
	swi #0
.L1:
	mov r7, #SYS_clone
	mov r0, #(CLONE_VM | SIGCHLD)
    sub r1, sp, #thread_func_s_max_stack_space
    mov r2, #0
    mov r3, #0
    mov r4, #0
	mov r5, #0
	mov r6, #0
    swi #0
	cmp r0,#0
	bne .L2
	@child thread do start
	ldm sp,{r0,r1,r2,r3}
	mov r0,r9
	add r9,r9,r8
	mov r1,r9
	bl thread_func
	@child thread do end
	mov r0, #0
    mov r7, #SYS_exit
	swi #0
.L2:
	mov r7, #SYS_clone
	mov r0, #(CLONE_VM | SIGCHLD)
    sub r1, sp, #thread_func_s_max_stack_space*2
    mov r2, #0
    mov r3, #0
    mov r4, #0
	mov r5, #0
	mov r6, #0
    swi #0
	cmp r0,#0
	bne .L3
	@child thread do start
	ldm sp,{r0,r1,r2,r3}
	mov r0,r9
	add r9,r9,r8
	mov r1,r9
	bl thread_func
	@child thread do end
	mov r0, #0
    mov r7, #SYS_exit
	swi #0
.L3:
	mov r7, #SYS_clone
	mov r0, #(CLONE_VM | SIGCHLD)
    sub r1, sp, #thread_func_s_max_stack_space*3
    mov r2, #0
    mov r3, #0
    mov r4, #0
	mov r5, #0
	mov r6, #0
    swi #0
	cmp r0,#0
	bne .L4
	@child thread do start
	ldm sp,{r0,r1,r2,r3}
	mov r0,r9
	bl thread_func
	@child thread do end
	mov r0, #0
    mov r7, #SYS_exit
	swi #0
.L4:
	mov r0, #P_ALL
    mov r1, #0
    mov r2, #0
    mov r3, #WEXITED
    mov r7, #SYS_waitid
    swi #0
	pop	{r0,r1,r2,r3}
	vpop	{s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,s30,s31}
	pop	{r4,r5,r6,r7,r8,r9,r10,fp,pc}
	.size	multi_threads, .-multi_threads
	.ident	"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
