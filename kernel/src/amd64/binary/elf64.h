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
// ELF64 - Constants
//----------------------------------------------------------------------------//

#define ELF64_ERROR                 (((uintptr_t) 1) << 63)

/**
 * ELF64 loader error code.
 *
 * Returned when the binary format is not supported or the file is corrupted.
 */
#define ELF64_ERROR_BAD_FORMAT      (ELF64_ERROR | (1 << 0))

/**
 * ELF64 loader error code.
 *
 * Returned when the binary type (executable/relocatable) is not supported by
 * the loader function.
 */
#define ELF64_ERROR_BAD_TYPE        (ELF64_ERROR | (1 << 1))

/**
 * ELF64 loader error code.
 *
 * Returned when the binary requests to map itself to an address not in the
 * lower half of 48-bit memory space.
 */
#define ELF64_ERROR_ILLEGAL_ADDR    (ELF64_ERROR | (1 << 2))

//----------------------------------------------------------------------------//
// ELF64
//----------------------------------------------------------------------------//

/**
 * Loads an ELF64 linked executable in the current address space.
 *
 * @param binary The ELF64 binary to load.
 * @return The address of the binary's entry-point or error code (error bit set).
 */
uintptr_t elf64_exec_load(void *binary);
