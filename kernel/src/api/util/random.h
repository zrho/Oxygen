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
// Random
//----------------------------------------------------------------------------//

/**
 * Returns a random number generated by a fast, but insecure non-CSPRNG.
 *
 * Numbers generated by this function should NOT be used in ANY application that
 * feature cryptography or require high randomness for other reasons.
 *
 * @return Random 32 bit number.
 */
uint32_t random_fast_next(void);

/**
 * Returns a seed determined by some frequently chaging variable, like real time.
 *
 * @return Seed for pseudo-random number generation.
 */
uint32_t random_seed(void);
