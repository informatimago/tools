/******************************************************************************
FILE:               smallcrypt.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program encrypt or decript a file.
    The third char in the key file is assumed to represent the space and 
    other special characters.
    
    Note: you need a key file specificly built for this algorythm.
    
USAGE
    sencrypt keyfile [ clearfile   | < clearfile   ] > cryptedfile
    sdecrypt keyfile [ cryptedfile | < cryptedfile ] > clearfile
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1994-02-15 <PJB> Creation.
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1994 - 2011
    
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
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/file.h>

static char*        pname;
static int          quiet=0;

static void qerror(const char* format,...)
{
    va_list     plist;
    char        message[BUFSIZ];
        
    va_start(plist,format);
    vsprintf(message,format,plist);
    va_end(plist);
    perror(message);
}/*qerror*/
    

static int fsize(FILE* stream)
{
    int     res;
    int     size;
    int     pos;
            
    pos=(int)ftell(stream);
    if(pos<0){
        return(pos);
    }
    res=fseek(stream,0,SEEK_END);
    if(res<0){
        return(res);
    }else{
        size=(int)ftell(stream);
        res=fseek(stream,pos,SEEK_SET);
        if(res<0){
            return(res);
        }else{
            return(size);
        }
    }
}/*fsize;*/
            
    
static char* loadkey(FILE* keyfile)
{
    int         size;
    char*       key;
    int         len;
    char        buffer[BUFSIZ];
            
    size=fsize(keyfile);
    if(size<0){
        qerror("fsize '%s'",pname);
        return(NULL);
    }

    key=malloc((unsigned)size);
    key[0]='\0';
            
    while((size>0)&&(fgets(buffer,sizeof(buffer),keyfile)!=NULL)){
        len=(int)strlen(buffer);
        size-=len;
        if(buffer[len-1]=='\n'){
            len--;
            buffer[len]='\0';
        }
        strcat(key,buffer);
    }
        
    return(key);
}/*loadkey;*/
    
    
static char* dpos(/*char*/int c,/*char*/int d,const char* key,int keyindex)
{
    char*       kip;
        
    kip=index(key+keyindex,c);
    while(kip!=NULL){
        if(kip[1]==d){
            return(kip);
        }
        kip=index(kip+1,c);
    }
    kip=index(key,c);
    while(kip!=NULL){
        if(kip[1]==d){
            return(kip);
        }
        kip=index(kip+1,c);
    }
    return(NULL);
}/*dpos;*/
    
    
static int encrypt(const char* key,FILE* input,FILE* output)
{
    int             keylength;
    int             keyindex;
    const char*     kip;
    const char*     kop;
    int             c;
    int             count;
        
    count=0;
    keylength=(int)strlen(key);
    keyindex=2;
    c=fgetc(input);
    while(c!=EOF){
        if((c!='\n')&&(c!='\0')){
            kip=index(key+keyindex,c);
            if(kip==NULL){
                kip=index(key,c);
            }
            if(kip==NULL){
                if((c!=' ')&&(!quiet)){
                    fprintf(stderr,"%s error:   char '%c' not in "
                            "key file. Substituted with a space.\n",
                            pname,c);
                }
                c=key[2];
                kip=index(key+keyindex,c);
                if(kip==NULL){
                    kip=key+2;
                }
            }
                
            kop=2+dpos(kip[-2],kip[-1],key,keyindex-2);
            if((key+keyindex<=kop)&&(kop<kip)&&(!quiet)){
                fprintf(stderr,"%s warning: char '%c' from "
                        "keyindex=%d will be wrongly decoded as '%c'\n",
                        pname,c,keyindex,*kop);
            }
                
            keyindex=(int)(kip-key);
            if(EOF==fputc(kip[-2],output)){
                qerror("fputc to output");
                return(4);
            }
            if(EOF==fputc(kip[-1],output)){
                qerror("fputc to output");
                return(4);
            }
            count+=2;
            if(count>=72){
                count=0;
                if(EOF==fputc('\n',output)){
                    qerror("fputc to output");
                    return(4);
                }
            }
            keyindex++;
            if(key[keyindex]=='\0'){
                keyindex=2;
            }
        }
        c=fgetc(input);
    }
    if(EOF==fputc('\n',output)){
        qerror("fputc to output");
        return(4);
    }
    return(0);
}/*encrypt;*/

    
static int decrypt(const char* key,FILE* input,FILE* output)
{
    int             keylength;
    int             keyindex;
    const char*     kip;
    int             c;
    int             d;
    int             count;
        
    count=0;
    keylength=(int)strlen(key);
    keyindex=0;
    c=fgetc(input);
    while(c!=EOF){
        while((c=='\n')||(c=='\0')){
            c=fgetc(input);
        }
        d=fgetc(input);
        while((d=='\n')||(d=='\0')){
            d=fgetc(input);
        }
        if(d!=EOF){
            kip=dpos(c,d,key,keyindex);
            if(kip==NULL){
                if(!quiet){
                    fprintf(stderr,"%s error:   char '%c' not in "
                            "key file. Substituted with a '?'\n",
                            pname,c);
                }
                if(EOF==fputc('?',output)){
                    qerror("fputc to output");
                    return(4);
                }
                count++;
            }else{
                kip+=2;
                c=*kip;
                keyindex=(int)(kip-key);
                if(c==key[2]){
                    c=' ';
                }
                if(EOF==fputc(c,output)){
                    qerror("fputc to output");
                    return(4);
                }
                count++;
            }
                
            if((count>=60)&&((c==' ')||(c=='?'))){
                count=0;
                if(EOF==fputc('\n',output)){
                    qerror("fputc to output");
                    return(4);
                }
            }
            if(key[keyindex]=='\0'){
                keyindex=0;
            }
        }
        c=fgetc(input);
    }
    if(EOF==fputc('\n',output)){
        qerror("fputc to output");
        return(4);
    }
    return(0);
}/*decrypt;*/
    
    
static void usage(const char* upname)
{
    fprintf(stderr,"%s usage:\n"
            "   sencrypt [-q] keyfile [ clearfile   | < clearfile   ] > cryptedfile\n"
            "   sdecrypt [-q] keyfile [ cryptedfile | < cryptedfile ] > clearfile\n",
            upname);
}/*usage;*/


