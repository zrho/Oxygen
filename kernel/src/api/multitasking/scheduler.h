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
// Scheduler
//----------------------------------------------------------------------------//

/**
 * Returns the next thread to be scheduled on the current CPU or a null-pointer
 * when there is no such thread and the CPU can go idle.
 *
 * When a thread is returned, it is temporarily removed from scheduling. Read
 * it using {@link scheduler_return(thread_t *)}.
 *
 * Not required to be thread safe.
 *
 * @return The thread that is scheduled next.
 */
thread_t *scheduler_next(void);

/**
 * Returns a thread to scheduling after is has been scheduled.
 *
 * Does not return the thread, if its SCHEDULED flag has been removed.
 *
 * @param thread The thread to return to scheduling.
 */
void scheduler_return(thread_t *thread);

/**
 * Adds a thread to the scheduler.
 *
 * Sets the thread's SCHEDULED flag.
 *
 * Not required to be thread safe.
 *
 * @praram thread The thread to add to the scheduler.
 */
void scheduler_add(thread_t *thread);

/**
 * Removes a thread from the scheduler.
 *
 * Removes the thread's SCHEDULED flag.
 *
 * Not required to be thread safe.
 *
 * @param thread The thread to remove from the scheduler.
 */
void scheduler_remove(thread_t *thread);
