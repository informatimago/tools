/******************************************************************************
FILE:               join.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program joins a file from its segments. The segments must be named
    file.xxx where xxx is a number from 000 to 999.
USAGE
    join [ -r ] [ -p ] file...  
        -r          removes the segments once appended to the file.
        -p          prints progress information.
        file        whole file name.
BUG
    * Can join only files that have been segmented with the default 
      format "%s.%03d".
    * cat can easily be used instead of join: `cat file.??? > file`.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1990/04/28 <PJB> Creation.
    1993/12/18 <PJB> Updated.
LEGAL
    Copyright Pascal J. Bourguignon 1990 - 1993
    All rights reserved.
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
MPW
    c -w -o "{Obj}Tmp:" -mbg on -y "{tmp}" -i "{dLib}CIncludes:" join.c
    link -w -o join -t MPST -c 'MPS '   æ
        "{Obj}"Tmp:join.c.o         æ
        "{CLib}"CRuntime.o                  æ
        "{CLib}"StdCLib.o                   æ
        "{CLib}"CInterface.o                æ
        "{Lib}"Interface.o
    move -y join "{upw}Tools:"
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <BcTypes.h>
#include <BcImplementation.h>

#define FileNameSize    (FILENAME_MAX)
#define BufferSize      (10240)

static void MakeSegName(char* wholename,CARD32 n,char* segname){
/*
    
*/
    char        num[4];
    CARD16      len;
    
    strcpy(segname,wholename);
    len=strlen(segname);
    if(len>FileNameSize-4){
        len=FileNameSize-4;
    }
    sprintf(num,"%03lu",n);
    segname[len]='.';       len++;
    segname[len]=num[0];    len++;
    segname[len]=num[1];    len++;
    segname[len]=num[2];    len++;
    segname[len]=(char)0;
}/*MakeSegName*/


static void Join(char* wholename,BOOLEAN rmflag,BOOLEAN progress,BOOLEAN debug){
/*
*/
    FILE*       wholefile;
    FILE*       segfile;
    CARD16      segcount;
    char        segname[FileNameSize];
    char        buffer[BufferSize];
    CARD32      wholesize;  /* wholefile size */
    CARD32      segsize;    /* segfile size */
    CARD32      csize;      /* current size */
    INT32       err;
    
    if(progress){
        fprintf(stdout,"# joining %s\n",wholename);
        fflush(stdout);
    }
    wholefile=fopen(wholename,"wb");
    if(wholefile==NIL){
        fprintf(stderr,"### Cannot open the file %s: skipping.\n",wholename);
        fflush(stderr);
    }else{
        wholesize=0;
        segcount=0;
        clearerr(wholefile);
        MakeSegName(wholename,segcount,segname);
        segfile=fopen(segname,"rb");
        
        while(segfile!=NIL){
            clearerr(segfile);
            segsize=0;
            while(!feof(segfile)){
                csize=fread(buffer,1,BufferSize,segfile);
                if(debug){
                    fprintf(stdout,"$ in while\n");
                    fprintf(stdout,"$ csize=%lu\n",csize);
                    fflush(stdout);
                }
                if(ferror(segfile)){
                    fprintf(stderr,"### Read error on %s: skipping %s.\n",segname,wholename);
                    fflush(stderr);
                    fclose(segfile);
                    break;
                }
                fwrite(buffer,1,csize,wholefile);
                if(ferror(segfile)){
                    fprintf(stderr,"### Write error on %s: skipping.\n",wholename);
                    fflush(stderr);
                    fclose(segfile);
                    break;
                }
                fflush(wholefile);
                segsize+=csize;
                if(debug){
                    fprintf(stdout,"$ csize=%lu\n",csize);
                    fprintf(stdout,"$ segsize=%lu\n",segsize);
                    fflush(stdout);
                }
            }
            fclose(segfile);
            if(progress){
                fprintf(stdout,"# %s contains %lu bytes.\n",segname,segsize);
                fflush(stdout);
            }
            wholesize+=segsize;
            if(rmflag){
                err=remove(segname);
                if(progress){
                    if(err==0){
                        fprintf(stdout,"# %s removed.\n",segname);
                    }else{
                        fprintf(stdout,"# %s NOT removed.\n",segname);
                    }
                    fflush(stdout);
                }
            }
            segcount++;

            MakeSegName(wholename,segcount,segname);
            segfile=fopen(segname,"rb");
            
        }/*while*/
        fclose(wholefile);
        if(progress){
            fprintf(stdout,"# %s contains %lu bytes.\n",wholename,wholesize);
            fflush(stdout);
        }
    }   
}/*Join*/


/* getparams - main */

static const char*  usage           ="# Usage:  %s [ -r ] [ -p ] file ...\n";

#if 0
static void getnextparam(int argc,char *argv[],INT32 params,INT32 *value,char *p1,char *p2,char *scanformat,char *format){
    INT32               res;
    
    if(params<argc){
        res=sscanf(argv[params],scanformat,value);
        if(res!=1){
            fprintf(stderr,"### %s - Syntax error in %s:%s\n%s\n",argv[0],p1,argv[params],format);
            fprintf(stderr, usage, argv[0]);
            exit(1);
        }
    }else{
        fprintf(stderr,"### %s - %s is missing.\n",argv[0],p2);
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }
}/*getnextparam*/
#endif


int main(int argc, char *argv[])
{
    BOOLEAN         progress=   FALSE;
    BOOLEAN         debug=      FALSE;
    BOOLEAN         doremove=       FALSE;
    
    CARD16          param;
    CARD16          length;
    BOOLEAN         file;
    
    file=FALSE;
    if(argc<=1){
        fprintf(stderr, usage, argv[0]);
        return(1);
    }

    param=1;
    while((param<argc)&&(!file)){
        length=strlen(argv[param]);
        if(*argv[param] != '-'){
            /* file ... */
            file=TRUE;
            break;
        }

        length=strlen(argv[param]);
        if(length!=2){
          illegal_option:
            fprintf(stderr,
                    "### %s - Illegal option: \"%s\"!\n",
                    argv[0],argv[param]);
            fprintf(stderr, usage, argv[0]);
            return(1);
        }

        switch(argv[param][1]){

        case 'r':
            doremove=TRUE;
            param++;
            break;
        case 'p':

            progress=TRUE;
            param++;
            break;
        case 'd':
            debug=TRUE;
            param++;
            break;
        default:
            goto illegal_option;
        }
    }

    if(!file){
        fprintf(stderr,"### %s - you must give a file name!\n",argv[0]);
        fprintf(stderr, usage, argv[0]);
        return(1);
    }
    if(debug){
        progress=TRUE;
    }
    while(param<argc){
        Join(argv[param],doremove,progress,debug);
        param++;
    }
    fflush(stdout);
    fflush(stderr);
    return(0);
}/*main*/

/*** join.c                           --                     --          ***/
