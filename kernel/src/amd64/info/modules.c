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
#include <api/memory/page.h>

#include <amd64/info/modules.h>

//----------------------------------------------------------------------------//
// Modules
//----------------------------------------------------------------------------//

void modules_map(boot_info_mod_t *modules)
{
    // Map modules
    uintptr_t placement = MODULES_VIRTUAL_ADDR;
    
    while (0 != modules) {
        // Set mapping address
        modules->mapping = placement;
        
        // Map module
        size_t offset;
        for (offset = 0; offset < modules->length; offset += 0x1000) {
            page_map(placement, modules->address + offset, PG_PRESENT | PG_WRITABLE);
            placement += 0x1000;
        }
        
        // Next
        modules = modules->next;
    }
}
