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
// PIT - Constants
//----------------------------------------------------------------------------//

/**
 * The PIT's maximum frequency in Hz.
 */
#define PIT_FREQ_MAX            1193180

/**
 * The PIT's IRQ number.
 */
#define PIT_IRQ                 0x0

//----------------------------------------------------------------------------//
// PIT
//----------------------------------------------------------------------------//

/**
 * Adjusts the frequency of the PIT.
 *
 * @param frequency The new frequency.
 */
void cpu_pit_freq_set(uint32_t frequency);

/**
 * Returns the PIT's current frequency.
 *
 * @return The current frequency.
 */
uint32_t cpu_pit_freq_get(void);

/**
 * Enables the PIT by unmasking its IRQ in the PIC.
 */
void cpu_pit_enable(void);

/**
 * Disabled the PIT by masking its IRQ in the PIC.
 */
void cpu_pit_disable(void);
