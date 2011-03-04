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
// Timer
//----------------------------------------------------------------------------//

/**
 * Initializes the LAPIC timer.
 *
 * Makes the following assumptions about the system's state for calibration:
 *  * The PIC is enabled.
 *  * IRQs are enabled.
 *  * The current CPU's LAPIC is enabled.
 *  * The CPU is able to receive IRQs.
 *
 * @param calibrate Whether to calibrate the timer.
 */
void cpu_timer_init(bool calibrate);
