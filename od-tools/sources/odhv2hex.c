/******************************************************************************
FILE:               odhv2hex.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    The purpose of this filter is to remove the address part of a dump 
    produced by od.
USAGE
    od -hv file | odhv2hex > file.hex

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
        char        buffer[2048];
    
    while(fgets(buffer,sizeof(buffer),stdin)){
        int i,j;
        int start=0;
        while((buffer[start]!=' ')&&(buffer[start]!='\0')){
            start++;
        }
        i=start;
        j=start;
        while(buffer[i]!='\0'){
            switch(buffer[i]){
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                buffer[j++]=buffer[i++];
                break;
            default:
                i++;
            }
        }
        buffer[j]='\0';
        puts(&(buffer[start]));
    }
    return(0);
}


/*** odhv2hex.c                       -- 2003-11-23 02:16:30 -- pascal   ***/
