#include "pTable.h"
#include "system.h"
#include "openfile.h"

pTable::pTable(int size){
    if(size <0){
        this= NULL;
        return;
    }

    psize = size;
    bm = new BitMap(size);
    bmsem = new Semaphore("bmsem",1);

    for(int i=0;i<MAX_PROCESS,i++){
        pcb[i] = 0;
    }

    bm->Mark(0);

    pcb[0] = new PCB(0);
    pcb[0]->SetFileName("./test/scheduler");
    pcb[0]->parentId = -1;
}

pTable::~pTable(){
    if( bm != 0) {
        delete bm;
    }

    for(int i=0;i<psize;i++){
        if(pcb[i]!=0){
            delete pcb[i];
        }
    }

    if(bmsem!=0){
        delete bmsem;
    }
}

int pTable::ExecUpdate(char * name){
    bmsem->P();
    if(name==NULL){
        printf("\n execute name is NULL\n");
        bmsem->V();
        return -1;
    }

    if(strcmp(name,"./test/scheduler")==0 || strcmp(name,currentThread->getName())==0 ){
        printf("\n Cannot execute itself \n");
        bmsem->V();
        return -1;
    }

    int index = this->GetFreeSlot();

    if(index<0){
        printf("\n There is no free slot\n");
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);

    pcb[index]->parentId = currentThread->ProccessID;
    int pid = pcb[index]->Exec(name, index);
    bmsem->V();
    return pid;
}

int pTable::ExitUpdate(int exit_code){
    int id = currentThread->ProccessID;
    if(id==0){
        currentThread->FreeSpace();
        interrupt->Halt();
        return 0;
    }

    if(!isExist(id)){
        printf("%d is not exist",id);
        return -1;
    }

    pcb[id]->SetExitCode(exit_code);
    pcb[pcb[id]->parentId]->DecNumWait();

    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();
    Remove(id);
    return exit_code;
    
}

int pTable::JoinUpdate(int id){
    if(id<0){
        printf("\n id: %d",id);
        return -1;
    }

    if(currentThread->ProccessID != pcb[id]->parentId){
        printf("\n Cannot Join");
        return -1;
    }

    pcb[pcb[id]->parentId]->IncNumWait();
    pcb[id]->JoinWait();
    int exit_code = pcb[id]->GetExitCode();
    pcb[id]->ExitRelease();
    return exit_code;
}

int pTable::GetFreeSlot(){
    return bm->Find();
}

bool pTable::isExist(int pid){
    return bm->Test(pid);
}

void pTable::Remove(int pid){
    bm->Clear(pid);
    if(pcb[pid]!=0){
        delete pcb[pid];
    }
}

char * pTable::GetFileName(int id){
    return pcb[id]->GetFileName();
}