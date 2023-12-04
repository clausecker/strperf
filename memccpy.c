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

/* memccpy benchmark */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

struct testparam
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 345, 678, 910 }};

static void
domemccpybench(char **dest, char **src)
{
	size_t i;

	for (i = 0; dest[i+1] != NULL; i++)
		memccpy(dest[i], src[i], '\0', (size_t)(dest[i+1] - dest[i]));
}

static void
memccpybench(struct B *b, void *payload)
{
	struct testparam param;
	long i;
	size_t nsrc, ndest;
	char *src, *dest, **srcp, **destp;

	memcpy(&param, payload, sizeof param);
	b->bytes = param.buflen;
	src = gentests(&param);

	/* tweak the seed for the destination */
	param.xseed[0] ^= 0x55aa;
	param.xseed[1] ^= 0x3c3c;
	param.xseed[2] ^= 0xf00f;
	dest = gentests(&param);

	srcp = mkpointers(&nsrc, src, param.buflen);
	destp = mkpointers(&ndest, dest, param.buflen);

	/* ensure there's a src string for each dest string */
	if (nsrc < ndest) {
		char **tmpp;

		tmpp = srcp;
		srcp = destp;
		destp = tmpp;
	}

	domemccpybench(destp, srcp);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		domemccpybench(destp, srcp);
	stoptimer(b);

	free(src);
	free(dest);
	free(srcp);
	free(destp);
}

extern int
main(void)
{
	preamble();

	runbenchmark("short", memccpybench, (void *)&shortparam);
	runbenchmark("mid", memccpybench, (void *)&midparam);
	runbenchmark("long", memccpybench, (void *)&longparam);
}
