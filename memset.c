/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Strahinja Stanisic <strajabot@FreeBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* memset benchmark */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

struct testparam
    param40   = { .avglen = 40,   .buflen = 1 << 18, .charlen = 1, .maxchar = 127, .xseed = { 234, 567, 890 }},
    param168  = { .avglen = 168,  .buflen = 1 << 18, .charlen = 1, .maxchar = 127, .xseed = { 0x3336, 0x8176, 0xfa5e }},
    param2k   = { .avglen = 2048, .buflen = 1 << 18, .charlen = 1, .maxchar = 127, .xseed = { 0x1337, 0x4223, 0xf001 }},
    param256k = { .avglen = 1e18, .buflen = 1 << 18, .charlen = 1, .maxchar = 127, .xseed = { 0xdead, 0xbeef, 0xfee1 }},
    param16m  = { .avglen = 1e18, .buflen = 1 << 24, .charlen = 16, .maxchar = 127, .xseed = { 0xc0fe, 0xf00d, 0xb01d }},
    param1g   = { .avglen = 1e18, .buflen = 1 << 30, .charlen = 16, .maxchar = 127, .xseed = { 4711, 3125, 4096 }};

static void
domemsetbench(void **ptrs, char *dest, size_t n)
{
	size_t i;
	void *src;

	src = ptrs[0];
	for (i = 0; i < n; i++)
		memset(dest + (ptrs[i] - src), '1', (size_t)(ptrs[i + 1] - ptrs[i]));
}

static void
memsetbench(struct B *b, void *payload)
{
	struct testparam *param;
	size_t n;
	long i;
	void **ptrs;
	char *dest;

	param = payload;
	b->bytes = param->buflen;
	ptrs = genmemtests(param, &n);
	dest = malloc(param->buflen);
	if (dest == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	domemsetbench(ptrs, dest, n);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		domemsetbench(ptrs, dest, n);
	stoptimer(b);

	free(dest);
	free(ptrs);
}

extern int
main(void)
{
	preamble();

	runbenchmark("40", memsetbench, (void *)&param40);
	runbenchmark("168", memsetbench, (void *)&param168);
	runbenchmark("2k", memsetbench, (void *)&param2k);
	runbenchmark("256k", memsetbench, (void *)&param256k);
	runbenchmark("16m", memsetbench, (void *)&param16m);
	runbenchmark("1g", memsetbench, (void *)&param1g);
}
