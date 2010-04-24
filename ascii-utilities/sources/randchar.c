/******************************************************************************
FILE:               randchar.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program generate ad infinitum lines of 78 ASCII printable characters,
    or a continuous flow of ASCII printable characters.
USAGE
    randchar [-n] [-w charcount] [-c charcount | -l linecount] 
            [ -r mincode maxcode ]... > randomtextfile
        -n      print a continuous flow (no eoln).
        -w      specify the line length (default=78, max=4095).
        -c      specify the number of ASCII printable characters generated.
        -l      specify the number of lines generated 
                (-l suppercedes -n).
        -r min max  
                defines the range of generated characters. min and max are
                decimal ASCII codes, or extended (127<code<256). Several
                ranges may be specified.
                
    default is an infinite flow of 78-character lines, in the range 32..126.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/09/26 <PJB> Updated.
    1994/01/06 <PJB> Added -n, -l, -c, -w options.
    1994/02/15 <PJB> Added -r option.
    Revision 1.2  2004/06/24 18:19:37  pjbpjb
    Updated. Added a concatenate function to paste back split strings for
    ISO-C limitations.

    Revision 1.1  2003/12/04 03:46:03  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:10  pascal
    Added to CVS.
    
    Revision 1.7  95/11/04  00:08:09  pascal
    Corrected a bug in BcRandom.
    
    Revision 1.6  95/09/08  19:52:32  pascal
    no comment.
    
    Revision 1.5  1994/07/13  12:11:43  pbo
    Transformed C++ comments into C comments.

    Revision 1.4  1994/02/15  06:07:37  pascal
    Removed debugging message.

    Revision 1.3  94/02/15  05:21:28  pascal
    Added RCS keywords.
    
LEGAL
    Copyright Pascal J. Bourguignon 1993 - 1994
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined(linux)||defined(AIX)
#include <unistd.h>
#else
#include <libc.h>
#endif
#include <concatenate.h>
#include <BcRandom.h>
#include <BcImplementation.h>


    static FILE*    randfile;
    static CARD32   X[56];
    static CARD32   save[56];
    static BOOLEAN  copying;
    

    static void CopyX(CARD32* from,CARD32* to)
    {
            INT16       i;

        i=0;
        copying=TRUE;
        while(i<56){
            to[i]=from[i];
            INC(i);
        }
        copying=FALSE;
    }/*CopyX;*/
    

    static void handler(int sig)
    {
        if(copying){
            CopyX(&X[0],&save[0]);
        }
        BcRandom_save(randfile,save);
        exit(0);
    }/*handler;*/

    static const char*  DefaultCharList=
    " !\"#$%&'()*+,-./0123456789:;<=>?"
    "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
    "`abcdefghijklmnopqrstuvwxyz{|}~";
    
    static char CharList[258];
    static int  CharListCount;
    static int  CharSet[256/32];
    
#define setinclude(set,bit)     ((set)[(bit)/32]|=(1<<((bit)%32)))
#define setexclude(set,bit)     ((set)[(bit)/32]&=(~(1<<((bit)%32))))
#define setcontains(set,bit)    (((set)[(bit)/32]&(1<<((bit)%32)))!=0)
#define setempty(set)   \
        {int setempty_i;\
        for(setempty_i=sizeof(set)/sizeof(int)-1;setempty_i>=0;setempty_i--){\
        (set)[setempty_i]=0;}}


