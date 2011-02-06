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
#include <api/binary/elf64.h>
#include <amd64/boot/loader.h>
#include <amd64/boot/page.h>
#include <api/debug/console.h>
 
extern uint32_t mem_align32(uint32_t);
 
//----------------------------------------------------------------------------//
// Methods
//----------------------------------------------------------------------------//

uint64_t boot_load_kernel_elf64(uintptr_t target, uint64_t address, uint64_t length)
{
    // Get ELF header
    Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *) (uintptr_t) address;
    
    // TODO: Maybe check magic numbers and do other sanity checks
    
    // Get program header table and do manual mapping 
    Elf64_Phdr *elf_phdr = (Elf64_Phdr *) (uintptr_t) (address + elf_hdr->e_phoff);
    size_t i;
    
    for (i = 0; i < elf_hdr->e_phnum; ++i) {
        // Only loadable segments are supported (and required)
        if (PT_LOAD == elf_phdr->p_type) {
            // Debug
            console_print("[ELF64] Loadable segment found at offset ");
            console_print_hex((uintptr_t) elf_phdr->p_offset);
            console_print("\n[ELF64] Memsz: ");
            console_print_hex((uintptr_t) elf_phdr->p_memsz);
            console_print("  Filesz: ");
            console_print_hex((uintptr_t) elf_phdr->p_filesz);
            console_print("\n");
                
            // Page flags
            uint64_t flags = PG_PRESENT | PG_GLOBAL;
            
            if (elf_phdr->p_flags & PF_W)
                flags |= PG_WRITABLE;
                
            // Map segment
            uint64_t off;
            
            for (off = 0; off < elf_phdr->p_memsz; off += 0x1000)
                boot_page_map(target + off, elf_phdr->p_vaddr + off, flags);
                
            // Copy bytes
            console_print("[ELF64] Copying ");
            console_print_hex((uintptr_t) elf_phdr->p_filesz);
            console_print(" bytes from binary...\n");
            
            memcpy(
                (void *) target,
                (void *) (uintptr_t) (address + elf_phdr->p_offset),
                elf_phdr->p_filesz);
                
            // Fill the rest with zeroes
            size_t fill_len = (size_t) (elf_phdr->p_memsz - elf_phdr->p_filesz);
            
            console_print("[ELF64] Reserving ");
            console_print_hex(elf_phdr->p_memsz - elf_phdr->p_filesz);
            console_print(" bytes...\n");
            
            memset(
                (void *) (uintptr_t) (target + elf_phdr->p_filesz), 0,
                fill_len);
                
            target += elf_phdr->p_memsz;
            // TODO: target = mem_align32(target);
            
        } else {
            // Debug
            console_print("[ELF64] Non-loadable segment found at offset ");
            console_print_hex((uintptr_t) elf_phdr->p_offset);
            console_print("\n");
        }
            
        // Next header
        elf_phdr = (Elf64_Phdr *) (((uintptr_t) elf_phdr) + elf_hdr->e_phentsize);
    }
    
    console_print("[ELF64] Binary loaded.\n");
    
    // Return entry point address
    return elf_hdr->e_entry;
}
