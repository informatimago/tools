#******************************************************************************
#FILE:				makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
#	This is the makefile for cryptographic utilities.
#
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1994-02-15 <PJB> Creation.
#LEGAL
#    GPL
#    
#    Copyright Pascal Bourguignon 1994 - 2011
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
CSOURCES=bocrypt.c smallcrypt.c xor.c

INCLUDES		=	\
					-I../sources \
					-I$(PREFIX)/lib \
					-I$(PREFIX)/lib/bclib/interfaces \
					$(TARGET_INCLUDES)
LIBS		=	\
					$(PREFIX)/lib/bclib/libraries/libbclib.a \
					$(TARGET_LIBRARIES)

OTHERS = sdecrypt
PROGRAMS = sencrypt bocrypt xor
sencrypt_OBJECTS = smallcrypt.o
sencrypt_LIBRARIES = $(LIBS)
bocrypt_OBJECTS = bocrypt.o
bocrypt_LIBRARIES = $(LIBS)
xor_OBJECTS = xor.o
xor_LIBRARIES = $(LIBS)
MANPAGES =

include $(MAKEDIR)/common
-include Makefile.depend


sdecrypt:
	-rm -f sdecrypt
	ln -s sencrypt sdecrypt

install::
	-rm -f         $(BINIDIR)/sdecrypt
	ln -s sencrypt $(BINIDIR)/sdecrypt

clean::
	-rm -f ./test-*.key*

TESTS=test-1 test-2 test-3
BOKEYS=key3b key4b
SMALLKEYS=key1t key2t

test:all
	@for t in $(TESTS) ; do \
		for k in $(BOKEYS) ; do \
			echo  === $$t $$k BOCRYPT ============================ ;\
			./bocrypt 'password' 0 $$k $$t.txt > $$t.bocrypt.$$k ;\
			./bocrypt 'password' 0 $$k $$t.bocrypt.$$k > $$t.bocrypt.$$k.decrypted ;\
			cmp $$t.txt $$t.bocrypt.$$k.decrypted ;\
		done ;\
		for k in $(SMALLKEYS) ; do \
			echo  === $$t $$k SMALLCRYPT ============================ ;\
			./sencrypt -q $$k $$t.txt > $$t.smallcrypt.$$k ;\
			./sdecrypt -q $$k $$t.smallcrypt.$$k > $$t.smallcrypt.$$k.decrypted ;\
			cmp $$t.txt $$t.smallcrypt.$$k.decrypted ;\
		done ;\
	done


#### Makefile                         -- 2003-12-02 12:45:21 -- pascal   ####
