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
#include <amd64/cpu/pic.h>
#include <amd64/cpu/pit.h>
#include <amd64/io/io.h>

//----------------------------------------------------------------------------//
// PIT - Variable
//----------------------------------------------------------------------------//

/**
 * The last known frequency of the PIT (in Hz).
 */
static uint32_t cpu_pit_freq = 0;

//----------------------------------------------------------------------------//
// PIT - Constants
//----------------------------------------------------------------------------//

#define IO_PIT_COMMAND          0x43
#define IO_PIT_DATA             0x40
#define IO_PIT_COMMAND_FREQ     0x36

//----------------------------------------------------------------------------//
// PIT
//----------------------------------------------------------------------------//

void cpu_pit_freq_set(uint32_t frequency)
{
    // Set frequency
    cpu_pit_freq = frequency;

    // Calculate divisor
    uint32_t divisor = PIT_FREQ_MAX / frequency;
    uint8_t divLow = (uint8_t) (divisor & 0xFF);
    uint8_t divHigh = (uint8_t) ((divisor >> 8) & 0xFF);
    
    // Adjust frequency
    io_outb(IO_PIT_COMMAND, IO_PIT_COMMAND_FREQ);
    io_outb(IO_PIT_DATA, divLow);
    io_outb(IO_PIT_DATA, divHigh);
}

uint32_t cpu_pit_freq_get(void)
{
    return cpu_pit_freq;
}

void cpu_pit_enable(void)
{
    cpu_pic_unmask(PIT_IRQ);
}

void cpu_pit_disable(void)
{
    cpu_pic_mask(PIT_IRQ);
}
