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
#include <api/compiler.h>
#include <api/debug/console.h>

//----------------------------------------------------------------------------//
// Printing
//----------------------------------------------------------------------------//

void console_print(const char *str)
{
    char *ptr;
    for (ptr = (char *) str; *ptr; ++ptr)
        console_put(*ptr);
}

void console_print_hex(uintptr_t number)
{
    // Put prefix
    console_put('0');
    console_put('x');

    // Is zero?
    if (UNLIKELY(0 == number)) {
        console_put('0');
        return;
    }
    
    // Get characters in reverse order
    int32_t i = 0;
    int8_t out[16];
    
    for (i = 15; number != 0; --i) {
        uint32_t digit = number & 0xF;
        number = number >> 4;
        
        if (digit > 9) {
            out[i] = (int8_t) ('A' + digit - 0xA);
        } else
            out[i] = (int8_t) ('0' + digit);
    }

    // Print in correct order
    for (++i; i < 16; ++i)
        console_put(out[i]);
}

void console_print_dec(intptr_t number)
{
    // Is zero?
    if (UNLIKELY(0 == number)) {
        console_put('0');
        return;
    }
    
    // Is negative?
    if (UNLIKELY(number < 0)) {
        console_put('-');
        number = -number;
    }
    
    // Get characters in reverse order
    uint32_t i = 0;
    uint8_t out[16];
    
    for (i = 15; number != 0; --i) {
        uint32_t digit = number / 10;
        number = number % 10;

        out[i] = (int8_t) ('0' + digit);
    }

    // Print in correct order
    for (++i; i < 16; ++i)
        console_put(out[i]);
}
