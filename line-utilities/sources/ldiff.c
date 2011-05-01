/******************************************************************************
FILE:               ldiff.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This program build the difference (set operator) between two input files.
        ie.:
            file1:      file2:          ldiff file1 file2
            aaaa        bbb             aaaa
            bbb         ccc             ddd
            bbb         eee             <EOF>
            ccc         <EOF>
            ddd                 
            <EOF>
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1992/12/18 <PJB> Creation.
LEGAL
    GPL
    
    Copyright Pascal J. Bourguignon 1992 - 2011
    
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
#include <string.h>

#define LineSize    (4096)

static void ldiff(FILE* file1,FILE* file2,FILE* output)
{
        char        line1[LineSize];
        char        line2[LineSize];
        int         neof1;
        int         neof2;
        int         cmp;
        
    neof1=(fgets(line1,LineSize-1,file1)!=NULL);
    neof2=(fgets(line2,LineSize-1,file2)!=NULL);
    while((neof1)&&(neof2)){
    /*  
        line1 < line2   => fputs(line1,output); fgets(line1,LineSize-1,file1);
        line1 = line2   => fgets(line1,LineSize-1,file1);
        line1 > line2   => fgets(line2,LineSize-1,file2);
    */
        cmp=strcmp(line1,line2);
        if(cmp<0){
            fputs(line1,output);
            neof1=(fgets(line1,LineSize-1,file1)!=NULL);
        }else if(cmp==0){
            neof1=(fgets(line1,LineSize-1,file1)!=NULL);
        }else{
            neof2=(fgets(line2,LineSize-1,file2)!=NULL);
        }
    }
    while(neof1){
        fputs(line1,output);
        neof1=(fgets(line1,LineSize-1,file1)!=NULL);
    }
}/*ldiff*/

int main(int argc,char** argv)
{
    FILE*       file1;
    FILE*       file2;
    
    if(argc!=3){
        fprintf(stderr,"# usage: %s file1 file2  \n",argv[0]);
        fprintf(stderr,"# outputs all lines in file1 not in file2 "
                        "(sorted files).\n");
        return(1);
    }
    
    file1=fopen(argv[1],"r");
    if(file1==NULL){
        fprintf(stderr,"Cannot open file <%s>.\n",argv[1]);
        return(8+1);
    }
    
    file2=fopen(argv[2],"r");
    if(file2==NULL){
        fprintf(stderr,"Cannot open file <%s>.\n",argv[2]);
        return(8+2);
    }
    
    ldiff(file1,file2,stdout);
    
    fclose(file1);
    fclose(file2);
    return(0);
}/*main*/

/*** ldiff.c                          -- 2003-11-18 19:41:46 -- pascal   ***/
