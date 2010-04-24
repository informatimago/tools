#include <stdio.h>
#include <ctype.h>

int main(int argc,char** argv)
{
    int i;
    for(i=1;i<argc;i++){
        if(isalpha(argv[i][0])){
            argv[i][0]=toupper(argv[i][0]);
        }
        printf("%s\n",argv[i]);
    }
    return(0);
}
