// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32
#define MaxIntLength 9
#define MaxStringLength 256
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

//Copy từ User space -> System Space

char *UserToSystem(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

int SystemToUser(int virtAddress, int lengthBuffer, char *buffer)
{
    if (lengthBuffer < 0)
    {
        return -1;
    }
    if (lengthBuffer == 0)
    {
        return lengthBuffer;
    }
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddress + i, 1, oneChar);
        i++;
    } while (i < lengthBuffer && oneChar != 0);
    return i;
}

char *IntToChar(int &sizeBuffer, int number)
{
    char *numberBuffer = new char[MaxIntLength];
    char *resultBuffer = new char[MaxIntLength];
    int nSize;
    int i = 0;
    int check = 0;
    int count = 0;
    number < 0 ? (check = 1, number *= -1, resultBuffer[i] = '-', count = 1) : 0;
    number == 0 ? (resultBuffer[i] = '0', count = 1) : 0;
    while (number > 0)
    {
        numberBuffer[i] = (number % 10) + '0';
        i++;
        number /= 10;
    }
    nSize = i;
    for (int j = nSize - 1; j >= 0; j--)
    {
        resultBuffer[count] = numberBuffer[j];
        count++;
    }
    resultBuffer[count] = '\0';
    sizeBuffer = count + 1;

    delete[] numberBuffer;
    return resultBuffer;
}

void _Sub()
{
    int op1 = machine->ReadRegister(4);
    int op2 = machine->ReadRegister(5);
    int res = op1 - op2;
    machine->WriteRegister(2, res);
}

void _CreateFile()
{
    int virtAddress;
    char *filename;
    filename = new char[MaxFileLength];
    virtAddress = machine->ReadRegister(4);
    filename = UserToSystem(virtAddress, MaxFileLength + 1);
    if (filename == NULL)
    {
        machine->WriteRegister(2, -1);
        delete[] filename;
        return;
    }
    if (!fileSystem->Create(filename, 0))
    {
        machine->WriteRegister(2, -1);
        delete[] filename;
        return;
    }
    machine->WriteRegister(2, 0);
    delete[] filename;
}

void _ReadInt()
{
    int number = 0;
    int nSize = 0;
    int i;
    char *buffer = new char[MaxIntLength];
    nSize = synchConsole->Read(buffer, MaxIntLength);
    i = (buffer[0] == '-') ? 1 : 0;
    for (; i < nSize; i++)
    {
        number *= 10;
        number += buffer[i] - 48;
    }
    number = (buffer[0] == '-') ? -1 * number : number;
    machine->WriteRegister(2, number);
    delete[] buffer;
}

void _PrintInt()
{
    int number;
    number = machine->ReadRegister(4);
    int nSize;
    char *numberBuffer = new char[MaxIntLength];
    numberBuffer = IntToChar(nSize, number);
    synchConsole->Write(numberBuffer, nSize);
    delete[] numberBuffer;
}

void _ReadChar()
{
    int size;
    char *buffer = new char[MaxIntLength];
    size = synchConsole->Read(buffer, MaxIntLength);
    int nBuffer = buffer[0];
    delete[] buffer;
    machine->WriteRegister(2, nBuffer);
}

void _PrintChar()
{
    char cBuffer;
    int nBuffer = machine->ReadRegister(4);
    cBuffer = (char)nBuffer;
    synchConsole->Write(&cBuffer, 1);
}

void _ReadString()
{
    char *buffer = new char[MaxStringLength];
    int length = synchConsole->Read(buffer, MaxStringLength);
    int vAdrr = machine->ReadRegister(4);
    int check = SystemToUser(vAdrr, length, buffer);
    delete[] buffer;
}

void _PrintString()
{
    int vAdrr = machine->ReadRegister(4);
    char *buffer;
    buffer = UserToSystem(vAdrr, 256);
    int i = 0;
    while (buffer[i] != '\0')
        i++;
    buffer[i] = '\n';
    synchConsole->Write(buffer, i + 1);
    machine->WriteRegister(2, 0);
}

void _OpenFile()
{
    char *filename = new char[MaxStringLength];
    int bufferAdd = machine->ReadRegister(4);
    int type = machine->ReadRegister(5);
    filename = UserToSystem(bufferAdd, MaxFileLength + 1);
    if (fileSystem->getIndex() > 10)
    {
        machine->WriteRegister(2, -1);
    }
    else
    {
        if (strcmp(filename, "stdin") == 0)
        {
            machine->WriteRegister(2, 0);
        }
        if (strcmp(filename, "stdout") == 0)
        {
            machine->WriteRegister(2, 1);
        }
        fileSystem->file[fileSystem->getIndex()] = fileSystem->Open(filename, type);
        if (fileSystem != NULL)
        {
            machine->WriteRegister(2, fileSystem->getIndex() - 1);
        }
        else
        {
            machine->WriteRegister(2, -1);
        }
    }
    delete[] filename;
}

