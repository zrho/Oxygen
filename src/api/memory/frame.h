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
// Initialization
//----------------------------------------------------------------------------//

/**
 * Initializes the frame manager with the given offset and length.
 *
 * @param offset The offset the frames begin on.
 * @param length The length of the framed area.
 * @param storage A storage area allocated for the implementation's storage.
 */
void frame_init(uintptr_t offset, uintptr_t length, void *storage);

/**
 * Marks a frame unavailable.
 *
 * Used to bypass small memory holes and to prevent important memory regions to
 * be overridden.
 *
 * @param frame The address of the frame to mark as unavailable.
 */
void frame_mark_unaviable(uintptr_t frame);

/**
 * Changes the address of the internal structures.
 *
 * @param virt The new virtual address of the structures.
 */
void frame_relocate(uintptr_t virt);

//----------------------------------------------------------------------------//
// Alloc and free
//----------------------------------------------------------------------------//

/**
 * Allocates a new frame.
 *
 * @return Address of the new frame or <tt>(uintptr_t) -1</tt> on error.
 */
uintptr_t frame_alloc();

/**
 * Frees a frame.
 *
 * @param frame The address of the frame to free.
 */
void frame_free(uintptr_t frame);
