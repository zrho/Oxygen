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
#include <api/syscall.h>

#include <api/cpu/int.h>
#include <api/debug/console.h>

#include <api/multitasking.h>
#include <api/multitasking/process.h>

#include <amd64/syscall.h>

#include <amd64/cpu/int.h>

//----------------------------------------------------------------------------//
// Syscall - Macros
//----------------------------------------------------------------------------//

#define SYSCALL_BINDING(name) static void _syscall_##name(syscall_id_t id, cpu_int_state_t *ctx)

//----------------------------------------------------------------------------//
// Syscall - Types
//----------------------------------------------------------------------------//

typedef void (*_syscall_handler_t)(syscall_id_t, cpu_int_state_t *);

//----------------------------------------------------------------------------//
// Binding - mmap
//----------------------------------------------------------------------------//

/**
 * Binding of the <tt>mmap</tt> system call.
 *
 * Input:
 * R8: Virtual address
 * R9: Physical address
 * R10: Pid of target process.
 * R11: Flags
 *
 * Output:
 * RAX: Return code
 */
SYSCALL_BINDING(mmap) {
    // Get calling process
    process_t *caller = cpu_get(cpu_current_id())->process;

    // Invoke system call
    syscall_ret_t result = syscall_mmap(
        ctx->r8, ctx->r9, ctx->r10, ctx->r11 & 0xFF, caller);
        
    // Set result
    ctx->ax = result;
}

//----------------------------------------------------------------------------//
// Syscall - Map
//----------------------------------------------------------------------------//

static _syscall_handler_t _syscall_handlers[SYSCALL_COUNT_MAX] = {
    _syscall_mmap};

//----------------------------------------------------------------------------//
// Syscall - Interrupt Handler
//----------------------------------------------------------------------------//

void syscall_int_handler(interrupt_vector_t vector, void *_ctx)
{
    // Cast context
    cpu_int_state_t *ctx = (cpu_int_state_t *) _ctx;

    // Get syscall id
    syscall_id_t id = (syscall_id_t) ctx->ax;
    
    if (UNLIKELY(id >= SYSCALL_COUNT_MAX || 0 == _syscall_handlers[id]))
        return;
        
    // Call handler
    _syscall_handler_t handler = _syscall_handlers[id];
    handler(id, ctx);
}
