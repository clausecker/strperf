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

/* strcmp benchmark */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "benchmark.h"

struct testparam
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .charlen = 1, .maxchar = 16, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .charlen = 1, .maxchar = 16, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = 16, .xseed = { 345, 678, 910 }};

static void
dostrcmpbench(const char *bufa, const char *bufb, const char **ptrs)
{
	volatile int result = 0;
	size_t i;

	for (i = 0; ptrs[i+1] != NULL; i++)
		result += strcmp(ptrs[i], bufb + (ptrs[i] - bufa));
}

static void
strcmpbench(struct B *b, void *payload)
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

	memcpy(dest, src, param->buflen);
	dostrcmpbench(src, dest, ptrs);

	resettimer(b);
	for (i = 0; i < b->n; i++)
		dostrcmpbench(src, dest, ptrs);
	stoptimer(b);

	free(dest);
	free(ptrs);
	free(src);
}

static void
dostrcmpbenchu(const char *src, const char *dest, const char **ptrs)
{
	volatile int result = 0;
	size_t i;

	for (i = 0; ptrs[i+1] != NULL; i++)
		result += strcmp(ptrs[i], dest + (ptrs[i] - src) + i + 1);
}

static void
strcmpbenchu(struct B *b, void *payload)
{
	struct testparam *param;
	size_t nstr, j;
	char *src, *dest, *p;
	const char **ptrs;
	long i;

	param = payload;
	b->bytes = param->buflen;
	src = gentests(param);
	ptrs = (const char **)mkpointers(&nstr, src, param->buflen);
	dest = calloc(param->buflen + nstr, 1);
	if (dest == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	for (j = 0, p = dest; j < nstr; j++)
		p = stpcpy(p + 1, ptrs[j]) + 1;

	dostrcmpbenchu(src, dest, ptrs);

	resettimer(b);
	for (i = 0; i < b->n; i++)
		dostrcmpbenchu(src, dest, ptrs);
	stoptimer(b);

	free(dest);
	free(ptrs);
	free(src);
}

static int
strcmp_compar(const void *a, const void *b)
{
	return (strcmp(*(char **)a, *(char **)b));
}

static void
strcmpqsort(struct B *b, void *payload)
{
	struct testparam *param;
	size_t nstr;
	char *src;
	const char **orig, **ptrs;
	long i;

	param = payload;
	b->bytes = param->buflen;
	src = gentests(param);
	orig = (const char **)mkpointers(&nstr, src, param->buflen);
	ptrs = calloc(sizeof *ptrs, nstr);
	if (ptrs == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	memcpy(ptrs, orig, nstr * sizeof *ptrs);
	qsort(ptrs, nstr, sizeof *ptrs, strcmp_compar);

	resettimer(b);
	for (i = 0; i < b->n; i++) {
		stoptimer(b);
		memcpy(ptrs, orig, nstr * sizeof *ptrs);
		starttimer(b);
		qsort(ptrs, nstr, sizeof *ptrs, strcmp_compar);
	}
	stoptimer(b);

	free(ptrs);
	free(orig);
	free(src);
}

extern int
main(void)
{
	preamble();

	/* benchmark with equal alignment */
	runbenchmark("strcmpShortAligned", strcmpbench, (void *)&shortparam);
	runbenchmark("strcmpMidAligned", strcmpbench, (void *)&midparam);
	runbenchmark("strcmpLongAligned", strcmpbench, (void *)&longparam);

	/* benchmark with unequal alignment */
	runbenchmark("strcmpShortUnaligned", strcmpbenchu, (void *)&shortparam);
	runbenchmark("strcmpMidUnaligned", strcmpbenchu, (void *)&midparam);
	runbenchmark("strcmpLongUnaligned", strcmpbenchu, (void *)&longparam);

	/* qsort benchmarks */
	runbenchmark("strcmpShortQsort", strcmpqsort, (void *)&shortparam);
	runbenchmark("strcmpMidQsort", strcmpqsort, (void *)&midparam);
}
