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
#include <api/boot/multiboot.h>
 
//----------------------------------------------------------------------------//
// Memory
//----------------------------------------------------------------------------//

#define BOOT_INFO_PHYSICAL  0x1000
#define BOOT_INFO_VIRTUAL   0xFFFFFF7FFFFFE000
 
//----------------------------------------------------------------------------//
// Modules
//----------------------------------------------------------------------------//
 
/**
 * Relocates the modules specified in the given boot info structure to the space
 * after placement and returns the end of that space.
 *
 * @param info The information structure.
 * @param placement The address to put the modules to.
 * @return End address of last module.
 */
uintptr_t boot_modules_relocate(boot_info_t *info, uintptr_t placement);

//----------------------------------------------------------------------------//
// Relocation
//----------------------------------------------------------------------------//
 
/**
 * Relocates the multiboot info and all its internal pointers to a new address.
 *
 * @param info The boot info structure to relocate.
 * @param address The new address of the boot info structure.
 */
void boot_info_relocate(boot_info_t *info, uint64_t address);
 
//----------------------------------------------------------------------------//
// Parsing
//----------------------------------------------------------------------------//
 
/**
 * Creates the kernel's boot info structures, given the information passed by
 * the multiboot boot loader.
 *
 * After this method has returned, the multiboot structures are no longer
 * neccessary.
 *
 * Does not set the <tt>entry_point</tt> field.
 *
 * @param mbi The multiboot information.
 */
boot_info_t *boot_create_info(multiboot_info_t *mbi);
