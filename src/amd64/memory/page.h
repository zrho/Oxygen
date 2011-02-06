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
// Public Macros
//----------------------------------------------------------------------------//

#define PAGE_SIZE                   0x1000

#define PAGE_PML4E_INDEX(a)         ((a >> 39) & 0x1FF)
#define PAGE_PDPE_INDEX(a)          ((a >> 30) & 0x1FF)
#define PAGE_PDE_INDEX(a)           ((a >> 21) & 0x1FF)
#define PAGE_PTE_INDEX(a)           ((a >> 12) & 0x1FF)

#define PAGE_OFFSET_PML4            0xFFFFFFFFFFFFF000
#define PAGE_OFFSET_PDP             0xFFFFFFFFFFE00000
#define PAGE_OFFSET_PD              0xFFFFFFFFC0000000
#define PAGE_OFFSET_PT              0xFFFFFF8000000000

#define PAGE_VIRT_PML4              PAGE_OFFSET_PML4
#define PAGE_VIRT_PDP(pdp)          (PAGE_OFFSET_PDP | (pdp << 12))
#define PAGE_VIRT_PD(pdp, pd)       (PAGE_OFFSET_PD | (pdp << 21) | (pd << 12))
#define PAGE_VIRT_PT(pdp, pd, pt)   (PAGE_OFFSET_PT | (pdp << 30) | (pd << 21) \
                                    | (pt << 12))
                                    
#define PAGE_VIRT_PML4E(i)          PAGE_VIRT_PML4 + i * 8
#define PAGE_VIRT_PDPE(pdp, i)      PAGE_VIRT_PDP(pdp) + i * 8
#define PAGE_VIRT_PDE(pdp, pd, i)   PAGE_VIRT_PD(pdp, pd) + i * 8
#define PAGE_VIRT_PTE(pdp, pd, pt, i) PAGE_VIRT_PT(pdp, pd, pt) + i * 8

#define PAGE_VIRT_PAGE(a)           PAGE_VIRT_PTE( \
                                       PAGE_PML4E_INDEX(a), \
                                       PAGE_PDPE_INDEX(a), \
                                       PAGE_PDE_INDEX(a), \
                                       PAGE_PTE_INDEX(a))
                                       
//----------------------------------------------------------------------------//
// Initialization
//----------------------------------------------------------------------------//

/**
 * Initializes paging.
 *
 * @param virt Virtual address of the system's paging structures.
 * @param phys Physical address of the system's paging strucutures.
 */
void page_init(uintptr_t virt, uintptr_t phys);
