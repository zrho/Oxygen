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
// PIC
//----------------------------------------------------------------------------//

/**
 * Initializes the PIC by remapping the IRQs.
 */
void cpu_pic_init(void);

/**
 * Issues an EOI for the IRQ of the given number.
 *
 * @param irq The number of the IRQ to send an EOI for.
 */
void cpu_pic_eoi(uint8_t irq);

/**
 * Masks the given IRQ.
 *
 * @param irq The number of the IRQ to mask.
 */
void cpu_pic_mask(uint8_t irq);

/**
 * Unmasks the given IRQ.
 *
 * @param irq The number of the IRQ to unmask.
 */
void cpu_pic_unmask(uint8_t irq);

/**
 * Disables the PIC.
 */
void cpu_pic_disable(void);
