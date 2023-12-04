CFLAGS+=	-fno-builtin -g -Wall
TESTS=		memchr memcmp memcpy memccpy stpcpy stpncpy strchrnul strcmp \
		strcspn strncmp strlcpy strlen strrchr strspn

all: ${TESTS}

.c: framework.o strtest.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

clean:
	rm -f *.o ${TESTS}

.PHONY: clean
