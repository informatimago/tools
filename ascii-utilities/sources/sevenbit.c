/******************************************************************************
FILE:               sevenbit.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This is a filter to convert between 8-bit ASCII and 7-bit ASCII streams.
    Accented letters are converted to custom \?? sequences, and other extended
    characters (>127) are converted to \#?? sequences.
print_usage
    sevenbit    [--encoding encname] < 8_bit_stream > 7_bit_stream
    sevenbit -8 [--encoding encname] < 7_bit_stream > 8_bit_stream
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/08/10 <PJB> Creation.
    Revision 1.2  2004/06/24 18:19:37  pjbpjb
    Updated. Added a concatenate function to paste back split strings for
    ISO-C limitations.

    Revision 1.1  2003/12/04 03:46:03  pjbpjb
    Cleaned-up.
    Fix.

    Revision 1.1  2001/04/30 01:58:08  pascal
    Initial entry into CVS.
    
    Revision 1.1  2001/04/27 06:45:11  pascal
    Added to CVS.
    
    Revision 1.4  96/06/04  04:53:57  pascal
    Added some character encodings.
    
    Revision 1.3  1994/07/13  12:11:43  pbo
    Transformed C++ comments into C comments.

    Revision 1.2  1994/02/15  05:21:29  pascal
    Added RCS keywords.

LEGAL
    Copyright Pascal J. Bourguignon 1993 - 1993
    All rights reserved.
    This program or any part of it may not be included in any commercial 
    product without the author written permission. It may be used freely for 
    any non-commercial purpose, provided that this header is always included.
******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <Bencoding.h>


    static const char* pname="sevenbit";


    static const char* default_encoding_name(void)
    {
        const char* default_name="isolatin1";
        if((BenEncodingTableT*)0==BenEncodingTableNamed(default_name)){
            default_name=(BenEncodingTableNames())[0];
        }
        return(default_name);
    }/*default_encoding_name*/


    static char default_escape_char(void)
    {
        return('\\');
    }/*default_escape_cahr*/


    static char default_number_char(void)
    {
        return('#');
    }/*default_escape_cahr*/


    static BenApproximationTableT* approx;
    static BenEncodingTableT*  eightBitEncoding;
    static char  escapeChar;
    static char  numberChar;


#if 0
    static int hexatoint(char c)
    {
        if(c<='9'){
            return(c-'0');
        }else if(c<='F'){
            return(c-'A'+10);
        }else{
            return(c-'a'+10);
        }
    }/*hexatoint;*/
    

    static char inttohexa(int d)
    {
        if(d<10){
            return('0'+d);
        }else{
            return('a'+d-10);
        }
    }/*inttohexa;*/


    static char high_nibble(char c)
    {
        int d=((unsigned char)c)>>4;
        if(d<10){
            return('0'+d);
        }else{
            return('a'+d-10);
        }
    }/*high_nibble*/


    static char low_nibble(char c)
    {
        int d=((unsigned char)c)&0xf;
        if(d<10){
            return('0'+d);
        }else{
            return('a'+d-10);
        }
    }/*low_nibble*/
#endif

    static void update_approx(BenApproximationTableT* appr,/*char*/int escapeCh)
    {
        int i;
        char* escapeStr=malloc(2);
        escapeStr[0]=escapeCh;
        escapeStr[1]='\0';
        for(i=0;i<appr->count;i++){
            appr->approximations[i].asciiApproximation=escapeStr;
        }
    }/*update_approx*/


    static BenApproximationTableT* copy_approximation_table(
                                  BenApproximationTableT* appr)
    {
        BenApproximationTableT* copy;
        char*                   copyData;
        int l;
        int i; 
        int j;

        l=0;
        for(i=0;i<appr->count;i++){
            l+=strlen(appr->approximations[i].asciiApproximation)+1;
        }

        copy=(BenApproximationTableT*)malloc(sizeof(BenApproximationTableT));
        copy->count=appr->count;
        copy->approximations=(BenApproximationT*)
            malloc(sizeof(BenApproximationT)*(copy->count+1));
        copyData=(char*)malloc(sizeof(char)*l);

        for(i=0;i<copy->count;i++){
            copy->approximations[i].charName=appr->approximations[i].charName;
            copy->approximations[i].asciiApproximation=copyData;
            j=0;
            while(appr->approximations[i].asciiApproximation[j]!='\0'){
                (*copyData++)=appr->approximations[i].asciiApproximation[j];
                j++;
            }
            (*copyData++)=appr->approximations[i].asciiApproximation[j];
        }
        return(copy);
    }/*copy_approximation_table*/

