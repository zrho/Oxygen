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

//----------------------------------------------------------------------------//
// Macros
//----------------------------------------------------------------------------//

#define CONSOLE_WIDTH           80
#define CONSOLE_HEIGHT          25

#define CONSOLE_MEM_PHYSICAL    0xB8000
#define CONSOLE_MEM_VIRTUAL     0xFFFFFF7FFFFFF000

//----------------------------------------------------------------------------//
// Video RAM
//----------------------------------------------------------------------------//

/**
 * Changes the address of the video memory.
 *
 * @param address New video memory address.
 */
void console_memory_relocate(uintptr_t address);
