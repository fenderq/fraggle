PROG=		fraggle
SRCS=		main.c diceware.c

CFLAGS+=	-Wall -I${.CURDIR}
CFLAGS+=	-Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wshadow -Wpointer-arith
CFLAGS+=	-Wsign-compare -Wcast-qual

BINDIR=		/usr/local/bin
MANDIR=		/usr/local/man/man

.include <bsd.prog.mk>
