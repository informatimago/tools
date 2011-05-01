#******************************************************************************
#FILE:				makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for the cookie tool.
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1993-??-??? <PJB> Creation.
#LEGAL
#    GPL
#    
#    Copyright Pascal Bourguignon 1993 - 2011
#    
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version
#    2 of the License, or (at your option) any later version.
#    
#    This program is distributed in the hope that it will be
#    useful, but WITHOUT ANY WARRANTY; without even the implied
#    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#    PURPOSE.  See the GNU General Public License for more details.
#    
#    You should have received a copy of the GNU General Public
#    License along with this program; if not, write to the Free
#    Software Foundation, Inc., 59 Temple Place, Suite 330,
#    Boston, MA 02111-1307 USA
#******************************************************************************

CSOURCES = random.c cookie.c
INCLUDES = \
		-I. \
		-I$(PREFIX)/lib \
		-I$(PREFIX)/lib/bclib/interfaces \
		$(TARGET_INCLUDES)
LIBS = \
		$(PREFIX)/lib/bclib/libraries/libbclib.a \
		$(TARGET_LIBRARIES)

PROGRAMS = random cookie
random_OBJECTS=random.o
random_LIBRARIES=$(LIBS)
cookie_OBJECTS=cookie.o
cookie_LIBRARIES=$(LIBS)

include $(MAKEDIR)/common
-include Makefile.depend

#### Makefile                         -- 2003-12-02 12:56:22 -- pascal   ####
