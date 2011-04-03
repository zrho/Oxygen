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
#include <api/multitasking/thread.h>
#include <api/multitasking/process.h>

//----------------------------------------------------------------------------//
// Stack - Structures
//----------------------------------------------------------------------------//

/**
 * A stack classified by its address and size.
 *
 * May grow up or down depending on the architecture.
 */
typedef struct stack_t
{
    /**
     * The address of the stack.
     */
    uintptr_t addr;
    
    /**
     * The size of the stack.
     */
    size_t size;
    
} stack_t;

//----------------------------------------------------------------------------//
// Stack
//----------------------------------------------------------------------------//

/**
 * Changes a stack's size.
 *
 * @param size The new size of the stack. Will be page aligned.
 * @param stack The stack to resize.
 * @return The new size of the stack.
 */
size_t stack_resize(size_t size, stack_t *stack);

/**
 * Disposes a stack by freeing and unmapping all of its memory.
 *
 * @param stack The stack to dispose.
 */
void stack_dispose(stack_t *stack);
