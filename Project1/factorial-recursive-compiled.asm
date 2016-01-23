b main

kertoma:
	sub SP, #12
	ldr r2, [SP, #0]
	ldr r1, [SP, #4]
	cmp r2, #1
	beq finish
	
	sub r0, #1
	mul r1, r2
	mov lr, pc
	push {r0,r1, lr}
	b kertoma
finish:
	add sp, #12
	pop {r0,r1,pc}
	
main:
	mov r0, #5
	mov r1, #1
	mov lr, pc
	push {r0,r1,lr}
	b kertoma
end:
	mov r3, #20
