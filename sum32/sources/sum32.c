#include <stdio.h>

    enum {
        bufSize=1024
    };
    
    static void sum32(const char* fname)
    {
            FILE*               in;
            unsigned long int   buffer[1024];
            int                 i;
            size_t              size;
            unsigned long int   sum;
        
        sum=0;
        in=fopen(fname,"rb");
        if(in!=NULL){
            while(!feof(in)){
                for(i=0;i<bufSize;i++){
                    buffer[i]=0;
                }
                size=fread((void*)(buffer),1,sizeof(buffer),in);
                if(size%sizeof(unsigned long int)!=0){
                    size/=sizeof(unsigned long int);
                    size++;
                }else{
                    size/=sizeof(unsigned long int);
                }
                for(i=0;i<(int)size;i++){
                    sum^=buffer[i];
                }
            }
            fclose(in);
        }
        printf("%10ld %s\n",sum,fname);
    }/*sum32*/
    

int main(int argc,const char** argv)
{
        int                i;
        
    for(i=1;i<argc;i++){
        sum32(argv[i]);
    }
    return(0);
}/*main*/


/*** sum32.c                          -- 2003-12-02 13:20:07 -- pascal   ***/
