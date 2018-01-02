#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

class PCB
{
  private:
    Semaphore *joinSem;
    Semaphore *exitSem;
    Semaphore *multex;
    int exitcode;
    int numwait;

    char FileName[32];
    Thread * thread;

  public:
    int parentId;
    PCB();
    PCB(int id);
    ~PCB();

    int Exec(char *fileName, int pid);
    int GetId();
    int GetNumWait();
    void JoinWait();
    void ExitWait();
    void JoinRelease();
    void ExitRelease();
    void IncNumWait();
    void DecNumWait();
    void SetExitCode(int exit_code);
    int GetExitCode();
    void SetFileName(char * file_name);
    char * GetFileName();
};

#endif