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
#include <api/sync/spinlock.h>
#include <api/cpu.h>

//----------------------------------------------------------------------------//
// Variables
//----------------------------------------------------------------------------//

static bool spinlock_interrupt_state;

//----------------------------------------------------------------------------//
// Spinlock
//----------------------------------------------------------------------------//

void spinlock_acquire(spinlock_t *lock)
{
    // Check if interrupts are active
    bool interruptsActive = cpu_is_interruptable();
    
    // Stop interrupts
    if (!interruptsActive)
        cpu_set_interruptable(false);
        
    // Try to acquire lock
    while (!__sync_bool_compare_and_swap(lock, false, true));
    
    // Store interrupt state
    spinlock_interrupt_state = interruptsActive;
}

void spinlock_release(spinlock_t *lock)
{
    // Release lock
    if (!__sync_bool_compare_and_swap(lock, true, false))
        return;
        
    // Restore interrupt state
    if (spinlock_interrupt_state)
        cpu_set_interruptable(true);
}
