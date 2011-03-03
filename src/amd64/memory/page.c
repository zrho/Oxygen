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

#include <api/memory/page.h>
#include <api/memory/frame.h>

#include <api/sync/spinlock.h>

#include <amd64/cpu.h>

#include <amd64/memory/page.h>

#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// Variables
//----------------------------------------------------------------------------//

static SPINLOCK_INIT(page_lock);
static uintptr_t page_kernel_pdp;
static uintptr_t page_kernel_pml4;

//----------------------------------------------------------------------------//
// Internal Macros
//----------------------------------------------------------------------------//

#define PAGE_KERNEL_BEGIN           0xFFFFFF0000000000
#define PAGE_KERNEL_END             0xFFFFFF8000000000
#define PAGE_AUX                    PAGE_KERNEL_END - PAGE_SIZE
                                            
#define PAGE_GET_PHYS(page)         mem_align(page, PAGE_SIZE)

#define PAGE_FLAGS_KERNEL           PG_PRESENT | PG_WRITABLE | PG_GLOBAL
#define PAGE_FLAGS_RECURSIVE        PG_PRESENT | PG_WRITABLE
#define PAGE_FLAGS_AUX              PG_PRESENT | PG_WRITABLE

//----------------------------------------------------------------------------//
// Internals
//----------------------------------------------------------------------------//

/**
 * Invalidates the page for the given virtual address on this processor.
 *
 * @param virt The virtual address mapped by the page to invalidate.
 */
static void _page_do_invalidate(uintptr_t virt)
{
	asm volatile ("invlpg %0" :: "m" (virt));
}

/**
 * Invalidates the page for the given virtual address on this and all other processors
 * using this address space.
 *
 * As the kernel pages are mapped in all address spaces, invalidation of a kernel page
 * will cause invalidation of that page on all other processors.
 *
 * @param virt The virtual address mapped by the page to invalidate.
 */
static void _page_invalidate(uintptr_t virt)
{
	// Invalidate page on this processor
	_page_do_invalidate(virt);
	
	// Get current address space
	uintptr_t addrSpace = cpu_get_cr3();
	
	// Is kernel?
	bool kernel = (virt >= PAGE_KERNEL_BEGIN && virt < PAGE_KERNEL_END);
	
	// Broadcast IPI to all CPUs currently using the addres space
	/*size_t cpuCount = cpu_count();
	size_t i;
	
	for (i = 0; i < cpuCount; ++i) {
		cpu_t *cpu = cpu_get_idx(i);
		cpu_id_t cpuId = cpu->id;
		
		// Is current CPU?
		if (cpu_current()->id == cpuId)
			continue;
			
		// Using the address space or kernel?
		if (!kernel && addrSpace != cpu->addressSpace)
			return;
			
		// TODO: Send IPI
	}*/
}

/**
 * Maps given page to the given physical address and sets the given flags.
 *
 * @param page The page to map.
 * @param phys The physical address to map to.
 * @param flags The flags to set.
 */
static void _page_map(page_t *page, uintptr_t phys, uint16_t flags)
{
    *page = mem_align(phys, PAGE_SIZE) | flags | PG_PRESENT;
}

/**
 * Unmaps the given page.
 *
 * @param page The page to unmap.
 */
static void _page_unmap(page_t *page)
{
    *page &= ~PG_PRESENT;
}

/**
 * Allocates a new frame and maps the given page to it.
 *
 * @param page The page to map.
 * @param flags The flags to map the flags with.
 * @param virt The virtual address to invalidate.
 */
static void _page_alloc_frame(page_t *page, uint16_t flags, uintptr_t virt)
{
    // TODO: PANIC on error
    uintptr_t frame = frame_alloc();
    _page_map(page, frame, flags);
    _page_invalidate(virt);
}

/**
 * Checks whether the page for the given virtual address exists in the current
 * address space.
 *
 * @param virt The virtual address that belongs to the page to create.
 * @param create Whether to create the page, if it does not exist.
 * @return Returns whether the page exists now.
 */
