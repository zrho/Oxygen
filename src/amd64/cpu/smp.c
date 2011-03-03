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
 
#include <api/cpu.h>
#include <api/string.h>
#include <api/cpu/int.h>
#include <api/debug/console.h>
#include <amd64/cpu/ipi.h>

//----------------------------------------------------------------------------//
// SMP - Variables
//----------------------------------------------------------------------------//
 
/**
 * Location of SMP trampoline in this binary (virtual).
 */
extern uint8_t cpu_smp_trampoline;

//----------------------------------------------------------------------------//
// SMP - Internal
//----------------------------------------------------------------------------//

/**
 * Entry point APs will jump to after initialization.
 */
static void _cpu_smp_entry_point(void)
{
    // Print message
    console_print("[SMP ] Application Processor ");
    console_print_hex(cpu_current_id());
    console_print(" initialized.\n");
    
    // Set init flag
    cpu_get(cpu_current_id())->flags |= CPU_FLAG_INIT;
    
    // Enable interrupts
    cpu_set_interruptable(true);
    
    // Trap in infinite loop
    while (1);
}
 
//----------------------------------------------------------------------------//
// SMP
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
        // Is AP?
        if (cpu->flags & CPU_FLAG_BSP) {
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
        }
        
        // Next
        cpu = cpu->next;
    }
}
