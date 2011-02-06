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
// Types
//----------------------------------------------------------------------------//

typedef uintptr_t page_t;

//----------------------------------------------------------------------------//
// Flags
//----------------------------------------------------------------------------//

#define PG_PRESENT      1 << 0          // Present
#define PG_WRITABLE     1 << 1          // Writable
#define PG_USER         1 << 2          // User-accessible
#define PG_DIRTY        1 << 6          // Dirty
#define PG_ACCESSED     1 << 7          // Accessed
#define PG_GLOBAL       1 << 8          // Global

//----------------------------------------------------------------------------//
// Page - Mapping
//----------------------------------------------------------------------------//

/**
 * Maps the given page to the given physical address and sets all the flags
 * given in <tt>flags</tt> in addition to the present flag.
 *
 * @param virt The virtual address mapped by the page.
 * @param phys The physical address to map to.
 * @param flags The flags (except the present flag) to set.
 * @return Pointer to the page.
 */
void page_map(uintptr_t virt, uintptr_t phys, uint16_t flags);

/**
 * Unmaps the given page.
 *
 * Unsets the present flag.
 *
 * @param virt The virtual address mapped by the page.
 * @return Pointer to the page.
 */
void page_unmap(uintptr_t virt);

//----------------------------------------------------------------------------//
// Page - Address Space
//----------------------------------------------------------------------------//

/**
 * Tries to switch to the given address space.
 *
 * @param space The location of the new address space in physical memory.
 */
void page_switch_space(uintptr_t space);

/**
 * Disposes the current address space and switches to a kernel-only address space.
 */
void page_dispose_space();

/**
 * Creates a new address space that is empty except for the kernel and recursive
 * mapping in higher half.
 *
 * Make sure that no other processor is currently using this address space.
 *
 * @return The location of the new address space in physical memory.
 */
uintptr_t page_create_space();
