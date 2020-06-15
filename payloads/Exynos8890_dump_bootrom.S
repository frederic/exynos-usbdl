.text
.global _start

_start:
	mov	w1, #0x20000 // size
	mov w0, #0x0 // address
	bl usb_send
    mov x0, #0
    br x0 //reset

usb_send:
	stp	x29, x30, [sp,#-48]!
	mov	w3, #0x0
	bfxil	w3, w1, #0, #24
	mov	w1, #0xc12
	mov	x29, sp
	stp	x19, x20, [sp,#16]
	mov	x5, #0xc834
	mov	w20, #0x1
	movk	x5, #0x1540, lsl #16
	ldr	x2, [x29,#40]
	mov	x4, #0xc838
	orr	w6, w1, w20
	movk	x4, #0x1540, lsl #16
	mov	x19, #0xc83c
	movk	x19, #0x1540, lsl #16
	stp	w3, w1, [x2,#8]
	mov	w3, #0x406
	stp	w0, wzr, [x2]
	mov	w0, w20
	ldr	x1, [x29,#40]
	strb	w6, [x2,#12]
	mov	x2, #0x27c8
	str	w1, [x5]
	mov	w1, #0x1388
	str	wzr, [x4]
	str	w3, [x19]
	blr	x2
	mov	w0, w20
	ldr	w1, [x19]
	ldp	x19, x20, [sp,#16]
	ldp	x29, x30, [sp],#48
	ret
