/******************************************************************************
FILE:               seven.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
USAGE
    unseven < input.hqx7  > output.hqx8
    seven > output.hqx7 < input.hqx8
AUTHOR
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/04/28 <PJB> Creation.
LEGAL
    Copyright Pascal J. Bourguignon 1992 - 1993
    This program may not be included in any commercial product without the 
    author written permission. It may be used freely for any non-commercial 
    purpose, provided that this header is always included.
******************************************************************************/
extern "C"{
#include <stdio.h>
#include <string.h>
}
#include <BcInterface.h>
#include <BcTypes.h>
#include <SevenStream.hh>
#include <BinHex4Stream.hh>
#include <StdIOStream.hh>
#include <BcImplementation.h>


int main(int argc,char** argv)
{
        SevenStream     seven;
        BinHex4Stream   binhx;
        StdIOStream     input;
        StdIOStream     output;
        char*           pname;
    
    pname=argv[0]+strlen(argv[0]);
    while((pname>argv[0])&&(pname[0]!='/')){
        pname--;
    }
    if(pname[0]=='/'){
        pname++;
    }
    if(strcmp(pname,"seven")==0){
    
        seven.rewrite(binhx.rewrite(output.rewrite(stdout)));
        input.reset(stdin);
        input.copyTo(&seven,MAX_CARD32);
        
    }else if(strcmp(pname,"unseven")==0){

        output.rewrite(stdout);
        seven.reset(binhx.reset(input.reset(stdin)));
        seven.copyTo(&output,MAX_CARD32);
        
    }else{
        fprintf(stderr, "Usage: unseven < input.hqx7  > output.hqx8\n"
                        "   or:   seven > output.hqx7 < input.hqx8\n");
        return(1);
    }
    
    seven.close();
    binhx.close();
    output.close();
    input.close();
    
    return(0);
}

/*** seven.cc                         -- 2003-11-20 03:15:04 -- pascal   ***/
