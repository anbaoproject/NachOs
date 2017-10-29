#include "syscall.h"

int main()
{
	int b;
	PrintString("Nhap mot so: ");
	b = ReadInt();
	PrintString("So vua nhap: \n");
	PrintInt(b);
	return 0;
}
