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
// Macroes
//----------------------------------------------------------------------------//

#define SPINLOCK_INIT(name) spinlock_t name = {0, 0}

//----------------------------------------------------------------------------//
// Types
//----------------------------------------------------------------------------//

typedef struct spinlock_t
{
    uint8_t lock;
    uint8_t flags;
} spinlock_t;

//----------------------------------------------------------------------------//
// Spinlock
//----------------------------------------------------------------------------//

/**
 * Acquires the given spinlock.
 *
 * If the lock is already acquired, block until it is released.
 *
 * @param lock The lock to acquire. 
 */
void spinlock_acquire(spinlock_t *lock);

/**
 * Releases the given spinlock.
 *
 * @param lock The lock to release.
 */
void spinlock_release(spinlock_t *lock);
