//#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
//#include "mbox.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  mbox_t S2_handle;
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mole_struct *molecules; 

  if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_mbox_S2> <handle_to_process_semaphore> \n"); 
    Exit();
  }

  S2_handle = dstrtol(argv[1], NULL, 10);
  s_procs_completed = dstrtol(argv[2], NULL, 10);


  if (mbox_open(S2_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in injecting S2, PID %d\n", getpid());
    Exit();
  }
  if (mbox_send(S2_handle, 2, (void *)"S2") != MBOX_SUCCESS){
    Printf("Sending to mbox S2 injection failed, PID %d \n", getpid());
    Exit();
  }
  if (mbox_close(S2_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in injecting S2 failed, PID %d \n", getpid());
    Exit();
  }
  
  Printf("S2 injected into Radeon atmosphere, PID %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
