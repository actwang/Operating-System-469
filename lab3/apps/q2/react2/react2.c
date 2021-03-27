//2) 4 CO -> 2 O2 + 2 C2
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mbox_t CO_handle, O2_handle, C2_handle;
  char buffer[10];
  int i;

  if (argc != 5) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_CO> <handle_to_O2> <handle_to_C2> <handle_to_process_semaphore> \n"); 
    Exit();
  }

  CO_handle = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  O2_handle = dstrtol(argv[2], NULL, 10);
  C2_handle = dstrtol(argv[3], NULL, 10);
  s_procs_completed = dstrtol(argv[4], NULL, 10);
  
  // 4CO -> 2O2 + 2C2
  if (mbox_open(CO_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in CO react2, PID %d\n", getpid());
    Exit();
  }
  for (i = 0; i < 4; i++){
    mbox_recv(CO_handle,2,(void*)buffer);
  }
  if (mbox_close(CO_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in CO react2 failed, PID %d \n", getpid());
    Exit();
  }

  // 2O2
  if (mbox_open(O2_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in O2 react2, PID %d\n", getpid());
    Exit();
  }
  for (i = 0; i < 2; i++){
    mbox_send(O2_handle,2,(void*)"O2");
  }
  if (mbox_close(O2_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in O2 react2 failed, PID %d \n", getpid());
    Exit();
  }

  // 2C2
  if (mbox_open(C2_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in O2 react2, PID %d\n", getpid());
    Exit();
  }
  for (i = 0; i < 2; i++){
    mbox_send(C2_handle,2,(void*)"C2");
  }
  if (mbox_close(C2_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in C2 react2 failed, PID %d \n", getpid());
    Exit();
  }
  Printf("4CO -> 2O2 + 2C2 reacted, PID %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}