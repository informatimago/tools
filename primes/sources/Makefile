#******************************************************************************
#FILE:				~/src/tools/primes/sources/makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is an makefile template for a program subdirectory.
#
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1993/03/16 <PJB> Creation.
#	1993/09/26 <PJB> Made library_template_makefile and 
#					program_template_makefile from 
#					previous makefile_example file.
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

CSOURCES = primes.c

INCLUDES = \
	-I../sources \
	-I$(PREFIX)/lib \
	-I$(PREFIX)/lib/bclib/interfaces \
	$(TARGET_INCLUDES)

PROGRAMS = primes
primes_OBJECTS = primes.o
primes_LIBRARIES = $(TARGET_LIBRARIES)

include $(MAKEDIR)/common
-include Makefile.depend

CFLAGS = -D_GNU_SOURCE $(TARGET_CFLAGS)

#### Makefile                         -- 2003-12-02 12:49:45 -- pascal   ####