static bool _page_exists(uintptr_t virt, bool create)
{
    // PML4E
    page_t *pml4e = (page_t *) PAGE_VIRT_PML4E(PAGE_PML4E_INDEX(virt));
    
    if (!(*pml4e & PG_PRESENT)) {
        if (create)
            _page_alloc_frame(
                pml4e,
                PAGE_FLAGS_RECURSIVE,
                PAGE_VIRT_PDP(PAGE_PML4E_INDEX(virt)));
        else
            return false;
    }
        
    // PDPE
    page_t *pdpe = (page_t *) PAGE_VIRT_PDPE(
        PAGE_PML4E_INDEX(virt),
        PAGE_PDPE_INDEX(virt));
        
    if (!(*pdpe & PG_PRESENT)) {
        if (create)
            _page_alloc_frame(
                pdpe,
                PAGE_FLAGS_RECURSIVE,
                PAGE_VIRT_PD(
                    PAGE_PML4E_INDEX(virt),
                    PAGE_PDPE_INDEX(virt)));
        else
            return false;
    }
        
    // PDE
    page_t *pde = (page_t *) PAGE_VIRT_PDE(
        PAGE_PML4E_INDEX(virt),
        PAGE_PDPE_INDEX(virt),
        PAGE_PDE_INDEX(virt));
        
    if (!(*pde & PG_PRESENT)) {
        if (create)
            _page_alloc_frame(
                pde,
                PAGE_FLAGS_RECURSIVE,
                PAGE_VIRT_PT(
                    PAGE_PML4E_INDEX(virt),
                    PAGE_PDPE_INDEX(virt),
                    PAGE_PDE_INDEX(virt)));
        else
            return false;
    }
    
    // Exists
    return true;
}

/**
 * Internal function for switching the address space, that does not aquire the
 * page lock and performs no integrity checks.
 *
 * @param space The address space to switch to.
 */
static void _page_switch_space(uintptr_t space)
{
    // Write to CR3
    cpu_set_cr3(space);
}

//----------------------------------------------------------------------------//
// Initialization
//----------------------------------------------------------------------------//

void page_init(uintptr_t virt, uintptr_t phys)
{
    // Save physical PML4 address
    page_kernel_pml4 = phys;
    
    // Save the physical address of the kernel PDP
    page_kernel_pdp = mem_align(*((page_t *) (virt + 8 * 510)), PAGE_SIZE);
    
    // Setup recursive mapping
    page_t *last_pdp = (page_t *) (virt + 8 * 511);
    _page_map(last_pdp, phys, PAGE_FLAGS_RECURSIVE);
    
    // Clear complete TLB by rewriting CR3
    _page_switch_space(page_kernel_pml4);
}

void page_unmap_low()
{
    // Unmap low virtual memory (first PDP)
    // Assumes that the GLOBAL flag is not set for these pages, i.e. they are
    // removed from TLB at the next write to CR3.
    _page_unmap((page_t *) PAGE_VIRT_PML4E(0));
}

//----------------------------------------------------------------------------//
// Page - Mapping
//----------------------------------------------------------------------------//

void page_map(uintptr_t virt, uintptr_t phys, uint16_t flags)
{
    // Acquire lock (TODO Only lock the affected address space)
    spinlock_acquire(&page_lock);

    // Create the page (if it does not already exist)
    _page_exists(virt, true);
    
    // Map page
    _page_map((page_t *) PAGE_VIRT_PAGE(virt), phys, flags);
	
	// Invalidate TLB entry
    _page_invalidate(virt);
    
    // Release lock
    spinlock_release(&page_lock);
}

void page_unmap(uintptr_t virt)
{
    // Acquire lock
    spinlock_acquire(&page_lock);
    
    // Check if the page exists
    if (_page_exists(virt, false)) {
        // Remove present flag
        _page_unmap((page_t *) (PAGE_VIRT_PAGE(virt)));
	
		// Invalidate TLB
		_page_invalidate(virt);
	}
    
    // Release lock
    spinlock_release(&page_lock);
}

