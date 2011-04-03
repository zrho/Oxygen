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
#include <api/string.h>

#include <api/multitasking/stack.h>

#include <api/memory/page.h>
#include <api/memory/frame.h>

//----------------------------------------------------------------------------//
// Stack
//----------------------------------------------------------------------------//

size_t stack_resize(size_t size, stack_t *stack)
{
    // Align size on page boundary
    size = mem_align(size, 0x1000);
    
    // Expand?
    if (size > stack->size) {
        // Map new pages
        uintptr_t virt = stack->addr - size;
        for (; virt < stack->addr - stack->size; virt += 0x1000)
            page_map(virt, frame_alloc(), PG_PRESENT | PG_WRITABLE | PG_USER);
            
    // Collapse
    } else if (size < stack->size) {
        // Free pages
        uintptr_t virt = stack->addr - stack->size;
        for (; virt < stack->addr - size; virt += 0x1000) {
            uintptr_t phys = page_get_phys(virt);
            page_unmap(virt);
            frame_free(phys);
        }
    }
    
    // Assign new size
    stack->size = size;
    return size;
}

void stack_dispose(stack_t *stack)
{
    stack_resize(0, stack);
}

