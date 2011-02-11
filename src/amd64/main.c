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
#include <api/debug/console.h>
#include <amd64/debug/console.h>
#include <api/boot/info.h>
#include <amd64/boot/info.h>
#include <amd64/memory/frame.h>
#include <amd64/memory/page.h>
#include <amd64/cpu.h>
#include <api/cpu.h>
#include <api/memory/page.h>
#include <api/memory/heap.h>
#include <api/string.h>

void page_fault(interrupt_vector_t vector, void *regs)
{
    console_print("PF!");
    while (1);
}

void gp_fault(interrupt_vector_t vector, void *regs)
{
    console_print("GP!");
}

void main()
{
    // Relocate video memory
    console_memory_relocate(CONSOLE_MEM_VIRTUAL);

    // Clear screen
    console_clear();
    
    // Print header
    int8_t *spacer = "-------------------------------------------------------------------------------";
    
    console_print(spacer);
    console_print("\nOxygen Microkernel\n");
    console_print("Copyright (c) 2011 by Lukas Heidemann <lukasheidemann@aim.com>\n");
    console_print(spacer);
    console_print("\n");
    
    // Print info
    boot_info_t *info = (boot_info_t *) BOOT_INFO_VIRTUAL;

    console_print("Entry: ");
    console_print_hex(info->entry_point);
    
    console_print("\nLoader: ");
    console_print((int8_t *) info->loader_name);
    
    console_print("\nCommand: ");
    console_print((int8_t *) info->args);
    console_print("\n");
    console_print(spacer);
    console_print("\n");
    
    // Set up frame heap
    console_print("Initializing frame heap...\n");
    frame_setup(info, (void *) mem_align(info->free_mem_begin, 0x1000));
    
    // Initialize paging
    console_print("Initializing paging...\n");
    uintptr_t pml4 = cpu_get_cr3();
    page_init(pml4, pml4);
    
    // Relocate frame bitset
    frame_setup_relocate();
    page_unmap_low();
    
    // Initialize interrupts
    console_print("Initializing interrupts...\n");
    cpu_int_init();
    cpu_int_register(14, &page_fault);
}
