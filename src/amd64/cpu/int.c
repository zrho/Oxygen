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
#include <api/string.h>

#include <api/cpu/int.h>

#include <api/memory/page.h>

#include <api/debug/console.h>

#include <amd64/cpu/int.h>

//----------------------------------------------------------------------------//
// Variables
//----------------------------------------------------------------------------//

cpu_int_entry_t cpu_int_idt[255];
uintptr_t cpu_int_handlers[255];

//----------------------------------------------------------------------------//
// Internal
//----------------------------------------------------------------------------//

/**
 * Writes the given IDT pointer to the IDT register.
 *
 * @param pointer IDT pointer to write.
 */
static void _cpu_int_flush(cpu_int_pointer_t pointer)
{
    //return;
    asm volatile ("lidt %0" : : "m" (pointer));
}

//----------------------------------------------------------------------------//
// Interrupts
//----------------------------------------------------------------------------//

/**
 * Interrupt handlers defined in ASM.
 */
extern uintptr_t _cpu_int_handlers[256];

void cpu_int_init()
{
    // Setup pointer
    cpu_int_pointer_t pointer;
    pointer.offset = (uintptr_t) &cpu_int_idt;
    pointer.limit = (uint16_t) (256 * sizeof(cpu_int_entry_t) - 1);
    
    // Clear handler function table
    memset((void *) &cpu_int_handlers, 0, sizeof(uintptr_t) * 256);
    
    // Setup table
    memset((void *) &cpu_int_idt, 0, sizeof(cpu_int_entry_t) * 256);
    
    size_t vector;
    for (vector = 0; vector < 255; ++vector) {
        uintptr_t offset = _cpu_int_handlers[vector];
        cpu_int_entry_t *entry = &cpu_int_idt[vector];
        
        entry->offsetLow = (uint16_t) offset;
        entry->offsetMiddle = (uint16_t) (offset >> 16);
        entry->offsetHigh = (uint32_t) (offset >> 32);
        entry->zero0 = entry->zero1 = 0;
        entry->cs = 0x08;
        entry->flags = 0x8E;
    }
    
    // Load idt
    _cpu_int_flush(pointer);
}

void cpu_int_register(interrupt_vector_t vector, interrupt_handler_t handler)
{
    cpu_int_handlers[vector] = (uintptr_t) handler;
}

void _cpu_int_handler(cpu_int_state_t regs)
{
    // Handler for vector?
    interrupt_vector_t vector = regs.vector;
    
    if (0 == cpu_int_handlers[vector])
        return;
        
    // Call handler
    interrupt_handler_t handler = (interrupt_handler_t) cpu_int_handlers[vector];
    (*handler)(vector, (void *) &regs);
}

//----------------------------------------------------------------------------//
// Interruptable
//----------------------------------------------------------------------------//

bool cpu_is_interruptable()
{
    // Check IF in flags
    size_t flag;
    
    asm volatile (
        "pushf\n"
        "pop %0\n"
        "and $0x200, %0\n" : "=r" (flag));
        
    return (flag != 0);
}

void cpu_set_interruptable(bool flag)
{
    if (flag)
        asm volatile ("sti");
    else
        asm volatile ("cli");
}
