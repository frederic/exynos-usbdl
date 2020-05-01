.text
.global _start

_start:
	mov w1, #5000 // ~ ms
	mov w0, #0x1
	mov x2, #0
	ldr x2, _sleep
	ldr w2, [x2]
	blr x2
    mov x0, #0
    br x0

_sleep: .dword 0x020200f4