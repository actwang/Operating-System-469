#include "usertraps.h"
#include "misc.h"

#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  int* mem_adr;

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("q2_2 (%d): Access memory beyond the maximum virtual address\n", getpid());
 

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("q2_2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  mem_adr = (MEM_MAX_VIRTUAL_ADDRESS + 7);
  Printf("Try to access memory location: %d\n", mem_adr);
  Printf("Accessed memory: %d\n", *((int*)mem_adr));

  Printf("q2_2 (%d): Done!\n", getpid());
}
