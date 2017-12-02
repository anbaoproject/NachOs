#include "syscall.h"

#define MaxFileName 32
#define MaxBuffer 256

//Chuong trinh cho phep nguoi dung nhap vao ten file nguon va ten file dich sau do doc noi dung file nguon dao nguoc noi dung roi ghi vao file dich

int main()
{
	char souFile[MaxFileName], disFile[MaxFileName], buffer;
	int fileID1, fileID2, fend;

	PrintString("Nhap vao ten file nguon: ");
	ReadString(souFile,MaxFileName);
	
	fileID1 = Open(souFile,1); // Mo file nguon chi doc
	if (fileID1 == -1)
	{
		PrintString("Loi mo file. Kiem tra lai ten file");
		return 0;
	}

	PrintString("Nhap vao ten file dich: ");
	ReadString(disFile,MaxFileName);
	fileID2 = Open(disFile,0); // Mo file dich de ghi

	if (fileID2 == -1)
	{
		Create(disFile);
		fileID2 = Open(disFile,0); // Mo file dich de ghi
	}

	fend = Seek(-1,fileID1); //Lay vi tri cuoi tap tin
	fend--;
	while(fend != 0)
	{
		fend--;
		Seek(fend,fileID1);
		Read((char*)&buffer,1,fileID1);  // Doc du lieu
		Write((char*)&buffer,1,fileID2); // Ghi du lieu
	}
	Close(fileID1);
	Close(fileID2);
	return 0;
}
