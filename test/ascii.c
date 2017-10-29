#include "syscall.h"

int main(){
    int i=32;
    for(;i<=127;i++){
        PrintInt(i);
        PrintChar(':');
        PrintChar(i);
        PrintChar('\n');
    }
    return 0;
}