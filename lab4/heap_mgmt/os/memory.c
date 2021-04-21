//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"

// num_pages = size_of_memory / size_of_one_page
static uint32 freemap[NUM_PAGES];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}


//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {

  int i;
  int bit;
  int pagenum;
  nfreepages = 0;

  pagestart = (lastosaddress >> 12) + 1;    //(lastosaddress + MEM_PAGESIZE - 4) / MEM_PAGESIZE;

  for (i = 0 ; i < NUM_PAGES; i++){
    freemap[i] = 0; //reset the whole freemap including 0x0--lastosaddress
  }

  // Set all non-OS bits to free(1) in the freemap
  for (i = pagestart ; i < MEM_MAX_PAGES ; i ++){
    //find the bit position of the bit we want to set
    bit = i % 32;
    pagenum = i/32;
    // Set the bit to 1
    freemap[pagenum] = freemap[pagenum] | (1 << bit);
    nfreepages++;
  }

}


//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {
  uint32 phys_addr, page_num, page_offset;

  if (addr > MEM_MAX_VIRTUAL_ADDRESS){
    printf("Virtual address full, exiting translation\n");
    return 0;
  }

  page_num = addr >> MEM_L1FIELD_FIRST_BITNUM;
  page_offset = addr & 0xFFF;

  phys_addr = (pcb->pagetable[page_num] & MEM_PTE2PAGE_MASK) | page_offset;

  // Check if it's valid. If not, we need to trigger a page fault handler
  if ( (pcb->pagetable[page_num] & MEM_PTE_VALID) == 0){
    // Save address to currentSavedFrame
    pcb->currentSavedFrame[PROCESS_STACK_FAULT] = addr;
    // Call page fault handler
    MemoryPageFaultHandler(pcb);
    return 0;
  }

  return phys_addr;
}


//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGESIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);

    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference.
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  unsigned int fault_pagenum, userStack_pagenum;
  // Page number of the faulting address
  fault_pagenum = pcb->currentSavedFrame[PROCESS_STACK_FAULT] >> MEM_L1FIELD_FIRST_BITNUM;
  // Page number for user stack
  userStack_pagenum = pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER] >> MEM_L1FIELD_FIRST_BITNUM;
  
  if (fault_pagenum < userStack_pagenum){
    dbprintf('m',"SegFault(fault address higher than user stack pointer) in Memory Page Fault Handler.\n");
    ProcessKill();
    return MEM_FAIL;
  }
  // if user stack caused fault and new page allocated
  else   
  {
    pcb->pagetable[fault_pagenum] = MemorySetupPte(MemoryAllocPage());
    pcb->npages++;
    return MEM_SUCCESS;
  }
  
  return MEM_FAIL;
}


//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------
// Find an available bit in freemap, set it, decrement nfreepages and return allocated page number
int MemoryAllocPage(void) {
  int i = 0, page_num, page_var;
  uint32 bit = 0;
  //no free pages
  if (nfreepages == 0){
    printf("No free pages in MemoryAllocPage\n");
    return -1;
  }

  //find the freemap index that has an available bit
  while (freemap[i] == 0) i++;

  page_var = freemap[i];    // the page variable at index i
  // find the bit in the 32 bit integer that is 1(while unavailable, keep looking)
  while (!(page_var & (1 << bit))) bit++;

  freemap[i] &= invert(1 << bit);   // Mark as unavailable
  page_num = i*32 + bit;

  nfreepages--;

  return page_num;
}

// Return the PTE given page number
uint32 MemorySetupPte (uint32 page) {
  uint32 pte = (MEM_PAGESIZE * page) | MEM_PTE_VALID;
  return pte;
}

// Given the page number, set the bit in the freemap to mark it as free
void MemoryFreePage(uint32 page) {
  uint32 index = page / 32;
  uint32 bit_pos = page % 32;
  freemap[index] |= (1 << bit_pos);

  nfreepages++;
}



void* malloc(PCB* pcb, int memsize) {
  int node_address;
  int heap_size;
  int virt_address, phys_address;

  if ((memsize <= 0) || (memsize > MEM_PAGESIZE)) {
    return NULL;
  }

  node_address = MallocNodeHelper(&(pcb->heap_array[0]), pcb, memsize);

  if (node_address >= 0) {
    heap_size = pcb->heap_array[node_address].heapSize;
    virt_address = ((MEM_PAGESIZE * 4) | node_address);
    phys_address = MemoryTranslateUserToSystem(pcb, virt_address);
    printf("Malloc a heap of size %d bytes: virtual address %d, physical address %d\n", heap_size, virt_address, phys_address);
    return (void*)virt_address;
  }
  return NULL;
}


