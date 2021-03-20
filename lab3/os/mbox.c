#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

mbox mbox_arr[MBOX_NUM_MBOXES];
mbox_message mbox_msg_arr[MBOX_NUM_BUFFERS];
//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
  int i;
  for (i = 0; i < MBOX_NUM_BUFFERS; i++){
    mbox_msg_arr[i].inuse = 0; 
  }
  for (i = 0; i < MBOX_NUM_MBOXES; i++){
    mbox_arr[i].inuse = 0;
  }
}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
  //mbox loop index variable
  mbox_t mbox_index;
  int intrval, i;

  // disable interrupts to ensure atomicity in grabbing
    // the first availble mailbox
  intrval = DisableIntrs();
  for (mbox_index = 0; mbox_index < MBOX_NUM_MBOXES; mbox_index++){
    if (mbox_arr[mbox_index].inuse == 0){
      mbox_arr[mbox_index].inuse = 1;
      break;
    }
  }
  RestoreIntrs(intrval);

  if (mbox_index == MBOX_NUM_MBOXES) return MBOX_FAIL;

  //Create lock
  if ((mbox_arr[mbox_index].lock = LockCreate()) == SYNC_FAIL){
    printf("ERROR: Couldn't create lock in Mbox_Create\n");
    exitsim();
  }
  // Create conditional var.
  if ((mbox_arr[mbox_index].full = CondCreate(mbox_arr[mbox_index].lock)) == SYNC_FAIL){
    printf("ERROR: Couldn't create conditional full variable in Mbox_Create\n");
    exitsim();
  }
  if ((mbox_arr[mbox_index].empty = CondCreate(mbox_arr[mbox_index].lock)) == SYNC_FAIL){
    printf("ERROR: Couldn't create conditional empty variable in Mbox_Create\n");
    exitsim();
  }

  // Set bitmap to 0 because processes are not using the mbox
  for (i = 0; i < PROCESS_MAX_PROCS; i++) mbox_arr[mbox_index].procs[i] = 0;

  if (MboxInit(&mbox_arr[mbox_index]) != SYNC_SUCCESS) return SYNC_FAIL;

  return mbox_index;
}

