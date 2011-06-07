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
#include <api/syscall.h>

//----------------------------------------------------------------------------//
// mmap - Flags
//----------------------------------------------------------------------------//

/**
 * Flag (Value 1).
 *
 * When this flag is set, <tt>mmap</tt> will map the page; otherwise the page is
 * unmapped when invoking the system call.
 */
#define MMAP_FLAG_MAP (1 << 0)

/**
 * Flag (Value 2).
 *
 * When this flag is set, the page mapped by <tt>mmap</tt> will be writable (if
 * the platform supports this feature).
 */
#define MMAP_FLAG_RW (1 << 1)

/**
 * Flag (Value 4).
 *
 * When this flag is set, execution of the page's code will be prohibited (if
 * the platform supports this feature).
 */
#define MMAP_FLAG_NX (1 << 2)

//----------------------------------------------------------------------------//
// mmap - System Call
//----------------------------------------------------------------------------//

/**
 * Maps or unmaps a physical page to the desired destination in a given process's
 * address space.
 *
 * Mapping can be modified by giving one or more of the <tt>MMAP_FLAG_*</tt>
 * flags.
 *
 * Pages are mapped by setting the <tt>MMAP_FLAG_MAP</tt> flag, they are unmapped
 * by removing the flag. In the latter case, all other flags will be ignored.
 *
 * Each time this system call is invoked, a permission check will be performed.
 * If the check fails, the system call will return <tt>SYSCALL_ERROR_PERMISSION</tt>.
 *
 * When trying to alter mapping of a virtual memory region that is reserved by
 * the kernel (see concrete implementation for more details), the system call
 * will return <tt>SYSCALL_ERROR_BOUNDS</tt>.
 *
 * Note that Oxygen's mmap is in no way related to POSIX's mmap system call.
 *
 * @param virt The address of the virtual page frame to map. Will be aligned to
 * a page boundary automatically.
 * @param phys The address of the physical page frame to map to.
 * @param pid The pid of the processes its address space to alter.
 * @param flags Flags that modify the mapped page's attributes.
 * @param caller The process that is invoking the system call.
 * @return The system call's return code; <tt>SYSCALL_SUCCESS</tt> on success.
 */
syscall_ret_t syscall_mmap(
    uintptr_t virt, uintptr_t phys,
    process_id_t pid, uint8_t flags,
    process_t *caller);
