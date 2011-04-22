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

#include <api/multitasking.h>
#include <api/multitasking/scheduler.h>

//----------------------------------------------------------------------------//
// Scheduler - Internal
//----------------------------------------------------------------------------//

void scheduler_timer_handler(uint64_t ticks, void *_ctx, cpu_t *cpu)
{
    if (0 == (cpu->flags & CPU_FLAG_BSP))
        return;

    /*console_print("Ticks: ");
    console_print_hex(ticks);
    console_print("\n");*/
    
    // Acquire process lock
    process_lock();
    
    // Get current thread
    thread_t *thread = cpu->thread;
    
    if (0 != thread)
        // Decrease lifetime
        if (thread->ttl > 0)
            --thread->ttl;
    
    // Tick
    thread_tick(_ctx);
    
    // TODO: Idle
    
    // Release process lock
    process_unlock();
}
