#include "syscall.h"

int main()
{
	char buffer[255];
	PrintString("Nhap vao mot chuoi : ");
	ReadString(buffer,254);
	PrintString("Chuoi da nhap la : ");
	PrintString(buffer);
	return 0;
}
