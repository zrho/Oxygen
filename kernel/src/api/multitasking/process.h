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
#include <api/multitasking.h>

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

/**
 * Returns the number of processes.
 *
 * @return Number of processes.
 */
size_t process_count(void);

//----------------------------------------------------------------------------//
// Process - Lock
//----------------------------------------------------------------------------//

/**
 * Acquires a spinlock for dealing with the process and thread structures.
 *
 * Be sure to acquire this lock before calling any process related function
 * described in this header file, as they WON'T LOCK by THEIR OWN.
 */
void process_lock(void);

/**
 * Releases the process spinlock.
 */
void process_unlock(void);

//----------------------------------------------------------------------------//
// Process - Lifecycle
//----------------------------------------------------------------------------//

/**
 * Spawns a new process, given its parent, a prepared address space and the
 * entry point.
 *
 * This function will automatically create the process's main thread, but won't
 * schedule it.
 *
 * @param parent The process's parent process.
 * @param addr_space The process's address space.
 * @param entry_point The entry point of the process's main thread.
 */
process_t *process_spawn(
    process_t *parent, uintptr_t addr_space, uintptr_t entry_point);
    
/**
 * Marks the process of the given id and all child processes for termination.
 *
 * It is NOT guaranteed that the process is terminated after this method has
 * returned.
 *
 * @param process The process to terminate.
 */
void process_terminate(process_t *process);
    
//----------------------------------------------------------------------------//
// Process - Priorities
//----------------------------------------------------------------------------//
    
/**
 * Returns the default process priority.
 *
 * @return The default process priority.
 */
process_priority_t process_default_priority_get(void);

/**
 * Sets the default process priority.
 *
 * Priorities of already existing processes remain unchanged.
 *
 * @param pri New default process priority.
 */
void process_default_priority_set(process_priority_t pri);
