/******************************************************************************
FILE:               names.h
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This file defines names for includes of this program on several File 
    Systems.
AUTHOR
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:46:14  pascal
   Initial revision
   
    Revision 1.5  95/10/22  03:19:57  pascal
    Setting revision name V1.
    
    Revision 1.2  95/10/22  03:05:20  pascal
    Modifications while adding BpDict.
    
    1995-10-18 <PJB> Creation.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 2001

    GPL

    This file is part of the dosname tool.

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
******************************************************************************/
#ifndef dosname_names_h
#define dosname_names_h

#if defined(UNIX) || defined(MACOS)

#include <bplib/interfaces/names.h>

#define AttrString_hh         <AttrString.hh>
#define CapWord_hh            <CapWord.hh>
#define MfDirectory_hh        <MfDirectory.hh>
#define MfFile_hh             <MfFile.hh>
#define MfLink_hh             <MfLink.hh>
#define MfMode_hh             <MfMode.hh>
#define MfNode_hh             <MfNode.hh>
#define MfRootDirectory_hh    <MfRootDirectory.hh>
#define MfUtil_hh             <MfUtil.hh>
#define NameSeq_hh            <NameSeq.hh>
#define Renamer_hh            <Renamer.hh>
#define WordSeq_hh            <WordSeq.hh>
#define dosname_hh            <dosname.hh>
#define Pressure_hh           <Pressure.hh>

#endif

#if defined(MSDOS)

#error "names.h is not set for MSDOS yet."

#endif

#endif //names
