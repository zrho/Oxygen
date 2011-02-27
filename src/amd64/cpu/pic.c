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
#include <amd64/cpu/int.h>
#include <amd64/cpu/pic.h>
#include <amd64/io/io.h>

//----------------------------------------------------------------------------//
// PIC - Constants
//----------------------------------------------------------------------------//

/** @see http://http://wiki.osdev.org/PIC */
#define IO_PIC1                     0x20
#define IO_PIC2                     0xA0
#define IO_PIC1_COMMAND             IO_PIC1
#define IO_PIC1_DATA                (IO_PIC1 + 1)
#define IO_PIC2_COMMAND             IO_PIC2
#define IO_PIC2_DATA                (IO_PIC2 + 1)

#define PIC_ICW1_ICW4	            0x01		/* ICW4 (not) needed */
#define PIC_ICW1_SINGLE	            0x02		/* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4          0x04		/* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL              0x08		/* Level triggered (edge) mode */
#define PIC_ICW1_INIT               0x10		/* Initialization - required! */
 
#define PIC_ICW4_8086               0x01		/* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO               0x02		/* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE          0x08		/* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER         0x0C		/* Buffered mode/master */
#define PIC_ICW4_SFNM               0x10		/* Special fully nested (not) */

//----------------------------------------------------------------------------//
// PIC - Constants
//----------------------------------------------------------------------------//

void cpu_pic_init(void)
{
    // ICW1: Start initialization sequence
    io_outb(IO_PIC1_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
    io_outb(IO_PIC2_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
    
    // ICW2: Define PIC vectors
    io_outb(IO_PIC1_DATA, INT_PIC_IRQ_OFFSET);
    io_outb(IO_PIC2_DATA, INT_PIC_IRQ_OFFSET + 7);
    
    // ICW3: Continue init sequence
    io_outb(IO_PIC1_DATA, 4);
    io_outb(IO_PIC2_DATA, 2);
    
    // ICW4: Additional flags
    io_outb(IO_PIC1_DATA, PIC_ICW4_8086);
    io_outb(IO_PIC2_DATA, PIC_ICW4_8086);
    
    // Mask all
    io_outb(IO_PIC1_DATA, 0xFF & ~(1 << 2));
    io_outb(IO_PIC2_DATA, 0xFF);
}

void cpu_pic_eoi(uint8_t irq)
{
    // PIC2?
    if (irq > 7)
        io_outb(0xA0, 0x20);
    
    // PIC1
    io_outb(0x20, 0x20);
}

void cpu_pic_mask(uint8_t irq)
{
    // IRQ 2 cannot be masked
    if (2 == irq) return;

    // PIC 1 or PIC 2?
    if (irq > 7 && irq <= 15)
        io_outb(IO_PIC2_DATA, io_inb(IO_PIC2_DATA) | (1 << (irq - 8)));
    else if (irq <= 7)
        io_outb(IO_PIC1_DATA, io_inb(IO_PIC1_DATA) | (1 << irq));

}

void cpu_pic_unmask(uint8_t irq)
{
    // IRQ 2 is always unmasked
    if (2 == irq) return;

    // PIC 1 or PIC 2?
    if (irq > 7 && irq <= 15)
        io_outb(IO_PIC2_DATA, io_inb(IO_PIC2_DATA) & ~(1 << (irq - 8)));
    else if (irq <= 7)
        io_outb(IO_PIC1_DATA, io_inb(IO_PIC1_DATA) & ~(1 << irq));
}

void cpu_pic_disable()
{
    // Disable PIT2
    io_outb(0xA1, 0xFF);
    
    // Disable PIT1
    io_outb(0x21, 0xFF);
}
