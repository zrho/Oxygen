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
%include "amd64/boot/long.s"

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
    ENABLE_A20

    ; Enable PAE
    ENABLE_PAE

    ; Enable long mode
    ENABLE_LONG_MODE

    ; Call C initialization code
    call init
    
    ; Write PML4 address to CR3
    mov ebx, 0x2000
    mov cr3, ebx
    
    ; Enable paging with PG bit in CR0
    mov eax, cr0
    or eax, 1 << 31 | 1 << 0    ; PG and PM
    mov cr0, eax
    
    ; Loads the GDT
    call load_gdt

    ; Perform a far jump to a 64 bit realm that does the actual jump to the
    ; kernel's entry point
    ;jmp $
    jmp 0x08:realm64

; ------------------------------------------------------------------------------
; Global Descriptor Table
; ------------------------------------------------------------------------------

; Loads the GDT
load_gdt:
    lgdt [_cpu_gdt_pointer]
    ret

section .data

[GLOBAL _cpu_gdt_gates]
[GLOBAL _cpu_gdt_pointer]

align 4096

; GDT for 64bit long mode
_cpu_gdt_gates:
    .Null:
        dd 0x0
        dd 0x0
    .KernelCode:
        dd 0x0
        dd 0x0
    .KernelData:
        dd 0x0
        dd 0x0
    .UserCode:
        dd 0x0
        dd 0x0
    .UserData:
        dd 0x0
        dd 0x0
    
_cpu_gdt_pointer:
    dw 0x0
    dd 0x0
    dd 0x0
