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

#pragma once
#include <api/types.h>

//----------------------------------------------------------------------------//
// Process - Constants
//----------------------------------------------------------------------------//

#define PROCESS_BOOT_ID         0x0

#define THREAD_STATE_SLEEPING   0x0
#define THREAD_STATE_RUNNING    0x1

#define THREAD_PRIORITY_MIN     0x1
#define THREAD_PRIORITY_MAX     0xFF

//----------------------------------------------------------------------------//
// Process - Types
//----------------------------------------------------------------------------//

/**
 * Type for process ids.
 */
typedef uint32_t process_id_t;

/**
 * Type for process priorities.
 */
typedef uint8_t process_priority_t;

/**
 * Type for thread ids.
 */
typedef uint32_t thread_id_t;

/**
 * Type for thread states.
 */
typedef uint8_t thread_state_t;

/**
 * Type for thread priorities.
 */
typedef uint8_t thread_priority_t;

//----------------------------------------------------------------------------//
// Process - Structures
//----------------------------------------------------------------------------//

/**
 * Structure for a thread.
 *
 * Threads can be considered as a stream of exeuction that always belongs to
 * a process, but has its own (downgrowing) stack.
 *
 * Threads are assigned an id for simple identification. Thread ids are unique
 * among the ids of all other threads of the same process that have been spawned
 * since system startup.
 */
typedef struct thread_t
{
    /**
     * The thread id.
     */
    thread_id_t id;
    
    /**
     * The id of the process the thread belongs to.
     */
    process_id_t process_id;
    
    /**
     * The thread's state.
     */
    thread_state_t state;
    
    /**
     * The thread's priority.
     */
    thread_priority_t priority;
    
    /**
     * The address of the top of the thread's stack.
     */
    uintptr_t stack_addr;
    
    /**
     * The length of the thread's stack.
     */
    uintptr_t stack_length;
    
    /**
     * The thread's instruction pointer, i.e. the point in memory to continue
     * execution on.
     */
    uintptr_t instr_ptr;
    
    /**
     * Pointer to the system specific system state since the last preemption
     * of the thread. Null-pointer for thread that haven't been executed yet.
     */
    void *pause_ctx;
} thread_t;

/**
 * Structure for a process.
 *
 * Processes are isolated entities of code and data.
 *
 * Processes are assigned an id for simple identification. Process ids are
 * unique among the ids of all other processes that have been spawned since
 * system startup.
 *
 * Process ids of two exeuctions of the same platform are not guranteed to
 * point to the same process, except for the boot process, which is always
 * assigned an id of zero (0).
 *
 * All processes except for the boot process, which is spawned at kernel start-
 * up, are spawned as child processes of other, already running ones. When a
 * process terminates, all its child processes are terminated, too.
 *
 * Each process is assigned an individual and isolated address space, that contains:
 *  * The process's binary, including code and data.
 *  * The process's heap space that can be controlled by system calls.
 *  * The kernel mappings (inaccessible for user code).
 *  * Helper mappings (such as recursive mappings, inaccessible for user code).
 *
 * Each process has a list of threads which can be considered as streams of
 * execution. When a process is created, it will be created with exactly one
 * thread that starts at the process binary's entry point.
 *
 * A process can terminate, i.e. stop in the following conditions:
 *  * The process executes a system call for stopping itself, or another
 *    process, which is privileged to do so, executes a system call for stopping
 *    it.
 *  * The process's parent process is about to be terminated.
 *  * The process's main thread has returned.
 *  * All the process's threads are stopped.
 *
 * When the boot process is terminated, all other processes are terminated in
 * order and the system shuts down.
 */
typedef struct process_t
{
    /**
     * The process id.
     */
    process_id_t id;
    
    /**
     * Pointer to the parent process.
     */
    struct process_t *parent;
    
    /**
     * The process priority.
     */
    process_priority_t priority;
    
    /**
     * Linked list of threads in this process.
     */
    thread_t *threads;
    
    /**
     * The id that will be assigned to the next thread of this process.
     */
    thread_id_t thread_next_id;
    
    /**
     * Physical address of the process's address space.
     */
    uintptr_t address_space;
    
    /**
     * Pointer to the next process.
     */
    struct process_t *next;
    
} process_t;

//----------------------------------------------------------------------------//
// Process
//----------------------------------------------------------------------------//

/**
 * Returns a process, given its id.
 *
 * @param id The id of the process.
 * @return Pointer to the process structures or null-pointer if there is no
 *  such process.
 */
process_t *process_get(process_id_t id);

/**
 * Returns a pointer to the first process in the linked list.
 *
 * @return Pointer to first process.
 */
process_t *process_first(void);

//----------------------------------------------------------------------------//
// Process - Lifecycle
//----------------------------------------------------------------------------//

/**
 * Spawns a new process, given its parent a prepared address space and the
 * entry point.
 *
 * This function will automatically create the process's main thread and insert
 * the process into scheduling at default priority.
 *
 * @param parent The process's parent process.
 * @param addr_space The process's address space.
 * @param entry_point The entry point of the process's main thread.
 */
process_t *process_spawn(
    process_id_t parent, uintptr_t addr_space, uintptr_t entry_point);
    
/**
 * Terminates the process with the given id.
 *
 * @param pid 
 */
void process_terminate(process_id_t pid);
    
//----------------------------------------------------------------------------//
// Process - Priorities
//----------------------------------------------------------------------------//
    
/**
 * Sets the default process priority.
 */
process_priority_t process_default_priority_get();
process_priority_t process_default_priority_set(process_priority_t pri);
