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
// Interrupt - Vectors
//----------------------------------------------------------------------------//

#define INT_PIC_IRQ_OFFSET          0x40
#define INT_VECTOR_APIC_ERROR       0x21
#define INT_VECTOR_TIMER            0x31
#define INT_VECTOR_TIMER_HELPER     0x32

//----------------------------------------------------------------------------//
// Interrupt - Structures
//----------------------------------------------------------------------------//

typedef struct cpu_int_state_t
{ 
    uint64_t ds;
    uint64_t di, si, bp, sp, bx, dx, cx, ax;
    uint64_t vector;
    uint64_t error_code;
} PACKED cpu_int_state_t;

/**
 * An entry of the Interrupt Descriptor Table.
 */
typedef struct cpu_int_entry_t
{
    /**
     * The lowest 2 bytes of the offset.
     */
    uint16_t offsetLow;
    
    /**
     * The code segment selector.
     */
    uint16_t cs;
    
    /**
     * Always zero.
     */
    uint8_t zero0;
    
    /**
     * Flags and types.
     */
    uint8_t flags;
    
    /**
     * Middle 2 bytes of the offset.
     */
    uint16_t offsetMiddle;
    
    /**
     * Highest 4 bytes of the offset.
     */
    uint32_t offsetHigh;
    
    /**
     * Always zero.
     */
    uint32_t zero1;
    
} PACKED cpu_int_entry_t;

/**
 * Pointer to the Interrupt Descriptor Table.
 */
typedef struct cpu_int_pointer_t
{
    /**
     * Offset of the maximum addressable byte in the table.
     */
    uint16_t limit;
    
    /**
     * (Paged) address of the first byte of the table.
     */
    uint64_t offset;
    
} PACKED cpu_int_pointer_t;

//----------------------------------------------------------------------------//
// Interrupt - Initialization
//----------------------------------------------------------------------------//

/**
 * Sets up interrupt handling.
 */
void cpu_int_init(void);
