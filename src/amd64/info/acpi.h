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
#include <api/compiler.h>

//----------------------------------------------------------------------------//
// ACPI - Root Structures
//----------------------------------------------------------------------------//

/**
 * ACPI's Root System Description Pointer for versions 1.0 and 2.0.
 */
typedef struct acpi_rsdp_t
{
    /** 
     * Signature of the RSDP. Always equals "RSD PTR " (notice the last space
     * character).
     *
     * Version 1.0
     */
    int8_t     signature[8];
    
    /**
     * The checksum for the version 1.0 part of the RSDP.
     *
     * Version 1.0
     */
    uint8_t     checksum;
    
    /**
     * ID of the OEM.
     *
     * Version 1.0
     */
    int8_t     oemid[6];
    
    /**
     * ACPI's revision.
     *
     * Version 1.0.
     */
    uint8_t     revision;
    
    /**
     * Physical address of the RSDT (32 bit).
     *
     * Version 1.0
     */
    uint32_t    rsdt_addr;
    
    /**
     * Size of entire table from offset 0 to the end.
     *
     * Version 2.0
     */
    uint32_t    length;
    
    /**
     * Physical address of the XSDT (64 bit).
     *
     * Version 2.0
     */
    uint64_t    xsdt_addr;
    
    /**
     * Checksum for both the version 1.0 and version 2.0 fields.
     *
     * Version 2.0
     */
    uint8_t     checksum_ext;
    
    /**
     * Reserved bytes.
     *
     * Version 2.0
     */
    uint8_t     reserved[3];
} PACKED acpi_rsdp_t;

/**
 * Header for ACPI's System Descriptor Tables.
 */
typedef struct acpi_sdt_header_t
{
    /**
     * Signature of the SDT.
     */
    uint8_t     signature[4];
    
    /**
     * The length of the SDT.
     */
    uint32_t    length;
    
    /**
     * The revision of the SDT.
     */
    uint8_t     revision;
    
    /**
     * The checksum of the SDT.
     */
    uint8_t     checksum;
    
    /**
     * The ID of the SDT's OEM.
     */
    int8_t      oemid[6];
    
    /**
     * The OEM Table ID.
     */
    int8_t      oem_tbl_id[8];
    
    /**
     * The OEM revision.
     */
    uint32_t    oem_rev;
    
    /**
     * The SDT creator's id.
     */
    uint32_t    creator_id;
    
    /**
     * The SDT creator's revision.
     */
    uint32_t    creator_rev;
    
} PACKED acpi_sdt_header_t;

//----------------------------------------------------------------------------//
// ACPI - FADT
//----------------------------------------------------------------------------//

/**
 * Fixed ACPI Description Table (SDT signature "FACP").
 */
typedef struct acpi_fadt_t
{
    /**
     * Header of this SDT.
     */
    acpi_sdt_header_t header;
    
    /**
     * Physical address of FACS (32 bit).
     */
    uint32_t facs_addr;
    
    /**
     * Physical address of DSDT (32 bit).
     */
    uint32_t dsdt_addr;
} PACKED acpi_fadt_t;

//----------------------------------------------------------------------------//
// ACPI - MADT
//----------------------------------------------------------------------------//

#define ACPI_MADT_PCAT_COMPAT (1 << 0)

/**
 * Multiple APIC Description Table (SDT signature "MADT").
 *
 * Structure is followed by several structures, including
 *  * acpi_madt_lapic_t
 *  * acpi_madt_io_apic_t
 */
typedef struct acpi_madt_t
{
    /**
     * Header of this SDT.
     */
    acpi_sdt_header_t header;
    
    /**
     * Physical address each processor can access its LAPIC at (32 bit).
     */
    uint32_t lapic_addr;
    
    /**
     * Several flags.
     */
    uint32_t flags;
    
} PACKED acpi_madt_t;

#define ACPI_MADT_LAPIC_ENABLED (1 << 0)
#define ACPI_MADT_LAPIC_TYPE 0
#define ACPI_MADT_IO_APIC_TYPE 1

/**
 * Entry in MADT for Processor LAPICs (Type 0)
 */
typedef struct acpi_madt_lapic_t
{
    /**
     * Type of the MADT entry (Value 0).
     */
    uint8_t type;
    
    /**
     * Length of this MADT entry (Value 8).
     */
    uint8_t length;
    
    /**
     * The processor's ACPI id.
     */
    uint8_t acpi_id;
    
    /**
     * The processor's APIC id.
     */
    uint8_t apic_id;
    
    /**
     * Local APIC flags.
     */
    uint32_t flags;
} PACKED acpi_madt_lapic_t;

/**
 * Entry in MADT for I/O APICs (Type 1).
 */
typedef struct acpi_madt_io_apic_t
{
    /**
     * Type of the MADT entry (Value 1).
     */
    uint8_t type;
    
    /**
     * Length of this MADT entry (Value 8).
     */
    uint8_t length;
    
    /**
     * The I/O APIC's ID.
     */
    uint8_t apic_id;
    
    /**
     * A reserved byte.
     */
    uint8_t reserved;
    
    /**
     * The (unique) physical address to access this I/O APIC (32 bits).
     */
    uint32_t address;
    
    /**
     * The Global System Interrupt Base.
     *
     * The interrupt number where this I/O APIC's interrupt inputs start.
     */
    uint32_t int_base;
} PACKED acpi_madt_io_apic_t;

//----------------------------------------------------------------------------//
// ACPI - Parsing
//----------------------------------------------------------------------------//

/**
 * Parses the information given in the ACPI tables and initializes the
 * respective kernel subsystems.
 */
void acpi_parse();
