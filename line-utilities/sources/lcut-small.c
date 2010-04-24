/*
*/
#include <stdio.h>
#include <string.h>
#define defaultwidth (78)
#define bufsize (1024*1024)
int main(int argc,char** argv)
{
    char*   pname=argv[0];
    int     width;
    int     readsize;
    int     len;
    int     i;
    char    c;
    char*   buf;

    switch(argc){
    case 1:
        width=defaultwidth;
        break;
    case 2:
        if((1!=sscanf(argv[1],"%d",&width))||(width<=0)){
            fprintf(stderr,"%s: argument '%s' is an invalid width.\n",
                    pname,argv[1]);
            return(2);
        }
        break;
    default:
        fprintf(stderr,"%s usage:\n\t%s [width]\n",pname,pname);
        return(1);
    }
    readsize=0x4000*width;
    buf=malloc(readsize+1);
    if(buf==NULL){
        fprintf(stderr,"%s: cannot allocate buffer.\n",pname);
        return(3);
    }
    while(NULL!=fgets(buf,readsize,stdin)){
        len=strlen(buf);
        i=0;
        while(len>width){
            c=buf[i+width];
            buf[i+width]='\0';
            puts(buf+i);
            buf[i+width]=c;
            i+=width;
            len-=width;
        }
        fputs(buf+i,stdout);
    }
    return(0);
}
/*end lcut*/

