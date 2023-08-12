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

/* stpcpy benchmark */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

struct testparam
    shortparam = { .avglen = 16,      .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 123, 456, 789 }},
    midparam =   { .avglen = 64,      .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 234, 567, 890 }},
    longparam =  { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 345, 678, 910 }};

static void
dostpcpybench(char *dest, const char *src, size_t len)
{
	char *volatile destpos;

	destpos = dest;
	while (destpos < dest + len) {
		const char *srcpos;

		srcpos = src + (destpos - dest);
		destpos = stpcpy(destpos, srcpos) + 1;
	}
}

static void
stpcpybench(struct B *b, void *payload)
{
	struct testparam *param;
	char *src, *dest;
	long i;

	param = payload;
	b->bytes = param->buflen;
	src = gentests(param);
	dest = calloc(param->buflen, 1);

	if (dest == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	dostpcpybench(dest, src, param->buflen);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		dostpcpybench(dest, src, param->buflen);
	stoptimer(b);

	free(src);
	free(dest);
}

extern int
main(void)
{
	preamble();

	runbenchmark("short", stpcpybench, (void *)&shortparam);
	runbenchmark("mid", stpcpybench, (void *)&midparam);
	runbenchmark("long", stpcpybench, (void *)&longparam);
}
