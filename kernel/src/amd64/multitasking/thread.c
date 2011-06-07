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

#include <api/cpu.h>
#include <api/cpu/ipi.h>

#include <api/memory/heap.h>
#include <api/memory/page.h>

#include <api/multitasking.h>
#include <api/multitasking/scheduler.h>
#include <api/multitasking/thread.h>
#include <api/multitasking/process.h>
#include <api/multitasking/stack.h>

#include <amd64/cpu/int.h>

//----------------------------------------------------------------------------//
// Stack - Definitions
//----------------------------------------------------------------------------//

#define THREAD_STACK_SIZE_MIN      0x1000
#define THREAD_STACK_SIZE_MAX      0x200000
#define THREAD_STACK_ADDR_BEGIN    0x7F8000000000

//----------------------------------------------------------------------------//
// Thread - Variables
//----------------------------------------------------------------------------//

/**
 * Default thread priority.
 */
static thread_priority_t _thread_priority_default = 128;

//----------------------------------------------------------------------------//
// Thread - Internal
//----------------------------------------------------------------------------//

/**
 * Creates a stack for the given thread.
 *
 * @param thread The thread to create a stack for.
 * @param process The process hosting the thread.
 */
static void _thread_stack_create(thread_t *thread, process_t *proc)
{
    // Find first free stack frame
    uintptr_t frame = THREAD_STACK_ADDR_BEGIN + THREAD_STACK_SIZE_MAX;
    bool _free = true;
    
    do {
        thread_t *current = proc->threads;
        while (0 != current) {
            // Frame blocked?
            if (current->id != thread->id &&
                frame == current->stack.addr) {
                
                _free = false;
                frame += THREAD_STACK_SIZE_MAX;
                break;
            }
            
            // Next
            current = current->next_proc;
        }
    } while (!_free);
    
    // Assign stack frame
    thread->stack.addr = frame;
    thread->stack.size = 0;
    
    // Expand stack to minimum size
    stack_resize(THREAD_STACK_SIZE_MIN, &thread->stack);
}

/**
 * Does the actual work of stopping a thread.
 *
 * @param thread The thread to stop.
 * @param proc The process the thread belongs to.
 */
static void _thread_stop(thread_t *thread, process_t *proc)
{
    // Dispose stack
    stack_dispose(&thread->stack);
    
    // Remove from process
    thread_t *current = proc->threads;
    thread_t *previous = 0; 
    
    while (0 != current) {
        // Thread found?
        if (thread->id != current->id) {
            previous = current;
            current = current->next_proc;
            continue;
        }
        
        // Remove from list
        if (0 == previous)
            proc->threads = thread->next_proc;
        else
            previous->next_proc = thread->next_proc;
    }
    
    // Free structure
    free(thread);
    
    // Last thread?
    if (0 == proc->threads)
        process_terminate(proc);
}

//----------------------------------------------------------------------------//
// Thread
//----------------------------------------------------------------------------//

void thread_switch(thread_t *thread, process_t *proc, void *_ctx)
{
    // Current contex
    cpu_int_state_t *ctx = (cpu_int_state_t *) _ctx;

    // Get current CPU
    cpu_t *cpu = cpu_get(cpu_current_id());
    
    // Different thread executing?
    if (0 != cpu->thread && thread->id != cpu->thread->id) {
        // Save current state
        memcpy(cpu->thread->pause_ctx, ctx, sizeof(cpu_int_state_t));
        cpu->thread->instr_ptr = ctx->ip;
        
        // Return to scheduler
        scheduler_return(cpu->thread);
        
        // Marked for termination?
        if (0 != (cpu->thread->flags & THREAD_FLAG_TERMINATE))
            _thread_stop(cpu->thread, cpu->process);
    }
    
    // Address space switch, if required
    if (0 == cpu->process || proc->id != cpu->process->id)
        page_switch_space(proc->address_space);
    
    // Set new thread and process
    cpu->thread = thread;
    cpu->process = proc;
    
    // TODO: New TTL
    
    // Pause context given?
    if (0 != thread->pause_ctx)
        // Continue with pause context
        memcpy(ctx, thread->pause_ctx, sizeof(cpu_int_state_t));
    else {
        // Create new context
        thread->pause_ctx = (cpu_int_state_t *) malloc(sizeof(cpu_int_state_t));
        memset(thread->pause_ctx, 0, sizeof(cpu_int_state_t));
        
        ctx->ip = thread->instr_ptr;
        ctx->bp = ctx->sp = thread->stack.addr;
    }
}

thread_t *thread_tick(void *context)
{
    // Get current cpu
    cpu_t *cpu = cpu_get(cpu_current_id());
    
    // No thread executing or TTL elapsed?
    if (0 == cpu->thread || 0 == cpu->thread->ttl) {
        // Schedule new thread
        thread_t *thread = scheduler_next();
        
        // New thread scheduled?
        if (0 != thread)
            thread_switch(thread, process_get(thread->process_id), context);
        
        return thread;
    }
    
    return cpu->thread;
}

//----------------------------------------------------------------------------//
// Thread - Lifecycle
//----------------------------------------------------------------------------//

thread_t *thread_create(process_t *proc, uintptr_t entry_point)
{
    // Create thread structure
    thread_t *thread = (thread_t *) malloc(sizeof(thread_t));
    thread->id = proc->thread_next_id++;
    thread->process_id = proc->id;
    thread->flags = 0;
    thread->priority = _thread_priority_default;
    thread->ttl = 0;
    thread->instr_ptr = entry_point;
    thread->pause_ctx = 0;
    thread->next_sched = 0;
    
    // Create stack
    _thread_stack_create(thread, proc);
    
    // Insert into list
    thread->next_proc = proc->threads;
    proc->threads = thread;
    
    return thread;
}

void thread_stop(thread_t *thread)
{
    // Set termination flag
    thread->flags |= THREAD_FLAG_TERMINATE;

    // Remove from scheduler
    scheduler_remove(thread);

    // Find the CPU running the thread, if any
    cpu_t *cpu = cpu_get_first();
    while (0 != cpu) {
        // Found?
        if (0 != cpu->thread && thread->id == cpu->thread->id)
            break;
            
        // Next
        cpu = cpu->next;
    }
    
    // CPU found?
    if (0 != cpu && cpu->id != cpu_current_id()) {
        // Send IPI
        cpu_ipi_single(0x31, cpu->id); // TODO: Vector as a constant
        return;
    }
    
    // Find hosting process
    process_t *proc = process_get(thread->process_id);
    
    // Stop the thread
    _thread_stop(thread, proc);
}

//----------------------------------------------------------------------------//
// Thread - Priorities
//----------------------------------------------------------------------------//

thread_priority_t thread_default_priority_get(void)
{
    return _thread_priority_default;
}

void thread_default_priority_set(thread_priority_t pri)
{
    _thread_priority_default = pri;
}
