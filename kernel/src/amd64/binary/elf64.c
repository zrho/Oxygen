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
#include <amd64/binary/elf64.h>
#include <api/memory/page.h>

//----------------------------------------------------------------------------//
// ELF64 - Constants
//----------------------------------------------------------------------------//

#define LOWER_HALF_END  0x8000000000000000

//----------------------------------------------------------------------------//
// ELF64 - Internal
//----------------------------------------------------------------------------//

static bool _elf64_verify(Elf64_Ehdr *header)
{
    return
        header->e_ident[EI_MAG0]    == ELFMAG0      ||
        header->e_ident[EI_MAG1]    == ELFMAG1      ||
        header->e_ident[EI_MAG2]    == ELFMAG2      ||
        header->e_ident[EI_MAG3]    == ELFMAG3      ||
        header->e_ident[EI_CLASS]   == ELFCLASS64   ||
        header->e_ident[EI_DATA]    == ELFDATA2MSB  ||
        header->e_ident[EI_VERSION] == EV_CURRENT;
}

//----------------------------------------------------------------------------//
// ELF64
//----------------------------------------------------------------------------//

uintptr_t elf64_exec_load(void *binary)
{
    // Get ELF64 header
    Elf64_Ehdr *header = (Elf64_Ehdr *) binary;
    
    // Check ident field
    if (!_elf64_verify(header))
        return ELF64_ERROR_BAD_FORMAT;
        
    // Check type
    if (ET_EXEC != header->e_type)
        return ELF64_ERROR_BAD_TYPE;
        
    // Entry address
    uintptr_t entry = header->e_entry;
    
    if (entry >= LOWER_HALF_END)
        return ELF64_ERROR_ILLEGAL_ADDR;
    
    // Copy and map segments
    Elf64_Phdr *segment = (Elf64_Phdr *) ((uintptr_t) binary + header->e_phoff);
    size_t i;
    
    for (i = 0; i < header->e_phnum; ++i) {
        // Loadable segment?
        if (PT_LOAD == segment->p_type) {
            // Check lower half boundary
            if (segment->p_vaddr >= LOWER_HALF_END ||
                segment->p_vaddr + segment->p_memsz >= LOWER_HALF_END ||
                segment->p_memsz > LOWER_HALF_END)
                return ELF64_ERROR_ILLEGAL_ADDR;
                
            // File size must not exceed size in memory
            if (segment->p_filesz > segment->p_memsz)
                return ELF64_ERROR_BAD_FORMAT;
        
            // Page flags
            uint32_t flags = PG_PRESENT | PG_USER;
            
            if (segment->p_flags & PF_W)
                flags |= PG_WRITABLE;
                
            // Allocate and map space for segment
            // ASSUMPTION: all segments begin on page aligned address
            uintptr_t offset;
            
            for (offset = 0; offset < segment->p_memsz; offset += 0x1000)
                page_map(
                    segment->p_vaddr + offset,
                    frame_alloc(),
                    flags);
                    
            // Copy data from segment
            memcpy(
                (void *) segment->p_vaddr,
                (void *) ((uintptr_t) binary + segment->p_offset),
                segment->p_filesz);
                
            // Fill rest with zeros
            memset(
                (void *) (segment->p_vaddr + segment->p_filesz), 0,
                segment->p_memsz - segment->p_filesz);
        }
    
        // Next segment
        segment = (Elf64_Phdr *) ((uintptr_t) segment + header->e_phentsize);
    }
    
    // Return entry point
    return entry;
}
