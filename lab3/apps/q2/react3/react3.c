// 3) S + 2 O2 -> SO4
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mbox_t S_handle, O2_handle, SO4_handle;
  char buffer[10];
  //int i = 0;                // index for str[]

  if (argc != 5) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_S> <handle_to_O2> <handle_to_SO4> <handle_to_process_semaphore> \n"); 
    Exit();
  }

  S_handle = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  O2_handle = dstrtol(argv[2], NULL, 10);
  SO4_handle = dstrtol(argv[3], NULL, 10);
  s_procs_completed = dstrtol(argv[4], NULL, 10);
  
  // S
  if (mbox_open(S_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in S react3, PID %d\n", getpid());
    Exit();
  }
  
  mbox_recv(S_handle,1,(void*)buffer);

  if (mbox_close(S_handle) != MBOX_SUCCESS){
    Printf("Closing mbox S react3 failed, PID %d \n", getpid());
    Exit();
  }
  // 2O2
  if (mbox_open(O2_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in O2 react3, PID %d\n", getpid());
    Exit();
  }
  mbox_recv(O2_handle,2,(void*)buffer);
  mbox_recv(O2_handle,2,(void*)buffer);
  if (mbox_close(O2_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in O2 react3 failed, PID %d \n", getpid());
    Exit();
  }
  // SO4
  if (mbox_open(SO4_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in SO4 react3, PID %d\n", getpid());
    Exit();
  }
  mbox_send(SO4_handle,3,(void*)"SO4");
  if (mbox_close(SO4_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in S react3 failed, PID %d \n", getpid());
    Exit();
  }
  
  Printf("S + 2O2 == SO4 reacted, PID %d\n\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
