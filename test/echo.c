#include "syscall.h"

#define MaxLength 256

int main()
{
	char mess[255];
	unsigned int i ;
	OpenFileId in = Open("stdin",2);
	OpenFileId out = Open("stdout",3);
	if (in == -1 || out == -1)
	{
		PrintString("Can not open console\n");
		return 0;
	}
	Read(mess,255,in);
	Write(mess, 255, out);
	CloseFile(in);
	CloseFile(out);
	return 0;
}
