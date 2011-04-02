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
#include <api/cpu.h>
#include <api/cpu/timer.h>
#include <api/util/time.h>
#include <amd64/util/time.h>
#include <amd64/io/io.h>
#include <amd64/io/cmos.h>

#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// Time - Macros
//----------------------------------------------------------------------------//

/**
 * In BCD mode values are stored the way that the hexadecimal form matches the
 * decimal form of the actual value (0x67 matches an actual value of 67).
 *
 * Use this macro to convert the BCD mode values to actual ones.
 */
#define TIME_CONVERT_BCD(bcd)   ((bcd >> 1) + (bcd >> 3) + (bcd & 0xF))

//----------------------------------------------------------------------------//
// Time - Variables
//----------------------------------------------------------------------------//

/**
 * The current UNIX time.
 */
static time_t _time_current = 0;

//----------------------------------------------------------------------------//
// Time - Internal
//----------------------------------------------------------------------------//

/**
 * Timer handler that is called every second the increase the system time by one
 * (only on BSP).
 *
 * @param ticks Current number of ticks.
 * @param ctx Interrupt context.
 */
static void _time_update(uint64_t ticks, void *ctx)
{
    // Is BSP?
    if (cpu_get(cpu_current_id())->flags & CPU_FLAG_BSP)
        ++_time_current;
}

//----------------------------------------------------------------------------//
// Time
//----------------------------------------------------------------------------//

void time_init(void)
{
    // Ensure not in update mode
    uint8_t status_a =  cmos_read(CMOS_TIME_STATUS_A);
    if (status_a & CMOS_TIME_UPDATE_MODE)
        cmos_write(CMOS_TIME_STATUS_A, status_a & ~CMOS_TIME_UPDATE_MODE);

    // Components of human readable time
    uint8_t second =    cmos_read(CMOS_TIME_SECOND);
    uint8_t minute =    cmos_read(CMOS_TIME_MINUTE);
    uint8_t hour =      cmos_read(CMOS_TIME_HOUR);
    uint8_t dom =       cmos_read(CMOS_TIME_DOM);
    uint8_t month =     cmos_read(CMOS_TIME_MONTH);
    uint16_t year =     cmos_read(CMOS_TIME_YEAR);
    uint8_t century =   cmos_read(CMOS_TIME_CENTURY);
    uint8_t status_b =  cmos_read(CMOS_TIME_STATUS_B);
    
    // PM or AM? (Only interesting for 12 hour mode)
    bool pm = (hour & CMOS_TIME_PM);
    hour &= ~CMOS_TIME_PM;
    
    // BCD mode?
    if (status_b & CMOS_TIME_BCD) {
        second =    TIME_CONVERT_BCD(second);
        minute =    TIME_CONVERT_BCD(minute);
        hour =      TIME_CONVERT_BCD(hour);
        dom =       TIME_CONVERT_BCD(dom);
        month =     TIME_CONVERT_BCD(month);
        year =      TIME_CONVERT_BCD(year);
        century =   TIME_CONVERT_BCD(century);
    }
        
    // 12 hours?
    if (status_b & CMOS_TIME_12H) {
        // PM times are increased by 12 hours
        if (pm)
            hour += 12;
            
        // AM stays as it is (except for midnight)
        else if (12 == hour)
            hour = 0;
    }
    
    // Add century to year
    year += (century - 1) * 100;
    
    // Make time
    _time_current = time_make(
        year, month, dom, hour, minute, second);
        
    // Add timer handler
    cpu_timer_register(
        &_time_update,
        1000000 / cpu_timer_interval());
}

time_t time_current(void)
{
    return _time_current;
}
