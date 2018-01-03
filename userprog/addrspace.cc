// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader(NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------
FileTable :: FileTable()
{
	for ( int i = 0; i < NumFileTable; i++ )
	{
		//type[i] = NULL;
	//	currentOffset[i] = NULL;
		FileName[i] = NULL;
	}
	FileTableOpen[0] = new OpenFile(0); //Standard Input (stdin)
	FileTableOpen[1] = new OpenFile(1); // Standard Output (stdout)
	FileTableOpen[2] = new OpenFile(2); //read only
	FileTableOpen[3] = new OpenFile(3); // write and read
	FileTableOpen[4] = new OpenFile(4); //Standard Error (stderr)
	
	for ( int i = 5; i < NumFileTable; i++ )
	{
		FileTableOpen[i] = NULL;
	}
	FileName[0] = "stdin"; //type[0] = 0;
	FileName[1] = "stdout";//type[1] = 1;
	FileName[2] = "olread"; //type[0] = 2;
	FileName[3] = "reandwr";//type[1] = 3;
	FileName[4] = "stderr";//type[1] = 3;
	
	
}
FileTable :: ~FileTable()
{
		delete FileTableOpen;
		//delete type;
		for ( int i = 0; i < NumFileTable; i++ )
			delete FileName[i];
		delete FileName;
}

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int i, size;
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
        (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    // how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize; // we need to increase the size
                                                                                          // to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    ASSERT(numPages <= gPhysPageBitMap->NumClear()); // check we're not trying
                                                     // to run anything too big --
                                                     // at least until we have
                                                     // virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
          numPages, size);
    // first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++)
    {
        pageTable[i].virtualPage = i;                        // for now, virtual page # = phys page #
        pageTable[i].physicalPage = gPhysPageBitMap->Find(); //i;
        ASSERT(pageTable[i].physicalPage != -1);
        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE; // if the code segment was entirely on
                                       // a separate page, we could set its
                                       // pages to be read-only
    }

    // zero out the entire address space, to zero the unitialized data segment
    // and the stack segment
    //bzero(machine->mainMemory, size);

    // then, copy in the code and data segments into memory
    // if (noffH.code.size > 0)
    // {
    //     DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
    //           noffH.code.virtualAddr, noffH.code.size);
    //     executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
    //                        noffH.code.size, noffH.code.inFileAddr);
    // }
    // if (noffH.initData.size > 0)
    // {
    //     DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
    //           noffH.initData.virtualAddr, noffH.initData.size);
    //     executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
    //                        noffH.initData.size, noffH.initData.inFileAddr);
    // }
    bzero(machine->mainMemory, size);
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }
}

AddrSpace::AddrSpace(char* filename)
{
//	int temp;
   	 openfiletab = new FileTable();
	NoffHeader noffH; 
	unsigned int i, size, j; 
	unsigned int numCodePage, numDataPage; 
	int lastCodePageSize, lastDataPageSize, firstDataPageSize,tempDataSize; // kích 
	
	OpenFile* executable = fileSystem->Open(filename); 
	if (executable == NULL)
	{ 
		printf("\nAddrspace::Error opening file: %s",filename); 
		return; 
	} 
	
	executable->ReadAt((char *)&noffH, sizeof(noffH), 0); 
	if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost(noffH.noffMagic) == NOFFMAGIC)) 
		SwapHeader(&noffH); 
	ASSERT(noffH.noffMagic == NOFFMAGIC); 
	addrLock->P();
	// how big is address space? 
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;  // we need to increase the size  // to leave room for the stack 
	numPages = divRoundUp(size, PageSize); 
	size = numPages * PageSize; 
	// Check the available memory enough to load new process 
	//debug 

	if (numPages > gPhysPageBitMap->NumClear() )
	{ 
		printf("\nAddrSpace:Load: notenough memory for new process..!"); 
		numPages = 0; 
		delete executable; 
		addrLock->V(); 
		return ; 
	} 
	// first, set up the translation 
	pageTable = new TranslationEntry[numPages]; 
	for (i = 0; i < numPages; i++)
	{ 
		pageTable[i].virtualPage = i; // for now, virtual page # = phys page # 
		pageTable[i].physicalPage = gPhysPageBitMap->Find();
		pageTable[i].valid = TRUE; 
		pageTable[i].use = FALSE; 
		pageTable[i].dirty = FALSE; 
		pageTable[i].readOnly = FALSE;// if the code segment was entirely on 
		// a separate page, we could set its 
		// pages to be read-only 
		
		bzero(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]), PageSize); 
		//printf("phyPage %d \n",pageTable[i].physicalPage); 
	} 
	addrLock->V();
	// Calculate numCodePage and numDataPage 
	numCodePage = divRoundUp(noffH.code.size, PageSize); 
	// Calculate lastCodePageSize 
	lastCodePageSize = noffH.code.size - (numCodePage-1)*PageSize; 
	tempDataSize = noffH.initData.size - (PageSize - lastCodePageSize); 
	if (tempDataSize < 0)
	{ 
		numDataPage = 0;   firstDataPageSize = noffH.initData.size; 
 	} 
	else
	{ 
		 numDataPage = divRoundUp(tempDataSize, PageSize); 
		 lastDataPageSize = tempDataSize - (numDataPage-1)*PageSize; 
		 firstDataPageSize = PageSize - lastCodePageSize; 
	} 
//	 Copy the Code segment into memory 
if(noffH.code.size > 0) 
{ 
	for (i = 0; i < numCodePage; i++) 
	{  
	    executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]) + 
		pageTable[i].physicalPage*PageSize, i<(numCodePage-1)?PageSize:lastCodePageSize, noffH.code.inFileAddr + i*PageSize); 
	} 
}
	//Check whether last page of code segment is full and copy the first part of 
	//initData segment into this page 

	 if (lastCodePageSize < PageSize)
	{   // Copy initData into the remain part of lastCodePage 
		 if (firstDataPageSize > 0) 
		 executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr])+(pageTable[i-1].physicalPage*PageSize + lastCodePageSize), firstDataPageSize, noffH.initData.inFileAddr); 
	} 
	// Copy the remain of initData segment into memory 
if(noffH.initData.size > 0) 
{
	for (j = 0; j< numDataPage; j++)
	 { 
		// 
		 executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr])+pageTable[i].physicalPage*PageSize, 
		j<(numDataPage-1)?PageSize:lastDataPageSize, 
		noffH.initData.inFileAddr + j*PageSize + firstDataPageSize); 
		i++; 
	} 
}
	delete executable; 
	return ;

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    for (unsigned int x = 0; x < numPages; x++)
    {
        gPhysPageBitMap->Clear(pageTable[x].physicalPage);
    }
    delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
