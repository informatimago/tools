/******************************************************************************
FILE:               parity.c
LANGUAGE:           ANSI-C
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
    This filter computes or checks parity on a ASCII stream.
USAGE
    See the usage() function.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993-12-21 <PJB> Creation (this header).
    2000-10-12 <PJB> At least, a first implementation.
BUGS
    - The heuristic  to determine 8-bit  is extremely weak.  We should
      look for  human language words  with accents in  different 8-bit
      encodings  to decide.  It's  a little  better  for random  8-bit
      binary data...
LEGAL
    GPL
    Copyright Pascal J. Bourguignon 1983 - 2000

    This file is part of the PJB Ascii Utilities Suite.

    This  program is  free software;  you can  redistribute  it and/or
    modify it  under the  terms of the  GNU General Public  License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program  is distributed in the  hope that it  will be useful,
    but  WITHOUT ANY WARRANTY;  without even  the implied  warranty of
    MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a  copy of the GNU General Public License
    along with  this program; see the  file COPYING; if  not, write to
    the Free  Software Foundation, Inc.,  59 Temple Place,  Suite 330,
    Boston, MA 02111-1307 USA
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum { Action_Check, Action_Find, Action_Flag, Action_Set } ActionT;
typedef enum { Parity_None, Parity_Odd, Parity_Even, Parity_Mark  } ParityT;


typedef enum { 
    Status_NoError=0,
    Status_InvalidArgument=1,
    Status_ParityError=2,
    Status_Undetermined=3,
    Status_ParityNone=4,
    Status_ParityOdd=5,
    Status_ParityEven=6,
    Status_ParityMark=7,
    Status_EightBitStream=8,
    Status_IncompleteImplementation=17, /* Internal error */
    Status_InternalError=18             /* Internal error */
} StatusT;


    static void usage(const char* pname)
    {
        int len=strlen(pname);
        fprintf(stderr,
    "%s usage:\n"
    "    %s [-q|--quiet]\n"
    "    %*s [-c|--check|-f|--find|-t<err>|--flag=<err>|-s|--set] \n"
    "    %*s [-e|--even|-o|--odd|-m|--mark|-n|--none] \n"
    "    %*s < input > output\n"
    "  check:   check and return status code.\n"
    "  flag:    check and replace in output erroneous characters with <err>\n"
    "  find:    tells which parity was used on input.\n"
    "  set:     writes out bytes with the parity recomputed.\n"
    "  default: output stream = input stream with specified parity (--set).\n"
    "  default parity is none (--none).\n"
    "\n",
                pname,pname,len," ",len," ",len," ");
    }/*usage*/


    static const char* basename(const char* pname)
    {
        const char* bname=rindex(pname,'/');
        if(bname==0){
            return(pname);
        }else{
            return(bname+1);
        }
    }/*basename*/


    static struct {
        unsigned char even;
        unsigned char odd;
    }   parities[128];



    static void parity_compute_parities(unsigned int/*char*/ c,
                                        unsigned char* even,unsigned char* odd)
    {
        unsigned char currentBit;
        unsigned char parityBit=0;
        for(currentBit=1;currentBit<128;currentBit<<=1){
            if((c&currentBit)!=0){
                parityBit^=1;
            }
        }
        if(parityBit){
            (*even)=(c&0x7f)|0x80;
            (*odd)=(c&0x7f);
        }else{
            (*odd)=(c&0x7f)|0x80;
            (*even)=(c&0x7f);
        }
    }/*parity_compute_parities*/


    static void parity_initialize(void)
    {
        unsigned int c;
        for(c=0;c<128;c++){
            parity_compute_parities(c,&(parities[c].even),&(parities[c].odd));
        }
    }/*parity_initialize*/


    static short parity_byte_check(unsigned int/*char*/ c,ParityT parity)
    {
        switch(parity){
        case Parity_None:
            return((c&0x80)==0);
        case Parity_Even:
            return(c==parities[c&0x7f].even);
        case Parity_Odd:
            return(c==parities[c&0x7f].odd);
        case Parity_Mark:
            return((c&0x80)!=0);
        default:
            fprintf(stderr,"Unknown parity %d, aborting.\n",parity);
            exit(Status_InternalError);
        }
    }/*parity_byte_check*/


   static  unsigned char parity_byte_set(unsigned int/*char*/ c,ParityT parity)
    {
        switch(parity){
        case Parity_None:
            return(c&0x7f);
        case Parity_Even:
            return(parities[c&0x7f].even);
        case Parity_Odd:
            return(parities[c&0x7f].odd);
        case Parity_Mark:
            return((c&0x7f)|0x80);
        default:
            fprintf(stderr,"Unknown parity %d, aborting.\n",parity);
            exit(Status_InternalError);
        }
    }/*parity_byte_set*/



    static int parity_byte_find(unsigned int/*char*/ c)
        /*
          00 : none or even
          01 : none or odd
          10 : mark or even 
          11 : mark or odd
        */
    {
        int result=0;
        if((c&0x80)!=0){
            result|=2;
        }
        if(c==parities[c&0x7f].odd){
            result|=1;
        }
        return(result);
    }/*parity_byte_find*/



    static StatusT parity_check(FILE* input,FILE* output,ParityT parity,
                                int quiet)
    {
        int c;
        int count=0;
        while(EOF!=(c=fgetc(input))){
            if(!parity_byte_check((unsigned)c,parity)){
                if(!quiet){
                    fprintf(output,"Byte #%d should be %02x instead of %02x\n",
                            count,parity_byte_set((unsigned)c,parity),c);
                }
                return(Status_ParityError);
            }
            count++;
        }
        return(Status_NoError);
    }/*parity_check*/

