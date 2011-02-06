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
// Methods
//----------------------------------------------------------------------------//
 
/**
 * Loads the ELF64 kernel binary located at the given address and of the given
 * length, storing the loaded binary at the given target location.
 *
 * @param target The target location in physical memory to load the binary to.
 * @param address The address the kernel is located at.
 * @param length The length of the kernel binary.
 * @return Entry point address of the loaded kernel.
 */
uint64_t boot_load_kernel_elf64(uintptr_t target, uint64_t address, uint64_t length);
