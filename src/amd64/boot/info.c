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
#include <api/string.h>
#include <api/boot/info.h>
#include <api/boot/multiboot.h>
#include <amd64/boot/info.h>

//----------------------------------------------------------------------------//
// Variables
//----------------------------------------------------------------------------//

uint64_t boot_info_address = BOOT_INFO_PHYSICAL;

//----------------------------------------------------------------------------//
// Macros
//----------------------------------------------------------------------------//

#define BOOT_INFO_RELOCATE(a) (address - boot_info_address + (uint64_t) a)

//----------------------------------------------------------------------------//
// Modules
//----------------------------------------------------------------------------//
 
uintptr_t boot_modules_relocate(boot_info_t *info, uintptr_t placement)
{
    // Align placement
    placement = mem_align(placement, 0x1000);

    // Sort modules by begin address (ASC)
    boot_info_mod_t *sorted = 0;
    boot_info_mod_t *sorted_end = sorted;
    
    while (0 != info->mods) {
        // Find ealiest module
        boot_info_mod_t *current = (boot_info_mod_t *) (uintptr_t) info->mods;
        boot_info_mod_t *prev = 0;
        boot_info_mod_t *min_prev = 0;
        boot_info_mod_t *min = 0;
        
        while (0 != current) {
            if (0 == min || current->address < min->address) {
                min = current;
                min_prev = prev;
            }
            
            // Next
            prev = current;
            current = (boot_info_mod_t *) (uintptr_t) current->next;
        }
        
        // Remove from list
        if (0 == min_prev)
            info->mods = min->next;
        else
            min_prev->next = min->next;
            
        // Add to sorted list        
        if (0 == sorted_end)
            sorted = sorted_end = min;
        else {
            sorted_end->next = (uintptr_t) min;
            sorted_end = min;
        }
    }
    
    // Replace lists
    info->mods = (uintptr_t) sorted;
    
    // Move modules
    boot_info_mod_t *current = (boot_info_mod_t *) (uintptr_t) info->mods;
    
    while (0 != current) {
        memcpy(
            (void *) placement,
            (void *) ((uintptr_t) current->address),
            (size_t) current->length);
        
        current->address = (uint64_t) placement;
        placement += (uintptr_t) current->length;
        placement = mem_align(placement, 0x1000);
        current = (boot_info_mod_t *) (uintptr_t) current->next;
    }
    
    return placement;
}

//----------------------------------------------------------------------------//
// Relocation
//----------------------------------------------------------------------------//

void boot_info_relocate(boot_info_t *info, uint64_t address)
{
    // Root structure
    info->loader_name = BOOT_INFO_RELOCATE(info->loader_name);
    info->args = BOOT_INFO_RELOCATE(info->args);
        
    // Memory map
    boot_info_mmap_t *mmap = (boot_info_mmap_t *) (uintptr_t) info->mmap;
    
    while (0 != mmap) {
        uintptr_t old_next = (uintptr_t) mmap->next;
        
        if (0 != mmap->next)
            mmap->next = (uint64_t) BOOT_INFO_RELOCATE(mmap->next);
            
        mmap = (boot_info_mmap_t *) old_next;
    }
    
    info->mmap = BOOT_INFO_RELOCATE(info->mmap);
        
    // Modules
    boot_info_mod_t *mod = (boot_info_mod_t *) (uintptr_t) info->mods;
    
    while (0 != mod) {
        mod->name = BOOT_INFO_RELOCATE(mod->name);
    
        uintptr_t old_next = (uintptr_t) mod->next;
        if (0 != mod->next)
            mod->next = BOOT_INFO_RELOCATE(mod->next);
        mod = (boot_info_mmap_t *) old_next;
    }
    
    info->mods = BOOT_INFO_RELOCATE(info->mods);
    
    // Set address
    boot_info_address = address;
}
 
//----------------------------------------------------------------------------//
// Parsing
//----------------------------------------------------------------------------//

boot_info_t *boot_create_info(multiboot_info_t *mbi)
{
    // Placement address
    uintptr_t placement = BOOT_INFO_PHYSICAL;
    
    // Info structure
    boot_info_t *info = (boot_info_t *) placement;
    placement += sizeof(boot_info_t);
    
    // Loader name
    int8_t *loaderName = (int8_t *) placement;
    strcpy(loaderName, (int8_t *) mbi->boot_loader_name);
    placement += strlen(loaderName) + 1;
    
    info->loader_name = (uint64_t) (uintptr_t) loaderName;
    
    // Arguments
    int8_t *args = (int8_t *) placement;
    strcpy(args, (int8_t *) mbi->cmdline);
    placement += strlen(args) + 1;
    
    info->args = (uint64_t) (uintptr_t) args;
    
    // Memory map
    boot_info_mmap_t *mem_prev = 0;
    multiboot_memory_map_t *mb_mem = (multiboot_memory_map_t *) mbi->mmap_addr;
    
    while((uintptr_t) mb_mem < mbi->mmap_addr + mbi->mmap_length)
    {
        // Create structure
        boot_info_mmap_t *mem = (boot_info_mmap_t *) placement;
        placement += sizeof(boot_info_mmap_t);
        
        mem->address = mb_mem->addr;
        mem->length = mb_mem->len;
        mem->available = (1 == mb_mem->type) ? 1 : 0;
        
        // Link structure
        if (0 != mem_prev)
            mem_prev->next = (uintptr_t) mem;
        else
            info->mmap = (uintptr_t) mem;
        
        // Next structure
        mb_mem = (multiboot_memory_map_t *)
            (((uintptr_t) mb_mem) + mb_mem->size + sizeof(uint32_t));
        mem_prev = mem;
    }
    
    // Modules
    size_t i;
    boot_info_mod_t *mod_prev = 0;
    
    for (i = 0; i < mbi->mods_count; ++i) {
        // Find multiboot module info
        multiboot_module_t *mb_mod = (multiboot_module_t *) (
            mbi->mods_addr + i * sizeof(multiboot_module_t));
            
        // Create module structure
        boot_info_mod_t *mod = (boot_info_mod_t *) placement;
        placement += sizeof(boot_info_mod_t);
        
        mod->address = mb_mod->mod_start;
        mod->length = mb_mod->mod_end - mod->address;
        
        // Copy name
        mod->name = placement;
        strcpy((int8_t *) mod->name, (int8_t *) mb_mod->cmdline);
        placement += strlen((int8_t *) mod->name) + 1;
        
        // Link module
        if (0 != mod_prev)
            mod_prev->next = (uintptr_t) mod;
        else
            info->mods = (uintptr_t) mod;
            
        mod_prev = mod;
    }
    
    return info;
}
