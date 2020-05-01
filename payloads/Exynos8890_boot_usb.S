.text
.global _start

_start:
    mov x0, #0
    mov x30, #0
    ldr w30, _jmp_to_bl1 // set LR
    ldr w0, _boot_usb
    br x0 // must be last instruction since this call will overwrite us

_jmp_to_bl1: .dword 0x02021810
_boot_usb: .dword 0x000064e0