#if 0
    static int approx_compare(const void* a,const void* b)
    {
        const BenApproximationT* aa=(const BenApproximationT*)a;
        const BenApproximationT* bb=(const BenApproximationT*)b;
        return(strcmp(aa->charName,bb->charName));
    }/*approx_compare*/


    static BenApproximationTableT* escape_approximation_table(BenEncodingTableT* encoding)
    {
        static BenApproximationTableT approx={0,0};
        int i; 
        int j;

        if(approx.count==0){
            char* data=(char*)malloc(sizeof(char)*5*128);
            approx.approximations=(BenApproximationT*)
                malloc(sizeof(BenApproximationT)*129);
            j=0;
            for(i=0;i<129;i++){
                unsigned char c=128+i;
                if((*encoding)[c]!=0){
                    approx.approximations[j].charName=(*encoding)[c];
                    approx.approximations[j].asciiApproximation=data;
                    j++;
                    (*data++)=escapeChar;
                    (*data++)=numberChar;
                    (*data++)=high_nibble(c);
                    (*data++)=low_nibble(c);
                    (*data++)='\0';
                }
            }
            approx.approximations[j].charName=0;
            approx.approximations[j].asciiApproximation=0;
            approx.count=j;
            qsort(approx.approximations,j,sizeof(approx.approximations[0]),
                  approx_compare);
        }
        return(&approx);
    }/*escape_approximation_table*/
#endif

/* ------------------------------------------------------------------------ */

    static unsigned char* getascii(FILE* in)
        /*
            PRE:        escapeChar numberChar have been read from in.
            RETURN:     either a string containing the character with the code
                               read as two hexadecimal digits from in,
                        or the escape sequence, depending on the available data.
        */
    {
        static unsigned char buffer[8];
        int                  b;
        unsigned int         u;
            
        b=fgetc(in);
        if(b==EOF){
            buffer[0]=escapeChar;
            buffer[1]=numberChar;
            buffer[2]='\0';
            return(buffer);
        }
        buffer[0]=b;
        
        b=fgetc(in);
        if(b==EOF){
            buffer[2]=buffer[0];
            buffer[0]=escapeChar;
            buffer[1]=numberChar;
            buffer[3]='\0';
            return(buffer);
        }
        buffer[1]=b;
        buffer[2]='\0';
        
        if(sscanf((const char*)buffer,"%x",&u)!=1){
            buffer[2]=buffer[0];
            buffer[3]=buffer[1];
            buffer[0]=escapeChar;
            buffer[1]=numberChar;
            buffer[4]='\0';
        }else{
            buffer[0]=u;
            buffer[1]='\0';
        }
        return(buffer);
    }/*getascii;*/
    
    
    static const char* convtoeight(char* s)
        /*
            NOTE:       s should be an abreviation "\ accent letter".
            RETURN:     either a string containing the character corresponding to
                               s in the eightBitEncoding,
                        or the character name corresponding to s,
                        or s itself, depending on the available data.
        */
    {
        static char buffer[8];
        int i;
        for(i=0;i<approx->count;i++){
            if(strcmp(s,approx->approximations[i].asciiApproximation)==0){
                const char* charName=approx->approximations[i].charName;
                int code;
                for(code=0;code<256;code++){
                    if((((*eightBitEncoding)[code])!=0)
                    &&(strcmp((*eightBitEncoding)[code],charName)==0)){
                        buffer[0]=(char)code;
                        buffer[1]='\0';
                        return(buffer);
                    }
                }
                fprintf(stderr,"%s: No code found in target encoding for the "
                        "character %s.\n",pname,charName);
                return(charName);
            }
        }
        fprintf(stderr,"%s: Abreviation %s not found.\n",pname,s);
        return(s);
    }/*convtoeight;*/
    

    static void seventoeight(FILE* in,FILE* out)
    {
            int         b;
            char        buffer[8];
            
        b=fgetc(in);
        while(b!=EOF){
            if(b==escapeChar){
                b=fgetc(in);
                if(b==EOF){
                    fputc(escapeChar,out);
                    continue;
                }else if(b==numberChar){
                    fputs((char*)getascii(in),out);
                }else if(b==escapeChar){
                    fputc(escapeChar,out);
                }else{
                    buffer[0]=escapeChar;
                    buffer[1]=b;
                    b=fgetc(in);
                    if(b==EOF){
                        buffer[2]='\0';
                        fputs(buffer,out);
                        continue;
                    }else{
                        buffer[2]=b;
                        buffer[3]='\0';                 
                        fputs(convtoeight(buffer),out);
                    }
                }
            }else{
                fputc(b,out);           
            }
            b=fgetc(in);
        }
    }/*seventoeight;*/

