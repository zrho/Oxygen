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
#include <api/boot/info.h>
#include <api/memory/frame.h>
#include <amd64/memory/frame.h>

#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// Internal
//----------------------------------------------------------------------------//

/**
 * Returns the size of physical memory.
 *
 * @param info The boot info structure.
 */
static uint64_t _frame_setup_memsz(boot_info_t *info)
{
    // Get size of physical memory from memory map
    boot_info_mmap_t *mmap = (boot_info_mmap_t *) info->mmap;
    boot_info_mmap_t *last_available = 0;
    
    while (0 != mmap) {
        // Is available?
        if (mmap->available)
            last_available = mmap;
    
        // Next
        mmap = (boot_info_mmap_t *) mmap->next;
    }
    
    // End address of last available region
    return last_available->address + last_available->length;
}

//----------------------------------------------------------------------------//
// Setup
//----------------------------------------------------------------------------//

void frame_setup(boot_info_t *info, uint8_t *storage)
{
    // Physical memory size
    uint64_t physMemsz = _frame_setup_memsz(info);
    
    // Calculate number of frames and size of bitset
    size_t frameNumber = (physMemsz - 0x100000) / 0x1000;
    size_t bitsetSize = frameNumber / 8 + 1;
    
    // Initialize frame bitset
    frame_init(0x100000, bitsetSize, storage);
    
    // Mark every frame from 1MB to end of bitset as unavailable
    uintptr_t frame;
    uintptr_t end_of_bitset = mem_align((uintptr_t) storage + bitsetSize, 0x1000);
    
    for (frame = 0x100000; frame < end_of_bitset; frame += 0x1000)
        frame_mark_unavailable(frame);
        
    // Mark unavailable regions as unavailable in frame bitset
    boot_info_mmap_t *mmap = (boot_info_mmap_t *) info->mmap;
    
    while (0 != mmap) {
        // Is available?
        if (!mmap->available) {    
            // Mark as unavailable
            frame = mem_align(mmap->address, 0x1000);
            frame = (frame != mmap->address) ? frame - 0x1000 : frame; // Align down
            
            uintptr_t end_of_region = mem_align(mmap->address, 0x1000);
            
            for (; frame < end_of_region; frame += 0x1000)
                frame_mark_unavailable(frame);
        }
    
        // Next
        mmap = (boot_info_mmap_t *) mmap->next;
    }
}
