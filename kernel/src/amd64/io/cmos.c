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
#include <api/cpu/int.h>

#include <amd64/io/io.h>
#include <amd64/io/cmos.h>

//----------------------------------------------------------------------------//
// CMOS - Constants
//----------------------------------------------------------------------------//

/**
 * Bit for preventing the CMOS to send unwanted NMIs.
 */
#define CMOS_DISABLE_NMI    (1 << 7)

/**
 * Command port of CMOS.
 */
#define CMOS_IO_COMMAND     0x70

/**
 * Data port of CMOS.
 */
#define CMOS_IO_DATA        0x71
 
//----------------------------------------------------------------------------//
// CMOS
//----------------------------------------------------------------------------//

uint8_t cmos_read(uint8_t offset)
{
    // Disable IRQs
    bool irq_enabled = cpu_is_interruptable();
    cpu_set_interruptable(false);
    
    // Prepare CMOS
    io_outb(CMOS_IO_COMMAND, offset | CMOS_DISABLE_NMI);
    
    // TODO: Delay?
    
    // Read data
    uint8_t data = io_inb(CMOS_IO_DATA);
    
    // Enable IRQs
    cpu_set_interruptable(irq_enabled);
    
    return data;
}

void cmos_write(uint8_t offset, uint8_t value)
{
    // Disable IRQs
    bool irq_enabled = cpu_is_interruptable();
    cpu_set_interruptable(false);
    
    // Prepare CMOS
    io_outb(CMOS_IO_COMMAND, offset | CMOS_DISABLE_NMI);
    
    // TODO: Delay?
    
    // Write data
    io_outb(CMOS_IO_DATA, value);
    
    // Enable IRQs
    cpu_set_interruptable(irq_enabled);
}
