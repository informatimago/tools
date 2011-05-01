#******************************************************************************
#FILE:				Makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for the s2d utility.		
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1994-03-16 <PJB> Added clean and clean-all targets.
#	1992-03-25 <PJB> Creation.
#LEGAL
#    GPL
#    
#    Copyright Pascal Bourguignon 1992 - 2011
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

CSOURCES= s2d.c
OTHERS = setfile is type
PROGRAMS = s2d
INCLUDES=\
	-I.\
	-I$(PREFIX)/lib \
	-I$(PREFIX)/lib/bclib/interfaces 

s2d_OBJECTS=s2d.o
s2d_LIBRARIES=

include $(MAKEDIR)/common
-include Makefile.depend

CFLAGS =  $(TARGET_CFLAGS)


setfile:
	-rm -f setfile
	ln -s s2d setfile

is:
	-rm -f is
	ln -s  s2d is

type:
	-rm -f type
	ln -s  s2d type

install:: 
	-rm -f     $(BINIDIR)/setfile
	ln -s  s2d $(BINIDIR)/setfile
	-rm -f     $(BINIDIR)/is
	ln -s  s2d $(BINIDIR)/is
	-rm -f     $(BINIDIR)/type
	ln -s  s2d $(BINIDIR)/type


cleanall::	
	-rm -f $(OTHERS)


#### Makefile                         -- 2003-12-04 04:56:13 -- pascal   ####