#if 0
    static void swap_int(int* i,int* j)
    {
        int t=(*i);
        (*i)=(*j);
        (*j)=t;
    }/*swap_int*/
#endif

    static StatusT parity_find(FILE* input,FILE* output,ParityT parity,
                               int quiet)
    {
        int my_parities[4]={0};
        int c;
        int total=0;
        int highPercentage;
        int lowPercentage;
        int errors;
        StatusT status;

        while(EOF!=(c=fgetc(input))){
            my_parities[parity_byte_find((unsigned)c)]++;
        }

        /* Yeah ! It's a BUBBLE sort here ! But it should not lose more
           than one comparizon... */
        /*
        int order[4]={0,1,2,3};
        int i,j;
        for(i=0;i<3;i++){
            for(j=i+1;j<4;j++){
                if(my_parities[order[i]]<my_parities[order[j]]){
                    int t=order[i];
                    order[i]=order[j];
                    order[j]=t;
                }
            }
        }   
        */

        /*
          00 : none or even
          01 : none or odd
          10 : mark or even 
          11 : mark or odd
        */

        /*
          100   0   0   0   -> good 7-bit stream with parity
           50  50   0   0   -> good 7-bit stream with parity
           80   7   7   6   -> 7-bit stream with parity and errors
           40  40  20   0   -> 7-bit stream with parity and errors
           40  40  10  10   -> 7-bit stream with parity and errors

           40  39  11  10  \
           39  39  11  11   \
           33  33  33   0    > ?
           31  31  29   9   /
           31  30  30   9  /

           30  30  30  10   \
           30  23  23  23    > 8-bit binary stream (without parity)
           25  25  25  25   /
        */

        total=my_parities[0]+my_parities[1]+my_parities[2]+my_parities[3];
        highPercentage=total*85/100;
        lowPercentage=total*30/100;


        if(my_parities[0]+my_parities[1]>=highPercentage){
            status=Status_ParityNone;
            errors=my_parities[2]+my_parities[3];
        }else if(my_parities[0]+my_parities[2]>=highPercentage){
            status=Status_ParityEven;
            errors=my_parities[1]+my_parities[3];
        }else if(my_parities[2]+my_parities[3]>=highPercentage){
            status=Status_ParityMark;
            errors=my_parities[0]+my_parities[1];
        }else if(my_parities[1]+my_parities[3]>=highPercentage){
            status=Status_ParityOdd;
            errors=my_parities[0]+my_parities[2];
        }else if(my_parities[0]+my_parities[3]>=highPercentage){
            /* none or even + mark or odd */
            status=Status_ParityOdd;
            errors=my_parities[1]+my_parities[2];
        }else if(my_parities[1]+my_parities[2]>=highPercentage){
            /*  none or odd + mark or even */
            status=Status_ParityOdd;
            errors=my_parities[0]+my_parities[3];
        }else if((my_parities[0]<lowPercentage)
                 &&(my_parities[1]<lowPercentage)
                 &&(my_parities[2]<lowPercentage)
                 &&(my_parities[3]<lowPercentage)){
            status=Status_EightBitStream;
            errors=0;
        }else{
            status=Status_Undetermined;
            errors=0;
        }

         if(!quiet){
            fprintf(output,"%10d bytes are of parity none or even\n",
                    my_parities[0]);
            fprintf(output,"%10d bytes are of parity none or odd\n",
                    my_parities[1]);
            fprintf(output,"%10d bytes are of parity mark or even\n",
                    my_parities[2]);
            fprintf(output,"%10d bytes are of parity mark or odd\n",
                    my_parities[3]);
            fprintf(output,"\n");

            switch(status){
            case Status_ParityNone:
                fprintf(output,
                        "Input is 7-bit with parity None (%3d %% errors)\n",
                        100*errors/total);
                break;
            case Status_ParityEven:
                fprintf(output,
                        "Input is 7-bit with parity Even (%3d %% errors)\n",
                        100*errors/total);
                break;
            case Status_ParityOdd:
                fprintf(output,
                        "Input is 7-bit with parity Odd (%3d %% errors)\n",
                        100*errors/total);
                break;
            case Status_ParityMark:
                fprintf(output,
                        "Input is 7-bit with parity Mark (%3d %% errors)\n",
                        100*errors/total);
                break;
            case Status_EightBitStream:
                fprintf(output,
                        "Input is 8-bit without parity   (%3d %% errors)\n",
                        100*errors/total);
                break;
            case Status_Undetermined:
                fprintf(output,
                        "Input is undertermined parity  \n");
                break;
            default:
                fprintf(stderr,"Unexpected status %d, aborting.\n",status);
                exit(Status_InternalError);
            }
            return(Status_NoError);
         }else{
             return(status);
         }
    }/*parity_find*/


    static StatusT parity_flag(FILE* input,FILE* output,ParityT parity,
                               int quiet,const char* err)
    {
        int c;
        int total=0;
        int errors=0;
        int errlen=strlen(err);

        while(EOF!=(c=fgetc(input))){
            if(c==parity_byte_find((unsigned)c)){
                fputc(c,output);
            }else{
                fputs(err,output);
                errors++;
            }
            total++;
        }
        if(!quiet){
            fprintf(stderr,"%10d bytes read, %10d bytes written, %10d errors\n",
                    total,total+(errlen-1)*errors,errors);
            return(Status_NoError);
        }else if(errors>0){
            return(Status_ParityError);
        }else{
            return(Status_NoError);
        }
    }/*parity_flag*/


    static StatusT parity_set(FILE* input,FILE* output,ParityT parity)
    {
        int c;

        while(EOF!=(c=fgetc(input))){
            fputc(parity_byte_set((unsigned)c,parity),output);
       }
        return(Status_NoError);
    }/*parity_set*/

    
