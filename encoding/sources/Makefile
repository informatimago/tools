#******************************************************************************
#FILE:				makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for several ASCII utilities.
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1993/09/26 <PJB> Updated.
#	1994/03/16 <PJB> Added clean and clean-all targets.
#	Revision 1.1  2003/12/04 03:46:05  pjbpjb
#	Cleaned-up.
#	Fix.
#	
#	Revision 1.3  2003/11/22 22:31:01  pascal
#	Corrected some compilation problems.
#	
#	Revision 1.2  2001/04/30 03:14:48  pascal
#	Added $$pgm to target for install, since it seems it's needed
#	by install in NEXTSTEP.
#	
#	Revision 1.1  2001/04/30 03:08:20  pascal
#	First entry into CVS.
#	
#	Revision 1.1  2001/04/30 01:11:21  pascal
#	Initial entry into CVS.
#	
#	Revision 1.6  94/07/17  15:03:23  pascal
#	Added text.1 and sevenbit.1 manual pages.
#	
#	Revision 1.4  94/02/15  06:07:46  pascal
#	Corrected install script.
#	
#	Revision 1.3  94/02/15  05:38:26  root
#	Added RCS keywords.
#	Corrected install script.
#	
#LEGAL
#	Copyright Pascal J. Bourguignon 1993 - 1993
#	All rights reserved.
#	This program or any part of it may not be included in any commercial 
#	product without the author written permission. It may be used freely for 
#	any non-commercial purpose, provided that this header is always included.
#******************************************************************************

CSOURCES=\
	encoding.c \
	bentest.c 

INCLUDES=\
	-I.\
	-I../sources \
	-I$(PREFIX)/lib \
	-I$(PREFIX)/lib/bclib/interfaces \
	-I$(PREFIX)/lib/benlib/interfaces \
	-I$(PREFIX)/lib/bjlib/interfaces 


PROGRAMS = nema bentest
nema_OBJECTS = encoding.o
nema_LIBRARIES = \
	$(PREFIX)/lib/benlib/libraries/libbenlib.a \
	$(PREFIX)/lib/bclib/libraries/libbclib.a \
	$(PREFIX)/lib/bjlib/libraries/libbjlib.a 

bentest_OBJECTS = bentest.o 
bentest_LIBRARIES = \
	$(PREFIX)/lib/benlib/libraries/libbenlib.a \
	$(PREFIX)/lib/bclib/libraries/libbclib.a \
	$(PREFIX)/lib/bjlib/libraries/libbjlib.a 

include $(MAKEDIR)/common
-include Makefile.depend

#### Makefile                         --                     --          ####
