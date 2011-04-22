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
// Thread
//----------------------------------------------------------------------------//

/**
 * Switches to the given thread, returning the currently running thread to the
 * scheduler.
 *
 * @param thread The thread to switch to.
 * @param proc The process the thread belongs to.
 * @param context The CPU context.
 */
void thread_switch(thread_t *thread, process_t *proc, void *context);

/**
 * Checks whether the current thread's TTL has elapsed. If this is the case,
 * this method will switch to a new task or decide to idle.
 *
 * @param context The CPU context.
 * @return The new thread or a null-pointer if the method decided to idle.
 */
thread_t *thread_tick(void *context);

//----------------------------------------------------------------------------//
// Thread - Lifecycle
//----------------------------------------------------------------------------//

/**
 * Creates a new thread for the given process that starts at the given entry
 * point.
 *
 * Does not add the new thread into scheduling.
 *
 * @param proc The process hosting the thread.
 * @param entry_point The entry point for the thread to start at.
 * @return The structure describing the thread.
 */
thread_t *thread_create(process_t *proc, uintptr_t entry_point);

/**
 * Stops the given thread.
 *
 * May terminate the hosting process, if this is the last thread for the process.
 *
 * @param thread The thread to stop.
 */
void thread_stop(thread_t *thread);

//----------------------------------------------------------------------------//
// Thread - Scheduler Hooks
//----------------------------------------------------------------------------//

/**
 * Called when a thread is scheduled.
 *
 * @param thread The thread that is scheduled.
 * @param proc The process of the scheduled thread.
 */
void thread_on_schedule(thread_t *thread, process_t *proc);

/**
 * Called when a thread is unscheduled.
 *
 * @param thread The thread that is scheduled.
 * @param proc The process of the scheduled thread.
 */
void thread_on_unschedule(thread_t *thread, process_t *proc);

//----------------------------------------------------------------------------//
// Thread - Priorities
//----------------------------------------------------------------------------//

/**
 * Returns the default thread priority.
 *
 * @return The default thread priority.
 */
thread_priority_t thread_default_priority_get(void);

/**
 * Sets the default thread priority.
 *
 * Priorities of already existing threads remain unchanged.
 *
 * @param pri New default thread priority.
 */
void thread_default_priority_set(thread_priority_t pri);
