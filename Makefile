CFLAGS+=	-fno-builtin -g -Wall

all: strlen

strlen: framework.o strtest.o strlen.o
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $> ${LDLIBS}

clean:
	rm -f *.o strlen

.PHONY: clean
