#include "syscall.h"
#include "copyright.h"

#define maxlen 32
int 
main(){
    int len;
    char *filename[maxlen+1];
    PrintfString("Input file's name:");
    ReadString(filename,maxlen);
    if(Create(filename)==-1){
        PrintString("Error when create file");
    }else{
        PrintString("Create file succesfull");
    }
    return 0;
}
