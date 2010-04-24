/*
  This program create a file and fills it with 0x55's until the disk is full.
  It may be used to physically erase the date from a deleted file.
  (Note that some sophisticated methods may recover overwritten data, so
   a more sophisticated scheme should be used to insure no data can be 
   recovered ; overwritting several times with 0x00, 0xFF, and random data
   could do the trick).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


char buffer[65536];

int main(int argc,char** argv)
{
    int filler; 
    unsigned int i;
    if(argc!=2){
        printf("Usage: %s file\n",argv[0]);
        exit(1);
    }
    for(i=0;i<sizeof(buffer);i++){
        buffer[i]=(char)0x55;
    }
    filler=open(argv[1],O_CREAT|O_WRONLY,0666);
    if(filler<0){
        perror("I cannot open the specified file"); 
        exit(2);
    }
    while(write(filler,&buffer,sizeof(buffer))==sizeof(buffer)){
        printf(".");
        fflush(stdout);
    }
    close(filler);
    exit(0);
}


/*** filldisk.c                       -- 2003-12-02 11:25:31 -- pascal   ***/
