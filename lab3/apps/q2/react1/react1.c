
// 1) S2 -> S + S
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  mole_struct *molecule; 
  int i = 0;                // index for str[]

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> \n"); 
    Exit();
  }

  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);

   // Map shared memory page into this process's memory space
  if ((molecule = (sem_struct*) shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

 
    // sem_wait(molecule->numH2O);
    // sem_signal(molecule->numH2);
    // sem_signal(molecule->numO2);
    Printf("S2 -> S + S reacted, PID %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}