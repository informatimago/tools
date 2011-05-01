/******************************************************************************
FILE:               lrev.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program output its input files reversing the order of the line: the 
    last line is output first, the first last.
USAGE:
    lrev [file...]
    lrev <in >out
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1995/10/11 <PJB> Creation.
LEGAL
    Copyright Pascal J. Bourguignon 1995 - 2011
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LineSize    (4096)

    typedef struct llist_t{
        struct llist_t*     next;
        char*               line;
    }                   llist_t;
    typedef llist_t*    llist_p;
    
    
    static void llist_push(llist_p* ll,const char* line)
    {
            llist_p         newelem;
        newelem=(llist_p)malloc(sizeof(llist_t));
        newelem->next=*ll;
        newelem->line=(char*)malloc(sizeof(char)*(strlen(line)+1));
        strcpy(newelem->line,line);
        (*ll)=newelem;
    }/*llist_push*/
    
    
    static void llist_free(llist_p* ll)
    {
            llist_p     oldelem;
            
        oldelem=*ll;
        while(oldelem!=NULL){
            *ll=oldelem->next;
            free(oldelem->line);
            free(oldelem);
            oldelem=*ll;
        }
    }/*llist_free*/
    
    
    static void llist_generate(llist_p ll,FILE* file)
    {
        while(ll!=NULL){
            fputs(ll->line,file);
            ll=ll->next;
        }
    }/*llist_generate*/
    
    
    static void llist_read(llist_p* ll,FILE* file)
    {
            int         neof;
            char        line[LineSize];
        neof=(fgets(line,LineSize-1,file)!=NULL);
        while(neof){
            llist_push(ll,line);
            neof=(fgets(line,LineSize-1,file)!=NULL);
        }
    }/*llist_read*/
    
    
    
int main(int argc,char** argv)
{
        FILE*       file;
        llist_p     ll;
        int         i;
        int         nerrors;
    
    nerrors=0;
    ll=NULL;
    if(argc==1){
        llist_read(&ll,stdin);
        llist_generate(ll,stdout);
        llist_free(&ll);
    }else{
        for(i=1;i<argc;i++){
            file=fopen(argv[i],"r");
            if(file==NULL){
                fprintf(stderr,
                    "Cannot open file <%s> for reading; skipping.\n",
                    argv[i]);
                nerrors++;
            }else{
                llist_read(&ll,file);
                fclose(file);
                file=fopen(argv[i],"w");
                if(file==NULL){
                    fprintf(stderr,
                        "Cannot open file <%s> for writting; skipping.\n",
                        argv[i]);
                    nerrors++;
                }else{
                    llist_generate(ll,file);
                    fclose(file);
                }
                llist_free(&ll);
            }
        }
    }
    return(nerrors!=0);
}/*main*/

/*** lrev.c                           -- 2003-12-02 11:51:06 -- pascal   ***/
