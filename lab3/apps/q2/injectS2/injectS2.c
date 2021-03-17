//#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "mbox.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  mbox_t handle;            //Handle to mbox
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mole_struct *molecules; 
  int i = 0;                // index for str[]
  int num_H2O;

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <handle_to_number of H2O> \n"); 
    Exit();
  }

  handle = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  num_H2O = dstrtol(argv[3], NULL, 10);

   // Map shared memory page into this process's memory space
  if ((molecules = (mole_struct*) shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  while (i < num_H2O){
    Printf("H2O injected into Radeon atmosphere, PID %d\n", getpid());
    sem_signal(molecules->numCO); 
    i++;
  }

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}