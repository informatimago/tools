/******************************************************************************
FILE:               postodax.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    The purpose of this filter is to put on the same line the ASCII and 
    the hexadecimal lines of a dump produced by 'od -ha'.
    
USAGE
    od -xa $file | postodax

0000000     feed    face    0000    0006    0000    0001    0000    0002
012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888

AUTHORS
    PJB  Pascal J. Bourguignon
MODIFICATIONS
    1993-08-01 PJB  Creation.
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1993 - 2011
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    2 of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public
    License along with this program; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA 02111-1307 USA
******************************************************************************/
#include <stdio.h>

int main(void)
{

    int    current;
    int    next;
    
    current=getc(stdin);
    next=getc(stdin);
    while(EOF!=next){
        if((current==(char)10)&&((next==(char)9)||(next==' '))){
            current=' ';
        }
        putc(current,stdout);
        current=next;
        next=getc(stdin);
    }
    putc(current,stdout);
    return(0);
}


/*** postodax.c                       --                     --          ***/
