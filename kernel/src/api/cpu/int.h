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
// Interrupts - Types
//----------------------------------------------------------------------------//

/**
 * An interrupt vector.
 *
 * A number (for 0 < n < 255) that identifies one type of interrupt.
 */
typedef uint8_t interrupt_vector_t;

//----------------------------------------------------------------------------//
// Interrupts - Handling
//----------------------------------------------------------------------------//

/**
 * An interrupt handler.
 *
 * @param The vector of the interrupt.
 * @param A platform-specific context structure.
 */
typedef void (*interrupt_handler_t)(interrupt_vector_t, void *);

/**
 * Registers an interrupt handler for the given interrupt vector.
 *
 * @param vector The interrupt vector to register the handler for.
 * @param handler The interrupt handler to register.
 */
void cpu_int_register(interrupt_vector_t vector, interrupt_handler_t handler);

//----------------------------------------------------------------------------//
// Interrupts - State
//----------------------------------------------------------------------------//

/**
 * Checks whether the CPU is currently interruptable by non-critical interrupt
 * requests.
 *
 * @return Whether the CPU is interruptable.
 */
bool cpu_is_interruptable(void);

/**
 * Sets whether or not the CPU shall be interruptable by non-critical interrupt
 * requests.
 *
 * @param flag <tt>true</tt> if the CPU shall be interruptable, <tt>false</tt>
 *  otherwise.
 */
void cpu_set_interruptable(bool flag);
