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
// Time - Types
//----------------------------------------------------------------------------//

/**
 * Type for keeping track of time values.
 */
typedef uint64_t time_t;

//----------------------------------------------------------------------------//
// Time
//----------------------------------------------------------------------------//

/**
 * Returns the current UNIX time (since The Epoch).
 *
 * @return Current UNIX time.
 */
time_t time_current(void);

/**
 * Creates a UNIX timestamp out of the given UTC human-readable date (Gregorian
 * Calendar).
 *
 * @param year The year of the date.
 * @param month The month of the year.
 * @param day The day of the month.
 * @param hour The hour of the day (24h).
 * @param minute The minute of the hour.
 * @param second The second of the minute.
 */
time_t time_make(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
    uint8_t minute, uint8_t second);
