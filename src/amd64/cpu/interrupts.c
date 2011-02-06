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

#pragma once
#include <api/types.h>
#include <api/cpu.h>

//----------------------------------------------------------------------------//
// Interruptable
//----------------------------------------------------------------------------//

bool cpu_is_interruptable()
{
    // Check IF in flags
    size_t flag;
    
    asm volatile (
        "pushf\n"
        "pop %0\n"
        "and $0x200, %0\n" : "=r" (flag));
        
    return (flag != 0);
}

void cpu_set_interruptable(bool flag)
{
    if (flag)
        asm volatile ("sti");
    else
        asm volatile ("cli");
}
