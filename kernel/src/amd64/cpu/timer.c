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
#include <api/cpu/timer.h>
#include <amd64/cpu/timer.h>
#include <amd64/cpu/lapic.h>
#include <amd64/cpu/pit.h>
#include <amd64/cpu/pic.h>
#include <amd64/cpu/int.h>
#include <api/memory/heap.h>
#include <api/cpu/int.h>

//----------------------------------------------------------------------------//
// Timer - Constants
//----------------------------------------------------------------------------//

/**
 * The frequency to use for the PIT for timer calibration (in Hz).
 *
 * 1000 Hz equal an interval of one millisecond.
 */
#define TIMER_INIT_FREQ                 1000

/**
 * The timer's interval in microseconds.
 */
#define TIMER_INTERVAL                  100

//----------------------------------------------------------------------------//
// Timer - Structures
//----------------------------------------------------------------------------//

/**
 * Structure for registered timer handlers.
 */
typedef struct cpu_timer_handler_t
{
    /**
     * The handler's callback.
     */
    timer_handler_t callback;
    
    /**
     * The handler's granularity.
     */
    uint32_t granularity;
    
    /**
     * Pointer to next handler structure.
     */
    struct cpu_timer_handler_t *next;
    
} cpu_timer_handler_t;

//----------------------------------------------------------------------------//
// Timer - Variables
//----------------------------------------------------------------------------//

/**
 * The current timer's ticks.
 */
static uint64_t _cpu_timer_ticks = 0;

/**
 * The current timer's initialization state.
 *
 * @see _cpu_timer_init_irq
 * @see _cpu_timer_init
 */
static uint32_t _cpu_timer_stage = 0;

/**
 * The timer's multiplier.
 *
 * The number the LAPIC's timer will count in one millisecond.
 */
static uint32_t _cpu_timer_multiplier = 0;

/**
 * A linked list of the registered timer handlers.
 */
static cpu_timer_handler_t *_cpu_timer_handlers = 0;

//----------------------------------------------------------------------------//
// Timer - Internal
//----------------------------------------------------------------------------//

/**
 * The IRQ handler to use for the LAPIC timer.
 */
static void *_cpu_timer_irq(uint8_t vector, void *ctx)
{
    // Increase tick count
    ++_cpu_timer_ticks;
    
    // Iterate over handlers
    cpu_timer_handler_t *current = _cpu_timer_handlers;
    while (0 != current) {
        // Check granularity
        if (0 == _cpu_timer_ticks % current->granularity)
            current->callback(_cpu_timer_ticks, ctx);
            
        // Next
        current = current->next;
    }
    
    // EOI
    cpu_lapic_eoi();
    
    return ctx;
}

/**
 * The IRQ handler to use for the PIT timer for LAPIC timer calibration.
 *
 * On the first call, the LAPIC Timer is initialized at a known count, on the
 * second the difference between the current and the initial LAPIC Timer count
 * is calculated and used as the timer multiplier.
 *
 * @param vector The interrupt vector.
 * @param ctx The interrupt context.
 */
static void *_cpu_timer_init_irq(uint8_t vector, void *ctx)
{
    // Increase stage number
    ++_cpu_timer_stage;

    // First timer IRQ?
    if (1 == _cpu_timer_stage) {
        // Set Initial Count Register to maximum 32 bit integer value
        *LAPIC_REGISTER(LAPIC_INIT_COUNT_OFFSET) = 0xFFFFFFFF;
        
    // Second timer IRQ?
    } else if (2 == _cpu_timer_stage) {
        // Save Current Count Register's value
        uint32_t current = *LAPIC_REGISTER(LAPIC_CURRENT_COUNT_OFFSET);
        
        // Calculate difference, i.e. the LAPIC counter ticks per milli second
        _cpu_timer_multiplier = 0xFFFFFFFF - current;
    }
    
    // Send EOI to PIC
    cpu_pic_eoi(0);
    
    return ctx;
}

//----------------------------------------------------------------------------//
// Timer - Handling
//----------------------------------------------------------------------------//

void cpu_timer_register(timer_handler_t handler, uint32_t granularity)
{
    // Create new handler structure
    cpu_timer_handler_t *_handler = malloc(sizeof(cpu_timer_handler_t));
    
    _handler->callback = handler;
    _handler->granularity = granularity;
    _handler->next = _cpu_timer_handlers;
    
    // Insert into list
    _cpu_timer_handlers = _handler;
}

void cpu_timer_unregister(timer_handler_t handler)
{
    // Iterate over handlers
    cpu_timer_handler_t *current = _cpu_timer_handlers;
    cpu_timer_handler_t *previous = 0;
    
    while (0 != current) {
        // Matches given handler callback?
        if (current->callback != handler) {
            current = current->next;
            continue;
        }
            
        // Remove from list
        if (0 == previous)
            _cpu_timer_handlers = current->next;
        else
            previous->next = current->next;
            
        // Free memory
        free(current);
        
        break;
    }
}

//----------------------------------------------------------------------------//
// Timer
//----------------------------------------------------------------------------//

void cpu_timer_init(bool calibrate)
{
    // Set LAPIC timer's Divide Configuration to a divisor of 1
    *LAPIC_REGISTER(LAPIC_DCR_OFFSET) = 0xB;

    // Calibrate?
    if (calibrate) {
        // Set PIT frequency
        cpu_pit_freq_set(TIMER_INIT_FREQ);
    
        // Register handler for PIT's IRQ
        cpu_int_register(INT_PIC_IRQ_OFFSET, &_cpu_timer_init_irq);
    
        // Enable PIT
        cpu_pit_enable();
    
        // Wait for timer calibration to complete
        while (3 > _cpu_timer_stage);
    
        // Disable PIT
        cpu_pit_disable();
    }
    
    // Configure timer interval
    *LAPIC_REGISTER(LAPIC_INIT_COUNT_OFFSET) =
         (TIMER_INTERVAL * _cpu_timer_multiplier) / 1000;

    // Interrupt Vector
    *LAPIC_REGISTER(LAPIC_LVT_OFFSET) =
        (INT_VECTOR_TIMER & 0xFF) |         // 0-7      (Vector)
        (1 << 17);                          // 17       (Timer Mode: Periodic)
        
    // Register interrupt handler
    cpu_int_register(INT_VECTOR_TIMER, &_cpu_timer_irq);
}

uint32_t cpu_timer_interval(void)
{
    return TIMER_INTERVAL;
}

uint64_t cpu_timer_ticks(void)
{
    return _cpu_timer_ticks;
}
