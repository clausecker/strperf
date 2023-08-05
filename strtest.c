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

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "benchmark.h"

/*
 * Generate a test case according to the given test
 * parameters.
 */
extern char *
gentests(const struct testparam *params)
{
	double p;
	size_t i;
	unsigned short state[3];
	char *buf;

	buf = malloc(params->buflen);
	if (buf == NULL) {
		perror("gentests");
		exit(EXIT_FAILURE);
	}

	assert(0 < params->maxchar && params->maxchar <= UCHAR_MAX);
	assert(params->avglen > 0);
	p = 1.0 / (params->avglen + 1.0);
	memcpy(state, params->xseed, sizeof state);

	for (i = 0; i < params->buflen - 1; i++) {
		double prob;

		prob = erand48(state);
		if (prob <= p) {
			buf[i] = '\0';
			continue;
		}

		prob = erand48(state);
		buf[i] = 1 + (int)(prob * (params->maxchar - 1.0));
	}

	buf[i] = '\0';

	return (buf);
}

/*
 * For the given test buffer, generate an array of pointers to the
 * beginnings of the strings contained within.  Store the number of
 * strings found in *nstr and return an array of pointers.  The array
 * is terminated by a null pointer after *nstr entries.
 */
extern char **
mkpointers(size_t *nstr, const char *buf, size_t len)
{
	size_t i, n;
	char **ptrs;

	/* dry run to find the number of strings */

	for (i = n = 0; i < len; n++)
		i += strlen(buf + i) + 1;

	ptrs = malloc((n + 1) * sizeof *ptrs);
	if (ptrs == NULL) {
		perror("mkpointers");
		exit(EXIT_FAILURE);
	}

	for (i = n = 0; i < len; n++) {
		ptrs[n] = (char *)buf + i;
		i += strlen(buf + i) + 1;
	}

	*nstr = n;
	ptrs[n] = NULL;
	return (ptrs);
}
