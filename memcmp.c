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

/* memcmp benchmark */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "benchmark.h"

static int (*mymemcmp)(const void *, const void *, size_t);

struct testparam
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 345, 678, 910 }};

static void
domemcmpbench(const char *bufa, const char *bufb, const char **ptrs)
{
	volatile int result = 0;
	size_t i;

	for (i = 0; ptrs[i+1] != NULL; i++)
		result += mymemcmp(ptrs[i], bufb + (ptrs[i] - bufa), (size_t)(ptrs[i+1] - ptrs[i]));
}

static void
memcmpbench(struct B *b, void *payload)
{
	struct testparam *param;
	char *src, *dest;
	const char **ptrs;
	long i;

	param = payload;
	b->bytes = param->buflen;
	src = gentests(param);
	ptrs = (const char **)mkpointers(NULL, src, param->buflen);
	dest = calloc(param->buflen, 1);
	if (dest == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	memcpy(src, dest, param->buflen);
	domemcmpbench(src, dest, ptrs);

	resettimer(b);
	for (i = 0; i < b->n; i++)
		domemcmpbench(src, dest, ptrs);
	stoptimer(b);

	free(dest);
	free(ptrs);
	free(src);
}

extern int
main(void)
{
	preamble();

	mymemcmp = memcmp;
	runbenchmark("memcmpShort", memcmpbench, (void *)&shortparam);
	runbenchmark("memcmpMid", memcmpbench, (void *)&midparam);
	runbenchmark("memcmpLong", memcmpbench, (void *)&longparam);

	mymemcmp = bcmp;
	runbenchmark("bcmpShort", memcmpbench, (void *)&shortparam);
	runbenchmark("bcmpMid", memcmpbench, (void *)&midparam);
	runbenchmark("bcmpLong", memcmpbench, (void *)&longparam);

#ifdef __FreeBSD__
	mymemcmp = timingsafe_bcmp;
	runbenchmark("tsBcmpShort", memcmpbench, (void *)&shortparam);
	runbenchmark("tsBcmpMid", memcmpbench, (void *)&midparam);
	runbenchmark("tsBcmpLong", memcmpbench, (void *)&longparam);

	mymemcmp = timingsafe_memcmp;
	runbenchmark("tsMemcmpShort", memcmpbench, (void *)&shortparam);
	runbenchmark("tsMemcmpMid", memcmpbench, (void *)&midparam);
	runbenchmark("tsMemcmpLong", memcmpbench, (void *)&longparam);
#endif
}
