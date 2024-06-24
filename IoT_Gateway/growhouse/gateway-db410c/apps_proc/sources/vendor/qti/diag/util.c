/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Copyright (c) 2016, Bjorn Andersson <bjorn@kryo.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include "util.h"
#include "qdiag.h"

static uint8_t to_hex(uint8_t ch)
{
	ch &= 0xf;
	return ch <= 9 ? '0' + ch : 'a' + ch - 10;
}

void print_hex_dump(const char *prefix, const void *buf, size_t len)
{
	const uint8_t *ptr = buf;
	size_t linelen;
	uint8_t ch;
	char line[16 * 3 + 16 + 1];
	int li;
	int i;
	int j;

	for (i = 0; i < len; i += 16) {
		linelen = MIN(16, len - i);
		li = 0;

		for (j = 0; j < linelen; j++) {
			ch = ptr[i + j];
			line[li++] = to_hex(ch >> 4);
			line[li++] = to_hex(ch);
			line[li++] = ' ';
		}

		for (; j < 16; j++) {
			line[li++] = ' ';
			line[li++] = ' ';
			line[li++] = ' ';
		}

		for (j = 0; j < linelen; j++) {
			ch = ptr[i + j];
			line[li++] = isprint(ch) ? ch : '.';
		}

		line[li] = '\0';

		printf("%s %04x: %s\n", prefix, i, line);
	}
}

struct allocation
{
	void *p;
	size_t s;
} allocations[ALLOCATION_MAP_SIZE];

void *diag_malloc(uint8_t align, size_t s)
{
	static uint8_t first_time = true;
	int next_avail = ALLOCATION_MAP_SIZE;
	size_t input = s;

	void *p;
	if (first_time) {
		for (int i=0; i<ALLOCATION_MAP_SIZE; i++) {
			allocations[i].s=0;
#if defined USE_DIAG_MEM_POOL
			allocations[i].p=malloc((1024));
#else
			allocations[i].p=NULL;
#endif
		}
		first_time = false;
	}
	for (int i=0; i<ALLOCATION_MAP_SIZE; i++) {
		if (allocations[i].s == 0) {
			next_avail = i;
		    diag_dbg(DIAG_DBG_MEMORY, "next_avail=%u\n", next_avail);
			break;
		}
	}
	if (next_avail < ALLOCATION_MAP_SIZE) {
#if defined USE_DIAG_MEM_POOL
		p=allocations[next_avail].p;
#else
		if (align) {
			s = QUAD_ALIGNED(s);
		}
		p = malloc(s);
		free(p);
		p = malloc(s);
		allocations[next_avail].p = p;
#endif
		allocations[next_avail].s = s;
		diag_dbg(DIAG_DBG_MEMORY, "+ [%d %lu -> %p] %s%c\n", next_avail, allocations[next_avail].s, allocations[next_avail].p, align ? "+" : "", align ? (int)((s-input)+'0') : '0');
	} else {
		diag_dbg(DIAG_DBG_MEMORY, "NO MORE ALLOCATION SLOTS! Max of %d reached!\n", next_avail);
		p=NULL;
	}
	return p;
}

size_t diag_free(void *p)
{
	size_t s = 0;
	int found = ALLOCATION_MAP_SIZE;
	for (int i=0; i<ALLOCATION_MAP_SIZE; i++) {
		if (allocations[i].p == p) {
			found = i;
			break;
		}
	}
	if (found < ALLOCATION_MAP_SIZE) {
		diag_dbg(DIAG_DBG_MEMORY, "- [%d %lu -> %p]\n", found, allocations[found].s, allocations[found].p);
		s = allocations[found].s;
		allocations[found].s = 0;
	} else {
		diag_dbg(DIAG_DBG_MEMORY, "ALLOCATION SLOT NOT FOUND!\n");
		return(0);
	}
#if !defined USE_DIAG_MEM_POOL
	free(p);
	allocations[found].p = NULL;
#endif
	return s;
}

void diag_alloc_map()
{
	for (int i=0; i<ALLOCATION_MAP_SIZE; i++) {
		 allocations[i].s=0;
		 allocations[i].p=malloc((1024));
		diag_dbg(DIAG_DBG_MEMORY, ": [%d %lu -> %p]\n", i, allocations[i].s, allocations[i].p);
	}
}