void _CloseFile()
{
    int id = machine->ReadRegister(4);
    if (fileSystem->file[id] == NULL)
    {
        machine->WriteRegister(2, -1);
        return;
    }
    machine->WriteRegister(2, 0);
    delete fileSystem->file[id];
    fileSystem->file[id] = NULL;
}

void _ReadFile()
{
    int bufferAdd = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    int id = machine->ReadRegister(6);
    char *buffer = new char[size];
    if (id < 0 || id > 10)
    {
        machine->WriteRegister(2, -1);
        delete[] buffer;
        return;
    }
    if (fileSystem->file[id] == NULL)
    {
        machine->WriteRegister(2, -1);
        delete[] buffer;
        return;
    }
    if (fileSystem->file[id]->getType() == 2)
    {
        int cSize = synchConsole->Read(buffer, size);
        SystemToUser(bufferAdd, cSize, buffer);
        machine->WriteRegister(2, cSize);
        return;
    }
    int firstPos = fileSystem->file[id]->getCurrentPos();
    int temp=fileSystem->file[id]->Read(buffer, size);
    if (temp)
    {
        int secondPos = fileSystem->file[id]->getCurrentPos();
        SystemToUser(bufferAdd, secondPos - firstPos + 1, buffer);
        machine->WriteRegister(2, secondPos - firstPos + 1);
    }
    else
    {
        int secondPos = fileSystem->file[id]->getCurrentPos();
        machine->WriteRegister(2, -1);
    }
    delete[] buffer;
}

void _WriteFile()
{
    int bufferAdd = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    int id = machine->ReadRegister(6);

    char* buffer = new char[size];

    if (id < 0 || id > 10)
    {
        machine->WriteRegister(2, -1);
        delete[] buffer;
        return;
    }
    if (fileSystem->file[id] == NULL)
    {
        machine->WriteRegister(2, -1);
        delete[] buffer;
        return;
    }
    int firstPos = fileSystem->file[id]->getCurrentPos();
    UserToSystem(bufferAdd, size);
    if (fileSystem->file[id]->getType() == 0 || fileSystem->file[id]->getType() == 3)
    {
        if (fileSystem->file[id]->Write(buffer, size) > 0)
        {
            int secondPos = fileSystem->file[id]->getCurrentPos();
            machine->WriteRegister(2, secondPos - firstPos + 1);
        }
    }
    if (fileSystem->file[id]->getType() == 3)
    {
        int i = 0;
        while (buffer[i] != '\0')
            i++;
        buffer[i] = '\n';
        synchConsole->Write(buffer, i + 1);
        machine->WriteRegister(2, i-1);
    }
    if (fileSystem->file[id]->getType() == 1)
    {
        machine->WriteRegister(2, -1);
    }
    delete[] buffer;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case SyscallException:
    {
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG('a', "Shutdown, initiated by user program.\n");
            interrupt->Halt();
            break;
        }
        case SC_Sub:
        {
            _Sub();
            break;
        }
        case SC_Create:
        {
            _CreateFile();
            break;
        }
        case SC_ReadInt:
        {
            _ReadInt();
            break;
        }
        case SC_PrintInt:
        {
            _PrintInt();
            break;
        }
        case SC_ReadChar:
        {
            _ReadChar();
            break;
        }
        case SC_PrintChar:
        {
            _PrintChar();
            break;
        }
        case SC_ReadString:
        {
            _ReadString();
            break;
        }
        case SC_PrintString:
        {
            _PrintString();
            break;
        }
        case SC_Open:
        {
            _OpenFile();
            break;
        }
        case SC_Close:
        {
            _CloseFile();
            break;
        }
        case SC_Read:
        {
            _ReadFile();
            break;
        }
        case SC_Write:
        {
            _WriteFile();
            break;
        }
        }
        //Avanced Program Counter
        machine->registers[PrevPCReg] = machine->registers[PCReg];
        machine->registers[PCReg] = machine->registers[NextPCReg];
        machine->registers[NextPCReg] = machine->registers[NextPCReg] + 4;
        break;
    }
    case PageFaultException:
        printf("No valid translation found %d %d \n", which, type);
        ASSERT(FALSE);
        break;
    case ReadOnlyException:
        printf("Write attempted to page marked "
               "read-only"
               " %d %d \n",
               which, type);
        ASSERT(FALSE);
        break;
    case BusErrorException:
        printf("Translation resulted in an \n invalid physical address %d %d \n", which, type);
        ASSERT(FALSE);
        break;
    case AddressErrorException:
        printf("Unaligned reference or one that\n was beyond the end of the \naddress space %d %d \n", which, type);
        ASSERT(FALSE);
        break;
    case OverflowException:
        printf("nteger overflow in add or sub %d %d \n", which, type);
        ASSERT(FALSE);
        break;
    case IllegalInstrException:
        printf("Unimplemented or reserved instr %d %d \n", which, type);
        ASSERT(FALSE);
        break;
    case NumExceptionTypes:
        printf("NumExceptionTypes");
        ASSERT(FALSE);
        break;
    case NoException:
        break;
    }
}
