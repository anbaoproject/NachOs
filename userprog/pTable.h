#ifndef PTABLE_H
#define PTABLE_H

#include "thread.h"
#include "synch.h"
#include "bitmap.h"
#include "PCB.h"


#define MAX_PROCESS 10

class pTable
{
  private:
    BitMap bm;
    PCB *pcb[MAX_PROCESS];
    int psize;
    Semaphore *bmsem;

  public:
    pTable(int size);
    ~pTable();
    int ExecUpdate(char *name);
    int ExitUpdate(int exit_code);
    int JoinUpdate(int id);
    int GetFreeSlot();
    bool isExist(int pid);
    void Remove(int pid);
    char *GetFileName(int id);
};
#endif