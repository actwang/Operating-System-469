
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mbox_t S_handle, S2_handle;
  char buffer[10];

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_S2> <handle_to_S> <handle_to_process_semaphore> \n"); 
    Exit();
  }

  S2_handle = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  S_handle = dstrtol(argv[2], NULL, 10);
  s_procs_completed = dstrtol(argv[3], NULL, 10);
 
  // 1) S2 -> S + S
  Printf("loop\n");
  if (mbox_open(S2_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in react1, PID %d\n", getpid());
    Exit();
  }
  if (mbox_recv(S2_handle, 2, (void*)buffer) != MBOX_SUCCESS){
    Printf("Sending to mbox react1 failed, PID %d \n", getpid());
    Exit();
  }
  if (mbox_close(S2_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in react1 failed, PID %d \n", getpid());
    Exit();
  }

  if (mbox_open(S_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in react1, PID %d\n", getpid());
    Exit();
  }
  mbox_send(S_handle, 1, (void *)"S");
  mbox_send(S_handle, 1, (void *)"S");
  if (mbox_close(S_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in react1 failed, PID %d \n", getpid());
    Exit();
  }
  Printf("S2 -> S + S reacted, PID %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}