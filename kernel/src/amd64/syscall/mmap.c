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

#include <api/types.h>
#include <api/compiler.h>
#include <api/cpu.h>

#include <api/multitasking.h>
#include <api/multitasking/process.h>

#include <api/syscall.h>
#include <api/syscall/mmap.h>

#include <api/memory/page.h>

#include <amd64/syscall.h>

//----------------------------------------------------------------------------//
// mmap - System Call
//----------------------------------------------------------------------------//

syscall_ret_t syscall_mmap(
    uintptr_t virt, uintptr_t phys,
    process_id_t pid, uint8_t flags,
    process_t *caller)
{
    // TODO: Perform permission check
    
    // TODO: Check memory region
    
    // Get the target process's descriptor
    // When there is no matching process, fail.
    process_t *target;

    if (pid == caller->id)
        target = caller;
    else
        target = process_get(pid);
    
    if (UNLIKELY(0 == target))
        return SYSCALL_ERROR_INVALID_ARGS;
    
    // Map page?
    if (0 != flags & MMAP_FLAG_MAP) {
        // Switch to the target's address space, if neccessary
        if (target->address_space != caller->address_space)
            page_switch_space(target->address_space);
            
        // Get page flags
        uint16_t pflags = PG_PRESENT | PG_USER;
        
        if (0 != (flags & MMAP_FLAG_RW)) pflags |= PG_WRITABLE;
        if (0 != (flags & MMAP_FLAG_NX)) pflags |= PG_NX;
            
        // Map the page
        console_print("Mapping: ");
        console_print_hex(virt);
        console_print(" => ");
        console_print_hex(phys);
        console_print("\n");
        page_map(virt, phys, pflags);
        
        // Switch to caller's address space, if neccessary
        if (target->address_space != caller->address_space)
            page_switch_space(caller->address_space);
     
    // Unmap page       
    } else {
        page_unmap(virt);
    }
    
    // Success
    return SYSCALL_SUCCESS;
}
