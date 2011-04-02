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

//----------------------------------------------------------------------------//
// LAPIC - Constants
//----------------------------------------------------------------------------//

/**
 * The virtual address the LAPIC is mapped to.
 */
#define LAPIC_VIRTUAL_ADDR 0xFFFFFF7FFFFFC000

//----------------------------------------------------------------------------//
// LAPIC - Register Offsets
//----------------------------------------------------------------------------//

#define LAPIC_ID_OFFSET             0x020
#define LAPIC_VERSION_OFFSET        0x030
#define LAPIC_TPR_OFFSET            0x080
#define LAPIC_EOI_OFFSET            0x0B0
#define LAPIC_SVR_OFFSET            0x0F0
#define LAPIC_IRR_OFFSET            0x200
#define LAPIC_ICR_OFFSET            0x300
#define LAPIC_LVT_OFFSET            0x320
#define LAPIC_LINT0_OFFSET          0x350
#define LAPIC_LINT1_OFFSET          0x360
#define LAPIC_EVR_OFFSET            0x370
#define LAPIC_INIT_COUNT_OFFSET     0x380
#define LAPIC_CURRENT_COUNT_OFFSET  0x390
#define LAPIC_DCR_OFFSET            0x3E0

//----------------------------------------------------------------------------//
// LAPIC - Macroes
//----------------------------------------------------------------------------//

#define LAPIC_REGISTER(offset)      ((uint32_t *) (LAPIC_VIRTUAL_ADDR + offset))

//----------------------------------------------------------------------------//
// LAPIC
//----------------------------------------------------------------------------//

/**
 * Signals the end of an interrupt to the LAPIC.
 */
void cpu_lapic_eoi(void);

/**
 * Enables the processor's LAPIC.
 */
void cpu_lapic_enable(void);

/**
 * Disables the processor's LAPIC.
 */
void cpu_lapic_disable(void);

/**
 * Initializes the following LAPIC interrupts to the defined interrupt vectors.
 *  * LINT0
 *  * LINT1
 *  * ERROR
 */
void cpu_lapic_init(void);

/**
 * Sets the physical address of the LAPIC.
 *
 * @param addr The address of the LAPIC.
 */
void cpu_lapic_set(uintptr_t addr);

/**
 * Returns the physical address of the LAPIC.
 *
 * @return The address of the LAPIC.
 */
uintptr_t cpu_lapic_get(void);
