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

/* strchrnul benchmark */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

char *strchrnul(const char *, int);

struct testparam
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .maxchar = 127, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .maxchar = 127, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .maxchar = 127, .xseed = { 345, 678, 910 }};

static void
dostrchrnulbench(const char *buf, size_t n)
{
	const char *volatile pos;
	size_t i;

	pos = buf;
	for (i = 0; i < n; i++)
		/* this should never match */
		pos = strchrnul(pos, 128) + 1;
}

static void
strchrnulbench(struct B *b, void *payload)
{
	struct testparam *param;
	char *buf;
	size_t n;
	long i;

	param = payload;
	b->bytes = param->buflen;
	buf = gentests(param);
	free(mkpointers(&n, buf, param->buflen));

	dostrchrnulbench(buf, n);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		dostrchrnulbench(buf, n);
	stoptimer(b);

	free(buf);
}

extern int
main(void)
{
	preamble();

	runbenchmark("short", strchrnulbench, (void *)&shortparam);
	runbenchmark("mid", strchrnulbench, (void *)&midparam);
	runbenchmark("long", strchrnulbench, (void *)&longparam);
}
