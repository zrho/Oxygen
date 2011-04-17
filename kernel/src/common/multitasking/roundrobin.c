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
#include <api/multitasking.h>
#include <api/multitasking/scheduler.h>

//----------------------------------------------------------------------------//
// Scheduler - Variables
//----------------------------------------------------------------------------//

static thread_t *_scheduler_queue_first = 0;
static thread_t *_scheduler_queue_last = 0;

//----------------------------------------------------------------------------//
// Scheduler
//----------------------------------------------------------------------------//

thread_t *scheduler_next(void)
{
    // Queue empty?
    if (0 == _scheduler_queue_first)
        return 0;

    // Take top and remove
    thread_t *top = _scheduler_queue_first;
    _scheduler_queue_first = top->sched_next;
    
    if (0 == _scheduler_queue_first)
        _scheduler_queue_last = 0;
        
    return top;
}

void scheduler_return(thread_t *thread)
{
    // Scheduled flag set?
    if (0 == thread->flags & THREAD_FLAG_SCHEDULED)
        return;

    // Insert at the end of the queue
    thread->sched_next = 0;
    
    if (0 == _scheduler_queue_first)
        _scheduler_queue_first = _scheduler_queue_last = thread;
    else {
        _scheduler_queue_last->sched_next = thread;
        _scheduler_queue_last = thread;
    }
}

void scheduler_add(thread_t *thread)
{
    // Set flag
    thread->flags |= THREAD_FLAG_SCHEDULED;
    
    // Insert at the beginning of the queue
    thread->sched_next = _scheduler_queue_first;
    
    if (0 == _scheduler_queue_first)
        _scheduler_queue_first = _scheduler_queue_last = thread;
    else
        _scheduler_queue_first = thread;
}

void scheduler_remove(thread_t *thread)
{
    // Remove flag
    thread->flags & ~THREAD_FLAG_SCHEDULED;
    
    // Remove from queue
    thread_t *current = _scheduler_queue_first;
    thread_t *previous = 0;
    
    while (0 != current) {
        // Thread found?
        if (current->id == thread->id)
            break;
        
        // Next
        previous = current;
        current = current->sched_next;
    }
    
    // Thread found?
    if (0 == current)
        return;
        
    // Remove
    if (0 == previous)
        _scheduler_queue_first = thread->sched_next;
    else
        previous->sched_next = thread->sched_next;
        
    if (0 == _scheduler_queue_first)
        _scheduler_queue_last = 0;
}
