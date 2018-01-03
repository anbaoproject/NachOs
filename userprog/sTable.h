#ifndef STABLE_H
#define STABLE_H

#include "thread.h"
#include "synch.h"
#include "Sem.h"
#include "bitmap.h"

#define MAX_SEMAPHORE 10

class sTable
{
  private:
    BitMap *bm;
    Sem *semTab[MAX_SEMAPHORE];
  public:
    sTable();
    ~sTable();
    int Create(char *name, int init);
    int Wait(char *name);
    int Signal(char *name);
    int FindFreeSlot(int id);
};

#endif