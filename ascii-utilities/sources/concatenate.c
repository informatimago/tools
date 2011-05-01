/*****************************************************************************
FILE:               concatenate.c
LANGUAGE:           c
SYSTEM:             POSIX
USER-INTERFACE:     NONE
DESCRIPTION
    
    Export a concatenate function.
    
AUTHORS
    <PJB> Pascal Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2004-06-24 <PJB> Created.
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 2004 - 2011
    
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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <concatenate.h>

    
    char* concatenate(const char* str,...)
    {   
        char* result=0;
        if(str==0){
            result=malloc(2);
            if(result){
                *result=0;
            }
        }else{
            size_t length=0;
            const char* arg;
            va_list ap;
            /* 1- count the characters. */
            va_start(ap,str);
            length=strlen(str);
            arg=va_arg(ap,const char*);
            while(arg!=0){
                length+=strlen(arg);
                arg=va_arg(ap,const char*);
            }
            va_end(ap);
            /* 2- allocate the concatenation. */
            result=malloc(1+length);
            if(result){
                /* 3- copy the characters. */
                char* next=result;
                strcpy(next,str);
                next=strchr(result,'\0');
                va_start(ap,str);
                arg=va_arg(ap,const char*);
                while(arg!=0){
                    strcpy(next,arg);
                    next=strchr(next,'\0');
                    arg=va_arg(ap,const char*);
                }
                va_end(ap);
            } 
        }
        return(result);
    }/*concatenate*/


/*** concatenate.c                    --                     --          ***/