/* ------------------------------------------------------------------------ */

    static const char* convtoseven(/*unsigned char*/int c)
    {
        static char             buffer[8];
        int                     i;

        if(127<c){
            const char* charName=(*eightBitEncoding)[c];
            if(charName==0){
                i=-1;
            }else{
                i=BenFindCharNameInApproximationTable(approx,charName);
            }
            if(i<0){
                sprintf(buffer,"%c%c%02x",escapeChar,numberChar,c);
                return(buffer);
            }else{
                return(approx->approximations[i].asciiApproximation);
            }
        }else if(c==escapeChar){
            buffer[0]=escapeChar;
            buffer[1]=escapeChar;
            buffer[2]='\0';
            return(buffer);
        }else{
            buffer[0]=(char)c;
            buffer[1]='\0';
            return(buffer);
        }
    }/*convtoseven;*/
    

    static void eighttoseven(FILE* in,FILE* out)
    {
        int                 b;
        
        b=fgetc(in);
        while(b!=EOF){
            fputs(convtoseven(b),out);
            b=fgetc(in);
        }
    }/*eighttoseven;*/
    
/* ------------------------------------------------------------------------ */

    static void print_usage(const char* upname)
    {
        fprintf(stderr,"%s usage:\n"
                "\t%s [-8] [--encoding encname] \n"
                "\t\t[--escape-char char] [--escape-char-code number]\n"
                "\t\t[--number-char char] [--number-char-code number]\n"
                "\t\t< input > output\n",
                upname,upname);
        fprintf(stderr,"The escape and number characters must be 7-bit ASCII "
                "charactergs.\n");
        fprintf(stderr,"Default encoding     is: %s\n",default_encoding_name());
        fprintf(stderr,"Default escape char  is: %c (%d)\n",
                default_escape_char(),default_escape_char());
        fprintf(stderr,"Default number char  is: %c (%d)\n",
                default_number_char(),default_number_char());
        {
            const char** codeNames=BenEncodingTableNames();
            const char* message="Available encodings are: ";
            int code=0;
            int col=strlen(message)+strlen(codeNames[code])+2;
            fprintf(stderr,"%s%s",message,codeNames[code]);
            code++;
            while(codeNames[code]!=0){
                col+=strlen(codeNames[code])+2;
                if(col>72){
                    fprintf(stderr,",\n    %s",codeNames[code]);
                    col-=68;
                }else{
                    fprintf(stderr,", %s",codeNames[code]);
                }
                code++;
            }
            fprintf(stderr,".\n");
        }
    }/*print_usage;*/


    static void expect_argument(BOOLEAN ok,const char* message,...)
    {
        va_list ap;
        if(!ok){
            fprintf(stderr,"%s error: ",pname);
            va_start(ap,message);
            vfprintf(stderr,message,ap);
            va_end(ap);
            print_usage(pname);
            exit(1);
        }
    }/*expect_argument*/


