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

//----------------------------------------------------------------------------//
// IPI - Constants
//----------------------------------------------------------------------------//

#define IPI_DELIVERY_FIXED              0x0
#define IPI_DELIVERY_LOWEST_PRIORITY    0x1
#define IPI_DELIVERY_SMI                0x2
#define IPI_DELIVERY_NMI                0x4
#define IPI_DELIVERY_INIT               0x5
#define IPI_DELIVERY_STARTUP            0x6

#define IPI_DEST_DEST_FIELD             0x0
#define IPI_DEST_SELF                   0x1
#define IPI_DEST_ALL_INC_SELF           0x2
#define IPI_DEST_ALL_EX_SELF            0x3

#define IPI_MODE_PHYSICAL               0x0
#define IPI_MODE_LOGICAL                0x1

#define IPI_LEVEL_DEASSERT              0x0
#define IPI_LEVEL_ASSERT                0x1

//----------------------------------------------------------------------------//
// IPI
//----------------------------------------------------------------------------//

/**
 * Sends an IPI, given its vector, destination, destination shorthand and delivery
 * mode.
 *
 * Defaults destination mode to physical, level to assert and trigger mode to
 * edge.
 *
 * @param vector The interrupt vector.
 * @param dest The interrupt's destination.
 * @param shorthand The interrupt's destination shorthand.
 * @param mode The interrupt's destination mode. 0 for physical, 1 for logical.
 * @param delivery The interrupt's delivery mode.
 * @param level The interrupt's level. 1 for assert, 0 for de-assert.
 * @param cpu The current CPU structure used for locking.
 */
void cpu_ipi(
    uint8_t vector, uint8_t dest, uint8_t shorthand,
    uint8_t mode, uint8_t delivery, uint8_t level, cpu_t *cpu);
