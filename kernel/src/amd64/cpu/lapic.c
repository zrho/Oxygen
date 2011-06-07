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
#include <api/memory/page.h>
#include <amd64/cpu/lapic.h>

//----------------------------------------------------------------------------//
// LAPIC - Variables
//----------------------------------------------------------------------------//

/**
 * The LAPIC's physical address.
 */
static uintptr_t cpu_lapic_addr = 0;

//----------------------------------------------------------------------------//
// LAPIC
//----------------------------------------------------------------------------//

void cpu_lapic_eoi(void)
{
    // Write arbibrary value to EOI register
    *LAPIC_REGISTER(LAPIC_EOI_OFFSET) = 0x0;
}

void cpu_lapic_enable(void)
{
    // Set APIC Enabled bit in SVR
    *LAPIC_REGISTER(LAPIC_SVR_OFFSET) |= 0x100;
}

void cpu_lapic_disable(void)
{
    // unset APIC Enabled bit in SVR
    *LAPIC_REGISTER(LAPIC_SVR_OFFSET) &= ~0x100;
}

void cpu_lapic_set(uintptr_t addr)
{
    cpu_lapic_addr = addr;
    page_map(
        MEMORY_LAPIC_VADDR,
        cpu_lapic_addr,
        PG_GLOBAL | PG_PRESENT | PG_WRITABLE);
}

uintptr_t cpu_lapic_get(void)
{
    return cpu_lapic_addr;
}
