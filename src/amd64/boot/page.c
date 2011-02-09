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
#include <amd64/memory/page.h>
#include <amd64/boot/page.h>
#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// Variables
//----------------------------------------------------------------------------//

/**
 * Placement address for new paging structures.
 */
uint32_t boot_page_placement = BOOT_PAGE_HEAP_OFFSET;

//----------------------------------------------------------------------------//
// Internal methods
//----------------------------------------------------------------------------//

/**
 * Allocates a page of memory.
 *
 * @return Pointer to the allocated chunk.
 */
static uintptr_t _boot_page_alloc()
{
    uint32_t address = boot_page_placement;
    memset((void *) address, 0, 0x1000);
    boot_page_placement += 0x1000;
    return address;
}

/**
 * Returns a physical pointer to the page that maps the given virtual address
 * and creates all the required structures on the way, if neccessary.
 *
 * @param virt The virtual address the page maps.
 * @return A physical pointer to the page.
 */
static uint64_t *_boot_page_get(uint64_t virt)
{
    // PML4E
    uint64_t *pml4e = (uint64_t *) (uintptr_t) (BOOT_PML4_OFFSET + PAGE_PML4E_INDEX(virt) * 8);
    
    if (!(*pml4e & PG_PRESENT))
        *pml4e = _boot_page_alloc() | PG_PRESENT;
        
    // PDPE
    uint64_t *pdpe = (uint64_t *) (uintptr_t) (((uintptr_t) *pml4e & 0xFFFFF000) + PAGE_PDPE_INDEX(virt) * 8);
        
    if (!(*pdpe & PG_PRESENT))
        *pdpe = _boot_page_alloc() | PG_PRESENT;
        
    // PDE
    uint64_t *pde = (uint64_t *) (uintptr_t) (((uintptr_t) *pdpe & 0xFFFFF000) + PAGE_PDE_INDEX(virt) * 8);
        
    if (!(*pde & PG_PRESENT))
        *pde = _boot_page_alloc() | PG_PRESENT;
        
    // PTE
    return (uint64_t *) (uintptr_t) (((uintptr_t) *pde & 0xFFFFF000) + PAGE_PTE_INDEX(virt) * 8);
}

//----------------------------------------------------------------------------//
// Methods
//----------------------------------------------------------------------------//

extern uint8_t _cpu_gdt_gates;

void boot_page_setup(uint64_t infoAddress)
{
    // Create PML4 and clear subsequent memory
    uint64_t *pml4 = (uint64_t *) BOOT_PML4_OFFSET;
    memset(pml4, 0, 0x1000);
    memset((void *) boot_page_placement, 0, 0x6000);

    // Identity map the first two megabytes of memory
    uint64_t *pdp = (uint64_t *) _boot_page_alloc();
    pml4[0] = ((uintptr_t) pdp) | PG_PRESENT;

    uint64_t *pd = (uint64_t *) _boot_page_alloc();
    pdp[0] = ((uintptr_t) pd) | PG_PRESENT;

    uint64_t *pt = (uint64_t *) _boot_page_alloc();
    pd[0] = ((uintptr_t) pt) | PG_PRESENT;

    size_t i;
    for (i = 0; i < 0x1000; ++i)
        pt[i] = (i * 0x1000) | PG_PRESENT | PG_WRITABLE;

    // Map video memory
    pdp = (uint64_t *) _boot_page_alloc();
    pml4[510] = ((uintptr_t) pdp) | PG_PRESENT;
    
    pd = (uint64_t *) _boot_page_alloc();
    pdp[511] = ((uintptr_t) pd) | PG_PRESENT;

    pt = (uint64_t *) _boot_page_alloc();
    pd[511] = ((uintptr_t) pt) | PG_PRESENT;

    pt[511] = 0xB8000 | PG_PRESENT | PG_WRITABLE | PG_GLOBAL;

    // Map boot info structure
    pt[510] = infoAddress | PG_PRESENT | PG_WRITABLE | PG_GLOBAL;
    
    // Map GDT
    console_print_hex((uintptr_t) &_cpu_gdt_gates);
    pt[509] = ((uintptr_t) &_cpu_gdt_gates) | PG_PRESENT | PG_GLOBAL;

    // Recursive page mapping
    pml4[511] = (uintptr_t) pml4;
}

void boot_page_map(uint64_t phys, uint64_t virt, uint16_t flags)
{
    // Get the page
    uint64_t *page = _boot_page_get(virt);
    
    // Map page
    *page = phys | flags;
}
