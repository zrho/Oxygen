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

#include <api/types.h>
#include <api/cpu.h>
#include <api/memory/heap.h>
#include <amd64/cpu.h>
#include <amd64/cpu/tss.h>

//----------------------------------------------------------------------------//
// TSS - External
//----------------------------------------------------------------------------//

/**
 * Reloads the GDT.
 */
extern void cpu_gdt_reload();

/**
 * Location of the GDT pointer.
 */
extern uint8_t gdt_pointer;

//----------------------------------------------------------------------------//
// TSS - Macros
//----------------------------------------------------------------------------//

#define CPU_TSS_OFFSET(id) (id * 0x10 + 0x28)

//----------------------------------------------------------------------------//
// TSS
//----------------------------------------------------------------------------//

void cpu_tss_setup()
{
    // Iterate over all CPUs and prepare TSS and segment descriptors in the GDT
    // for each of them
    cpu_t *cpu = cpu_get_first();
    
    while (0 != cpu) {
        // Create tss
        cpu_tss_t *tss = (cpu_tss_t *) malloc(sizeof(cpu_tss_t));
        
        // TODO: Definately find out why waiting is required here!
        size_t i;
        for (i = 0; i < 0xFFF; ++i);
        
        memset((void *) tss, 0, sizeof(cpu_tss_t));
        uintptr_t tss_addr = (uintptr_t) tss;
        
        tss->esp0 = cpu->kernel_stack;
        tss->ss0 = 0x10;
        tss->cs = 0x0B;
        tss->ss = tss->ds = tss->es = tss->fs = tss->gs = 0x13;
        
        // Calculate offset in GDT
        uint16_t offset = CPU_TSS_OFFSET(cpu->id);
        
        // Create System Segment Descriptor in GDT
        cpu_tss_ptr_t *ptr = (cpu_tss_ptr_t *) (CPU_GDT_VIRTUAL + offset);
        memset((void *) ptr, 0, sizeof(cpu_tss_ptr_t));
        
        // Set tss address as base
        ptr->baseLow = tss_addr & 0xFFFF;
        ptr->baseMiddleLow = (tss_addr >> 16) & 0xFF;
        ptr->baseMiddleHigh = (tss_addr >> 24) & 0xFF;
        ptr->baseHigh = (tss_addr >> 32) & 0xFFFFFFFF;
        
        // Set limit
        ptr->limitLow = sizeof(cpu_tss_t);
        
        // Set flags
        ptr->flags |=
            0x09 |      // TSS
            (1 << 7) |  // Present
            (3 << 5);   // Ring 3
            
        // Next CPU
        cpu = cpu->next;
    }
    
    // Increase the GDT's size
    *((uint16_t *) &gdt_pointer) = 0x28 + cpu_count() * 0x10 - 1;
}

void cpu_tss_load()
{
    // Reload GDT
    cpu_gdt_reload();
    
    // Load TSS
    asm volatile("ltr %%ax" :: "a" (CPU_TSS_OFFSET(cpu_current_id()) | 0x3));
}

cpu_tss_t *cpu_tss_get(cpu_id_t cpu)
{
    // Get system segment descriptor
    cpu_tss_ptr_t *ptr = (cpu_tss_ptr_t *)
        (CPU_GDT_VIRTUAL + CPU_TSS_OFFSET(cpu_current_id()));
        
    // Extract base
    uintptr_t base =
        ptr->baseLow |
        (((uintptr_t) ptr->baseMiddleLow) << 16) |
        (((uintptr_t) ptr->baseMiddleHigh) << 24) |
        (((uintptr_t) ptr->baseHigh) << 32);
        
    // Return pointer to TSS
    return (cpu_tss_t *) base;
}
