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
// Interrupts
//----------------------------------------------------------------------------//

typedef uint8_t interrupt_vector;

/**
 * Registers an interrupt handler for the given interrupt vector.
 *
 * @param vector The interrupt vector to register the handler for.
 * @param handler The interrupt handler to register.
 */
void cpu_int_register(interrupt_vector vector, void (*handler)(interrupt_vector, void *));

//----------------------------------------------------------------------------//
// Interruptable
//----------------------------------------------------------------------------//

/**
 * Checks whether the CPU is currently interruptable by non-critical interrupt
 * requests.
 *
 * @return Whether the CPU is interruptable.
 */
bool cpu_is_interruptable();

/**
 * Sets whether or not the CPU shall be interruptable by non-critical interrupt
 * requests.
 *
 * @param flag <tt>true</tt> if the CPU shall be interruptable, <tt>false</tt>
 *  otherwise.
 */
void cpu_set_interruptable(bool flag);
