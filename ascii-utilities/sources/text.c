/******************************************************************************
FILE:               text.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program filters out any ASCII non printable character other than
    newline and tabulation or pure 7-bit ASCII.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/09/26 <PJB> Added this comment.
    Revision 1.1  2003/12/04 03:46:03  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:11  pascal
    Added to CVS.
    
    Revision 1.2  1994/02/15  05:21:31  pascal
    Added RCS keywords.

    Revision 1.2  1994/02/15  05:21:31  pascal
    Added RCS keywords.

LEGAL
    Copyright Pascal J. Bourguignon 1993 - 2011
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>

int main(void )
{
    int c;
        
    c=getc(stdin);
    while(c>=0){
        if((c=='\n')||(c=='\t')||((c>=32)&&(c<127))){
            putc(c,stdout);
        }
        c=getc(stdin);
    }
    return(0);
}

/*** text.c                           --                     --          ***/
