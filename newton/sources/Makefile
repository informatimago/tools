CSOURCES = newton.c newton-ps.c

PROGRAMS = newton newton-ps
newton_OBJECTS=newton.o
newton-ps_OBJECTS=newton-ps.o

include $(MAKEDIR)/common
-include Makefile.depend

test:
	(./newton-ps < ps-1.data | gs)>/tmp/newton-ps.log 2>&1

#### Makefile                         -- 2003-12-02 12:50:59 -- pascal   ####
