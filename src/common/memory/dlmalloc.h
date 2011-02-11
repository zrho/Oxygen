/*
 * Copyright (c) 2008 James Molloy, Jörg Pfähler, Matthew Iselin
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 
#pragma once

#include <api/types.h>

//------------------------------------------------------------------------------
// Doug Lea's Malloc - Definitions
//------------------------------------------------------------------------------

#define ABORT dlmalloc_abort
#define HAVE_MORECORE 1
#define MORECORE heap_sbrk
#define MORECORE_CANNOT_TRIM 1
#define MORECORE_CONTIGUOUS 1
#define HAVE_MMAP 0
#define MALLOC_FAILURE_ACTION

#define LACKS_UNISTD_H 1
#define LACKS_FCNTL_H 1
#define LACKS_SYS_PARAM_H 1
#define LACKS_SYS_MMAN_H 1
#define LACKS_STRINGS_H 1
#define LACKS_STRING_H 1
#define LACKS_SYS_TYPES_H
#define LACKS_ERRNO_H
#define LACKS_STDLIB_H

#define DEBUG 1
#define FOOTERS 1