#define maxbuffer               (4096)

    int main(int argc,char* argv[])
    {
        int         i;
        char        buffer[maxbuffer];
        long int    count=0;
        int         linelength=78;
        int         countmode=0;    /* 0 = infinite, 1 = characters, 2 = lines.*/
        int         linemode=1;     /* 0 = no line, 1 = 78-char lines.*/
        int         generate;
    
    
        strcpy(CharList,DefaultCharList);
        CharListCount=strlen(CharList);
        setempty(CharSet);
    
        for(i=1;i<argc;i++){
            if(strcmp(argv[i],"-n")==0){
                if(countmode!=2){
                    linemode=0;
                }
            }else if((strcmp(argv[i],"-w")==0)&&(++i<argc)){
                linelength=atoi(argv[i]);
                if((linelength<=0)||(maxbuffer<linelength)){
                    linelength=78;
                }
            }else if((strcmp(argv[i],"-l")==0)&&(++i<argc)){
                linemode=1;
                countmode=2;
                count=atoi(argv[i]);
            }else if((strcmp(argv[i],"-r")==0)&&(++i<argc-1)){
                int     min,max,k;
                min=atoi(argv[i++]);
                max=atoi(argv[i]);
                if(min>max){
                    fprintf(stderr,"%s: range minimum %d (%s) cannot be greater"
                            " than maximum %d (%s).\n",
                            argv[0],min,argv[i-1],max,argv[i]);
                    return(1);
                }else if((min<0)||(255<max)){
                    fprintf(stderr,"%s: range minimum %d (%s) and maximum "
                            "%d (%s) must both belong to the [0, 255] range.\n",
                            argv[0],min,argv[i-1],max,argv[i]);
                    return(1);
                }else{
                    CharListCount=0;
                    for(k=min;k<=max;k++){
                        setinclude(CharSet,k);
                    }
                }
            }else if((strcmp(argv[i],"-c")==0)&&(++i<argc)){
                countmode=1;
                count=atoi(argv[i]);
            }else{
                fprintf(
                    stderr,
                    concatenate(
                        "%s usage:\n",
                        "%s [-n] [-w charcount] [-c charcount | -l linecount] \n",
                        "   [-r mincode maxcode]... > randomtextfile \n",
                        "     -n    print a continuous flow (no eoln). \n",
                        "     -w    specify the line length (default=78, max=4095). \n",
                        "     -c    specify the number of ASCII printable characters generated. \n",
                        "     -l    specify the number of lines generated  \n",
                        "           (-l suppercedes -n). \n",
                        "     -r min max\n",
                        "           defines the range of generated characters. min and max are\n",
                        "           decimal ASCII codes, or extended (127<code<256). Several\n",
                        "           ranges may be specified.\n",
                        "     default is an infinite flow of 78-character lines, \n",
                        "     in the range 32..126.\n",
                        0),
                    argv[0],argv[0]);
                return(1);
            }
        }/*for arg*/
    
        if(CharListCount==0){
            int         ii,jj;
            jj=0;
            for(ii=0;ii<256;ii++){
                /*DEBUG printf("%d: %s\n",ii,setcontains(CharSet,ii)?"in":"out");*/
                if(setcontains(CharSet,ii)){
                    CharList[jj]=(char)ii;
                    jj++;
                }
            }
            CharListCount=jj;
        }
    
        /* CharListCount > 0, for strlen(DefaultCharList)>0 and min<=max.*/
    
        /* printf("%d : %s\n",MaxAscii,Ascii);*/
        randfile=BcRandom_load(X);
        CopyX(&X[0],&save[0]);
        signal(SIGINT,handler);
        generate=(countmode==0)||(count>0);
        while(generate){
            i=0;
            while(i<linelength){
                buffer[i]=CharList[BcRandom_random(X) % CharListCount];
                i++;
            }
            buffer[i]='\0';
            CopyX(&X[0],&save[0]);
        
            switch(countmode){
            case 1:/*character count*/
                if(count<linelength){
                    buffer[count]='\0';
                    count=0;
                }else{
                    count-=linelength;
                }
                generate=(count>0);
                break;
            case 2:/*line count*/
                count--;
                generate=(count>0);
                break;
            case 0:/*infinite*/
                /* fall thru*/
            default:
                break;
            }/*switch*/
            if(linemode){
                puts(buffer);           /* puts adds a newline.*/
            }else{
                fputs(buffer,stdout);   /* fputs doesn't add a newline.*/
            }   
        }
        BcRandom_save(randfile,save);
        return(0);
    }/*main*/


/*** randchar.c                       --                     --          ***/
