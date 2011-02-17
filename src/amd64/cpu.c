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
#include <api/compiler.h>
#include <api/cpu.h>
#include <api/string.h>

#include <api/memory/heap.h>

#include <amd64/cpu.h>

#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// CPU - Variables
//----------------------------------------------------------------------------//

static cpu_t *cpu_first = 0;
static cpu_t *cpu_last = 0;
static size_t cpu_list_len = 0;
static uintptr_t cpu_lapic_addr = 0;

//----------------------------------------------------------------------------//
// CPU - LAPIC
//----------------------------------------------------------------------------//

void cpu_set_lapic(uintptr_t addr)
{
    cpu_lapic_addr = addr;
}

uintptr_t cpu_get_lapic()
{
    return cpu_lapic_addr;
}

//----------------------------------------------------------------------------//
// CPU - API
//----------------------------------------------------------------------------//

void cpu_add(cpu_t cpu)
{
    // Increase list length
    ++cpu_list_len;

    // Copy cpu structure
    cpu_t *_cpu = (cpu_t *) malloc(sizeof(cpu_t));
    memcpy(_cpu, &cpu, sizeof(cpu_t));

    // Add to cpu list
    if (0 == cpu_first)
        cpu_first = cpu_last = _cpu;
        
    else {
        cpu_last->next = _cpu;
        cpu_last = _cpu;
    }
}

cpu_t *cpu_get(cpu_id_t id)
{
    cpu_t *current = cpu_get_first();
    
    while (0 != current) {
        // CPU we're looking for?
        if (id == current->id)
            return current;
            
        // Next one
        current = current->next;
    }
    
    return 0;
}

cpu_t *cpu_get_first()
{
    return cpu_first;
}

size_t cpu_count()
{
    return cpu_list_len;
}
