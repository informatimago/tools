/*****************************************************************************
FILE:               cookie.c
LANGUAGE:           c
SYSTEM:             POSIX
USER-INTERFACE:     NONE
DESCRIPTION
    
    This program search a cookie in the cookie files and print it.
    
AUTHORS
    <PJB> Pascal Bourguignon <pjb@informatimago.com>
MODIFICATIONS
    2003-12-01 <PJB> 
    1990-12-20 <PJB> Creation.
    1993-03-28 <PJB> Began updating to lookup the file "cookie.files" before
                     using the hard-coded files.
    1993-09-08 <PJB> Implemented the lookup of the file "cookie.files".
BUGS
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1990 - 2003
    
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BcTypes.h>
#include <BcRandom.h>
#include <BcImplementation.h>

#ifndef DEBUG
#define DEBUG           (0)
#endif
#define MAXFILES        (256)
#define cookienvarname  ("cookiefiles")

#ifdef MPW
#define defaultablename ("Lacuste:local:games:cookies:cookie.files")
    static  const char* defaultfiles[]={
        "Lacuste:local:games:cookies:fortune.cookies",
        "Lacuste:local:games:cookies:mit.cookies",
        "Lacuste:local:games:cookies:cmirh.cookies",
        "Lacuste:local:games:cookies:litp.cookies",
        NIL
    };
#else
#define defaultablename ("/local/lib/cookie.files")
    static  const char* defaultfiles[]={
        "/local/lib/cookies/fortune.cookies",
        "/local/lib/cookies/mit.cookies",
        "/local/lib/cookies/cmirh.cookies",
        "/local/lib/cookies/litp.cookies",
        NIL
    };
#endif

    static char*    loadedfiles[MAXFILES];

    static char* newstr(const char* str)
    {
        char* result=malloc(1+strlen(str));
        strcpy(result,str);
        return(result);
    }/*newstr*/


    static void getdefaultfilenames(void)
    {
        INT16       i;
        i=-1;
        do{
            i++;
            loadedfiles[i]=newstr(defaultfiles[i]);
        }while(defaultfiles[i]!=NIL);
    }/* ; */
    
    
    static void loadfilenames(const char* tablename)
    {
        char        filename[BUFSIZ];
        FILE*       table;
        INT16       i;
        INT32       l;
        
        if(tablename==NIL){
            tablename=defaultablename;
        }
        table=fopen(tablename,"r");
        if(table==NIL){
            table=fopen(defaultablename,"r");
        }
        if(table==NIL){
            getdefaultfilenames();
        }else{
            i=0;
            while(i<MAXFILES-1){
                if(fgets(filename,BUFSIZ,table)==NIL){
                    break;
                }
                if(filename[0]!='#'){
                    l=strlen(filename);
                    loadedfiles[i]=malloc((unsigned)l); 
                    /* (we're removing the newline). */
                    strncpy(loadedfiles[i],filename,(unsigned)(l-1));
                    loadedfiles[i][l-1]='\0';
                    i++;
                }
            }
            if(i==0){
                getdefaultfilenames();
            }else{
                loadedfiles[i]=NIL;
            }
            fclose(table);
        }
#if DEBUG
        i=0;
        while(loadedfiles[i]!=NIL){
            fprintf(stderr,"loadedfiles[%d]='%s'\n",i,loadedfiles[i]);
            i++;
        }
#endif
    }/* loadfilenames; */


#define BUFFER_SIZE (2048)

    int main(int argc,char** argv,char** envv){
        FILE*       fd;
        CARD32      sizes[MAXFILES+1];
        CARD32      total;
        CARD32      position;
        INT32       i;
        char*       n;
        char        buffer[BUFFER_SIZE];
        BOOLEAN     twice;
        BOOLEAN     foundcomment;
        BOOLEAN     foundcookie;


        if(argc>1){
            int ii;
            for(ii=1;ii<=argc;ii++){
                if(0==strcmp(argv[ii],"--help")){
                }
            }
        }
        loadfilenames(getenv(cookienvarname));
    
        sizes[MAXFILES]=0;
        i=0;
        while(loadedfiles[i]!=NIL){
            fd=fopen(loadedfiles[i],"r");
            if(fd!=NIL){
                fseek(fd,0,SEEK_END);
                sizes[i]=ftell(fd);
                sizes[MAXFILES]+=sizes[i];
                fclose(fd);
            }else{
                sizes[i]=0;
            }
            i++;
        }
    
        if(sizes[MAXFILES]==0){
            fprintf(stderr,"cookie: not a good cookie file!"
                    " (This is not a cookie!!)\n");
            return(1);
        }
    
        position=BcRandom_fromfile() % sizes[MAXFILES];
    
        total=sizes[0];
        i=1;
        while(total<position){
            total+=sizes[i];
            i++;
        }
        i--;
        fd=fopen(loadedfiles[i],"r");
        total-=sizes[i];
        position-=total;
        fseek(fd,(signed)position,SEEK_SET);
#if DEBUG
        fprintf(stderr,"Opening cookie file \"%s\", seeking from %lu.\n",
                loadedfiles[i],position);
#endif
    
        twice=FALSE;
        foundcomment=FALSE;
        foundcookie=FALSE;
        while(TRUE){
            n=fgets(buffer,BUFFER_SIZE,fd);
            if(n==NIL){
                if(twice){
                    fprintf(stderr,"cookie: bad file '%s'."
                            " (This is not a cookie!!)\n",loadedfiles[i]);
                    fclose(fd);
                    exit(1);
                }
                twice=TRUE;
                fseek(fd,0,SEEK_SET);
                continue;
            }
            if(foundcookie){
                if(buffer[0]=='#'){
                    fclose(fd);
                    exit(0);
                }else{
                    fprintf(stdout,"%s",buffer);
                }
            }else{
                if(buffer[0]=='#'){
                    foundcomment=TRUE;
                }else{
                    foundcookie=foundcomment;
                    if(foundcookie){
                        fprintf(stdout,"%s",buffer);
                    }
                }
            }
        }
    }/*main*/

/*** cookie.c                         --                     --          ***/
