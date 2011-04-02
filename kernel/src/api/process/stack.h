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
#include <api/type.h>

//----------------------------------------------------------------------------//
// Copying
//----------------------------------------------------------------------------//

/**
 * Copies the current stack to the given location changing its size and switches
 * to the copy.
 *
 * @param address The new address of the stack.
 * @param size The new size of the stack.
 * @param current The initial address of the current stack.
 */
void stack_copy(uintptr_t address, uintptr_t size, uintptr_t initial_addr);

/**
 * Copies the current stack to the given location changing its size.
 *
 * @param address The begin address of the stack copy.
 * @param size The size of the stack copy.
 * @param current The initial begin address of the current stack.
 * @param size The size of the current stack.
 */
void stack_copy(uintptr_t address, uintptr_t size, uintptr_t initial_addr);
