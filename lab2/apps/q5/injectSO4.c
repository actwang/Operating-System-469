#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  sem_struct *all_sems; 
  int i = 0;                // index for str[]
  int num_SO4;

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); 
    Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <handle_to_number of SO4> \n"); 
    Exit();
  }

  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  num_SO4 = dstrtol(argv[3], NULL, 10);

   // Map shared memory page into this process's memory space
  if ((all_sems = (sem_struct*) shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Signal the semaphore to tell the original process that we're done
  while (i < num_SO4){
    Printf("SO4 injected into Radeon atmosphere, PID %d\n", getpid());
    all_sems->numSO4 ++; 
    i++;
  }

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
