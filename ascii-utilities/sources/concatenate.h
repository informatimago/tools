/*****************************************************************************
FILE:               concatenate.h
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
    
    Copyright Pascal Bourguignon 2004 - 2004
    
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
#ifndef concatenate_h 
#define concatenate_h
   
    extern char* concatenate(const char* str,...);
        /*
            RETURN: A newly mallocated string containing the concatenation
                    of all the arguments (null terminated argument list).
        */

#endif
/*** concatenate.h                    --                     --          ***/
