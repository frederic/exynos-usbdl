.text
.global _start

_start:
	mov w1, #5000 // ~ ms
	mov w0, #0x1
	ldr x3, _sleep
	blr x3
    mov x0, #0
    br x0

_sleep: .dword 0x000027c8