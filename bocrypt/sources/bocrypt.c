/******************************************************************************
FILE:               bocrypt.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program encrypt or decript a file using the One Time Pad algorithm.
    Moreover, the key is protected with a password. The algorithm is symetric.
USAGE
    bocrypt password keyfile inputfile  > outputfile
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992-09-03 <PJB> Creation.
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 2003 - 2003
    
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

    static void usage(char* pname)
    {
        fprintf(stderr,
            "Usage: %s 'password' offset keyfile inputfile > outputfile\n",
            pname);
    }/* usage; */
    

    static char* getpassword(char* argument)
    {
            char*   password;
            int     len;
            int     i;
            
        len=strlen(argument);
        if(len<=0){
            return(NULL);
        }
        password=malloc((unsigned)(len+1));
        i=0;
        while(i<len){
            password[i]=argument[i];
            argument[i]='X';
            i++;
        }
        password[i]=(char)0;
        return(password);
    }/* getpassword; */
    

    static int getoffset(char* argument)
    {
            int     offset;
            
        offset=0;
        sscanf(argument,"%d",&offset);
        return(offset);
    }/* getoffset; */
    

    static FILE* openfile(const char* fkind,const char* fname)
    {
            FILE*       file;
        
        file=fopen(fname,"rb");
        if(file==NULL){
            fprintf(stderr,"I cannot open the %s file '%s'.\n",
                    fkind,fname);
        }
        return(file);
    }/* openfile; */
        

    static int process(char* password,int offset,FILE* key,FILE* input,FILE* output)
    {
            int         err;
            int         i;
            int         pindex;
            int         keysize;
            int         datsize;
            int         wrtsize;
            int         processedcount;
            char        keybuffer[8192];
            char        datbuffer[8192];
            
        err=fseek(key,offset,SEEK_SET);
        if(err==-1){
            perror("fseek on the key file");
            return(1);
        }
        processedcount=0;
        pindex=0;
        datsize=fread(datbuffer,sizeof(char),sizeof(datbuffer)/sizeof(char),input);
        while(datsize>0){
            keysize=fread(keybuffer,sizeof(char),(unsigned)datsize,key);
            if(keysize!=datsize){
                fprintf(stderr,"I cannot read as many bytes from the key file than from the input file!\n");
                fprintf(stderr,"Processed count=%d, datsize=%d, keysize=%d\n",
                        processedcount,datsize,keysize);
                return(1);
            }
            i=0;
            while(i<datsize){
                datbuffer[i]=((password[pindex]^keybuffer[i])^datbuffer[i]);
                pindex++;
                if(password[pindex]==(char)0){
                    pindex=0;
                }
                i++;
            }
            wrtsize=fwrite(datbuffer,sizeof(char),(unsigned)datsize,output);
            if(wrtsize!=datsize){
                fprintf(stderr,"I cannot write as many bytes to the output file than read from the input file!\n");
                fprintf(stderr,"Processed count=%d, datsize=%d, keysize=%d\n",
                        processedcount,datsize,keysize);
                return(1);
            }
            processedcount+=datsize;
            datsize=fread(datbuffer,sizeof(char),sizeof(datbuffer)/sizeof(char),input);
        }
        return(0);
    }/* process; */
    
int main(int argc,char** argv)
{   
        FILE*           key;
        FILE*           input;
        char*           password;
        int             offset;
        
    if(argc!=5){
        usage(argv[0]);
        return(1);
    }
    password=getpassword(argv[1]);
    if(password==NULL){
        fprintf(stderr,"The password must have at least one character!\n");
        usage(argv[0]);
        return(1);
    }
    offset=getoffset(argv[2]);
    if(offset<0){
        fprintf(stderr,"The offset must be an integer between 0 and the size of the key file!\n");
        usage(argv[0]);
        return(1);
    }
    key=openfile("key",argv[3]);
    input=openfile("input",argv[4]);
    if((key!=NULL)&&(input!=NULL)){
        return(process(password,offset,key,input,stdout));
    }else{
        usage(argv[0]);
        return(1);
    }
    return(0);
}/* main */

/*** bocrypt.c                        -- 2003-12-02 11:09:08 -- pascal   ***/
