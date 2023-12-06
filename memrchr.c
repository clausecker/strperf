/*-
 * Copyright (c) 2023 The FreeBSD Foundation
 *
 * This software was developed by Robert Clausecker <fuz@FreeBSD.org>
 * under sponsorship from the FreeBSD Foundation.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ''AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE
 */

/* memrchr benchmark */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

struct testparam
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .charlen = 1, .maxchar = 127, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .charlen = 1, .maxchar = 127, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = 127, .xseed = { 345, 678, 910 }};

static void
domemrchrbench(void **ptrs, size_t n)
{
	volatile uintptr_t accum = 0;
	size_t i;

	for (i = 0; i < n; i++)
		/* this should never match */
		accum += (uintptr_t)memrchr(ptrs[i], 128, (size_t)(ptrs[i + 1] - ptrs[i]));
}

static void
memrchrbench(struct B *b, void *payload)
{
	struct testparam *param;
	size_t n;
	long i;
	void **ptrs;

	param = payload;
	b->bytes = param->buflen;
	ptrs = genmemtests(param, &n);

	domemrchrbench(ptrs, n);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		domemrchrbench(ptrs, n);
	stoptimer(b);

	free(ptrs);
}

extern int
main(void)
{
	preamble();

	runbenchmark("short", memrchrbench, (void *)&shortparam);
	runbenchmark("mid", memrchrbench, (void *)&midparam);
	runbenchmark("long", memrchrbench, (void *)&longparam);
}
