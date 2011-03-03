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
#include <api/compiler.h>
#include <api/cpu.h>

//----------------------------------------------------------------------------//
// CPU - API
//----------------------------------------------------------------------------//

/**
 * Adds a detected CPU.
 *
 * @param cpu The CPU to add.
 */
void cpu_add(cpu_t cpu);

/**
 * Initializes the BSP and all enabled APs.
 *
 * Only to be called once on the BSP.
 */
void cpu_startup(void);

//----------------------------------------------------------------------------//
// CPU - SMP
//----------------------------------------------------------------------------//

/**
 * Initializes the systems Application Processors.
 */ 
void cpu_smp_init(void);

//----------------------------------------------------------------------------//
// CPU - Control registers
//----------------------------------------------------------------------------//

/**
 * Sets the value of the <tt>CR3</tt> register.
 *
 * @param cr3 New value for the register.
 */
void cpu_set_cr3(uintptr_t cr3);

/**
 * Returns the value of the <tt>CR3</tt> register.
 *
 * @return The value of the register.
 */
uintptr_t cpu_get_cr3(void);
