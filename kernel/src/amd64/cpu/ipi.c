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
#include <api/cpu/ipi.h>
#include <amd64/cpu/ipi.h>
#include <amd64/cpu/lapic.h>

//----------------------------------------------------------------------------//
// IPI
//----------------------------------------------------------------------------//

void cpu_ipi(
    uint8_t vector, uint8_t dest, uint8_t shorthand,
    uint8_t mode, uint8_t delivery, uint8_t level, cpu_t *cpu)
{
    // Lock CPU
    spinlock_acquire(&cpu->lock);

    // Get ICR address
    uintptr_t icr = (uintptr_t) LAPIC_REGISTER(LAPIC_ICR_OFFSET);
    
    // Lower 32 bits
    uint32_t lower = 
        vector |                    // 0-7      (Vector)
        ((delivery & 0x7) << 8) |   // 8-10     (Delivery Mode)
        ((mode & 0x1) << 11) |      // 11       (Destination Mode)
        ((level & 0x1) << 14) |     // 14       (Level)
        ((shorthand & 0x3) << 18);  // 18-19    (Destination Shorthand)
        
    // Upper 32 bits
    uint32_t upper =
        dest << (56 - 32);                 // 56-63    (Destination)
    
    // Write ICR
    *((uint32_t *) (icr + 0x10)) = upper; // Be sure to write upper 32 bits first!
    *((uint32_t *) icr) = lower;
    
    // Release lock
    spinlock_release(&cpu->lock);
}

void cpu_ipi_startup(uint16_t addr, cpu_id_t cpu)
{
    cpu_ipi(
        addr / 0x1000,              // Vector
        cpu,                        // Destination
        IPI_DEST_DEST_FIELD,        // Destination shorthand
        IPI_MODE_PHYSICAL,          // Destination mode
        IPI_DELIVERY_STARTUP,       // Delivery mode
        IPI_LEVEL_ASSERT,           // Level
        cpu_get(cpu_current_id())); // Current CPI
}

void cpu_ipi_broadcast(interrupt_vector_t vector, bool incl_self)
{
    // Destination shorthand
    uint8_t shorthand = (incl_self)
        ? IPI_DEST_ALL_INC_SELF
        : IPI_DEST_ALL_EX_SELF;

    // Send IPI
    cpu_ipi(
        vector,                     // Vector
        0,                          // Destination
        shorthand,                  // Destination shorthand
        IPI_MODE_PHYSICAL,          // Destination mode
        IPI_DELIVERY_FIXED,         // Delivery mode
        IPI_LEVEL_ASSERT,           // Level
        cpu_get(cpu_current_id())); // Current CPU        
}

void cpu_ipi_single(interrupt_vector_t vector, cpu_id_t cpu)
{
    // Send IPI
    cpu_ipi(
        vector,                     // Vector
        cpu,                        // Destination
        IPI_DEST_DEST_FIELD,        // Destination shorthand
        IPI_MODE_PHYSICAL,          // Destination mode
        IPI_DELIVERY_FIXED,         // Delivery mode
        IPI_LEVEL_ASSERT,           // Level
        cpu_get(cpu_current_id())); // Current CPU
}
