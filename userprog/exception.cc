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
char *UserToSystem(int virtAddress, int limit)
{
    int index;
    int oneChar;
    char *kernelBuffer = NULL;
    kernelBuffer = new char[limit + 1];
    if (kernelBuffer == NULL)
    {
        return kernelBuffer;
    }
    memset(kernelBuffer, 0, limit + 1);

    for (int i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddress + i, 1, &oneChar);
        kernelBuffer[i] = (char)oneChar;
        printf("%c", oneChar);
        if (oneChar == 0)
        {
            break;
        }
    }

    return kernelBuffer;
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
            int op1  = machine->ReadRegister(4);
            int op2  = machine->ReadRegister(5);
            int res = op1-op2;
            machine->WriteRegister(2,res);
            break;
        }
            case SC_Create:
        {
            int virtAddress;
            char *filename;
            DEBUG('a', "\n SC_CREATE call ..,");
            DEBUG('a', "\n Reading virtual address of file name ...");
            filename = new char[MaxFileLength];
            virtAddress = machine->ReadRegister(4);
            DEBUG('a', "\n Reading filename ...");
            filename = UserToSystem(virtAddress, MaxFileLength + 1);
            if (filename == NULL)
            {
                printf("\n Not enough memory in system ");
                DEBUG('a', "\n Not engouh memory in system ");
                machine->WriteRegister(2, -1);
                delete[] filename;
                break;
            }
            DEBUG('a', "\n Finish reading filename.");
            if (!fileSystem->Create(filename, 0))
            {
                printf("\n Error create file %s", filename);
                machine->WriteRegister(2, -1);
                delete[] filename;
                break;
            }
            printf("\n Create File %s\n", filename);
            machine->WriteRegister(2, 0);
            delete[] filename;
            break;
        }
        case SC_ReadInt:
        {
            int number =0;
            int nSize =0;
            int i;
            char *buffer = new char[MaxIntLength];
            nSize = synchConsole->Read(buffer, MaxIntLength);
            i = (buffer[0]=='-') ? 1 : 0;
            for(;i<nSize;i++){
                number*=10;
                number+=buffer[i]-48;
            }
            number = (buffer[0]=='-') ? -1*number : number;
            machine->WriteRegister(2, number);
            delete[] buffer;
            break;
        }
        case SC_PrintInt:
        {
            int number;
            DEBUG('a', "\n SC_PrintInt call ..,");
            DEBUG('a', "\n Reading virtual address of number ...");
            number= machine->ReadRegister(4);
            char * numberBuffer = new char[MaxIntLength];
            char * resultBuffer = new char[MaxIntLength];
            int nSize; 
            int i=0;
            int check =0;
            int count =0;
            number < 0 ? (check=1,number*=-1,resultBuffer[i]='-',count=1) : 0;
            while(number>0){
                numberBuffer[i]=(number%10) + '0';
                i++;
                number/=10;
            }
            nSize=i;
            for(int j=nSize-1;j>=0;j--){
                resultBuffer[count]=numberBuffer[j];
                count++;
            }
            resultBuffer[count]='\n';
            nSize=count;
            synchConsole->Write(resultBuffer, nSize+1);
            delete[] numberBuffer;
            delete[] resultBuffer;
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
	case NoException:
        break;
    }
}
