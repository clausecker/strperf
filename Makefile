CFLAGS+=	-fno-builtin -g -Wall
TESTS=		memcmp stpcpy strchrnul strcspn strlen strspn

all: ${TESTS}

memcmp: framework.o strtest.o memcmp.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

stpcpy: framework.o strtest.o stpcpy.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

strcspn: framework.o strtest.o strcspn.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

strspn: framework.o strtest.o strspn.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

strlen: framework.o strtest.o strlen.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

strchrnul: framework.o strtest.o strchrnul.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

clean:
	rm -f *.o ${TESTS}

.PHONY: clean
