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

/* strspn benchmark */

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

enum { EXTRACHARS = 64 };

struct testparam /* maxchar to be overridden in main function */
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .charlen = 1, .maxchar = -1, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .charlen = 1, .maxchar = -1, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = -1, .xseed = { 345, 678, 910 }};

static void
dostrspnbench(const char *buf, const char *set, size_t n)
{
	const char *volatile pos;
	size_t i;

	pos = buf;
	for (i = 0; i < n; i++)
		pos += strspn(pos, set) + 1;
}

static void
strspnbench(struct B *b, void *payload)
{
	struct testparam *param;
	char *buf, set[64];
	size_t n;
	long i;
	int j;

	param = payload;
	b->bytes = param->buflen;
	buf = gentests(param);
	free(mkpointers(&n, buf, param->buflen));

	i = 0;
	for (j = 1; j <= param->maxchar; j++)
		set[i++] = j;

	set[i] = '\0';

	assert(i <= sizeof set);

	dostrspnbench(buf, set, n);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		dostrspnbench(buf, set, n);
	stoptimer(b);

	free(buf);
}

extern int
main(void)
{
	preamble();

	shortparam.maxchar = 1;
	runbenchmark("short1", strspnbench, (void *)&shortparam);
	midparam.maxchar = 1;
	runbenchmark("mid1", strspnbench, (void *)&midparam);
	longparam.maxchar = 1;
	runbenchmark("long1", strspnbench, (void *)&longparam);

	shortparam.maxchar = 5;
	runbenchmark("short5", strspnbench, (void *)&shortparam);
	midparam.maxchar = 5;
	runbenchmark("mid5", strspnbench, (void *)&midparam);
	longparam.maxchar = 5;
	runbenchmark("long5", strspnbench, (void *)&longparam);

	shortparam.maxchar = 20;
	runbenchmark("short20", strspnbench, (void *)&shortparam);
	midparam.maxchar = 20;
	runbenchmark("mid20", strspnbench, (void *)&midparam);
	longparam.maxchar = 20;
	runbenchmark("long20", strspnbench, (void *)&longparam);

	shortparam.maxchar = 40;
	runbenchmark("short40", strspnbench, (void *)&shortparam);
	midparam.maxchar = 40;
	runbenchmark("mid40", strspnbench, (void *)&midparam);
	longparam.maxchar = 40;
	runbenchmark("long40", strspnbench, (void *)&longparam);
}
