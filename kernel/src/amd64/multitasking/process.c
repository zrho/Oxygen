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

#include <api/memory/heap.h>
#include <api/memory/page.h>

#include <api/sync/spinlock.h>

#include <api/multitasking.h>
#include <api/multitasking/scheduler.h>
#include <api/multitasking/process.h>
#include <api/multitasking/thread.h>

//----------------------------------------------------------------------------//
// Process - Variables
//----------------------------------------------------------------------------//

/**
 * Pointer to first process in the linked list.
 */
static process_t *_process_first = 0;

/**
 * Pointer to last process in the linked list.
 */
static process_t *_process_last = 0;

/**
 * Lock for process operations.
 */
static SPINLOCK_INIT(_process_lock);

/**
 * The next process id to assign.
 */
static process_id_t _process_id_next = 0;

/**
 * The default process priority.
 */
static process_priority_t _process_priority_default = 128;

//----------------------------------------------------------------------------//
// Process
//----------------------------------------------------------------------------//

process_t *process_get(process_id_t id)
{
    // Try to find process
    process_t *current = _process_first;
    while (0 != current) {
        if (id == current->id)
            break;
            
        current = current->next;
    }
    
    // Return the found process or a null-pointer
    return current;
}

process_t *process_first(void)
{
    return _process_first;
}

size_t process_count(void)
{
    // Count
    size_t c = 0;
    process_t *current = _process_first;
    while (0 != current) {
        ++c;
        current = current->next;
    }
    
    return c;
}

//----------------------------------------------------------------------------//
// Process - Lock
//----------------------------------------------------------------------------//

void process_lock(void)
{
    spinlock_acquire(&_process_lock);
}

void process_unlock(void)
{
    spinlock_release(&_process_lock);
}

//----------------------------------------------------------------------------//
// Process - Lifecycle
//----------------------------------------------------------------------------//

process_t *process_spawn(
    process_t *parent, uintptr_t addr_space, uintptr_t entry_point)
{
    // Create process
    process_t *proc = (process_t *) malloc(sizeof(process_t));
    proc->id = _process_id_next++;
    proc->parent = parent;
    proc->flags = 0;
    proc->priority = _process_priority_default;
    proc->thread_next_id = 1;
    proc->address_space = addr_space;
    proc->next = 0;
    
    // Create thread
    thread_create(proc, entry_point);
    
    // Add process to list
    if (0 == _process_last)
        _process_first = proc;
    else
        _process_last->next = proc;
        
    _process_last = proc;
    
    return proc;
}

void process_terminate(process_t *proc)
{
    // Mark for termination
    bool marked = (0 != (proc->flags & PROCESS_FLAG_TERMINATE));
    proc->flags |= PROCESS_FLAG_TERMINATE;
    
    // Threads remaining?
    if (0 != proc->threads) {
        // When the thread has been marked before, its threads are already about
        // to terminate
        if (marked) return;
        
        // Terminate threads
        thread_t *thread = proc->threads;
        while (0 != thread) {
            // Terminate
            if (0 == (thread->flags & THREAD_FLAG_TERMINATE))
                thread_stop(thread);
                
            // Next
            thread = thread->next_proc;
        }
        
        // Real termination happens on termination of the last thread
        return;
    }
    
    // TODO: Close IPC channels
    
    // Mark children for termination
    process_t *current = _process_first;
    while (0 != current) {
        // Is child process?
        if (0 != current->parent && current->parent->id == proc->id)
            process_terminate(proc);
            
        // Next
        current = current->next;
    }
    
    // Is root process?
    if (0 == proc->id) {
        // TODO: Shutdown system
    } else {
        // Switch to process' address space
        page_switch_space(proc->address_space);
        
        // Dispose address space, switching to root kernel space
        page_dispose_space();
        
        // Remove process structure
        free(proc);
    }
}
    
//----------------------------------------------------------------------------//
// Process - Priorities
//----------------------------------------------------------------------------//
    
process_priority_t process_default_priority_get(void)
{
    return _process_priority_default;
}

void process_default_priority_set(process_priority_t pri)
{
    _process_priority_default = pri;
}
