#include "syscall.h"
#include "copyright.h"

#define maxlen 32
int 
main(){
    int len;
    char *filename[maxlen+1];
    PrintString("Input file's name:");
    ReadString(filename,maxlen);
    if(CreateFile(filename)==-1){
        PrintString("Error when create file");
    }else{
        PrintString("Create file succesfull");
	    PrintInt(Open(filename,1));
    }
    return 0;
}
