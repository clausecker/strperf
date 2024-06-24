/* strnlen benchmark */

#include <stdlib.h>
#include <string.h>

#include "benchmark.h"

struct testparam
	shortparam = { .avglen = 16,	  .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 123, 456, 789 }},
	midparam =	 { .avglen = 64,	  .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 234, 567, 890 }},
	longparam =	 { .avglen = 1 << 30, .buflen = 1 << 17, .charlen = 1, .maxchar = 255, .xseed = { 345, 678, 910 }};

static void
dostrnlenbench(const char *buf, size_t len, size_t maxlen)
{
	volatile size_t total = 0;

	while (total < len)
		total += strnlen(buf + total, maxlen) + 1;
}

static void
strnlenbench(struct B *b, void *payload)
{
	struct testparam *param;
	char *buf;
	long i;

	param = payload;
	b->bytes = param->buflen;
	buf = gentests(param);

	dostrnlenbench(buf, param->buflen, param->avglen);
	resettimer(b);
	for (i = 0; i < b->n; i++)
		dostrnlenbench(buf, param->buflen, param->avglen);
	stoptimer(b);

	free(buf);
}

extern int
main(void)
{
	preamble();

	runbenchmark("short", strnlenbench, (void *)&shortparam);
	runbenchmark("mid", strnlenbench, (void *)&midparam);
	runbenchmark("long", strnlenbench, (void *)&longparam);
}
