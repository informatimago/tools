#******************************************************************************
#FILE
#	makefile
#DESCRIPTION
#	This is the makefile for the line utilities.
#
#AUTHOR
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1993/09/26 <PJB> Updated.
#	1994/03/16 <PJB> Added clean and clean-all targets.
#LEGAL
#	Copyright Pascal J. Bourguignon 1993 - 1993
#	All rights reserved.
#	This program or any part of it may not be included in any commercial 
#	product without the author written permission. It may be used freely for 
#	any non-commercial purpose, provided that this header is always included.
#******************************************************************************

CSOURCES=\
	arguments.c \
	fdiff.c \
	fields.c \
	lcat.c \
	lcut-small.c \
	lcut.c \
	ldiff.c \
	lgetvalue.c \
	lines.c \
	lmax.c \
	lrev.c 

INCLUDES = \
	-I. \
	-I$(PREFIX)/lib \
	-I$(PREFIX)/lib/bclib/interfaces

LIBS = $(PREFIX)/lib/bclib/libraries/libbclib.a $(TARGET_LIBRARIES)

PROGRAMS = lcat ldiff lmax lrev fdiff lgetvalue

# (dolist (p '(lcat ldiff lmax lrev fdiff lgetvalue))
# (insert (format "%s_OBJECTS = %s.o\n" p p))
# (insert (format "%s_LIBRARIES = $(LIBS)\n" p))
# (insert (format "%s_LD = $(CC)\n\n" p)))

lcat_OBJECTS = lcat.o
lcat_LIBRARIES = $(LIBS)
lcat_LD = $(CC)

ldiff_OBJECTS = ldiff.o
ldiff_LIBRARIES = $(LIBS)
ldiff_LD = $(CC)

lmax_OBJECTS = lmax.o
lmax_LIBRARIES = $(LIBS)
lmax_LD = $(CC)

lrev_OBJECTS = lrev.o
lrev_LIBRARIES = $(LIBS)
lrev_LD = $(CC)

fdiff_OBJECTS = fdiff.o lines.o fields.o arguments.o
fdiff_LIBRARIES = $(LIBS)
fdiff_LD = $(CC)

lgetvalue_OBJECTS = lgetvalue.o lines.o fields.o arguments.o
lgetvalue_LIBRARIES = $(LIBS)
lgetvalue_LD = $(CC)


include $(MAKEDIR)/common
-include Makefile.depend

#### Makefile                         -- 2003-12-02 13:13:00 -- pascal   ####
