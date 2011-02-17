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
; Handlers
;-------------------------------------------------------------------------------

; Macro for non-error interrupts
; Pushes a fake error code (zero) to create a stack of equal length
%macro INT_NOERRCODE 1
    _cpu_int_handler%1:
        cli
        push qword 0
        push qword %1
        jmp _cpu_int_handler_common
%endmacro

; Macro for error-code interrupts
%macro INT_ERRCODE 1
    _cpu_int_handler%1:
        cli
        push qword %1
        jmp _cpu_int_handler_common
%endmacro

; Exception interrupts
INT_NOERRCODE 0
INT_NOERRCODE 1
INT_NOERRCODE 2
INT_NOERRCODE 3
INT_NOERRCODE 4
INT_NOERRCODE 5
INT_NOERRCODE 6
INT_NOERRCODE 7
INT_ERRCODE   8
INT_NOERRCODE 9
INT_ERRCODE   10
INT_ERRCODE   11
INT_ERRCODE   12
INT_ERRCODE   13
INT_ERRCODE   14
INT_NOERRCODE 15
INT_NOERRCODE 16
INT_NOERRCODE 17
INT_NOERRCODE 18
INT_NOERRCODE 19
INT_NOERRCODE 20
INT_NOERRCODE 21
INT_NOERRCODE 22
INT_NOERRCODE 23
INT_NOERRCODE 24
INT_NOERRCODE 25
INT_NOERRCODE 26
INT_NOERRCODE 27
INT_NOERRCODE 28
INT_NOERRCODE 29
INT_NOERRCODE 30
INT_NOERRCODE 31

; Remaining
%assign i 32
%rep 224
  INT_NOERRCODE i
  %assign i i+1
%endrep

;-------------------------------------------------------------------------------
; Common Handler
;-------------------------------------------------------------------------------

; Interrupt handler in C++ code
[EXTERN _cpu_int_handler]

; Common interrupt handler
_cpu_int_handler_common:
    push rdi
    push rsi
    push rbp
    push rsp
    push rbx
    push rdx
    push rcx
    push rax

    xor rax, rax
    mov ax, es              ; Lower 16-bits of eax = ds.
    push rax                ; save the data segment descriptor

    ;mov ax, 0x10            ; load the kernel data segment descriptor
    ;mov ds, ax
    ;mov es, ax
    ;mov fs, ax
    ;mov gs, ax

    call _cpu_int_handler

    pop rbx                 ; Reload the original data segment descriptor
    ;mov ds, bx
    ;mov es, bx
    ;mov fs, bx
    ;mov gs, bx

    pop rax
    pop rcx
    pop rdx
    pop rbx
    pop rsp
    pop rbp
    pop rsi
    pop rdi
    
    add rsp, 16             ; Cleans up the pushed error code and pushed ISR numbers
    iretq                   ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
    
;-------------------------------------------------------------------------------
; Handler Addresses
;-------------------------------------------------------------------------------
section .data

; Array with all interrupt handlers
[GLOBAL _cpu_int_handlers]
_cpu_int_handlers:
  %assign i 0
  %rep 256
    dq _cpu_int_handler %+ i
    %assign i i+1
  %endrep
