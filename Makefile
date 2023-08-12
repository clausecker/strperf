CFLAGS+=	-fno-builtin -g -Wall

all: memcmp stpcpy strlen strchrnul

memcmp: framework.o strtest.o memcmp.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

stpcpy: framework.o strtest.o stpcpy.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

strlen: framework.o strtest.o strlen.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

strchrnul: framework.o strtest.o strchrnul.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

clean:
	rm -f *.o memcmp stpcpy strlen strchrnul

.PHONY: clean