int main(int argc,char* argv[])
{
    int         encrypting=0;
    FILE*       keyfile;
    FILE*       inputfile;
    char*       key;
    int         res;
    int         i;
    char*       kfname=NULL;
    char*       ifname=NULL;
        
    pname=rindex(argv[0],'/');
    if(pname==NULL){
        pname=argv[0];
    }else{
        pname++;
    }
    
    if(strcmp(pname,"sencrypt")==0){
        encrypting=1;
    }else if(strcmp(pname,"sdecrypt")==0){
        encrypting=0;
    }else{
        fprintf(stderr,
                "You should do:\n"
                "    ln -s %s sencrypt\n"
                "    ln -s %s sdecrypt\n"
                " and use sencrypt or sdecrypt instead of %s.\n",
                pname,pname,pname);
        usage(pname);
        return(1);
    }
    
    for(i=1;i<argc;i++){
        if(strcmp(argv[i],"-q")==0){
            quiet=1;
        }else if(kfname==NULL){
            kfname=argv[i];
        }else if(ifname==NULL){
            ifname=argv[i];
        }else{
            usage(pname);
            return(1);
        }
    }

    if(kfname==NULL){
        fprintf(stderr,"%s: The keyfile is missing!\n",pname);
        usage(pname);
        return(1);
    }
    
    keyfile=fopen(kfname,"r");
    if(keyfile==NULL){
        qerror("Opening %s",argv[1]);
        return(2);
    }
    
    if(ifname==NULL){
        inputfile=stdin;
    }else{
        inputfile=fopen(argv[2],"r");
        if(inputfile==NULL){
            qerror("Openning %s",argv[2]);
            return(2);
        }
    }
        
    key=loadkey(keyfile);
    if(key==NULL){
        return(3);
    }
    if(encrypting){
        res=encrypt(key,inputfile,stdout);
    }else{
        res=decrypt(key,inputfile,stdout);
    }
    free(key);
    fclose(keyfile);
    return(res);
}/*main.*/

/**** THE END ****/
