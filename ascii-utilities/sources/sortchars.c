/******************************************************************************
FILE:               sortchars.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This filter sorts the characters of each input strings.
USAGE
    sortchars [-b] [-f] [-u] [-r] < input > output
        -b  Ignore and remove non-graphic characters
            (including spaces and tabulations).
        -f  Fold upper case letters onto lower case.
        -u  leave only one occurence of characters.
        -r  Reverse the sense of comparisons.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1994/03/28 <PJB> Creation.
    Revision 1.1  2003/12/04 03:46:03  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.2  2003/11/22 22:30:53  pascal
    Added options to ascii.
    Corrected some compilation problems.
    
    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:11  pascal
    Added to CVS.
    
 * Revision 1.3  1994/07/13  12:12:57  pbo
 * *** empty log message ***
 *
 * Revision 1.3  1994/07/13  12:12:57  pbo
 * *** empty log message ***
 *
 * Revision 1.2  1994/07/13  12:11:43  pbo
 * Transformed C++ comments into C comments.
 *
 * Revision 1.2  1994/07/13  12:11:43  pbo
 * Transformed C++ comments into C comments.
 *
 * Revision 1.1  94/03/28  13:11:16  pascal
 * Initial revision
 * 
LEGAL
    Copyright Pascal J. Bourguignon 1994 - 1994
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern void* qsort(void* base,size_t nmemb,size_t size,
            int(*compar)(const void*,const void*));


    static int comparechar(const void* a, const void* b)
    {
        return((*((const char*)a))-(*((const char*)b)));
    }/*comparechar;*/
    

    static int revcomparechar(const void* a, const void* b)
    {
        return((*((const char*)b))-(*((const char*)a)));
    }/*revcomparechar;*/
    

    static void usage(char* pname)
    {
        fprintf(stderr,"%s [-b] [-f] [-u] [-r] < input > output\n"
            "   -b  Ignore and remove non-graphic characters\n"
            "       (including spaces and tabulations).\n"
            "   -f  Fold upper case letters onto lower case.\n"
            "   -u  leave only one occurence of characters.\n"
            "   -r  Reverse the sense of comparisons.\n",pname);
    }/*usage;*/
    
    
int main(int argc,char** argv)
{
        char            buffer[BUFSIZ];
        unsigned char   maxchar=255;
        int             blanc=0;
        int             fold=0;
        int             unique=0;
        int(*compar)(const void*,const void*);
        int             i;
        
    compar=comparechar;
    for(i=1;i<argc;i++){
        if(strcmp(argv[i],"-b")==0){
            blanc=1;
        }else if(strcmp(argv[i],"-f")==0){
            fold=1;
        }else if((strcmp(argv[i],"-u")==0)){
            unique=1;
        }else if(strcmp(argv[i],"-r")==0){
            compar=revcomparechar;
        }else{
            usage(argv[0]);
            return(1);
        }
    }
    while(fgets(buffer,sizeof(buffer)-1,stdin)){
        if(buffer[0]!='\0'){
                int             len;
            len=strlen(buffer);
            if(blanc){
                int         ii,jj;
                ii=0;
                for(jj=0;jj<len;jj++){
                    if((' '<(unsigned char)(buffer[jj]))
                    &&((unsigned char)(buffer[jj])<=maxchar)){
                        buffer[ii]=buffer[jj];
                        ii++;
                    }
                }
                buffer[ii]='\0';
                len=ii;
            }
            if(fold){
                int             ii;
                for(ii=0;ii<len;ii++){
                    if(isupper(buffer[ii])){
                        buffer[ii]=tolower(buffer[ii]);
                    }
                }
            }
            qsort(buffer,(unsigned)len,sizeof(char),compar);
            if(unique){
                int         ii,jj;
                ii=0;
                for(jj=1;jj<len;jj++){
                    if(buffer[ii]!=buffer[jj]){
                        ii++;
                        buffer[ii]=buffer[jj];
                    }
                }
                ii++;
                buffer[ii]='\0';
                /* len=ii;*/
            }
        }
        puts(buffer);
    }
    return(0);
}/*main;*/



/*** sortchars.c                      --                     --          ***/
