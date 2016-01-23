bl main
b end
factorial:
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7, #0]
	ldr	r3, [r7, #4]
	cmp	r3, #1
	bne	.L2
	ldr	r3, [r7, #0]
	b	.L3
.L2:
	ldr	r3, [r7, #4]
	add	r2, r3, #-1
	ldr	r3, [r7, #4]
	ldr	r1, [r7, #0]
	mul	r3, r1, r3
	mov	r0, r2
	mov	r1, r3
	bl	factorial
	mov	r3, r0
.L3:
	mov	r0, r3
	add	r7, r7, #8
	mov	sp, r7
	pop	{r7, pc}
main:
	push	{r7, lr}
	add	r7, sp, #0
	mov	r0, #5
	mov	r1, #1
	bl	factorial
	mov	r3, #0
	mov	r0, r3
	pop	{r7, pc}
end:
