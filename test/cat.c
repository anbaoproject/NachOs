#include "syscall.h"

#define MaxFileName 32
#define MaxBuffer 256
//Chuong trinh yeu cau nhap vao ten file roi hien thi noi dung cua file do
int main()
{
	int FileID, type, size, pos = 0;
	char fileName[MaxFileName], buffer[MaxBuffer]; 

	PrintString("Nhap vao ten file: ");
	ReadString(fileName,MaxFileName); //Nhap ten chuoi tu console

	FileID	= Open(fileName,1); //Mo file chi doc
	if (FileID == -1)
	{
		PrintString("Loi mo file. Kiem tra lai ten file");
		return 0;
	}

	while(1)
	{
		size = Read(buffer,MaxBuffer,FileID);
		pos += size;
		if(size == -1)
		{
			PrintString("Doc file loi \n");
			Close(FileID); 
			return 1;
		}
		if(size == -2)
			break; // Ket thuc file
		Seek(pos, FileID);
	}

	Close(FileID);
	return 0;
}
