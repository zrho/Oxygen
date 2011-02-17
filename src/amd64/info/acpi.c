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
 
// TODO: Maybe clean this code up later...
 
#include <api/types.h>
#include <api/string.h>
#include <api/cpu.h>

#include <api/memory/page.h>
#include <api/memory/heap.h>

#include <api/debug/console.h>

#include <amd64/io/io.h>

#include <amd64/info/acpi.h>

#include <amd64/cpu.h>

//----------------------------------------------------------------------------//
// ACPI - Constants
//----------------------------------------------------------------------------//

#define ACPI_AUX_VIRT       0xFFFFFF7FFFE00000
#define BIOS_MAIN_ADDR      0xE0000
#define BIOS_MAIN_LENGTH    0x20000

//----------------------------------------------------------------------------//
// ACPI - Variables
//----------------------------------------------------------------------------//

static uint8_t acpi_table_count = 0;
static uint64_t *acpi_tables;
static uintptr_t acpi_tmp_mapping = ACPI_AUX_VIRT;

//----------------------------------------------------------------------------//
// ACPI - Internal - Mapping
//----------------------------------------------------------------------------//

static void *_acpi_tmp_map(uintptr_t phys, uintptr_t length)
{
    // Calculate offset
    uintptr_t offset = (phys & 0xFFF);
    
    // Align down address
    phys = phys & ~0xFFF;
    
    // Calculate number of required pages
    size_t pages = mem_align(offset + length, 0x1000) / 0x1000;
    
    // Map
    size_t i;
    for (i = 0; i < pages; ++i) {
        page_map(
            acpi_tmp_mapping,
            phys + i * 0x1000,
            PG_PRESENT | PG_WRITABLE);
        acpi_tmp_mapping += 0x1000;
    }
    
    // Return address of the mapped structure
    return (void *) (acpi_tmp_mapping - pages * 0x1000 + offset);
}

static void _acpi_tmp_unmap_all()
{
    // Unmap pages
    while (acpi_tmp_mapping > ACPI_AUX_VIRT) {
        acpi_tmp_mapping -= 0x1000;
        page_unmap(acpi_tmp_mapping);
    }
}

//----------------------------------------------------------------------------//
// ACPI - Internal - Table Discovery
//----------------------------------------------------------------------------//

static bool _acpi_rsdp_check(acpi_rsdp_t *rsdp)
{
    // Check signature
    if (!memcmp((int8_t *) &rsdp->signature, (int8_t *) "RSD PTR ", 8))
        return false;
        
    // Checksum
    uint8_t sum = 0;
    uint8_t *data = (uint8_t *) rsdp;
    size_t i;
    
    for (i = 0; i < sizeof(acpi_rsdp_t); ++i)
        sum += data[i];
        
    return (sum == 0);
}

static uintptr_t _acpi_find_rsdp()
{
    acpi_rsdp_t *rsdp = 0;

    // Find begin address of EBDA
    // To do this, map first 4KB
    page_map(ACPI_AUX_VIRT, 0x0, PG_WRITABLE | PG_PRESENT);
    uintptr_t ebda_addr = mem_align(*((uint16_t *) (0x040E + ACPI_AUX_VIRT)), 0x10);
    
    uintptr_t ebda_virt = (uintptr_t) _acpi_tmp_map(ebda_addr, 0x400);
    ebda_virt = mem_align(ebda_virt, 0x10);
    uintptr_t offset;
    
    for (offset = 0; offset < 0x400 - sizeof(acpi_rsdp_t); offset += 0x10) {
        rsdp = (acpi_rsdp_t *) (ebda_virt + offset);
        
        if (_acpi_rsdp_check(rsdp)) {
            // Unmap
            _acpi_tmp_unmap_all();
        
            // Return RSDP address
            return ebda_addr + offset;
        }
    }
    
    _acpi_tmp_unmap_all();
    
    // Map main BIOS area
    for (offset = 0; offset < BIOS_MAIN_LENGTH; offset += 0x1000)
        page_map(
            ACPI_AUX_VIRT + offset,
            BIOS_MAIN_ADDR + offset,
            PG_WRITABLE | PG_PRESENT);
            
    // Seach for RSDP
    uintptr_t rsdp_phys = 0;
    
    for (offset = 0; offset < BIOS_MAIN_LENGTH - sizeof(acpi_rsdp_t); offset += 0x10) {
        rsdp = (acpi_rsdp_t *) (ACPI_AUX_VIRT + offset);

        if (_acpi_rsdp_check(rsdp)) {
            rsdp_phys = BIOS_MAIN_ADDR + offset;
            break;
        }
    }
    
    // Unmap pages
    for (offset = 0; offset < BIOS_MAIN_LENGTH; offset += 0x1000)
        page_unmap(ACPI_AUX_VIRT + offset);
        
    // TODO: Panic if not found
    if (0 == rsdp_phys) {
        console_print("No RSDP found!");
        while (1);
    }
        
    // Return RSDP address
    return rsdp_phys;
}

