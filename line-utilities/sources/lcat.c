/******************************************************************************
FILE:               lcat.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    Takes arguments files as input and writes to stdout lines made 
    of the concatenation of lines from each input files.
    Example:
        lcat - tete -L toto -T tata titi -e tutu -t! toto tyty
    gives:
        stdinline1teteline1
        totoline1   tataline1   titiline1tutuline1!totoline1!tytyline1
        stdinline2teteline2
        totoline2   tataline2   titiline2tutuline2!totoline2!tytyline2
        ...
USAGE
    lcat ( [ -L | -T | -tc | -t | -e ] file | - ) ...
        -L  use the new line character to separate output fields.
            (this merges the input files line by line).
        -T  use the TAB character to separate output fields.
        -tc use the character c to separate output fields. 
        -t  use \0 to separate output fields.
        -e  (default) do not separate the output fields.
        -   stand for the stdin. Default does not read stdin.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/10/10 <PJB> Creation.
BUGS
    lines from input file may be no longer than 4*BUFSIZ.
LEGAL
    Copyright Pascal J. Bourguignon 1993 - 2011
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

    static void usage(char* name)
    {
        printf("%s usage:\n\t%s  [-T|-L|-t<c>|-e] [filename|-] ... "
                "< stdin > stdout\n",name,name);
    }/*usage;*/

#define linesize    (4*BUFSIZ)

#define sepnone     '\0'
#define septhis     '\1'

int main(int argc,char** argv)
{
    int             i;
    int             ffirst;
    int             flast;
    int             ocount;
    typedef struct {
        FILE*           file;
        char            dosep;
        char            separator;
    }               FDescT;
    FDescT*         input;
    char            buffer[linesize];
        
    input=malloc(sizeof(FDescT)*(size_t)(argc+1));
    if(input==NULL){
        sprintf(buffer,"%s fatal error: allocating argument descriptors:",
                argv[0]);
        perror(buffer);
        return(1);
    }
    input[0].file=stdin;
    input[0].dosep=sepnone;
    input[0].separator='\0';
    ffirst=1;
    flast=1;
    i=1;
    while(i<argc){
        if(strcmp("-T",argv[i])==0){
            input[flast-1].dosep=septhis;
            input[flast-1].separator='\t';
            input[flast].dosep=septhis;
            input[flast].separator='\t';
        }else if(strcmp("-L",argv[i])==0){
            input[flast-1].dosep=septhis;
            input[flast-1].separator='\n';
            input[flast].dosep=septhis;
            input[flast].separator='\n';
        }else if(strcmp("-e",argv[i])==0){
            input[flast-1].dosep=sepnone;
            input[flast-1].separator='\0';
            input[flast].dosep=sepnone;
            input[flast].separator='\0';
        }else if(strncmp("-t",argv[i],2)==0){
            input[flast-1].dosep=septhis;
            input[flast-1].separator=argv[i][2];
            input[flast].dosep=septhis;
            input[flast].separator=argv[i][2];
        }else if(strcmp("-",argv[i])==0){
            ffirst=1;
            input[flast].dosep=input[flast-1].dosep;
            input[flast].separator=input[flast-1].separator;
            input[flast].file=stdin;
            flast++;
            input[flast].dosep=septhis;
            input[flast].separator='\n';
        }else if(strncmp("-",argv[i],1)==0){
            usage(argv[0]);
            return(1);
        }else{
            input[flast].dosep=input[flast-1].dosep;
            input[flast].separator=input[flast-1].separator;
            input[flast].file=fopen(argv[i],"r");
            if(input[flast].file==NULL){
                sprintf(buffer,"%s warning: cannot open the file '%s'",
                        argv[0],argv[i]);
                perror(buffer);
            }else{
                flast++;            
                input[flast].dosep=septhis;
                input[flast].separator='\n';
            }
        }
        i++;
    }
    ocount=flast-ffirst;
    while(ocount>0){
        i=ffirst;
        while(i<flast){
            if(input[i].file!=NULL){
                char* r=fgets(buffer,linesize,input[i].file);
                if(r==NULL){
                    fclose(input[i].file);
                    input[i].file=NULL;
                    ocount--;
                }else{
                    size_t l=strlen(buffer);
                    if(buffer[l-1]=='\n'){
                        buffer[l-1]='\0';
                    }
                    fprintf(stdout,"%s",buffer);
                }
            }
            if(input[i].dosep==septhis){
                fprintf(stdout,"%c",input[i].separator);
            }
            i++;
        }
        if(input[i].dosep==septhis){
            fprintf(stdout,"%c",input[i].separator);
        }
    }
    free(input);
    return(0);
}

/*** lcat.c                           -- 2003-12-02 11:50:39 -- pascal   ***/
