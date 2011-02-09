/**
 * Oxygen Operating System
 * Copyright (C) 2011 Lukas Heidemann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <api/types.h>
#include <amd64/cpu.h>

//----------------------------------------------------------------------------//
// Modification
//----------------------------------------------------------------------------//

void cpu_gdt_gate_set(
    cpu_gdt_gate_t *gate, uint64_t base, uint32_t limit, uint8_t flags, uint8_t flags2)
{
    gate->limitLow = limit & 0xFFFF;
    gate->baseLow = base & 0xFFFF;
    gate->baseMiddle = (base >> 16) & 0xFF;
    gate->flags = flags;
    gate->flags_limitHigh = ((flags2 & 0x0F) << 4) | ((limit >> 16) & 0x0F);
    gate->baseHigh = (base >> 24) & 0xFF;
}

//----------------------------------------------------------------------------//
// Initialization
//----------------------------------------------------------------------------//

extern cpu_gdt_gate_t _cpu_gdt_gates;
extern cpu_gdt_pointer_t _cpu_gdt_pointer;
extern void _cpu_gdt_flush(uintptr_t ptrAddress);

void cpu_gdt_init()
{
    // Write gates
    cpu_gdt_gate_t *gates = &_cpu_gdt_gates;
    
    cpu_gdt_gate_set(&gates[0], 0x00, 0x00, 0x00, 0x00); // Null
    cpu_gdt_gate_set(&gates[1], 0x00, 0x00, 0x98, 0x02); // Kernel code
    cpu_gdt_gate_set(&gates[2], 0x00, 0x00, 0x92, 0x02); // Kernel data
    cpu_gdt_gate_set(&gates[3], 0x00, 0x00, 0xF8, 0x02); // User code
    cpu_gdt_gate_set(&gates[4], 0x00, 0x00, 0xF2, 0x02); // User data
    
    // Write pointer
    cpu_gdt_pointer_t *pointer = &_cpu_gdt_pointer;
    pointer->limit = 5 * 0x8 - 1;
    pointer->base = (uint64_t) &_cpu_gdt_gates;
    
    // Flush GDT
    _cpu_gdt_flush((uintptr_t) &pointer);
}