int main(int argc,char** argv)
{
    BOOLEAN doEightToSeven=TRUE;
    const char* encodingName=default_encoding_name();
    int i;

    approx=BenApproximationTableNamed("accentApproximation");
    escapeChar=default_escape_char();
    numberChar=default_number_char();
    pname=argv[0];

    for(i=1;i<argc;i++){ 
        if((strcmp(argv[i],"-8")==0)||(strcmp(argv[i],"--8")==0)){
            doEightToSeven=FALSE;
        }else if(strcmp(argv[i],"--escape-char")==0){
            int code;
            i++;
            expect_argument(i<argc,"Expected a character after --escape-char.");
            expect_argument(strlen(argv[i])==1,"Only one character can be used "
                            "as escape character, not '%s'.",argv[i]);
            code=(unsigned char)(argv[i][0]);
            expect_argument((0<=code)&&(code<=127),"I expect 7-bit ASCII "
                            "character (code must be between 0 and 127), "
                            "not %d.\n",code);
            escapeChar=(char)code;
        }else if(strcmp(argv[i],"--escape-char-code")==0){
            int code;
            i++;
            expect_argument(i<argc,"Expected a number after --escape-char-code.");
            expect_argument((sscanf(argv[i],"%d",&code)==1),
                            "Expected a number after --escape-char-code, "
                            "not '%s'.",argv[i]);
            expect_argument((0<=code)&&(code<=127),"I expect 7-bit ASCII "
                            "character (code must be between 0 and 127), "
                            "not %d.\n",code);
            escapeChar=(char)code;
        }else if(strcmp(argv[i],"--number-char")==0){
            int code;
            i++;
            expect_argument(i<argc,"Expected a character after --number-char.");
            expect_argument(strlen(argv[i])==1,"Only one character can be used "
                            "as number character, not '%s'.",argv[i]);
            code=(unsigned char)(argv[i][0]);
            expect_argument((0<=code)&&(code<=127),"I expect 7-bit ASCII "
                            "character (code must be between 0 and 127), "
                            "not %d.\n",code);
            numberChar=argv[i][0];
        }else if(strcmp(argv[i],"--number-char-code")==0){
            int code;
            i++;
            expect_argument(i<argc,"Expected a number after --number-char-code.");
            expect_argument((sscanf(argv[i],"%d",&code)==1),
                            "Expected a number after --number-char-code, "
                            "not '%s'.",argv[i]);
            expect_argument((0<=code)&&(code<=127),"I expect 7-bit ASCII "
                            "character (code must be between 0 and 127), "
                            "not %d.\n",code);
            if(code==0){
                fprintf(stderr,"%s warning: escape code null will probably "
                        "run into bugs in a lot of programs.\n",pname);
            }
            numberChar=(char)code;
        }else if(strcmp(argv[i],"--encoding")==0){
            i++;
            expect_argument(i<argc,"Expected an encoding name after --encoding.");
            expect_argument((BenEncodingTableT*)0!=BenEncodingTableNamed(argv[i]),
                            "There is no such '%s' encoding.\n",argv[i]);
            encodingName=argv[i];
        }else{
            expect_argument(FALSE,"Unknown argument '%s'.\n",argv[i]);
        }
    }

    if(escapeChar==numberChar){
        fprintf(stderr,"%s error: the escape character and the number character "
               "must not be the same.\n",pname);
        return(1);
    }

    if(escapeChar!=default_escape_char()){
        approx=copy_approximation_table(approx);
        update_approx(approx,escapeChar);
    }

    eightBitEncoding=BenEncodingTableNamed(encodingName);
    if(doEightToSeven){
        eighttoseven(stdin,stdout);
    }else{
        seventoeight(stdin,stdout);
    }
    return(0);
}/*main;*/

/*** sevenbit.c                       --                     --          ***/
