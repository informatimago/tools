extern "C"{
#include <stdio.h>
#include <values.h>
}
#define BIT_PER_BYTE    (CHARBITS)

    unsigned long   _buffer;
    unsigned long   _bitMask;
    unsigned long   _currentBit;

int main(){
    printf("bit per byte                        = %lu\n",BIT_PER_BYTE);
    printf("sizeof(_buffer)                     = %lu\n",sizeof(_buffer));
    printf("sizeof(_currentBit)                 = %lu\n",sizeof(_currentBit));
    printf("(BIT_PER_BYTE*sizeof(_buffer)-1)    = %lu\n",(BIT_PER_BYTE*sizeof(_buffer)-1));
    printf("1<<(BIT_PER_BYTE*sizeof(_buffer)-1) = %llu\n",1ULL<<(BIT_PER_BYTE*sizeof(_buffer)-1));
    printf("1<<(BIT_PER_BYTE*sizeof(_buffer)-1) = %llx\n",1ULL<<(BIT_PER_BYTE*sizeof(_buffer)-1));
    return(0);
}

