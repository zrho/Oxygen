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
// CMOS - Constants
//----------------------------------------------------------------------------//

#define CMOS_TIME_SECOND        0x0
#define CMOS_TIME_MINUTE        0x2
#define CMOS_TIME_HOUR          0x4
#define CMOS_TIME_WEEKDAY       0x6
#define CMOS_TIME_DOM           0x7
#define CMOS_TIME_MONTH         0x8
#define CMOS_TIME_YEAR          0x9
#define CMOS_TIME_CENTURY       0x32
#define CMOS_TIME_STATUS_A      0xA
#define CMOS_TIME_STATUS_B      0xB

#define CMOS_TIME_PM            0x80
#define CMOS_TIME_UPDATE_MODE   0x80

#define CMOS_TIME_12H           0x2
#define CMOS_TIME_BCD           0x4

//----------------------------------------------------------------------------//
// CMOS
//----------------------------------------------------------------------------//

/**
 * Reads a value from CMOS, given the register offset.
 *
 * @param offset Offset of the register to read.
 * @return Contents of that register.
 */
uint8_t cmos_read(uint8_t offset);

/**
 * Writes a value to CMOS, given the register offset.
 *
 * @param offset Offset of the register to write to.
 * @param value The value to write to CMOS.
 */
void cmos_write(uint8_t offset, uint8_t value);
