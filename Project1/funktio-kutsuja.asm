b main

taakse:
	pop {pc}

main:
mov lr, pc
push {lr}
b taakse
b end1

end1:	
	mov lr, pc
	push {lr}
	b eteen
	b loppu

eteen:
	pop {pc}

loppu:
