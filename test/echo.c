#include "syscall.h"

#define MaxLength 256

int main()
{
	char buffer[MaxLength]; //Khoi tao chuoi toi da 256 ki tu
	ReadString(buffer,255); //Lay chuoi tu console
	PrintString(buffer); //Ghi chuoi ra console
	return 0;
}
