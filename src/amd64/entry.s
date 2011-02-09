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
[BITS 64]

;-------------------------------------------------------------------------------
; Entry point
;-------------------------------------------------------------------------------
[EXTERN main]
[GLOBAL entry]

section .text

entry:
    ; Setup stack
    mov rsp, stack_end
    
    ; Load GDT
    mov rdi, gdt_pointer
    lgdt [rdi]
    
    ; Call main
    call main
    
    ; Endless loop
    jmp $

;-------------------------------------------------------------------------------
; GDT
;-------------------------------------------------------------------------------
section .data

gdt_pointer:
    dw 0x27
    dq 0xFFFFFF7FFFFFD000

;-------------------------------------------------------------------------------
; Stack
;-------------------------------------------------------------------------------
[GLOBAL stack_begin]
[GLOBAL stack_end]

section .bss

align 0x1000
stack_begin:
    resb 0x1000  ; Reserve 4 KiB
stack_end:
