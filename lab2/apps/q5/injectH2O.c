#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  lock_t buff_lock;			    // Lock for the buffer
  int i = 0;                // index for str[]

  if (argc != ) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <> <handle_to_page_mapped_semaphore> <handle_to_lock>\n"); 
    Exit();
  }

  int numH2O = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
 
  // Now print a message to show that everything worked
  // Printf("Producer: This is one of the %d instances you created.  ", buffer1->numprocs);
  // Printf("Producer: Missile code is: %c\n", buffer1->really_important_char);
  // Printf("Producer: My PID is %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  while (i < numH2O){
    Printf("H2O injected into Radeon atmosphere, PID %d\n", getpid());
    i++;
  }

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
