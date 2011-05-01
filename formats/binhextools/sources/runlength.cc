/******************************************************************************
FILE:               runlength.cc
LANGUAGE:           C++
SYSTEM:             ANSI
USER-INTERFACE:     ANSI
DESCRIPTION
USAGE
    unrunlength < input.runl  > output
    runlength > output.runl < input
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1993/04/28 <PJB> Creation.
LEGAL
    Copyright Pascal J. Bourguignon 1993 - 2011
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
#include <RunLengthStream.hh>
#include <StdIOStream.hh>
#include <BcImplementation.h>


int main(int argc,char** argv)
{
        RunLengthStream runln;
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
    if(strcmp(pname,"runlength")==0){
    
        runln.rewrite(output.rewrite(stdout));
        input.reset(stdin);
        input.copyTo(&runln,MAX_CARD32);
        
    }else if(strcmp(pname,"unrunlength")==0){

        output.rewrite(stdout);
        runln.reset(input.reset(stdin));
        runln.copyTo(&output,MAX_CARD32);
        
    }else{
        fprintf(stderr, "Usage: unrunlength < input.runl  > output\n"
                        "   or:   runlength > output.runl < input\n");
        return(1);
    }
    
    runln.close();
    output.close();
    input.close();
    
    return(0);
}

/*** runlength.cc                     -- 2003-12-01 05:36:22 -- pascal   ***/
