; Oxygen Operating System
; Copyright (C) 2010 Lukas Heidemann
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;-------------------------------------------------------------------------------
; Constants
;-------------------------------------------------------------------------------

TRAMPOLINE_GDT32_OFFSET         equ         0x1C14
TRAMPOLINE_GDT32_PTR_OFFSET     equ         0x1C0E
TRAMPOLINE_ENTRY_POINT_OFFSET   equ         0x1C06
TRAMPOLINE_GDT64_PTR_OFFSET     equ         0x1C00

TRAMPOLINE_PROTECTED_OFFSET     equ         0x1400
TRAMPOLINE_LONG_OFFSET          equ         0x1800

;-------------------------------------------------------------------------------
; Real mode
;-------------------------------------------------------------------------------
[BITS 16]
section .cpu_smp_trampoline

; Sets up protected mode and jumps to trampoline_protected
trampoline_real:
    cli
    xor ax, ax
    mov ds, ax
    jmp 0x0000:0x1200
    
align 0x200
trampoline_real2:
    ; Enable A20 gate
    ENABLE_A20
    
    ; Load GDT
    mov edi, TRAMPOLINE_GDT32_PTR_OFFSET
    lgdt [edi]
    
    mov eax, cr0
    or eax, (1 << 0)
    mov cr0, eax
    jmp 0x08:TRAMPOLINE_PROTECTED_OFFSET

;-------------------------------------------------------------------------------
; Protected mode
;-------------------------------------------------------------------------------
[BITS 32]
%include "amd64/boot/long.s"

; Sets up long mode and jumps to trampoline_long
align 0x400
trampoline_protected:
    ; Enable PAE
    ENABLE_PAE

    ; Enable long mode
    ENABLE_LONG_MODE
    
    ; Write PML4 address to CR3 (assumes the PML4 is stored in 0x2000)
    mov ebx, 0x2000
    mov cr3, ebx
    
    ; Enable paging with PG bit in CR0
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ; Load the GDT
    lgdt [TRAMPOLINE_GDT64_PTR_OFFSET]
    
    ; Far jump to long mode code
    jmp 0x08:0x1800
        
;-------------------------------------------------------------------------------
; Long mode
;-------------------------------------------------------------------------------    
[BITS 64]
align 0x400
trampoline_long:
    mov rdi, TRAMPOLINE_ENTRY_POINT_OFFSET
    mov rax, [rdi]
    jmp rax
    
;-------------------------------------------------------------------------------
; Data
;-------------------------------------------------------------------------------    
align 0x400

; Pointer to 64 bit GDT, filled in by kernel
; Offset: 0
[GLOBAL trampoline_gdt64_ptr]
trampoline_gdt64_ptr:
    dw 0x28
    dd 0x0
    
; Entry point address, filled in by kernel
; Offset: 6
[GLOBAL trampoline_entry_point]
trampoline_entry_point:
    dq 0x0
    
; The pointer to the 32 bit GDT
; Offset: 14
trampoline_gdt32_ptr:
    dw 0x18                         ; Length of three segments
    dd TRAMPOLINE_GDT32_OFFSET      ; Address of GDT

; The GDT for 32 bit startup code
; Offset: 20
trampoline_gdt32:
    ; Null
    dd 0x0
    dd 0x0
    
    ; Code
    dw 0xFFFF
    dw 0x0000
    dw 0x9800
    dw 0x00CF

    ; Data
    dw 0xFFFF
    dw 0x0000
    dw 0x9200
    dw 0x00CF
    
    ;dw 0xFFFF                       ; Limit low
    ;dw 0x0                          ; Base low
    ;db 0x0                          ; Base mid
    ;db 0x9A                         ; Access
    ;db 0xCF                         ; Limit High + Flags
    ;db 0x0                          ; Base high
    
    ; Data
    ;dw 0xFFFF                       ; Limit low
    ;dw 0x0                          ; Base low
    ;db 0x0                          ; Base mid
    ;db 0x92                         ; Access
    ;db 0xCF                         ; Limit High + Flags
    ;db 0x0                          ; Base high
