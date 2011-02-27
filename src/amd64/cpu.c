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
#include <api/memory/page.h>

#include <amd64/cpu.h>
#include <amd64/cpu/int.h>
#include <amd64/cpu/pic.h>
#include <amd64/cpu/lapic.h>
#include <amd64/cpu/timer.h>

#include <amd64/io/io.h>

#include <api/cpu/int.h>

#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// CPU - Variables
//----------------------------------------------------------------------------//

static cpu_t *cpu_first = 0;
static cpu_t *cpu_last = 0;
static size_t cpu_list_len = 0;

//----------------------------------------------------------------------------//
// CPU - API
//----------------------------------------------------------------------------//

void cpu_startup(void)
{
    // Mark current processor as BSP
    cpu_t *bsp = cpu_get(cpu_current_id());
    bsp->flags |= CPU_FLAG_BSP;
    
    // Initialize PIC
    cpu_pic_init();
    
    // Enable IRQs
    cpu_set_interruptable(true);
    
    // Enable the BSP's LAPIC
    cpu_lapic_enable();
    
    // Initialize timer
    cpu_timer_init();
    
    // Disable PIC
    cpu_pic_disable();
}

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

cpu_t *cpu_get_first(void)
{
    return cpu_first;
}

size_t cpu_count(void)
{
    return cpu_list_len;
}

cpu_id_t cpu_current_id(void)
{
    return *((uint8_t *) (LAPIC_VIRTUAL_ADDR + LAPIC_ID_OFFSET));
}
