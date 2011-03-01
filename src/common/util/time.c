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
#include <api/util/time.h>

//----------------------------------------------------------------------------//
// Time - Variables
//----------------------------------------------------------------------------//

/**
 * Array of days of a year till the beginning of the months.
 */
static uint16_t _time_dtm[12] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

//----------------------------------------------------------------------------//
// Time
//----------------------------------------------------------------------------//

time_t time_make(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
    uint8_t minute, uint8_t second)
{
    // Calculate number of leap-years
    uint16_t leap_years =
        (year - 1969) / 4 -
        (year - 1901) / 100 +
        (year - 1601) / 400;
        
    // Adjust years by The Epoch
    uint16_t epoch_year = year - 1970;
    
    // Calculate UNIX time
    time_t time =
        second +
        minute * 60 +
        hour * 60 * 60 +
        (_time_dtm[month - 1] + day - 1) * 60 * 60 * 24 +
        (epoch_year * 365 + leap_years) * 60 * 60 * 24;
        
        
    // Add leap day if year is a leap year and we are after February
    if (month > 2 && (0 == year % 4 && (0 != year % 100 || 0 == year % 400)))
        time += 60 * 60 * 24;
        
    return time;
}
