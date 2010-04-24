CSOURCES=environment.c
PROGRAMS=environment
environment_OBJECTS=environment.o

include $(MAKEDIR)/common
-include Makefile.depend

CFLAGS = -D_GNU_SOURCE $(TARGET_CFLAGS)

clean::
	-rm -f core *.csh-out *.csh-err *.sh-out *.sh-err

.SUFFIXES:: .csh-out .csh-err .sh-out .sh-err .env
.env.csh-out: ; -environment --csh < $< > $*.csh-out 2> $*.csh-err
.env.csh-err: ; -environment --csh < $< > $*.csh-out 2> $*.csh-err
.env.sh-out:  ; -environment --sh  < $< > $*.sh-out  2> $*.sh-err
.env.sh-err:  ; -environment --sh  < $< > $*.sh-out  2> $*.sh-err

test:environment
	./do-test

dump:
	./do-dump

#### Makefile                         -- 2003-12-02 13:13:32 -- pascal   ####
