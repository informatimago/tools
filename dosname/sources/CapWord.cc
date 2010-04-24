/******************************************************************************
FILE:               CapWord.cc
LANGUAGE:           C++
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This is the implementation of the CapWord class.
    This class blah_blah_blah.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
   Revision 1.1  95/11/19  08:41:50  pascal
   Initial revision
   
    1995-11-05 <PJB> Creation. 
LEGAL
    Copyright Pascal J. Bourguignon 1995 - 2001

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
#include <math.h>
#include <stdlib.h>
#include <string.h>
}
#include "names.h"
#include CapWord_hh
#include BpClass_hh
#include BcImplementation_h

    static const char rcsid[]="$Id: CapWord.cc,v 1.1 2003/12/04 03:46:04 pjbpjb Exp $";


    CharKindT charKind[256]={
        ck_sep,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,
        ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,
        ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,
        ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,ck_end,
        /*        !      "      #      $      %       &     '    */
        ck_end,ck_ini,ck_ini,ck_ini,ck_end,ck_end,ck_ini,ck_ini,
        /* (      )      *      +      ,      -       .     /    */
        ck_end,ck_ini,ck_ign,ck_ign,ck_end,ck_end,ck_sep,ck_ign,
        /* 0 1 2 3 4 5 6 7 */
        ck_dig,ck_dig,ck_dig,ck_dig,ck_dig,ck_dig,ck_dig,ck_dig,
        /* 8 9 : ; < = > ? */
        ck_dig,ck_dig,ck_end,ck_end,ck_end,ck_ini,ck_ini,ck_ign,
        /* @ A B C D E F G  */
        ck_end,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,
        /* H I J K L M N O  */
        ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,
        /* P Q R S T U V W  */
        ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,ck_cap,
        /* X Y Z [ \ ] ^ _  */
        ck_cap,ck_cap,ck_cap,ck_end,ck_end,ck_ini,ck_ini,ck_end,
        /* ` a b c d e f g */
        ck_ini,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        /* h i j k l m n o  */
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        /* p q r s t u v w  */
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        /* x y z { | } ~   */
        ck_low,ck_low,ck_low,ck_end,ck_end,ck_ini,ck_ini,ck_end,
        /* >= 128 */
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,
        ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low,ck_low
    };
    


