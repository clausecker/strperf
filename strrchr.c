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

/* strrchr benchmark */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

char *strrchr(const char *, int);

struct testparam
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .charlen = 1, .maxchar = 127, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .charlen = 1, .maxchar = 127, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = 127, .xseed = { 345, 678, 910 }};

static void
dostrrchrbench(char **ptrs, size_t n)
{
	volatile uintptr_t sum = 0;
	size_t i;

	for (i = 0; ptrs[i+1] != NULL; i++)
		sum += (uintptr_t)strrchr(ptrs[i], 128) + 1;
}

static void
strrchrbench(struct B *b, void *payload)
{
	struct testparam *param;
	char *buf;
	char **ptrs;
	size_t n;
	long i;

	param = payload;
	b->bytes = param->buflen;
	buf = gentests(param);
	ptrs = mkpointers(&n, buf, param->buflen);

	dostrrchrbench(ptrs, n);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		dostrrchrbench(ptrs, n);
	stoptimer(b);

	free(ptrs);
	free(buf);
}

extern int
main(void)
{
	preamble();

	runbenchmark("short", strrchrbench, (void *)&shortparam);
	runbenchmark("mid", strrchrbench, (void *)&midparam);
	runbenchmark("long", strrchrbench, (void *)&longparam);
}
