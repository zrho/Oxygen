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
#include <api/sync/spinlock.h>

#include <api/multitasking/thread.h>
#include <api/multitasking/process.h>

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
#define CPU_FLAG_BSP        (1 << 1)

/** 
 * CPU Flag (Value 4).
 *
 * Whether the CPU has been initialized. Is set by the CPU's startup code.
 */
#define CPU_FLAG_INIT       (1 << 2)

/**
 * CPU Flag (Value 8).
 *
 * Whether the CPU is currently in kernel mode.
 */
#define CPU_FLAG_KERNEL     (1 << 3)

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
     * The CPU's lock.
     */
    spinlock_t lock;
    
    /**
     * The process currently active on this CPU.
     */
    process_t *process;
    
    /**
     * The thread currently active on this CPU.
     */
    thread_t *thread;
    
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
 * Returns the id of the CPU calling this function.
 *
 * @return Current CPU's id.
 */
cpu_id_t cpu_current_id(void);

/**
 * Returns a pointer to the first CPU in the linked list of CPUs.
 *
 * @return Pointer to the first CPU.
 */
cpu_t *cpu_get_first(void);

/**
 * Returns the number of CPUs installed into the system.
 *
 * @return Number of CPUs.
 */
size_t cpu_count(void);
