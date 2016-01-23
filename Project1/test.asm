b main 
multiply:
	sub sp, #8
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	mul r0, r1
	add sp, #8
	ldr r3, [sp, #0]
	mov pc, r3
	b end

main:
	mov r0, #0
	mov r1, #0
	mov r3, #0
	mov r0, #3
	mov r1, #2
	mov lr, pc
	push {r0,r1,lr}
	b multiply
end:
	mov r2, #6
