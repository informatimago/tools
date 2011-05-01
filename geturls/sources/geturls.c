/******************************************************************************
FILE:               geturls.c
LANGUAGE:           ANSI-C
SYSTEM:             None
USER-INTERFACE:     None
DESCRIPTION
    This program is a filter that extract URLS from its standard input. It
    writes extracted urls to its standard error output, and a HTML page with
    them to its standard output.
    
    URLs are identified by a set of prefix ("http://","ftp://","mailto:",...),
    and the set of valid character they may contain. The current definition
    of these set is embeded in the source into the starts variable and the
    isUrlChar function. Please change them to match your need and actual
    URL definitions.
    
    See also printHeader, printUrls, and printTrailer to change the outputs.
AUTHORS
    <PJB> Pascal J. Bourguignon
MODIFICATIONS
    1998-01-22 <PJB> Creation.
LEGAL
    GPL
    
    Copyright Pascal Bourguignon 1998 - 2011
    
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
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>


const char* starts[]={
    "ftp://",
    "http://",
    "https://",
    "mailto:",
    "news:",
    "telnet:",
    0};

static int isUrlChar(int/*char*/ token)
{
    return((' '<token)&&(token<='~')
           &&(token!=',')&&(token!='<')&&(token!='>'));
}/*isUrlChar*/
        

static int  numOfEntries;
static int  maxLength;
static int* lengths;

static void deallocStringArray(char** strings)
{
    int i;
    if(strings!=0){
        i=0;
        while(strings[i]!=0){
            free(strings[i]);
            i++;
        }
        free(strings);
    }
}/*deallocStringArray*/
    
    
static int maximumOfIntArray(const int integers[],int size)
{
    int maximum=0;
    int i;
    for(i=0;i<size;i++){
        if(maximum<integers[i]){
            maximum=integers[i];
        }
    }
    return(maximum);
}/*maximumOfIntArray*/
    
    
static int  sizeOfStringArray(const char* strings[])
{
    int n;
    n=0;
    while(strings[n]!=0){
        n++;
    }
    return(n);
}/*sizeOfStringArray*/
    
    
static int* computeLengths(const char* strings[])
{
    int* lens;
    int i;
    int n=sizeOfStringArray(strings);
    lens=(int*)malloc(sizeof(int)*(size_t)n);
    for(i=0;i<n;i++){
        lens[i]=(int)strlen(strings[i]);
    }
    return(lens);
}/*computeLengths*/
    
    
static void printHeader(FILE* out)
{
    fprintf(out,
            "<html>\n"
            "<body>\n"
            "<bookmarkInfo scrollbarPercentage=0 annotationHeight=168>\n"
            "<ul>\n"
            "<li><a>URLs</a>\n"
            "<ul>\n"
            );
}/*printHeader*/
    
    
static void printUrls(FILE* out,char** urls)
{
    if(urls!=0){
        int i=0;
        while(urls[i]!=0){
            fprintf(stderr,"%s\n",urls[i]);
            fprintf(out,
                    "<li><a href=\"%s\" checkFrequency=never>%s</a>\n",
                    urls[i],urls[i]);
            i++;
        }
    }
}/*printUrls*/
    

static void printTrailer(FILE* out)
{
    fprintf(out,
            "</ul>\n"
            "</ul>\n"
            "</body>\n"
            "</html>\n"
            );
}/*printTrailer*/
    
    
static void addUrl(char*** urls,int* allocatedUrls,int* numOfUrls,
                   const char* url,int lengthOfUrl)
{
    if((*urls)==0){
        (*allocatedUrls)=8;
        (*urls)=(char**)malloc(sizeof(char*)*(size_t)(*allocatedUrls));
    }
    if((*allocatedUrls)<=(*numOfUrls)+1){
        int i;
        char** oldUrls=(*urls);
        (*allocatedUrls)*=2;
        (*urls)=(char**)malloc(sizeof(char*)*(size_t)(*allocatedUrls));
        for(i=0;i<(*numOfUrls);i++){
            (*urls)[i]=oldUrls[i];
        }
        free(oldUrls);
    }
    (*urls)[(*numOfUrls)]=(char*)malloc(sizeof(char)*(size_t)(lengthOfUrl+1));
    strncpy((*urls)[(*numOfUrls)],url,(unsigned)lengthOfUrl);
    (*urls)[(*numOfUrls)][lengthOfUrl]='\0';
    (*numOfUrls)++;
    (*urls)[(*numOfUrls)]=0;
}/*addUrl*/

    
static char** searchUrlsInBuffer(const char* buffer,
                                 int size,int* scannedSize)
{
    char** urls=0;
    int    allocatedUrls=0;
    int    numOfUrls=0;
    int    i=0;
    int    max=size-maxLength;
        
    while(i<max){
        int s=0;
        while((s<numOfEntries)
              &&(strncmp(buffer+i,starts[s],(unsigned)(lengths[s]))!=0)){
            s++;
        }
        if(s<numOfEntries){
            int l=i;
            while((l<size)&&(isUrlChar(buffer[l]))){
                l++;
            }
            if(l<size){
                addUrl(&urls,&allocatedUrls,&numOfUrls,buffer+i,l-i);
                i=l;
            }else{
                (*scannedSize)=i;
                return(urls);
            }
        }else{
            i++;
        }
    }
    (*scannedSize)=i;
    return(urls);
}/*searchUrlsInBuffer*/
    
    
static void filterUrls(FILE* in,FILE* out)
{
    const size_t  BufferSize=10240;
    char*         inputBuffer=(char*)malloc(sizeof(char)*(BufferSize+1));
    int           readSize;
    int           inputBufferSize=0;
    int           scannedSize;
    char**        urls;
        
    lengths=computeLengths(starts);
    numOfEntries=sizeOfStringArray(starts);
    maxLength=maximumOfIntArray(lengths,numOfEntries);
        
    printHeader(out);
    while(!feof(in)){
        readSize=(int)fread(inputBuffer+inputBufferSize,sizeof(char),(size_t)((int)BufferSize-inputBufferSize),in);
        if(readSize>0){
            inputBufferSize+=(int)readSize;
            urls=searchUrlsInBuffer(inputBuffer,inputBufferSize,
                                    &scannedSize);
            printUrls(out,urls);
            deallocStringArray(urls);
            if((maxLength<=inputBufferSize)
               &&(inputBufferSize-maxLength<scannedSize)){
                scannedSize=inputBufferSize-maxLength;
            }
            if(scannedSize<inputBufferSize){
                memcpy(inputBuffer,inputBuffer+scannedSize,
                       (unsigned)(inputBufferSize-scannedSize));
                inputBufferSize=inputBufferSize-scannedSize;
            }else{
                inputBufferSize=0;
            }
        }   
    }
    if(0<inputBufferSize){
        inputBuffer[inputBufferSize]='\0';
        inputBufferSize++;
        urls=searchUrlsInBuffer(inputBuffer,inputBufferSize,
                                &scannedSize);
        printUrls(out,urls);
        deallocStringArray(urls);
    }
    printTrailer(out);
    free(inputBuffer);
    free(lengths);
}/*filterUrls*/
    
int main(void)
{
    filterUrls(stdin,stdout);
    exit(0);
    return(0);
}/*main*/

/**** THE END ****/
