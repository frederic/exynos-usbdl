.text
.global _start

_start:
    ldr x30, _jmp_to_bl1 // set LR
    ldr x0, _boot_sdmmc
    br x0 // must be last instruction since this call will overwrite us

_jmp_to_bl1: .dword 0x02021810
_boot_sdmmc: .dword 0x0000b910
