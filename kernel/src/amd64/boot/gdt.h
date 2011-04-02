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
#include <api/compiler.h>

//----------------------------------------------------------------------------//
// GDT - Initialization
//----------------------------------------------------------------------------//

/**
 * Initializes the system's GDT.
 */
void boot_gdt_init(void);

//----------------------------------------------------------------------------//
// GDT - Structures
//----------------------------------------------------------------------------//

typedef struct cpu_gdt_gate_t
{
    /**
     * Lower 2 bytes of the limit.
     */
    uint16_t limitLow;
    
    /**
     * Lower 2 bytes of the base address.
     */
    uint16_t baseLow;
    
    /**
     * Middle byte of the base address.
     */
    uint8_t baseMiddle;
    
    /**
     * Flags.
     */
    uint8_t flags;
    
    /**
     * Additional flags and highest 4 bits of the limit.
     */
    uint8_t flags_limitHigh;
    
    /**
     * Highest byte of the base address.
     */
    uint8_t baseHigh;
    
} PACKED cpu_gdt_gate_t;

typedef struct cpu_gdt_pointer_t
{
    uint16_t limit;
    uint64_t base;
} PACKED cpu_gdt_pointer_t;

//----------------------------------------------------------------------------//
// GDT - Modification
//----------------------------------------------------------------------------//

/**
 * Modifies the given GDT gate, given its new base, limit and flags.
 *
 * @param gate The gate descriptor to modify.
 * @param base The base address.
 * @param limit The limit address.
 * @param flags The gate's flags.
 * @param flags2 The gate's secondary flags.
 */
void cpu_gdt_gate_set(
    cpu_gdt_gate_t *gate, uint64_t base, uint32_t limit, uint8_t flags, uint8_t flags2);
