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
#include <api/boot/multiboot.h>
#include <api/boot/info.h>

#include <amd64/cpu.h>
#include <amd64/boot/info.h>
#include <amd64/boot/loader.h>
#include <amd64/boot/page.h>
#include <amd64/boot/gdt.h>

#include <api/debug/console.h>

/**
 * End symbol added by the linker.
 */
extern uint32_t *end;

extern uint32_t *realm64;

/**
 * Initializes the system's early page mapping, creates the boot information tables
 * and loads the kernel's binary that is attached as a multiboot module.
 *
 * Called by the 32 bit entry point.
 *
 * @param mbi The multiboot info structure.
 * @param magic The mulitboot magic number.
 */
void init(multiboot_info_t *mbi, uint32_t magic)
{
    // Clear screen
    console_clear();
    
    // Print header
    int8_t *separator = "-------------------------------------------------------------------------------\n";
    
    console_print(separator);
    console_print("Oxygen AMD64 Loader\n");
    console_print("Copyright (c) 2011 by Lukas Heidemann <lukasheidemann@aim.com>\n");
    console_print(separator);
    
    // Create info structures from multiboot info
    boot_info_t *info = boot_create_info(mbi);
    
    // Print info
    console_print("Loader: ");
    console_print((int8_t *) (uintptr_t) info->loader_name);
    
    console_print("\nCommand: ");
    console_print((int8_t *) (uintptr_t) info->args);
    console_print("\n");
    console_print(separator);
    
    // Get end address (page aligned)
    uintptr_t endAddress = mem_align((uintptr_t) &end, 0x1000);
    uintptr_t placement = endAddress;
    
    // Relocate modules
    console_print("[LOADER] Relocating modules...\n");
    placement = boot_modules_relocate(info, placement);
    
    // Find kernel module
    console_print("[LOADER] Searching for kernel...\n");
    
    boot_info_mod_t *module = (boot_info_mod_t *) (uintptr_t) info->mods;
    boot_info_mod_t *kernel = 0;
    while (0 != module) {
        if (strcmp(module->name, "/boot/kernel64.bin")) {
            kernel = module;
            break;
        }
            
        module = (boot_info_mod_t *) (uintptr_t) module->next;
    }
            
    // Found?
    if (0 == kernel) {
        console_print("[LOADER] Error: Could not find kernel module.\n");
        while (1);
    }
    
    // Setup paging
    console_print("[LOADER] Setting up paging...\n");
    boot_page_setup((uintptr_t) info);
    
    // Load the kernel
    console_print("[LOADER] Loading kernel from ");
    console_print_hex(kernel->address);
    console_print(" to ");
    console_print_hex(placement);
    console_print("...\n");
    
    boot_load_result_t load_res = boot_load_kernel_elf64(
        placement, kernel->address, kernel->length);
    info->entry_point = load_res.entry_point;
        
    // Initialize GDT
    cpu_gdt_init();
    
    // Create long mode trampoline
    console_print("[LOADER] Creating trampoline code at ");
    console_print_hex((uintptr_t) &realm64);
    console_print("...\n");
    
    uint8_t *trampoline = (uint8_t *) (uintptr_t) (&realm64);
    trampoline[0] = 0x48; // mov
    trampoline[1] = 0xb8;
    trampoline[10] = 0xff; // jmp rax
    trampoline[11] = 0xe0;
    
    uint64_t *trampoline_addr = (uint64_t *) ((uintptr_t) (&realm64) + 2);
    *trampoline_addr = (uint64_t) info->entry_point;
    
    // Relocate info structure
    boot_info_relocate(info, BOOT_INFO_VIRTUAL);
    
    
    // Set beginning of usable memory
    info->free_mem_begin = mem_align(load_res.mem_end, 0x1000);
}
