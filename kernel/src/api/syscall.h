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
// Syscall - Constants
//----------------------------------------------------------------------------//

#define SYSCALL_COUNT_MAX 16
#define SYSCALL_SUCCESS             0x0
#define SYSCALL_ERROR_UNKNOWN       0x1
#define SYSCALL_ERROR_INVALID_ARGS  0x2
#define SYSCALL_ERROR_PERMISSION    0x3
#define SYSCALL_ERROR_BOUNDS        0x4

//----------------------------------------------------------------------------//
// Syscall - Types
//----------------------------------------------------------------------------//

typedef uint8_t syscall_id_t;
typedef uint8_t syscall_ret_t;