int MallocNodeHelper(heapNode* node, PCB* pcb, int memsize) {
  int tmp_addr;
  heapNode* left_child, right_child;

  if (!node) return -1;
  if ((node->leftChild == NULL) && (node->heapUsage == 0)) {
    if ((memsize <= node->heapSize) && (memsize > (node->heapSize / 2))) {
      node->heapUsage = 1;
      printf("Allocated the block: order = %d, addr = %d, requested mem size = %d, block size = %d\n", node->nodeOrder, node->nodeAddress, memsize, node->heapSize);
      return node->nodeAddress;
    }

    if (memsize > (node->heapSize / 2)) {
      return -1;
    } else if (node->nodeOrder == 0) {
      return -1;
    } else {
      left_child = &(pcb->heap_array[2 * node->nodeIndex]);
      *left_child = {
        .parent = node,
        .heapSize = node->heapSize / 2,
        .nodeOrder = node->nodeOrder - 1,
        .nodeAddress = node->nodeAddress,
      };
      printf("Created a left child node (order = %d, addr = %d, size = %d) of parent (order = %d, addr = %d, size = %d)\n",
                                                left_child->nodeOrder, left_child->nodeAddress, left_child->heapSize,
                                                node->nodeOrder, node->nodeAddress, node->heapSize);

      right_child = &(pcb->heap_array[2*node->nodeIndex + 1]);
      *right_child = {
        .parent = node,
        .heapSize = node->heapSize / 2,
        .nodeOrder = node->nodeOrder - 1,
      };
      right_child->nodeAddress = (node->nodeAddress) + (right_child->heapSize);
      printf("Created a right child node (order = %d, addr = %d, size = %d) of parent (order = %d, addr = %d, size = %d)\n",
                                                right_child->nodeOrder, right_child->nodeAddress, right_child->heapSize,
                                                node->nodeOrder, node->nodeAddress, node->heapSize);

      node->leftChild = left_child;
      node->rightChild = right_child;
    }
  }

  tmp_addr = MallocNodeHelper(node->leftChild, pcb, memsize);
  if (tmp_addr >= 0) {
    return tmp_addr;
  } else {
    return MallocNodeHelper(node->rightChild, pcb, memsize);
  }
}



int mfree(PCB* pcb, void* ptr){
  int addr, i, sz;
  heapNode* Hnode;

  if (!ptr){return MEM_FAIL;}

  addr = ((int)ptr & MEM_ADDRESS_OFFSET_MASK);
  for (i = 0; i < MEM_MALLOC_MAX_NUM; i++){
    if (pcb->heap_array.nodeAddress == addr){
      sz = pcb->heap_array[i].heapSize;
      Hnode = &(pcb->heap_array[i]);
      break;
    }
  }

  dbprintf('m',"Freeing %d byte block @ virtual addr %d\n",sz,(int)ptr);
  MemoryMergeNodes(Hnode);

  return Hnode->heapSize;
}

void MemoryMergeNodes(heapNode* Hnode){
  if (!Hnode){return void;}

  Hnode->heapUsage = 0; // Not in use anymore
  Hnode->leftChild = NULL; Hnode->rightChild = NULL;

  // While we're not at the root node
  if (Hnode->parent == NULL){
    return;
  }

  // if current node is left node
  if (Hnode == Hnode->parent->leftChild){
    if (Hnode->parent->rightChild->heapUsage == 0){    // if we meet adjacent node inuse then we stop
      printf("Coalesced buddy nodes (order = %d, addr = %d, size = %d) & ", Hnode->nodeOrder, Hnode->nodeAddress, Hnode->heapSize);
      printf("(order = %d, addr = %d, size = %d)\n",Hnode->parent->right->nodeOrder, Hnode->parent->rightChild->nodeAddress, Hnode->parent->rightChild->heapSize);
      printf("into the parent node (order = %d, addr = %d, size = %d)\n", Hnode->parent->nodeOrder, Hnode->parent->nodeAddress, Hnode->parent->heapSize);
      MemoryMergeNodes(Hnode->parent);
    }
  }
  else{   // current node is right node
    if (Hnode->parent->leftChild->heapUsage == 0){
      printf("Coalesced buddy nodes (order = %d, addr = %d, size = %d) & ", Hnode->nodeOrder, Hnode->nodeAddress, Hnode->heapSize);
      printf("(order = %d, addr = %d, size = %d)\n",Hnode->parent->left->nodeOrder, Hnode->parent->leftChild->nodeAddress, Hnode->parent->leftChild->heapSize);
      printf("into the parent node (order = %d, addr = %d, size = %d)\n", Hnode->parent->nodeOrder, Hnode->parent->nodeAddress, Hnode->parent->heapSize);
      MemoryMergeNodes(Hnode->parent);
    }
  }
}
