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

[BITS 32]

; ------------------------------------------------------------------------------
; Multiboot
; ------------------------------------------------------------------------------
section .multiboot

; Constant definitions
MBOOT_PAGE_ALIGN equ 1<<0                                 ; Align loaded modules on
                                                          ; page boundaries
MBOOT_MEMINFO     equ 1<<1                                ; Provide memory map
MBOOT_FLAGS       equ MBOOT_PAGE_ALIGN | MBOOT_MEMINFO    ; Multiboot 'flag' field
MBOOT_MAGIC       equ 0x1BADB002                          ; Magic number used by the
                                                          ; bootloader to find this
MBOOT_CHECKSUM    equ -(MBOOT_MAGIC + MBOOT_FLAGS)        ; Checksum for bootloader

; Export of the header
align 4
MultiBootHeader:
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

; ------------------------------------------------------------------------------
; Entry point
; ------------------------------------------------------------------------------
section .text

[GLOBAL boot]
[EXTERN init]
[EXTERN realm64]

; Called by the multiboot bootloader
; Does some basic setup and transfers control to the boot C code
boot:
    ; Clear interrupts until interrupt handling is set up
    cli

    ; Push multiboot magic number and the pointer to the info structure
    ; on the stack for further usage in the kernel
    push eax    ; Magic number
    push ebx    ; Info structure

    ; Enable A20 gate
    call enable_a20

    ; Enable PAE
    call enable_pae

    ; Enable long mode
    call enable_long_mode

    ; Call C initialization code
    call init
    
    ; Write PML4 address to CR3
    mov ebx, 0x2000
    mov cr3, ebx
    
    ; Enable paging with PG bit in CR0
    mov eax, cr0
    or eax, 1 << 31 | 1 << 0    ; PG and PM
    mov cr0, eax
    
    ; Initialize GDT
    call init_gdt

    ; Perform a far jump to a 64 bit realm that does the actual jump to the
    ; kernel's entry point
    ;jmp $
    jmp GDT64.Code:realm64

; Enables the A20 gate.
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; Sets PAE bit in CR4
enable_pae:
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    ret

; Sets long mode bit in EFER MSR
enable_long_mode:
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    ret
    
; ------------------------------------------------------------------------------
; Global Descriptor Table
; ------------------------------------------------------------------------------
[BITS 32]

; Initializes the GDT
init_gdt:
    lgdt [GDT64.Pointer]
    ret

section .data

; GDT for 64bit long mode
GDT64:
    .Null: equ $ - GDT64    ; Null descriptor
    dw 0                    ; Limit (low)
    dw 0                    ; Base (low)
    db 0                    ; Base (middle)
    db 0                    ; Access
    db 0                    ; Granularity
    db 0                    ; Base (high)
    .Code: equ $ - GDT64    ; Code descriptor
    dw 0
    dw 0
    db 0
    db 10011000b
    db 00100000b
    db 0
    .Data: equ $ - GDT64
    dw 0
    dw 0
    db 0
    db 10010000b
    db 00000000b
    db 0
    .Pointer:
    dw $ - GDT64 - 1
    dd GDT64
