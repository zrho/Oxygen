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
#include <api/boot/info.h>

#define FRAME_BITSET_VIRTUAL 0xFFFFFF7F80000000

//----------------------------------------------------------------------------//
// Setup
//----------------------------------------------------------------------------//

/**
 * Sets up the frame bitset, given its location and the boot info structure,
 * beginning from 1MB.
 *
 * Marks every frame from 1MB to the end of the frame bitset as unavailable.
 *
 * @param info Boot info structure.
 * @param storage Storage location for the bitset.
 */
void frame_setup(boot_info_t *info, uint8_t *storage);

/**
 * Second stage of frame allocator initialization that moves the frame bitset
 * from low to high memory.
 */
void frame_setup_relocate();
