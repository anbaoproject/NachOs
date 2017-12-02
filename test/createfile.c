#include "syscall.h"
#include "copyright.h"

#define maxlen 32
int 
main(){
    OpenFileId id;
    char *filename[maxlen+1];
    PrintString("Input file's name:");
    ReadString(filename,maxlen);
    if(CreateFile(filename)==-1){
        PrintString("Error when create file");
    }else{
        PrintString("Create file succesfull");
 	id=Open(filename,1);
	PrintInt(id);
	Close(id);
    }
    return 0;
}
