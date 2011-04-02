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

//----------------------------------------------------------------------------//
// TSS - Structures
//----------------------------------------------------------------------------//

/**
 * The structre of the Task Segment Selector.
 *
 * Normally used for hardware multitasking, on X86 and AMD64 at least one TSS
 * per core is required for switching the stack when entering the kernel.
 */
typedef struct cpu_tss_t
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} cpu_tss_t;

/**
 * Structure for System-Segment Descriptors used in the GDT to link in the
 * platform's TSSs.
 */
typedef struct cpu_tss_ptr_t
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
     * Middle low byte of the base address.
     */
    uint8_t baseMiddleLow;
    
    /**
     * Flags.
     */
    uint8_t flags;
    
    /**
     * Additional flags and highest 4 bits of the limit.
     */
    uint8_t flags_limitHigh;
    
    /**
     * Middle high byte of the base address.
     */
    uint8_t baseMiddleHigh;
    
    /**
     * Highest dword of the base address.
     */
    uint32_t baseHigh;
    
    /**
     * Reserved fields.
     */
    uint32_t reserved;
    
} PACKED cpu_tss_ptr_t;

//----------------------------------------------------------------------------//
// TSS
//----------------------------------------------------------------------------//

/**
 * Sets up TSSs for all CPUs installed into the system.
 */
void cpu_tss_setup();

/**
 * Loads the TSS for the current CPU.
 */
void cpu_tss_load();

/**
 * Returns the TSS for the CPU with the given id.
 */
cpu_tss_t *cpu_tss_get(cpu_id_t cpu);