/////////////////////////////////////////////////////////////////////////////
//// CapWord 
/////////////////////////////////////////////////////////////////////////////


    CONSTRUCTOR(CapWord)
    {
        BpClass_PLUG(CapWord);
        initers=NIL;
        letters=NIL;
        trailers=NIL;
    }//CapWord;
    
    
    DESTRUCTOR(CapWord)
    {
        if(initers!=NIL){
            free(initers);
        }
        if(letters!=NIL){
            free(letters);
        }
        if(trailers!=NIL){
            free(trailers);
        }
    }//~CapWord;
    
    
    METHOD(CapWord,makeBrother,(void),BpObject*)
    {
        return(NEW(CapWord));
    }//makeBrother;
    
    
    METHOD(CapWord,printOnLevel,(FILE* file,CARD32 level),void)
    {
        CapWord_SUPER::printOnLevel(file,level);
        PRINTONLEVEL(file,level,"%s",initers,NULLSTR(initers));
        PRINTONLEVEL(file,level,"%s",letters,NULLSTR(letters));
        PRINTONLEVEL(file,level,"%s",trailers,NULLSTR(trailers));
    }//printOnLevel;
    
    
    METHOD(CapWord,analyzeRun,
                    (const char* source,INT32* next,INT32 kinds),char*)
    {
            INT32       cNext;
            INT32       mark;
            char*       run;
            INT32       size;
            INT32       i;
            INT32       j;
            
        cNext=(*next);
        mark=cNext;
        kinds|=(1<<ck_ign);
        while(BITIN(charKind[(unsigned char)source[cNext]],(CARD32)kinds)){
            cNext++;
        }
        if(cNext>mark){
            size=cNext-mark;
            run=(char*)malloc(size+1);
            j=0;
            for(i=mark;i<cNext;i++){
                if(charKind[(unsigned char)source[i]]!=ck_ign){
                    run[j]=source[i];
                    j++;
                }
            }
            run[size]='\0';
        }else{
            run=NIL;
        }
        (*next)=cNext;
        return(run);
    }//analyzeRun;
    
    
    METHOD(CapWord,analyze,(const char* source,INT32* next),void)
    {   
            char*       uppers;
            char*       lowers;
        if(initers!=NIL){
            free(initers);
        }
        if(letters!=NIL){
            free(letters);
        }
        if(trailers!=NIL){
            free(trailers);
        }
        initers=analyzeRun(source,next,1<<ck_ini);
        uppers=analyzeRun(source,next,(1<<ck_cap));
        lowers=analyzeRun(source,next,(1<<ck_low)|(1<<ck_dig));
        if(uppers==NIL){
            if(lowers==NIL){
                letters=NIL;
            }else{
                letters=lowers;
            }
        }else{
            if(lowers==NIL){
                letters=uppers;
            }else{
                letters=(char*)malloc(strlen(uppers)+strlen(lowers)+1);
                strcpy(letters,uppers);
                strcat(letters,lowers);
                free(uppers);
                free(lowers);
            }
        }
        trailers=analyzeRun(source,next,1<<ck_end);
    }//analyze;
    
    
    METHOD(CapWord,catenate,(char* buffer,INT32* bsize),void)
    {
            INT32       cNext;
            INT32       i;

        cNext=(*bsize);
        if(initers!=NIL){
            for(i=0;initers[i]!='\0';i++){
                buffer[cNext]=initers[i];
                cNext++;
            }
        }
        if(letters!=NIL){
            for(i=0;letters[i]!='\0';i++){
                buffer[cNext]=letters[i];
                cNext++;
            }
        }
        if(trailers!=NIL){
            for(i=0;trailers[i]!='\0';i++){
                buffer[cNext]=trailers[i];
                cNext++;
            }
        }
        buffer[cNext]='\0';
        (*bsize)=cNext;
    }//catenate;
    
    
    METHOD(CapWord,reduceBy,(PressureT pressure,float ratio),void)
    {
            INT32       slice;
            INT32       len;
        
        switch(pressure){
        case Pressure_heavy:
            if(initers!=NIL){
                free(initers);
                initers=NIL;
            }
            //fall thru;
        case Pressure_medium:
            if(trailers!=NIL){
                free(trailers);
                trailers=NIL;
            }
            //fall thru;
        case Pressure_light:
            //fall thru:
        default:
            break;
        }
        
        len=length(pressure);
        slice=len-(INT32)ceil((((float)len)/ratio));
        reduceSlice(slice);
    }//reduceBy;
    
    
    METHOD(CapWord,reduceSlice,(INT32 slice),INT32)
    {
            INT32       len;
            INT32       chunk;

        chunk=0;
        if((slice>0)&&(trailers!=NIL)){
            len=strlen(trailers);
            if(len>slice){
                trailers[len-slice]='\0';
                chunk+=slice;
                slice=0;
            }else{
                chunk+=len;
                slice-=len;
                free(trailers);
                trailers=NIL;
            }
        }
        if((slice>0)&&(initers!=NIL)){
            len=strlen(initers);
            if(len>slice){
                initers[len-slice]='\0';
                chunk+=slice;
                slice=0;
            }else{
                chunk+=len;
                slice-=len;
                free(initers);
                initers=NIL;
            }
        }
        if((slice>0)&&(letters!=NIL)){
            len=strlen(letters);
            if(len>slice){
                letters[len-slice]='\0';
                chunk+=slice;
                slice=0;
            }else{
                chunk+=len;
                slice-=len;
                free(letters);
                letters=NIL;
            }
        }
        return(chunk);
    }//reduceSlice;
    
    
    METHOD(CapWord,reduceSoft,(INT32* remainder),void)
    {
        (*remainder)-=reduceSlice(1);
    }//reduceSoft;
    
    
    METHOD(CapWord,print,(FILE* file),void)
    {
        fprintf(file,"\t%s %s %s\n",
                initers?initers:"<NIL>",
                letters?letters:"<NIL>",
                trailers?trailers:"<NIL>");
    }/*print*/
    
    
    METHOD(CapWord,length,(PressureT pressure),INT32)
    {
        switch(pressure){
        case Pressure_light:
            return((initers?strlen(initers):0)
                +(letters?strlen(letters):0)
                +(trailers?strlen(trailers):0));
        case Pressure_medium:
            return((initers?strlen(initers):0)
                +(letters?strlen(letters):0));
        case Pressure_heavy: //fall thru
        default:
            return((letters?strlen(letters):0));
        }
    }//length;
            

    METHOD(CapWord,compareLength,(CapWord* other),SignT)
    {
            INT32       thislen,otherlen;
        
        thislen=length(Pressure_heavy);
        otherlen=other->length(Pressure_heavy);
        if(thislen<otherlen){
            return(-1);
        }else if(thislen>otherlen){
            return(+1);
        }else{
            thislen=length(Pressure_medium);
            otherlen=other->length(Pressure_medium);
            if(thislen<otherlen){
                return(-1);
            }else if(thislen>otherlen){
                return(+1);
            }else{
                thislen=length(Pressure_light);
                otherlen=other->length(Pressure_light);
                if(thislen<otherlen){
                    return(-1);
                }else if(thislen>otherlen){
                    return(+1);
                }else{
                    return(0);
                }
            }
        }
    }//compareLength;

    

//END CapWord.

/*** CapWord.cc                       -- 2003-11-20 03:38:35 -- pascal   ***/
