#include "sTable.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"

Sem::Sem(char* Name, int init){
    if(name==NULL || init < 0) {
        this->name = "";
        sem = NULL;
    }
    strcpy(this->name,Name);
    sem = new Semaphore(this->name,init);
}

Sem::~Sem(){
    delete sem;
}

void Sem::wait(){
    sem->P();
}

void Sem::signal(){
    sem->V();
}

char* Sem::getName(){
    return name;
}

Semaphore * Sem::getSem(){
    return this->sem;
}

sTable::~sTable()
{
    delete bm;
    for (int i = 0; i < MAX_PROCESS; i++)
    {
        if (semTab[i] != NULL)
        {
            delete semTab[i];
        }
    }
}

int sTable::Create(char *name, int init)
{
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (semTab[i] != NULL)
        {
            if (strcpy(semTab[i]->getName(), name) == 0)
            {
                return -1;
            }
        }
    }
    int id = bm->Find();
    if (id == -1)
    {
        return -1;
    }
    semTab[id] = new Sem(name, init);
    if (semTab[id]->getSem() == NULL)
    {
        delete semTab[id];
        bm->Clear(id);
        return -1;
    }
    else
    {
        return 0;
    }
}

int sTable::Wait(char *name)
{
    if (name == NULL)
    {
        return -1;
    }
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (semTab[i] != NULL)
        {
            if (strcmp(semTab[i]->getName(), name) == 0)
            {
                semTab[i]->wait();
                return 0;
            }
        }
    }
    return -1;
}

int sTable::Signal(char *name)
{
    if (name == NULL)
    {
        return -1;
    }

    for(int i=0;i<MAX_SEMAPHORE;i++){
        if(semTab[i]!=NULL){
            if(strcmp(semTab[i]->getName(),name)==0){
                semTab[i]->signal();
                return 0;
            }
        }
    }
    return -1;
}