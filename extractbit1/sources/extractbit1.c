/*****************************************************************************
FILE:               extractbit1.c
LANGUAGE:           c
SYSTEM:             POSIX
USER-INTERFACE:     NONE
DESCRIPTION
    
   This program extract the lowest bit from each input byte and group
   them by 8 per output byte.

   This may be used to obtain a random flow from a noisy physical input.
    
AUTHORS
    <PJB> Pascal Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    1999-06-05 <PJB> Created.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1999 - 1999
    
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
*****************************************************************************/
    
#include <stdio.h>

    static void extractbit1(FILE* in,FILE* out)
    {
        char    outbyte;
        char    inbyte;
        int     i;
        while(!feof(in)){
            for(i=0;i<8;i++){
                fread(&inbyte,1,1,in);
                outbyte=(outbyte<<1)|(inbyte&1);
            }
            fwrite(&outbyte,1,1,out);
        }
    }/*extractbit1*/

int main(void)
{
    extractbit1(stdin,stdout);
    return(0);
}/*main*/


/*** extractbit1.c                    -- 2003-12-02 13:14:23 -- pascal   ***/
