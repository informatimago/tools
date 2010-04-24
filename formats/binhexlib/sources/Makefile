#******************************************************************************
#FILE:				makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for binhex tool.
#
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

CSOURCES		=	
CPLUSSOURCES	= 	\
					BinHex4Stream.cc	\
					CheckSumStream.cc	\
					RunLengthStream.cc	\
					SeqStream.cc	\
					SevenStream.cc	\
					StdIOStream.cc


INTERFACES      = \
					BinHex4Stream.hh \
					CheckSumStream.hh \
					RunLengthStream.hh \
					SeqStream.hh \
					SevenStream.hh \
					StdIOStream.hh

INCLUDES		=	-I../sources \
					-I../interfaces \
					-I$(PREFIX)/lib \
					-I$(PREFIX)/lib/bplib/interfaces \
					-I$(PREFIX)/lib/bclib/interfaces \
					-I$(PREFIX) 

LIBRARIES = binhexlib
binhexlib_OBJECTS=$(CSOURCES:.c=.o) $(CPLUSSOURCES:.cc=.o)


include $(MAKEDIR)/common
-include Makefile.depend

#### Makefile                         -- 2003-12-02 10:57:16 -- pascal   ####
