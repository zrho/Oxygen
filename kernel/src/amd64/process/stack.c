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
#include <api/type.h>
#include <api/process/stack.h>

// TODO: Complete this somewhen

//----------------------------------------------------------------------------//
// Internal
//----------------------------------------------------------------------------//

static void _stack_copy_switch(uintptr_t address, uintptr_t size, uintptr_t initial_addr, bool switch_stack)
{
    // Get stack and base pointer
    uintptr_t old_rsp;
    asm volatile("mov %%rsp, %0" : "=r" (old_rsp));
    
    uintptr_t old_rbp;
    asm volatile("mov %%rbp, %0" : "=r" (old_rbp));
    
    // Calculate offset to add to RSP/RBP
    uintptr_t offset = address - initial_addr;
    
    // New pointers
    uintptr_t new_rsp = old_rsp + offset;
    uintptr_t new_rbp = old_rbp + offset;
    
    // Copy the stack
    memcpy((void *) new_rsp, (void *) old_rsp, initial_addr - old_rsp);
    
    // Switch the stack?
    if (
}

//----------------------------------------------------------------------------//
// Copying
//----------------------------------------------------------------------------//

void stack_copy_switch(uintptr_t address, uintptr_t size, uintptr_t initial_addr);
{
    // Copy the stack
    stack_copy(address, size, initial_addr);
}

void stack_copy()
{

}
