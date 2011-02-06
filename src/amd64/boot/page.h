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
// Flags
//----------------------------------------------------------------------------//
 
#define PG_PRESENT  1 << 0
#define PG_WRITABLE 1 << 1
#define PG_GLOBAL   1 << 8
 
//----------------------------------------------------------------------------//
// Memory
//----------------------------------------------------------------------------//

#define BOOT_PML4_OFFSET                    0x2000
#define BOOT_PAGE_HEAP_OFFSET               0x3000

//----------------------------------------------------------------------------//
// Macros
//----------------------------------------------------------------------------//

// #define BOOT_PML4E_OFFSET(pdp)              (BOOT_PML4_OFFSET + pdp * 8)
// #define BOOT_PDP_OFFSET(pdp)                (BOOT_PML4E_OFFSET(pdp) & 0xFFFFFF00)

// #define BOOT_PDPE_OFFSET(pdp,pd)            (BOOT_PDP_OFFSET(pdp) + pd * 8)
// #define BOOT_PD_OFFSET(pdp,pd)              (BOOT_PDPE_OFFSET(pdp,pd) & 0xFFFFFF00)

// #define BOOT_PDE_OFFSET(pdp,pd,pt)          (BOOT_PDP_OFFSET(pdp,pd) + pt * 8)
// #define BOOT_PT_OFFSET(pdp,pd,pt)           (BOOT_PDP_OFFSET(pdp,pd,pt) & 0xFFFFFF00)

// #define BOOT_PAGE_OFFSET(pdp,pd,pt,p)       (BOOT_PT_OFFSET(pdp,pd,pt) + p * 8)
 
//----------------------------------------------------------------------------//
// Mapping
//----------------------------------------------------------------------------//
 
/**
 * Sets up early page mapping by:
 *
 *  * Identity mapping the first two megabytes of memory
 *  * Mapping video memory (physical address 0xB8000, one page) to
 *    virtual an address of 0xFFFFFF7FFFFFF000
 *  * Mapping the boot info structure to 0xFFFFFF7FFFFFE000
 *  * Setting up recursive page mapping at an address of 0xFFFFFF8000000000
 *
 * @param infoAddress The physical address of the boot info structure.
 */
void boot_page_setup(uint64_t infoAddress);
    
/**
 * Maps the given page with the given flags.
 *
 * Does not call invplg on the new pages and relies on placement heap from
 * <tt>BOOT_PAGE_HEAP_OFFSET</tt>.
 *
 * @param phys The physical address to map.
 * @param virt The virtual address to map the page to.
 * @param flags The flags for the page.
 */
void boot_page_map(uint64_t phys, uint64_t virt, uint16_t flags);
