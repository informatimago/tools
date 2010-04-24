#******************************************************************************
#FILE:				makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for the MacBinary converter.
#
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1994/02/15 <PJB> Creation.
#	Revision 1.1  2003/12/04 03:46:07  pjbpjb
#	Cleaned-up.
#	Fix.
#	
#LEGAL
#	Copyright Pascal J. Bourguignon 1994 - 1994
#	All rights reserved.
#	This program or any part of it may not be included in any commercial 
#	product without the author written permission. It may be used freely for 
#	any non-commercial purpose, provided that this header is always included.
#******************************************************************************
CSOURCES=mb.c
INCLUDES = \
	-I. \
	-I$(PREFIX)/lib/ \
	-I$(PREFIX)/lib/bclib/interfaces \
	$(TARGET_INCLUDES)

PROGRAMS = mb
mb_OBJECTS = mb.o
mb_LIBRARIES =  $(PREFIX)/lib/bclib/libraries/libbclib.a $(TARGET_LIBRARIES)


include $(MAKEDIR)/common
-include Makefile.depend


#### Makefile                         --                     --          ####
