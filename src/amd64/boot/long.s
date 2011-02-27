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

; Enables the A20 gate.
%macro ENABLE_A20 0
    in al, 0x92
    or al, 2
    out 0x92, al
%endmacro

; Sets PAE bit in CR4
%macro ENABLE_PAE 0
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
%endmacro

; Sets long mode bit in EFER MSR
%macro ENABLE_LONG_MODE 0
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
%endmacro
