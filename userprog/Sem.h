#ifndef SEM_H
#define SEM_H
#include "thread.h"
#include "synch.h"

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
    Semaphore * getSem();
};

#endif