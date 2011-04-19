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
#include <api/cpu.h>

//----------------------------------------------------------------------------//
// Timer
//----------------------------------------------------------------------------//

/**
 * Returns the timer's interval in micro seconds.
 *
 * @return Timer's interval in micro seconds.
 */
uint32_t cpu_timer_interval(void);

/**
 * Returns the current timer's ticks since system startup or last overflow.
 * 
 * @return The current timer's ticks.
 */
uint64_t cpu_timer_ticks(void);

//----------------------------------------------------------------------------//
// Timer - Handling
//----------------------------------------------------------------------------//

/**
 * Type of timer handlers.
 *
 * @param ticks The current timer's ticks.
 * @param ctx The interrupt context of the IRQ that caused the handler to be
 *  called.
 * @param cpu The current CPU.
 */
typedef void (*timer_handler_t)(uint64_t, void *, cpu_t *);

/**
 * Registers a handler to the timer, given its callback and granularity.
 *
 * @param handler The handler's callback.
 * @param granularity The granularity of the handler, i.e. the number of ticks
 *  that have to pass until the handler is called again.
 */
void cpu_timer_register(timer_handler_t handler, uint32_t granularity);

/**
 * Unregisters a handler from the timer.
 *
 * @param handler The callback of the handler to unregister.
 */
void cpu_timer_unregister(timer_handler_t handler);
