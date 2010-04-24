/******************************************************************************
FILE:               fields.h
LANGUAGE:           ANSI-C
SYSTEM:             POSIX
USER-INTERFACE:     POSIX
DESCRIPTION
    Some routines to handle lines of fields.
AUTHORS
    <PJB> Pascal J. Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2000-09-16 <PJB> Creation.
BUGS
    Please report them to <pjb@informatimago.com>
LEGAL
    Copyright Pascal J. Bourguignon 2000 - 2000

    GPL
    This file is part of the lgetvalue utility.

    This  program is  free software;  you can  redistribute  it and/or
    modify it  under the  terms of the  GNU General Public  License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program  is distributed in the  hope that it  will be useful,
    but  WITHOUT ANY WARRANTY;  without even  the implied  warranty of
    MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a  copy of the GNU General Public License
    along with  this program; see the  file COPYING; if  not, write to
    the Free  Software Foundation, Inc.,  59 Temple Place,  Suite 330,
    Boston, MA 02111-1307 USA

    Commercial licenses are available. 
    Send your requests to <pjb@informatimago.com>.
******************************************************************************/
#ifndef inhibit_fields_h
#define inhibit_fields_h

extern void get_field(char* field,const char* record,
                      int/*char*/ field_separator,unsigned int field_index);
/*
  Copies into field the field number field_index from the record line.
  If field_index==0, then copies the whole record.
  Fields are separated with the field_separator character.
  record is null-terminated.
  field_separator==0 ==> number of fields == 1 
  (then the only valid values for field_index is 0 or 1).
  If field_index>number of fields in record 
  then copies the empty string into field.
*/



#endif
/*** fields.h                         --                     --          ***/
