#include "syscall.h"
#include "copyright.h"

#define maxlen 32
int 
main(){
    int len;
    char filename[maxlen+1];
    if(Create("text_1.txt")==-1){
        PrintString("Error when create file");
    }else{
        PrintString("Create file succesfull");
    }
    return 0;
}
