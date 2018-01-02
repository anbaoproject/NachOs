#include "pcb.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"

extern void StartProccessByID(int id);

PCB::PCB()
{
    this->joinSem = NULL;
    this->exitSem = NULL;
    this->multex = NULL;
    this->exitcode = 0;
    this->numwait = 0;
    this->parentId = -1;
}

PCB::PCB(int id)
{
    if (id == 0)
    {
        parentId = -1;
    }
    else
    {
        parentId = currentThread->ProccessID;
    }

    this->numwait = 0;
    this->exitcode = 0;
    this->thread = NULL;
    this->joinSem = new Semaphore("joinsem", 0);
    this->exitSem = new Semaphore("exitsem", 0);
    this->multex = new Semaphore("multex", 1);
}

PCB::~PCB()
{
    if (joinSem != NULL)
    {
        delete this->joinSem;
    }
    if (exitSem != NULL)
    {
        delete this->exitSem;
    }
    if (multex != NULL)
    {
        delete this->multex;
    }
    if (thread != NULL)
    {
        thread->FreeSpace();
        thread->Finish();
    }
}

int PCB::GetId()
{
    return this->thread->ProccessID;
}

int PCB::GetNumWait()
{
    return this->numwait;
}

int PCB::GetExitCode()
{
    return this->exitcode;
}

void PCB::SetExitCode(int exit_code)
{
    this->exitcode = exit_code;
}

void PCB::JoinWait()
{
    joinSem->P();
}

void PCB::JoinRelease()
{
    joinSem->V();
}

void PCB::ExitWait()
{
    exitSem->P();
}

void PCB::ExitRelease()
{
    exitSem->V();
}

void PCB::IncNumWait()
{
    multex->P();
    ++numwait;
    multex->V();
}

void PCB::DecNumWait()
{
    multex->P();
    if (numwait > 0)
    {
        --numwait;
    }
    multex->V();
}

void PCB::SetFileName(char *fn)
{
    strcpy(FileName, fn);
}

char *PCB::GetFileName()
{
    return this->FileName;
}

int PCB::Exec(char *fileName, int pid)
{
    multex->P();
    this->thread = new Thread(filename);
    if (this->thread == NULL)
    {
        printf("\n Not enough memory \n");
        multex->V();
        return -1;
    }

    this->thread->ProccessID = pid;
    this->parentId = currentThread->ProccessID;
    this->thread->Fork(StartProccessByID, pid);
    multex->V();
    return pid;
}