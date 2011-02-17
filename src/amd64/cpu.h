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

//----------------------------------------------------------------------------//
// LAPIC Registers
//----------------------------------------------------------------------------//

#define LAPIC_ID_OFFSET         0x20
#define LAPIC_VERSION_OFFSET    0x30
#define LAPIC_IRR_OFFSET        0x200
#define LAPIC_ICR_OFFSET        0x300

//----------------------------------------------------------------------------//
// Control registers
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
uintptr_t cpu_get_cr3();
