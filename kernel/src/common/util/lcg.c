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
#include <api/util/random.h>

//----------------------------------------------------------------------------//
// LCG - Constants
//----------------------------------------------------------------------------//

#define LCG_M                   0x1000000000000
#define LCG_A                   25214903917
#define LCG_C                   11

//----------------------------------------------------------------------------//
// LCG - Variables
//----------------------------------------------------------------------------//

uint64_t _lcg_seed_usage = -1;
uint64_t _lcg_seed = 0;

//----------------------------------------------------------------------------//
// LCG
//----------------------------------------------------------------------------//

uint32_t random_fast_next()
{
    // Generate new seed?
    if (_lcg_seed_usage >= LCG_M) {
        _lcg_seed = time_current();
        _lcg_seed_usage = 0;
    }
    
    // Increase seed usage count
    ++_lcg_seed_usage;
        
    // Generate next number
    uint64_t next;
    _lcg_seed = next = (LCG_A * _lcg_seed + LCG_C) % LCG_M;
    
    // Return bits 16-47
    return (next >> 16) & 0xFFFFFFFF;
}
