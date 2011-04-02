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
#include <api/memory/frame.h>
#include <api/string.h>

#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// Variables
//----------------------------------------------------------------------------//

static uintptr_t frame_offset = 0;
static uintptr_t frame_length = 0;

static uintptr_t *frame_bitset;

//----------------------------------------------------------------------------//
// Macros
//----------------------------------------------------------------------------//

#define FRAME_SIZE 0x1000

#define FRAME_OUT_OF_BOUNDS(a) (a < frame_offset || a > frame_offset + frame_length)
#define FRAME_BITSET_SIZE (frame_length / 0x1000 / sizeof(uintptr_t) + 1)

#define FRAME_NUMBER(a) (a - frame_offset) / FRAME_SIZE
#define FRAME_ADDRESS(n) (n * FRAME_SIZE + frame_offset)

#define FRAME_INDEX(n) (n / FRAME_MAX_OFFSET)
#define FRAME_OFFSET(n) (n % FRAME_MAX_OFFSET)

#define FRAME_MAX_INDEX (frame_length / FRAME_SIZE)
#define FRAME_MAX_OFFSET (sizeof(uintptr_t) * 8)

//----------------------------------------------------------------------------//
// Implementation - Private
//----------------------------------------------------------------------------//

/**
 * Marks the frame with the given number as allocated.
 *
 * @param frameNumber The number of the frame.
 */
static void _frame_set_alloc(uintptr_t frameNumber)
{
    frame_bitset[FRAME_INDEX(frameNumber)] |= 1 << FRAME_OFFSET(frameNumber);
}

/**
 * Marks the frame with the given number as free.
 *
 * @param frameNumber The number of the frame.
 */
static void _frame_set_free(uintptr_t frameNumber)
{
    frame_bitset[FRAME_INDEX(frameNumber)] &= ~(1 << FRAME_OFFSET(frameNumber));
}

/**
 * Returns the number of the first free frame.
 *
 * @return Number of the first free frame.
 */
static uintptr_t _frame_find()
{
    uintptr_t index, offset;
    
    for (index = 0; index < FRAME_MAX_INDEX; ++index) {
        // Whole bunch allocated?
        if ((uintptr_t) (-1) != frame_bitset[index])
            // Check each frame
            for (offset = 0; offset < FRAME_MAX_OFFSET; ++offset)
                if (0 == (frame_bitset[index] & (1 << offset)))
                    // Get frame number
                    return index * FRAME_MAX_OFFSET + offset;
    }
                    
    return (uintptr_t) (-1);
}

//----------------------------------------------------------------------------//
// Implementation - Public
//----------------------------------------------------------------------------//

void frame_relocate(uintptr_t virt)
{
    frame_bitset = (uintptr_t *) virt;
}

void frame_init(uintptr_t offset, uintptr_t length, void *storage)
{
    // Set offset and length
    frame_offset = offset;
    frame_length = length;
    
    // Allocate bitset
    frame_bitset = (uintptr_t *) storage;
    memset(frame_bitset, 0, FRAME_BITSET_SIZE);
}

void frame_mark_unavailable(uintptr_t frame)
{
    // Out of bounds?
    if (FRAME_OUT_OF_BOUNDS(frame))
        return;
        
    // Mark as allocated
    _frame_set_alloc(FRAME_NUMBER(frame));
}

uintptr_t frame_alloc()
{
    // Find free frame
    uintptr_t num = _frame_find();
    
    // Found one?
    if ((uintptr_t) (-1) != num) {
        // Mark as allocated
        _frame_set_alloc(num);
        
        // Return address
        return FRAME_ADDRESS(num);
    }
        
    return num;
}

void frame_free(uintptr_t frame)
{
    // Out of bounds?
    if (FRAME_OUT_OF_BOUNDS(frame))
        return;
        
    // Mark as free
    _frame_set_free(FRAME_NUMBER(frame));
}