int main(int argc,char* argv[])
{
    const char* pname=basename(argv[0]);
    const char* err="";
    int         i;
    StatusT     status;
    ActionT     action=Action_Set;
    ParityT     parity=Parity_None;
    short       quiet=0;

    for(i=1;i<argc;i++){
        if((strcmp(argv[i],"-q")==0)||(strcmp(argv[i],"--quiet")==0)){
            quiet=1;
        }else if((strcmp(argv[i],"-c")==0)||(strcmp(argv[i],"--check")==0)){
            action=Action_Check;
        }else if((strcmp(argv[i],"-f")==0)||(strcmp(argv[i],"--find")==0)){
            action=Action_Find;
        }else if(strncmp(argv[i],"-t",2)==0){
            action=Action_Flag;
            err=argv[i]+2;
        }else if(strncmp(argv[i],"--flag=",7)==0){
            action=Action_Flag;
            err=argv[i]+7;
        }else if((strcmp(argv[i],"-s")==0)||(strcmp(argv[i],"--set")==0)){
            action=Action_Set;
        }else  if((strcmp(argv[i],"-n")==0)||(strcmp(argv[i],"--none")==0)){
            parity=Parity_None;
        }else  if((strcmp(argv[i],"-e")==0)||(strcmp(argv[i],"--even")==0)){
            parity=Parity_Even;
        }else  if((strcmp(argv[i],"-o")==0)||(strcmp(argv[i],"--odd")==0)){
            parity=Parity_Odd;
        }else  if((strcmp(argv[i],"-m")==0)||(strcmp(argv[i],"--mark")==0)){
            parity=Parity_Mark;
        }else{
            fprintf(stderr,"%s: invalid argument (%s).\n",pname,argv[i]);
            usage(pname);
            return(Status_InvalidArgument);
        }
    }

    parity_initialize();
    switch(action){
    case Action_Check:
        status=parity_check(stdin,stdout,parity,quiet);
        break;
    case Action_Find:
        status=parity_find(stdin,stdout,parity,quiet);
        break;
    case Action_Flag:
        status=parity_flag(stdin,stdout,parity,quiet,err);
        break;
    case Action_Set:
        status=parity_set(stdin,stdout,parity);
        break;
    default:
        fprintf(stderr,"%s: incomplete implementation.\n",pname);
        status=Status_IncompleteImplementation;
        break;
    }

    return(status);
}/*main*/



/*** parity.c                         -- 2003-12-01 03:33:04 -- pascal   ***/
