/******************************************************************************
FILE:               MfMode.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This module contains utility routines.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:40:58  pascal
   Initial revision
   
    
    1994-12-29 <PJB> Creation. 
LEGAL
    Copyright Pascal J. Bourguignon 1994 - 2011

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
extern "C"{
#include <sys/types.h>
#include <sys/stat.h>
}
#include <MfMode.hh>
#include BpClass_hh
#include <BcImplementation.h>

static const char rcsident[]="$Id: MfMode.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";

CONSTRUCTOR(MfMode)
{
    BpClass_PLUG(MfMode);
    fMode=0;
}//MfMode;
    
    
DESTRUCTOR(MfMode)
{
}//~MfMode;
    
    
// override of BpObject methods:

METHOD(MfMode,makeBrother,(void),BpObject*)
{
    return(NEW(MfMode));
}//makeBrother;
    
    
METHOD(MfMode,printOnLevel,(FILE* file,CARD32 level),void)
{
    MfMode_SUPER::printOnLevel(file,level);
    PRINTONLEVEL(file,level,"%o",fMode,fMode);
}//printOnLevel;

// MfMode methods:
    
METHOD(MfMode,mode,(void),CARD16)
{
    return(fMode);
}//mode;
    
    
METHOD(MfMode,modeSet,(CARD16 nMode),void)
{
    fMode=nMode;
}//modeSet;
    
static char MfMode_stringTemplate[]="-ugtrwxrwxrwx";
    
METHOD(MfMode,stringValue,(void),BpString*)
{
    BpString*       modeStr=NEW(BpString);
    char            buffer[14];
    CARD16          bit;
    INT32           i;
        
    switch(fMode&S_IFMT){
    case S_IFDIR:
        buffer[0]='d';
        break;
    case S_IFLNK:
        buffer[0]='l';
        break;
    default:
        buffer[0]='-';
        break;
    }
    for(i=1,bit=04000;i<13;i++,bit=(CARD16)(bit>>1)){
        if(bit&fMode){
            buffer[i]=MfMode_stringTemplate[i];
        }else{
            buffer[i]='-';
        }
    }
    buffer[i]='\0';
    modeStr->setString(buffer);
    return(modeStr);
}//stringValue;
    
    
METHOD(MfMode,stringValueSet,(BpString* nModeStr),void)
{
    const char*     buffer;
    CARD16          bit;
    INT32           i;
    CARD16          nMode;
        
    nMode=0;
    if(nModeStr->length()==13){
        buffer=nModeStr->string();
        switch(buffer[0]){
        case 'd':
            nMode=S_IFDIR;
            break;
        case 'l':
            nMode=S_IFLNK;
            break;
        default:
            nMode=S_IFREG;
            break;
        }
        for(i=1,bit=04000;i<13;i++,bit=(CARD16)(bit>>1)){
            if(buffer[i]==MfMode_stringTemplate[i]){
                nMode|=bit;
            }else if(buffer[i]!='-'){
                return;
            }
        }
        fMode=nMode;
    }
}//stringValueSet;

//// THE END ////
