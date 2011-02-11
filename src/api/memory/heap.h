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

//------------------------------------------------------------------------------
// Heap - Generic
//------------------------------------------------------------------------------

/**
 * Allocates a new chunk of memory, given its size.
 *
 * @param size The size of the memory chunk to allocate.
 * @return Pointer to the beginning of the allocated chunk.
 */
void *malloc(size_t size);

/**
 * Frees a previously allocated chunk of memory, given a pointer to it.
 *
 * @param ptr Pointer to memory chunk to free.
 */
void free(void *ptr);

/**
 * Changes the size of a previously allocated chunk of memory.
 *
 * @param ptr Pointer to memory chunk to reallocate.
 * @param size The new size for the memory chunk.
 * @return The new address of the memory chunk.
 */
void *realloc(void *ptr, size_t size);

//------------------------------------------------------------------------------
// Heap - Advanced
//------------------------------------------------------------------------------

uint64_t heap_sbrk(intptr_t increase);