//------------------------------------------------------
//
//  MboxInit
//
//  Initialize the mbox msg_queue
//  
//
//-----------------------------------------------------
int MboxInit(mbox *mailbox){
  if(!mailbox) return MBOX_FAIL;
  if(AQueueInit (&mailbox->msg_queue) != QUEUE_SUCCESS){
    printf("FATAL ERROR: could not initialize buffer waiting queue in MboxInit!\n");
    exitsim();
  }
  return MBOX_SUCCESS;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
  int pid;
  pid = GetCurrentPid();

  // if (LockHandleAcquire(mbox_arr[handle].lock) != SYNC_SUCCESS)  {
  //   printf("Lock handle failed aquire\n");
  //   return MBOX_FAIL;
  // }
  // Check if current mbox is opened by the current process, if not, set to 1
  if (mbox_arr[handle].inuse == 0) mbox_arr[handle].inuse = 1;
  if (mbox_arr[handle].procs[pid] == 0) mbox_arr[handle].procs[pid] = 1;
  
  //release lock
  //if (LockHandleRelease(mbox_arr[handle].lock) != SYNC_SUCCESS) return MBOX_FAIL;

  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
  int pid, i, sum;
  pid = GetCurrentPid();
  //printf("test%d\n",pid);

  //if (LockHandleAcquire(mbox_arr[handle].lock) != SYNC_SUCCESS) return MBOX_FAIL;
  if (mbox_arr[handle].inuse == 0) {
    //printf("Failing = %d, handle = %d\n", GetCurrentPid(), handle); 
    return MBOX_FAIL;
  }
  if (pid == 23) printf("PID 23 handle %d\n",handle);
  if (mbox_arr[handle].procs[pid] == 1) mbox_arr[handle].procs[pid] = 0;

  //check if no other process
  for (i = 0; i < PROCESS_MAX_PROCS; i++){
    sum += mbox_arr[handle].procs[i];
  }
  if (sum == 0){    //no other process indeed
    mbox_arr[handle].inuse = 0;   //disable mailbox
    AQueueInit(&(mbox_arr[handle].msg_queue));    // Initialize msg_q back to 0 items
  }

  //if (LockHandleRelease(mbox_arr[handle].lock) != SYNC_SUCCESS)  return MBOX_FAIL;


  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
  Link *l;
  int i, intrval;
  if (!message) {return MBOX_FAIL;
  }
  if (length > MBOX_MAX_MESSAGE_LENGTH){
    return MBOX_FAIL;
  }

  if (LockHandleAcquire(mbox_arr[handle].lock) != SYNC_SUCCESS){
    return MBOX_FAIL;
  }
  //printf("Comes here pid = %d\n", GetCurrentPid());
  if (mbox_arr[handle].inuse == 0) return MBOX_FAIL;
  // Wait for the mbox to be not full
  if (mbox_arr[handle].msg_queue.nitems >= MBOX_MAX_BUFFERS_PER_MBOX){
    CondHandleWait(mbox_arr[handle].full);
  }
  // Get first available buffer, ATOMIC
  intrval = DisableIntrs();
  for (i = 0; i < MBOX_NUM_BUFFERS; i++){
    if (mbox_msg_arr[i].inuse == 0){
      mbox_msg_arr[i].inuse = 1;
    }
  }
  // Set length, copy message to message buffer
  mbox_msg_arr[i].length = length;
  bcopy(message, mbox_msg_arr[i].buffer, length);
  RestoreIntrs(intrval);

  l = AQueueAllocLink((void*)&mbox_msg_arr[i]);
  AQueueInsertFirst(&mbox_arr[handle].msg_queue, l);
  /*DEBUG*//*
  if (length == 1){ //S
    printf("Inserted S to Queue of buffers belong to S_handle pid = %d\n", GetCurrentPid());
    printf("msg_queue.nitem = %d\n", mbox_arr[handle].msg_queue.nitems);
    printf("handle = %d\n",handle);
  }*/
  // Signal to consumer not empty anymore
  CondHandleBroadcast(mbox_arr[handle].empty);
  
  //Release Lock
  if (LockHandleRelease(mbox_arr[handle].lock) != SYNC_SUCCESS){
    //printf("GOt@@\n");
    return MBOX_FAIL;
  }
  //printf("GOt HERE, PId = %d\n", GetCurrentPid()); 
  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
  Link *l;

  if (LockHandleAcquire(mbox_arr[handle].lock) != SYNC_SUCCESS) return MBOX_FAIL;
  if (mbox_arr[handle].inuse == 0) return MBOX_FAIL;

  // wait for buffer to not be empty

  //printf("recv nitems = %d handle = %d, pid = %d\n", mbox_arr[handle].msg_queue.nitems,handle,GetCurrentPid());
  if (mbox_arr[handle].msg_queue.nitems == 0) {
    //printf("waiting here = %d\n\n", GetCurrentPid());
    CondHandleWait(mbox_arr[handle].empty);
  }

  l = AQueueFirst(&mbox_arr[handle].msg_queue); //extract the first link in the msg_q
  bcopy(((mbox_message*)l->object)->buffer, message, ((mbox_message*)l->object)->length);
  AQueueRemove(&l);

  // Signal not full anymore, Do we only need to signal when previously it was full?
  CondHandleBroadcast(mbox_arr[handle].full);
  //Release lock
  if (LockHandleRelease(mbox_arr[handle].lock) != SYNC_SUCCESS){
    return MBOX_FAIL;
  }

  return MBOX_SUCCESS;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
int MboxCloseAllByPid(int pid) {
  int i, n, procs_count;
  for (i = 0; i < MBOX_NUM_MBOXES; i ++){
    if (mbox_arr[i].procs[pid]){
      LockHandleAcquire(mbox_arr[i].lock);
      mbox_arr[i].procs[pid] = 0;
      //check if it's the only open process
      for (n = 0; n < PROCESS_MAX_PROCS; n++){
        procs_count += mbox_arr[i].procs[n];
      }
      if (!procs_count){
        mbox_arr[i].inuse = 0;
      }
      LockHandleRelease(mbox_arr[i].lock);
    }
  }
  
  return MBOX_SUCCESS;
}
