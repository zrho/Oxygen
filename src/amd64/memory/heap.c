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
#include <api/memory/heap.h>
#include <api/memory/page.h>
#include <api/memory/frame.h>
#include <api/sync/spinlock.h>
#include <api/string.h>
#include <api/debug/console.h>

//------------------------------------------------------------------------------
// Heap - Variables
//------------------------------------------------------------------------------

uintptr_t heap_begin = 0xFFFFFF0080000000;
uintptr_t heap_length = 0;
spinlock_t heap_lock = 0;

//------------------------------------------------------------------------------
// Heap - Advanced
//------------------------------------------------------------------------------

uint64_t heap_sbrk(intptr_t increase)
{
    // Acquire lock
    spinlock_acquire(&heap_lock);
    
    // Increase?
    if (increase > 0) {
        // Align
        increase = mem_align((uintptr_t) increase, 0x1000);
        
        // Determine amount of pages
        size_t pages = increase / 0x1000;
        size_t i;
        
        for (i = 0; i < pages; ++i) {
            // Allocate frame
            uintptr_t phys = frame_alloc();
            
            // Map frame
            page_map(
                heap_begin + heap_length,
                phys,
                PG_PRESENT | PG_GLOBAL | PG_WRITABLE);
                
            // Increase length
            heap_length += 0x1000;
        }
        
    // Decrease
    } else if (increase < 0) {
        // Align decrease
        uintptr_t decrease = mem_align((uintptr_t) (-increase), 0x1000);
        
        // Determine amount of pages
        size_t pages = decrease / 0x1000;
        size_t i;
        
        for (i = 0; i < pages; ++i) {
            // Get (virtual) begin address of last page
            uintptr_t virt = heap_begin + heap_length;
            
            // Get physical address
            uintptr_t phys = page_get_physical(virt);
            
            // Unmap page
            page_unmap(virt);
            
            // Decrease length
            heap_length -= 0x1000;
        }
    }
    
    // Release lock
    spinlock_release(&heap_lock);
    
    // Beginning of the heap
    return heap_begin;
}
