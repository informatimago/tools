#******************************************************************************
#FILE:				Makefile
#LANGUAGE:			make
#SYSTEM:			UNIX
#USER-INTERFACE:	None
#DESCRIPTION
# 
#	This Makefile compiles the ascii utilities.
#
#AUTHORS
#	<PJB> Pascal J. Bourguignon
#MODIFICATIONS
#	1993/03/16 <PJB> Creation.
#	1993/09/26 <PJB> Made library_template_makefile and 
#					program_template_makefile from 
#					previous makefile_example file.
#LEGAL
#	Copyright Pascal J. Bourguignon 1993 - 1993
#	All right reserved
#	This program may not be included in any commercial product without the 
#	author written permission. It may be used freely for any non-commercial 
#	purpose, provided that this header is always included.
#******************************************************************************

CSOURCES		=	\
					ascii.c beep.c binskip.c hextobin.c bintohex.c lc.c \
					sevenbit.c text.c whatisit.c sortchars.c \
					randchar.c concatenate.c

CPLUSSOURCES	=	\
					charcount.cc

INCLUDES		=	\
					-I../sources \
					-I$(PREFIX)/lib \
					-I$(PREFIX)/lib/benlib/interfaces \
					-I$(PREFIX)/lib/bclib/interfaces \
					$(TARGET_INCLUDES)


LIBS		=	\
					$(PREFIX)/lib/benlib/libraries/libbenlib.a \
					$(PREFIX)/lib/bclib/libraries/libbclib.a \
					$(TARGET_LIBRARIES)

PROGRAMS = \
		ascii beep binskip bintohex hextobin capitalize lc \
		randchar sevenbit sortchars text whatisit   \
		charcount parity encoding

MANPAGES		=	sevenbit.1 text.1 whatisit.1

# (dolist (p '(ascii beep binskip bintohex hextobin capitalize lc 
#               randchar sevenbit sortchars text whatisit   
#               charcount nema parity encoding))
# (insert (format "%s_OBJECTS = %s.o\n" p p))
# (insert (format "%s_LIBRARIES = $(LIBS)\n" p))
# (insert (format "%s_LD = $(CC)\n\n" p)))

ascii_OBJECTS = ascii.o
ascii_LIBRARIES = $(LIBS)
ascii_LD = $(CC)

beep_OBJECTS = beep.o
beep_LIBRARIES = $(LIBS)
beep_LD = $(CC)

binskip_OBJECTS = binskip.o
binskip_LIBRARIES = $(LIBS)
binskip_LD = $(CC)

bintohex_OBJECTS = bintohex.o
bintohex_LIBRARIES = $(LIBS)
bintohex_LD = $(CC)

hextobin_OBJECTS = hextobin.o
hextobin_LIBRARIES = $(LIBS)
hextobin_LD = $(CC)

capitalize_OBJECTS = capitalize.o
capitalize_LIBRARIES = $(LIBS)
capitalize_LD = $(CC)

lc_OBJECTS = lc.o
lc_LIBRARIES = $(LIBS)
lc_LD = $(CC)

randchar_OBJECTS = randchar.o concatenate.o
randchar_LIBRARIES = $(LIBS)
randchar_LD = $(CC)

sevenbit_OBJECTS = sevenbit.o
sevenbit_LIBRARIES = $(LIBS)
sevenbit_LD = $(CC)

sortchars_OBJECTS = sortchars.o
sortchars_LIBRARIES = $(LIBS)
sortchars_LD = $(CC)

text_OBJECTS = text.o
text_LIBRARIES = $(LIBS)
text_LD = $(CC)

whatisit_OBJECTS = whatisit.o
whatisit_LIBRARIES = $(LIBS)
whatisit_LD = $(CC)

charcount_OBJECTS = charcount.o
charcount_LIBRARIES = $(LIBS)
charcount_LD = $(CCPLUS)

nema_OBJECTS = nema.o
nema_LIBRARIES = $(LIBS)
nema_LD = $(CC)

parity_OBJECTS = parity.o
parity_LIBRARIES = $(LIBS)
parity_LD = $(CC)

encoding_OBJECTS = encoding.o concatenate.o
encoding_LIBRARIES = $(LIBS)
encoding_LD = $(CC)



include $(MAKEDIR)/common
-include Makefile.depend



test-parity:parity
	@for a in none even odd mark ; do \
		l="`echo $$a'    ' | sed -e 's/^\(....\).*/\1/'`" ;\
		echo "--------------------" ;\
		echo "Testing $$l parity: " ;\
		./parity --set --$$a < ../sources/wang > wang.$$a ;\
		echo -n "    Status for setting  $$l parity = $$? " ;\
		echo ' ()' ;\
		echo "Finding $$l parity: " ;\
		./parity --find < wang.$$a ;\
		echo -n "    Status for find  $$l parity = $$? " ;\
		./parity --quiet --find < wang.$$a ;\
		echo -n "    Status for find  $$l parity = $$? " ;\
		echo ' ()' ;\
		for b in none even odd mark ; do \
			m="`echo $$b'    ' | sed -e 's/^\(....\).*/\1/'`" ;\
			echo "    Checking $$m parity: " ;\
			./parity --check --$$b < wang.$$a ;\
			echo "        Status for checking $$m parity on file with $$a parity= $$? " ;\
		done ;\
	done
	@echo "Finding 8bit parity:"
	@./parity --find < ../sources/wang.isolatin1 ;\
		echo -n "    Status for find  8bit parity = $$? " ;\
		echo ' ()'
	@./parity --quiet --find < ../sources/wang.isolatin1 ;\
		echo -n "    Status for find  8bit parity = $$? " ;\
		echo ' ()'


#### Makefile                         --                     --          ####
