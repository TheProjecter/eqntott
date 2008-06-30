#
#
#	Copyright (C) 1981, Regents of the University of California
#
#	All rights reserved
#
#	Heavily modified by Peter Scott, 2008

# eqntott - description file

# macros for installation
NAME = eqntott
PREFIX = /usr/local
BINDIR = ${PREFIX}/bin
MANDIR = ${PREFIX}/man

# macros for compilation
YACC=yacc
CFLAGS = -O -s
CFLAGS2 = ${CFLAGS} -DCFLAGS='"${CFLAGS}"' -DVERSION=9 \
	-DCOMPDATE="\"`date`\""
HFILES = x.h y.tab.h
CFILES = bnode.c canon.c cover.c duple.c hash.c merge.c misc.c nt.c prexpr.c \
	preprocess.c procargs.c pterm.c pterm_ops.c putpla.c read_ones.c \
	reduce.c rmcvd.c substitute.c version.c x.c
OBJ = bnode.o canon.o cover.o duple.o hash.o merge.o misc.o nt.o prexpr.o \
	preprocess.o procargs.o pterm.o pterm_ops.o putpla.o read_ones.o \
	reduce.o rmcvd.o substitute.o x.o y.tab.o yystuff.o

DISTSRCS= ${CFILES} ${HFILES} ex* Makefile gram.y yystuff.c

# macros for documentation
MANPAGE = eqntott.1
PRFILES = Makefile ${MANPAGE} ${HFILES} gram.y yystuff.c ${CFILES}
PR = vgrind

${NAME}: ${OBJ} version.o
	${CC} ${CFLAGS} -o ${NAME} *.o

install: ${NAME}
	cp ${NAME} ${BINDIR}/${NAME}
	cp ${MANPAGE} ${MANDIR}/man1/${MANPAGE}

${OBJ}: x.h
y.tab.o yystuff.o: x.h y.tab

y.tab: gram.y
	${YACC} -d gram.y
	touch y.tab

version.o: ${OBJ} Makefile
	${CC} ${CFLAGS2} -c version.c

clean:
	rm -f *.o y.tab*