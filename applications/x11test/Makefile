SRC = x11test.c
CC ?= gcc
BIN ?= x11test
PREFIX ?= /usr/local
INSTALLPATH ?= ${DESTDIR}${PREFIX}/bin
MANPATH ?= ${DESTDIR}${PREFIX}/share/man/man1

x11test : ${SRC}

	@echo "building ${SRC}"
	${CC} ${CFLAGS} ${SRC} -o ${BIN} ${LDFLAGS} -lX11 -lxcb -lXau

clean :

	@echo "cleaning ${BIN}"
	@rm -f ${BIN}
	@echo "${BIN} cleaned"

