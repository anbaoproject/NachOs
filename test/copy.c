#include "syscall.h"

#define MaxFileName 32
#define MaxBuffer 1
//Chuong trinh cho phep nguoi dung nhap vao ten file nguon va ten file dich sau do copy file nguon vao file dich

int main ()
{
	
	char source[MaxFileName], destination[MaxFileName], buffer[MaxBuffer];
	int FileID1, FileID2, size = 0, pos = 0, i = 0;

	PrintString("Nhap vao ten file nguon: ");
	ReadString(source,MaxFileName);
	FileID1 = Open(source,1); // Mo file nguon chi doc

	
	if (FileID1 == -1)
	{
		PrintString("Loi mo file. Kiem tra lai ten file");
		return 0;
	}

	PrintString("Nhap vao ten file dich: ");
	ReadString(destination,MaxFileName);
	FileID2 = Open(destination,0); // Mo file dich de ghi
	
	if (FileID2 == -1)
	{
		Create(destination);
		FileID2 = Open(destination,0); // Mo file dich de ghi
	}
	
	while(1)
	{
		// Di chuyen con tro toi vi tri tiep theo can doc
		Seek(pos, FileID1);
		size = Read(buffer,MaxBuffer,FileID1);

		//Het file
		if(size == -2)
			break;

		if(size == -1)
		{
			PrintString("Loi khi doc file\n");
			CloseFile(FileID1);
			CloseFile(FileID2);
			return 1;
		}
		
		// Di chuyen con tro toi vi tri tiep theo can ghi
		Seek(pos, FileID2);
		Write(buffer,MaxBuffer,FileID2); 

		// Vi tri con tro tiep theo
		pos += size;
		i++;
	}
	PrintString("Do dai: = "); PrintInt(i);
	Close(FileID1);
	Close(FileID2);
	return 0;
}
