#include "syscall.h"
#include "copyright.h"
<<<<<<< HEAD
#define maxlen 256
int
main()
{
	Create("CreateMe.txt");
	return 0;
=======

#define maxlen 32
int 
main(){
    OpenFileId id;
    int res;
    char *buffer[100];
    id=Open("minhan.txt",1);
    res=Read(buffer,100,id);
    PrintInt(res);
    PrintString(buffer);
    res=Read(buffer,100,id);
    PrintInt(res);
    PrintString(buffer);
    CloseFile(id);
    // OpenFileId id;
    // int res;
    // id=Open("minhan",1);
    // PrintInt(0);
    // PrintInt(CloseFile(8));
    
    /*char *filename[maxlen+1];
    PrintString("Input file's name:");
    ReadString(filename,maxlen);
    if(CreateFile(filename)==-1){
        PrintString("Error when create file");
    }else{
        PrintString("Create file succesfull");
 	id=Open(filename,1);
	PrintInt(id);
	res=CloseFile(id);
        PrintInt(res);
    }*/
    return 0;
>>>>>>> bb501968911a0609e91636507d618e1ea7ce7908
}
