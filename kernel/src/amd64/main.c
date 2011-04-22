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

#include <api/multitasking/process.h>
#include <api/multitasking/scheduler.h>

#include <api/boot/info.h>
#include <api/debug/console.h>

#include <api/cpu/int.h>
#include <api/cpu/timer.h>

#include <api/util/random.h>
#include <api/util/time.h>

#include <api/memory/page.h>
#include <api/memory/heap.h>
#include <api/memory/frame.h>

#include <amd64/debug/console.h>
#include <amd64/boot/info.h>
#include <amd64/info/acpi.h>
#include <amd64/util/time.h>
#include <amd64/binary/elf64.h>

#include <amd64/memory/frame.h>
#include <amd64/memory/page.h>

#include <amd64/cpu.h>
#include <amd64/cpu/int.h>
#include <amd64/cpu/lapic.h>

static void pg_fault(interrupt_vector_t vector, void *ctx)
{
    console_print("PF!");
    while(1);
}

void scheduler_timer_handler(uint64_t ticks, void *_ctx);

/*static void tmr_handler(uint64_t ticks, void *ctx)
{
    console_print("Tick ");
    console_print_hex(ticks);
    console_print("\n");
}*/

int main(void)
{
    // Relocate video memory
    console_memory_relocate(CONSOLE_MEM_VIRTUAL);

    // Clear screen
    console_clear();
    
    // Print header
    int8_t *spacer = (int8_t *) "-------------------------------------------------------------------------------";
    
    console_print(spacer);
    console_print("\nOxygen Microkernel (AMD64)\n");
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
    console_print("[CORE] Initializing frame heap...\n");
    frame_setup(info, (void *) mem_align(info->free_mem_begin, 0x1000));
    
    // Initialize paging
    console_print("[CORE] Initializing paging...\n");
    uintptr_t pml4 = cpu_get_cr3();
    page_init(pml4, pml4);
    
    // Relocate frame bitset
    frame_setup_relocate();
    
    // Copy info data to other physical frame
    uintptr_t info_phys = frame_alloc();
    memcpy((void *) info_phys, (void *) BOOT_INFO_VIRTUAL, 0x1000);
    page_map(BOOT_INFO_VIRTUAL, info_phys, PG_GLOBAL | PG_PRESENT);
    
    // Initialize interrupts
    console_print("[CORE] Initializing interrupts...\n");
    cpu_int_init();
    cpu_int_load();
    
    cpu_int_register(14, (interrupt_handler_t) &pg_fault);
    cpu_int_register(0, (interrupt_handler_t) &pg_fault);
    
    // Parse ACPI tables and create sysinfo structure
    console_print("[INFO] Parsing ACPI tables...\n");
    acpi_parse();
    
    // Print some information found in the ACPI tables...
    console_print("[INFO] Processors: ");
    console_print_hex(cpu_count());
    console_print("\n[INFO] LAPIC Physical Address: ");
    console_print_hex(cpu_lapic_get());
    console_print("\n");

    // Initialize BSP
    console_print("[SMP ] Initializing BSP...\n");
    cpu_startup();
    
    // Initialize APs
    console_print("[SMP ] Initializing APs...\n");
    cpu_smp_init();
    
    // Initialize system time
    console_print("[CORE] Initializing system time...\n");
    time_init();
    
    // Map modules
    console_print("[CORE] Mapping modules...\n");
    modules_map((boot_info_mod_t *) info->mods);
    
    // Get rid of low memory mapping
    page_unmap_low();
    
    // Find root binary
    console_print("[CORE] Searching root binary...\n");
    
    boot_info_mod_t *module = (boot_info_mod_t *) info->mods;
    
    while (0 != module) {
        // Is root binary?
        if (module->name == (uintptr_t) strstr(
            (int8_t *) module->name, "/boot/root64.bin"))
            break;
            
        // Next
        module = (boot_info_mod_t *) module->next;
    }
    
    // Found root binary?
    if (0 == module) {
        console_print("[CORE] Failed to find root binary.\n");
        while (1);
    }
    
    // Load root binary
    console_print("[CORE] Loading root binary...\n");
    
    uintptr_t entry_point = elf64_exec_load((void *) module->mapping);
    
    if (entry_point & ELF64_ERROR) {
        console_print("[CORE] Failed to load root binary.\nError code: ");
        console_print_hex(entry_point & ~ELF64_ERROR);
        while (1);
    }
    
    // Spawn root process
    process_t *proc = process_spawn(0, page_get_space(), entry_point);
    scheduler_add(proc->threads);
    
    // Add multitasking timer handler
    console_print("[CORE] Initializing multitasking...\n");
    cpu_timer_register((timer_handler_t) &scheduler_timer_handler, 1);
    
    console_print("[CORE] Done.");
    return 0;
}
