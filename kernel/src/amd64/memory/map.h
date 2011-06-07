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
// Memory Map
//----------------------------------------------------------------------------//

#define MEMORY_BOOT_PML4_PADDR 0x2000
#define MEMORY_BOOT_PAGE_HEAP_PADDR 0x3000

#define MEMORY_BOOT_INFO_PADDR 0x1000
#define MEMORY_BOOT_INFO_VADDR 0xFFFFFF7FFFFFE000

#define MEMORY_VIDEO_PADDR 0xB8000
#define MEMORY_VIDEO_VADDR 0xFFFFFF7FFFFFF000

#define MEMORY_LAPIC_VADDR 0xFFFFFF7FFFFFC000
#define MEMORY_FRAMES_VADDR 0xFFFFFF7F80000000
#define MEMORY_GDT64_VADDR 0xFFFFFF7FFFFFD000
#define MEMORY_MODULES_VADDR 0xFFFFFE0000000000