/**
 * Tries to find the system's ACPI tables.
 *
 * Stores the results in <tt>acpi_table_count</tt> and <tt>acpi_tables</tt>.
 */
static void _acpi_find_tables()
{
    // (X|R)SDT pointer
    acpi_rsdp_t *rsdp = (acpi_rsdp_t *) _acpi_tmp_map(_acpi_find_rsdp(), sizeof(acpi_rsdp_t));
    acpi_sdt_header_t *rsdt = (acpi_sdt_header_t *) (uintptr_t) rsdp->rsdt_addr;
    if (rsdp->revision >= 1) rsdt = (acpi_sdt_header_t *) rsdp->xsdt_addr;
        
    // RSDT
    rsdt = (acpi_sdt_header_t *) _acpi_tmp_map((uintptr_t) rsdt, 0x1000); // TODO: Maybe find out real size?
    
    // Number of addresses
    uintptr_t addr_len = (rsdp->revision < 1) ? 4 : 8;
    acpi_table_count = (rsdt->length - sizeof(acpi_sdt_header_t)) / addr_len;
    uintptr_t addr_ptr = (uintptr_t) rsdt + sizeof(acpi_sdt_header_t);
    
    if (4 == addr_len) {
        // Create 8 byte pointers
        acpi_tables = (uint64_t *) malloc(8 * acpi_table_count);
        size_t i;
        
        for (i = 0; i < acpi_table_count; ++i)
            acpi_tables[i] = ((uint32_t *) addr_ptr)[i];
        
    } else // 8 == addr_len
        acpi_tables = ((uint64_t *) addr_ptr);
    
    // Unmap all temporary mappings
    _acpi_tmp_unmap_all();
}

//----------------------------------------------------------------------------//
// ACPI - Internal - MADT Parsing
//----------------------------------------------------------------------------//

static void _acpi_parse_madt_lapic(acpi_madt_lapic_t *lapic_tbl)
{
    // Flags
    uint8_t flags = 0;
    if (lapic_tbl->flags & ACPI_MADT_LAPIC_ENABLED)
        flags |= CPU_FLAG_ENABLED;

    // Create processor
    cpu_t cpu = {lapic_tbl->apic_id, flags};
    cpu_add(cpu);
}

static void _acpi_parse_madt_io_apic(acpi_madt_io_apic_t *apic_tbl)
{
    // TODO: Add I/O APIC
}

static void _acpi_parse_madt(acpi_madt_t *madt)
{
    // Store LAPIC address
    cpu_set_lapic(madt->lapic_addr);
    
    // Iterate over tables
    void *current = (void *) ((uintptr_t) madt + sizeof(acpi_madt_t));
    uintptr_t end = (uintptr_t) madt + madt->header.length;
    
    while ((uintptr_t) current < end) {
        uint8_t *generic = (uint8_t *) current;
    
        // LAPIC?
        if (ACPI_MADT_LAPIC_TYPE == generic[0])
            _acpi_parse_madt_lapic((acpi_madt_lapic_t *) current);
            
        // I/O APIC
        else if (ACPI_MADT_IO_APIC_TYPE == generic[0])
            _acpi_parse_madt_io_apic((acpi_madt_io_apic_t *) current);
        
        // Next table
        current = (void *) ((uintptr_t) current + generic[1]);
    }
}

//----------------------------------------------------------------------------//
// ACPI - Parsing
//----------------------------------------------------------------------------//

void acpi_parse()
{
    // Find ACPI tables
    _acpi_find_tables();
    
    // Iterate on tables
    size_t i;
    for (i = 0; i < acpi_table_count; ++i) {
        // Current table's header
        acpi_sdt_header_t *header = (acpi_sdt_header_t *) (acpi_tables[i]);
        
        // (For some reason QEMU crashes on SMP>2 without any I/O...)
        io_inb(0x3D4);
        
        // Map
        acpi_sdt_header_t *_header = (acpi_sdt_header_t *) _acpi_tmp_map(
            (uintptr_t) header, sizeof(acpi_sdt_header_t));
        uintptr_t length = _header->length;
        header = (acpi_sdt_header_t *) _acpi_tmp_map((uintptr_t) header, length);
        
        // Check on table's parse
        if (memcmp((int8_t *) header->signature, "APIC", 4) ||
            memcmp((int8_t *) header->signature, "MADT", 4))
            _acpi_parse_madt((acpi_madt_t *) header);
            
        // Unmap
        _acpi_tmp_unmap_all();
    }
}
