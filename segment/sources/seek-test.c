#include <stdlib.h>
#include <stdio.h>

    int main(void)
    {
        FILE* file;
        system("randchar -w 80 -l 128>test.data");
        file=fopen("test.data","r");
        if(file!=0){
            long pos;
            int res=fseek(file,-40,SEEK_SET);
            printf("fseek(file,-40,SEEK_SET)==%d\n",res);
            pos=ftell(file);
            printf("ftell(file)==%ld\n",pos);
        }
    }/*main*/
