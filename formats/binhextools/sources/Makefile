#******************************************************************************
#FILE:				makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for the binhex tools.
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1993/??/?? <PJB> Creation.
#LEGAL
#	Copyright Pascal J. Bourguignon 1993 - 1993
#	All right reserved
#	This program may not be included in any commercial product without the 
#	author written permission. It may be used freely for any non-commercial 
#	purpose, provided that this header is always included.
#******************************************************************************


CSOURCES = \
		bh.c
OBJCSOURCES =
CPLUSSOURCES = \
		seven.cc \
		runlength.cc

INCLUDES = \
		-I../sources \
		-I$(PREFIX)/lib \
		-I$(PREFIX)/lib/binhexlib/interfaces \
		-I$(PREFIX)/lib/bplib/interfaces \
		-I$(PREFIX)/lib/bclib/interfaces \
		$(TARGET_INCLUDES)

LIBS=\
		$(PREFIX)/lib/binhexlib/libraries/libbinhexlib.a \
		$(PREFIX)/lib/bplib/libraries/libbplib.a \
		$(PREFIX)/lib/bclib/libraries/libbclib.a \
		$(TARGET_LIBRARIES)

OTHERS = unseven unrunlength
PROGRAMS = bh seven runlength

bh_OBJECTS=bh.o
bh_LIBRARIES=$(LIBS)
bh_LD=$(CC)

seven_OBJECTS=seven.o
seven_LIBRARIES=$(LIBS)
seven_LD=$(CCPLUS)

runlength_OBJECTS=runlength.o
runlength_LIBRARIES=$(LIBS)
runlength_LD=$(CCPLUS)

LD=$(CCPLUS)

include $(MAKEDIR)/common
-include Makefile.depend

unseven:
	-rm -f      unseven
	ln -s seven unseven

unrunlength:
	-rm -f          unrunlength
	ln -s runlength unrunlength

install::
	-rm -f          $(BINIDIR)/unseven
	ln -s seven     $(BINIDIR)/unseven
	-rm -f          $(BINIDIR)/unrunlength
	ln -s runlength $(BINIDIR)/unrunlength

cleanall::
	-rm -f $(OTHERS)


#### Makefile                         -- 2003-12-02 15:59:21 -- pascal   ####
