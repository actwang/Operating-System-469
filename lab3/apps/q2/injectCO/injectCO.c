//#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
//#include "mbox.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  mbox_t CO_handle;            //Handle to mbox
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mole_struct *molecules; 

  if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_mbox_CO> <handle_to_process_semaphore> \n"); 
    Exit();
  }

  CO_handle = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);


  
  if (mbox_open(CO_handle) != MBOX_SUCCESS){
    Printf("Open mbox failed in injecting CO, PID %d\n", getpid());
    Exit();
  }
  if (mbox_send(CO_handle, 2, (void *)"CO") != MBOX_SUCCESS){
    Printf("Sending to mbox CO injection failed, PID %d \n", getpid());
    Exit();
  }
  if (mbox_close(CO_handle) != MBOX_SUCCESS){
    Printf("Closing mbox in injecting CO failed, PID %d \n", getpid());
    Exit();
  }
  Printf("CO injected into Radeon atmosphere, PID %d\n", getpid());
  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
