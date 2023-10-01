CFLAGS+=	-fno-builtin -g -Wall
TESTS=		memchr memcmp memcpy stpcpy strchrnul strcmp strcspn \
		strlen strspn

all: ${TESTS}

.c: framework.o strtest.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

clean:
	rm -f *.o ${TESTS}

.PHONY: clean
