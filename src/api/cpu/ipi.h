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
#include <api/cpu/int.h>
#include <api/cpu.h>

//----------------------------------------------------------------------------//
// IPI
//----------------------------------------------------------------------------//

/**
 * Broadcasts an IPI to either all CPUs including or exluding the current one,
 * given its interrupt vector.
 *
 * @param vector The vector of the IPI.
 * @param incl_self Whether or not to include the current processor in the
 *  list of CPUs the IPI is sent to.
 */
void cpu_ipi_broadcast(interrupt_vector_t vector, bool incl_self);

/**
 * Sends an IPI to a single CPU, given its id.
 *
 * @param vector The vector of the IPI.
 * @param cpu The id of the CPU the IPI is sent to.
 */
void cpu_ipi_single(interrupt_vector_t vector, cpu_id_t cpu);
