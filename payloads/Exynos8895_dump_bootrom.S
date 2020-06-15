.text
.global _start

_start:
	mov	w1, #0x20000 // size
	mov w0, #0x0000 // address
	movk w0, #0x0000, lsl #16 // address
	bl usb_send
	mov x0, #0
	br x0 //reset

usb_send:
	stp	x29, x30, [sp,#-80]!
	mov	w2, #0xc12                 	// #3090
	mov	w3, #0x0                   	// #0
	mov	x5, #0xc834                	// #51252
	mov	x29, sp
	stp	x19, x20, [sp,#16]
	movk	x5, #0x10c0, lsl #16
	mov	w20, #0x1                   	// #1
	bfxil	w3, w1, #0, #24
	mov	x4, #0xc838                	// #51256
	stp	w3, w2, [x29,#56]
	str	x21, [sp,#32]
	movk	x4, #0x10c0, lsl #16
	orr	w2, w2, w20
	mov	x21, #0xc83c                	// #51260
	stp	w0, wzr, [x29,#48]
	add	x19, x29, #0x30
	strb	w2, [x29,#60]
	movk	x21, #0x10c0, lsl #16
	str	w19, [x5]
	mov	w3, #0x406                 	// #1030
	mov	w0, w20
	str	wzr, [x4]
	str	w3, [x21]
	mov	w1, #0x1388                	// #5000
	mov	x2, #0x2778                	// #10104
	and	x19, x19, #0xffffffff
	blr	x2
	ldp	x2, x3, [x19]
	ldr	w0, [x21]
	stp	x2, x3, [x29,#64]
	mov	w0, w20
	ldr	x21, [sp,#32]
	ldp	x19, x20, [sp,#16]
	ldp	x29, x30, [sp],#80
	ret
