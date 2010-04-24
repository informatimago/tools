#******************************************************************************
#FILE:				dosname/sources/Makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the Makefile to build the BpClass Test program.
#
#	This Makefile uses the following environment variables:
#
#	Input variables are:
#
#       $(MAKEDIR)      where the makefiles are located.
#
#       $(PREFIX)   where the libraries are installed.
#                       It will be created a subdirectory in this 
#                       $(PREFIX) for each library compiled by 
#                       this makefile.
#
#       $(TARGET)       the name of the platform on which we're compiling.
#                       This is just used to include platform specific
#                       makefile rules.
#
#		$(MODULE)		BpClassTest, the name of this module.
#
#	Output:
#
#		$(PREFIX)/bin/$(PROGRAM)	the executable.
#
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1993-03-16 <PJB> Creation.
#   2001-06-19 <PJB> Reorganized & simplified.
#LEGAL
#	Copyright Pascal J. Bourguignon 1993 - 2001
#
#   This script is free software; you can redistribute it and/or
#   modify it under the terms of the GNU  General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This script is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this library; see the file COPYING.LIB.
#   If not, write to the Free Software Foundation,
#   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#******************************************************************************

CSOURCES=	
CPLUSSOURCES=\
	AttrString.cc \
	CapWord.cc \
	MfDirectory.cc \
	MfFile.cc \
	MfLink.cc \
	MfMode.cc \
	MfNode.cc \
	MfRootDirectory.cc \
	MfUtil.cc \
	NameSeq.cc \
	Renamer.cc \
	WordSeq.cc \
	Pressure.cc \
	dosname.cc

INCLUDES=\
	-I../sources \
	-I../interfaces \
	-I$(PREFIX)/lib \
	-I$(PREFIX)/lib/bplib/interfaces \
	-I$(PREFIX)/lib/bclib/interfaces \
	$(TARGET_INCLUDES)

CFLAGS     = -D_GNU_SOURCE $(TARGET_CFLAGS)
CPLUSFLAGS = -D_GNU_SOURCE $(TARGET_CFLAGS)

PROGRAMS=dosname
dosname_OBJECTS=$(CSOURCES:.c=.o) $(CPLUSSOURCES:.cc=.o)
dosname_LIBRARIES=\
	$(PREFIX)/lib/bplib/libraries/libbplib.a \
	$(PREFIX)/lib/bclib/libraries/libbclib.a \
	$(TARGET_LIBRARIES)
dosname_LD = $(CCPLUS)


include $(MAKEDIR)/common
-include Makefile.depend

tests:all
	cd .. ;\
	rm -r tests.dos tests.unix ;\
	./sources/dosname todos tests tests.dos  ;\
	./sources/dosname tounix tests.dos tests.unix  ;\
	ls -lR tests > /tmp/tests  ;\
	ls -lR tests.dos > /tmp/tests.dos  ;\
	ls -lR tests.unix > /tmp/tests.unix  ;\
	@[ -n "$EDITOR" ] && $EDITOR /tmp/tests{,.dos,.unix}


#### Makefile                         -- 2003-12-02 13:07:50 -- pascal   ####
