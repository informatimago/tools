#******************************************************************************
#FILE:				Makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for the segment and join tools.
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1990/04/28 <PJB> Creation.
#	1993/12/18 <PJB> Updated.
#	1994/03/16 <PJB> Added clean and clean-all targets.
#LEGAL
#	Copyright Pascal J. Bourguignon 1993 - 1993
#	All right reserved
#	This program may not be included in any commercial product without the 
#	author written permission. It may be used freely for any non-commercial 
#	purpose, provided that this header is always included.
#******************************************************************************
PREFIX:=/usr/local
CSOURCES= join.c segment.c unsparse.c
INCLUDES=\
	-I. \
	-I$(PREFIX)/lib \
	-I$(PREFIX)/lib/bclib/interfaces
LIBS=$(PREFIX)/lib/bclib/libraries/libbclib.a $(TARGET_LIBRARIES)

PROGRAMS= join segment unsparse
join_INCLUDES=$(INCLUDES)
join_OBJECTS=join.o
join_LIBRARIES=$(LIBS)
segment_INCLUDES=$(INCLUDES)
segment_OBJECTS=segment.o
segment_LIBRARIES=$(LIBS)
unsparse_INCLUDES=$(INCLUDES)
unsparse_OBJECTS=unsparse.o
unsparse_LIBRARIES=$(LIBS)

include $(MAKEDIR)/common
-include Makefile.depend

test:join segment_test
	rm -f test.*
	randchar -w 80 -l 1024 > test.data
	./segment_test -s 1000 -n test.splited.%03d   test.data
	cat test.splited.??? > test.cated
	./join test.splited
	diff test.splited test.cated
	diff test.splited test.data
	rm -f test.*
	randchar -w 80 -l 1024 > test.data
	for n in 0 1 2 3 4 5 6 7 8 ; do \
		./segment_test -s 10000 -u $$n -n test.splited.%03d  test.data ; done
	cat test.splited.??? > test.cated
	./join test.splited
	diff test.splited test.cated
	diff test.splited test.data

clean::
	-rm -f test.*

#### Makefile                         --                     --          ####
