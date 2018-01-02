#include "Sem.h"
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