//----------------------------------------------------------------------------//
// Page - Analyzation
//----------------------------------------------------------------------------//

uintptr_t page_get_physical(uintptr_t virt)
{
    // Acquire lock    
    spinlock_acquire(&page_lock);

    // Align (down) virtual address
    uintptr_t aligned = virt & ~0xFFF;
    
    // Calculate offset in page
    uintptr_t offset = virt - aligned;
    
    // Check if page exists
    uintptr_t phys = (uintptr_t) -1;
    
    if (_page_exists(virt, false)) {
        // Align (down) page value
        phys = *((page_t *) PAGE_VIRT_PAGE(virt));
        phys &= ~0xFFF;
        
        // Add offset
        phys += offset;
    }
    
    // Release lock
    spinlock_release(&page_lock);
    
    return phys;
}

//----------------------------------------------------------------------------//
// Page - Address Space
//----------------------------------------------------------------------------//

void page_switch_space(uintptr_t space)
{
    // Acquire lock
    spinlock_acquire(&page_lock);

    // Equal to current space?
    if (cpu_get_cr3() == space)
        return;
        
    // Switch address space
    _page_switch_space(space);
    
    // Release lock
    spinlock_release(&page_lock);
}

void page_dispose_space()
{
    // Acquire lock
    spinlock_acquire(&page_lock);
        
    // Dispose structures (except the kernel and recursive ones)
    size_t pml4e, pdpe, pde, pte;
    page_t *page;
    
    for (pml4e = 0; pml4e < 512; ++pml4e) {
        // Kernel or recursive?
        if (pml4e >= 510)
            break;
        
        // Children
        for (pdpe = 0; pdpe < 512; ++pdpe) {
            // Children
            for (pde = 0; pde < 512; ++pde) {
                // Children
                for (pte = 0; pte < 512; ++pte) {
                    // Get PTE
                    page = (page_t *) PAGE_VIRT_PTE(pml4e, pdpe, pde, pte);
                
                    // Free frame
                    // Makes the assumption that all frames belonged to the
                    // disposed address space
                    frame_free(PAGE_GET_PHYS(*page));
                }
                
                // Get PDE
                page = (page_t *) PAGE_VIRT_PDE(pml4e, pdpe, pde);
                
                // Free frame
                frame_free(PAGE_GET_PHYS(*page));
            }
            
            // Get PDPE
            page = (page_t *) PAGE_VIRT_PDPE(pml4e, pdpe);
            
            // Free frame
            frame_free(PAGE_GET_PHYS(*page));
        }
        
        // Get PML4E
        page = (page_t *) PAGE_VIRT_PML4E(pml4e);
        
        // Free frame
        frame_free(PAGE_GET_PHYS(*page));
    }
    
    // Switch to kernel PML4
    _page_switch_space(page_kernel_pml4);
    
    // Release lock
    spinlock_release(&page_lock);
}

uintptr_t page_create_space()
{
    // Acquire lock
    spinlock_acquire(&page_lock);
    
    // Create PML4 frame
    uintptr_t newPml4 = frame_alloc();
    
    // Map in PML4 into AUX page
    page_t *auxPage = (page_t *) PAGE_VIRT_PAGE(PAGE_AUX);
    _page_map(auxPage, newPml4, PAGE_FLAGS_AUX);
	_page_do_invalidate(PAGE_AUX);
    
    // Setup kernel mapping
    page_t *kernelPdpEntry = (page_t *) (PAGE_AUX + 8 * 510);
    _page_map(kernelPdpEntry, page_kernel_pdp, PAGE_FLAGS_KERNEL);
    
    // Setup recursive mapping
    page_t *recPdpEntry = (page_t *) (PAGE_AUX + 8 * 511);
    _page_map(recPdpEntry, newPml4, PAGE_FLAGS_RECURSIVE);
    
    // Unmap AUX
    *auxPage &= ~PG_PRESENT;
	_page_do_invalidate(PAGE_AUX);
    
    // Release lock
    spinlock_release(&page_lock);
}
