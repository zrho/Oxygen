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
#include <api/compiler.h>
#include <api/cpu.h>
#include <api/string.h>

#include <api/memory/heap.h>
#include <api/memory/page.h>

#include <amd64/cpu.h>
#include <amd64/cpu/int.h>
#include <amd64/cpu/ipi.h>
#include <amd64/cpu/pic.h>
#include <amd64/cpu/lapic.h>
#include <amd64/cpu/timer.h>

#include <amd64/io/io.h>

#include <api/cpu/int.h>

#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// CPU - Variables
//----------------------------------------------------------------------------//

static cpu_t *cpu_first = 0;
static cpu_t *cpu_last = 0;
static size_t cpu_list_len = 0;

/**
 * Location of SMP trampoline in this binary (virtual).
 */
extern uint8_t cpu_smp_trampoline;

//----------------------------------------------------------------------------//
// CPU - Internal
//----------------------------------------------------------------------------//

/**
 * Entry point APs will jump to after initialization.
 */
static void _cpu_smp_entry_point(void)
{    
    // Load IDT
    cpu_int_load();
    
    // Disable PIC
    cpu_pic_disable();
    
    // Enable interrupts
    cpu_set_interruptable(true);
    
    // Enable LAPIC
    cpu_lapic_enable();
    
    // Initialize timer
    cpu_timer_init(false);
    
    // Set init flag
    cpu_get(cpu_current_id())->flags |= CPU_FLAG_INIT;
    
    // Trap in infinite loop
    while (1);
}

//----------------------------------------------------------------------------//
// CPU - API
//----------------------------------------------------------------------------//

void cpu_startup(void)
{
    // Mark current processor as BSP
    cpu_t *bsp = cpu_get(cpu_current_id());
    bsp->flags |= CPU_FLAG_BSP | CPU_FLAG_INIT;
    
    // Initialize PIC
    cpu_pic_init();
    
    // Enable IRQs
    cpu_set_interruptable(true);
    
    // Enable the BSP's LAPIC
    cpu_lapic_enable();
    
    // Initialize timer
    cpu_timer_init(true);
    
    // Disable PIC
    cpu_pic_disable();
}

void cpu_add(cpu_t cpu)
{
    // Increase list length
    ++cpu_list_len;

    // Copy cpu structure
    cpu_t *_cpu = (cpu_t *) malloc(sizeof(cpu_t));
    memcpy(_cpu, &cpu, sizeof(cpu_t));

    // Add to cpu list
    if (0 == cpu_first)
        cpu_first = cpu_last = _cpu;
    else {
        cpu_last->next = _cpu;
        cpu_last = _cpu;
    }
}

cpu_t *cpu_get(cpu_id_t id)
{
    cpu_t *current = cpu_get_first();
    
    while (0 != current) {
        // CPU we're looking for?
        if (id == current->id)
            return current;
            
        // Next one
        current = current->next;
    }
    
    return 0;
}

cpu_t *cpu_get_first(void)
{
    return cpu_first;
}

size_t cpu_count(void)
{
    return cpu_list_len;
}

cpu_id_t cpu_current_id(void)
{
    return (*LAPIC_REGISTER(LAPIC_ID_OFFSET) >> 24) & 0xFF;
}

//----------------------------------------------------------------------------//
// CPU - SMP
//----------------------------------------------------------------------------//

void cpu_smp_init(void)
{
    // Get BSP
    cpu_t *bsp = cpu_get(cpu_current_id());
    
    // Wait some time
    size_t i = 0;
    for (i = 0; i < 0xFFFFF; ++i);

    // Load trampoline code
    memcpy(
        (void *) 0x1000,
        (void *) (uintptr_t) (&cpu_smp_trampoline),
        0x1000);
    
    // GDT
    memcpy(
        (void *) 0x1C00,
        (void *) 0xFFFFFF7FFFFFD028,
        0x6);
        
    // Entry point
    *((uint64_t *) 0x1C06) = (uint64_t) (&_cpu_smp_entry_point);
    
    // Boot APs
    cpu_t *cpu = cpu_get_first();
    
    while (0 != cpu) {
        // Is AP and enabled??
        if (cpu->flags & CPU_FLAG_BSP | !(cpu->flags & CPU_FLAG_ENABLED)) {
            cpu = cpu->next;
            continue;
        }
        
        // Send init ipi
        cpu_ipi(
            0,                          // Vector
            cpu->id,                    // Destination
            IPI_DEST_DEST_FIELD,        // Destination shorthand
            IPI_MODE_PHYSICAL,          // Destination mode
            IPI_DELIVERY_INIT,          // Delivery mode
            IPI_LEVEL_ASSERT,           // Level
            bsp);                       // Current CPI
    
        // Send first startup ipi
        cpu_ipi_startup(0x1000, cpu->id);
        
        // Wait
        for (i = 0; i < 0xFFFFF & !(cpu->flags & CPU_FLAG_INIT); ++i);
        
        // Initialized now?
        if (!(cpu->flags & CPU_FLAG_INIT))
            // Retry startup ipi
            cpu_ipi_startup(0x1000, cpu->id);
            
        // Wait
        for (i = 0; i < 0xFFFFFF & !(cpu->flags & CPU_FLAG_INIT); ++i);
        
        // Intiailized now?
        if (!(cpu->flags & CPU_FLAG_INIT)) {
            console_print("[SMP ] Failed to initialize AP ");
            console_print_hex(cpu->id);
            console_print("\n");
        } else {
            // Print message
            console_print("[SMP ] Application Processor ");
            console_print_hex(cpu->id);
            console_print(" initialized.\n");
        }
        
        // Next
        cpu = cpu->next;
    }
}
