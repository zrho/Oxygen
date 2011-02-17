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

//------------------------------------------------------------------------------
// CPU - Types
//------------------------------------------------------------------------------

/**
 * The id of a CPU.
 */
typedef uint8_t cpu_id_t;

//------------------------------------------------------------------------------
// CPU - Flags
//------------------------------------------------------------------------------

/**
 * CPU Flag (Value 1).
 *
 * Whether the CPU is enabled. Disabled CPUs can and should NOT be used. Not
 * equal to halting/sleeping state.
 */
#define CPU_FLAG_ENABLED    (1 << 0)

/**
 * CPU Flag (Value 2).
 *
 * Whether the CPU is the BootStrap Processor, i.e. the processor that started
 * the system.
 */
#define CPU_FLAG_BSP        (1 << 0)

//------------------------------------------------------------------------------
// CPU - Structures
//------------------------------------------------------------------------------

/**
 * Structure representing a CPU installed into the system.
 */
typedef struct cpu_t
{
    /**
     * The CPU's id.
     */
    cpu_id_t id;
    
    /**
     * The CPU's flags.
     */
    uint8_t flags;
    
    /**
     * Pointer to the next CPU.
     */
    struct cpu_t *next;
    
} cpu_t;

//------------------------------------------------------------------------------
// CPU - API
//------------------------------------------------------------------------------

/**
 * Returns a pointer to a CPU, given its id.
 *
 * @param id The id of the CPU.
 * @return Pointer to the CPU or a null-pointer.
 */
cpu_t *cpu_get(cpu_id_t id);

/**
 * Returns a pointer to the first CPU in the linked list of CPUs.
 *
 * @return Pointer to the first CPU.
 */
cpu_t *cpu_get_first();

/**
 * Returns the number of CPUs installed into the system.
 *
 * @return Number of CPUs.
 */
size_t cpu_count();
