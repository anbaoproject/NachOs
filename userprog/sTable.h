#ifndef STABLE_H
#define STABLE_H

#include "thread.h"
#include "synch.h"
#include "bitmap.h"

#define MAX_SEMAPHORE 10

class Sem
{
private:
  char name[50];
  Semaphore *sem;

public:
  Sem(char *Name, int init);
  ~Sem();
  void wait();
  void signal();
  char *getName();
  Semaphore *getSem();
};

class sTable
{
private:
  BitMap *bm;
  Sem *semTab[MAX_SEMAPHORE];

public:
  sTable()
  {
    bm = new BitMap(10);
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
      semTab[i] = NULL;
    }
  }
  ~sTable();
  int Create(char *name, int init);
  int Wait(char *name);
  int Signal(char *name);
  int FindFreeSlot(int id);
};

#endif
