#include "usertraps.h"
#include "misc.h"

#include "os/memory_constants.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  /*
  ///////////////////////////////////////////////////////////////////
  /User stack (top)
  /
  /
  /
  /
  /       Anything in between is acceptable to be considered outside allocated pages
  /
  /
  /Code & data    4 pages
  /Code & data
  /Code & data
  /Code & data
  ///////////////////////////////////////////////////////////////////
  */
  int *mem_addr = 0x0 + 5 * MEM_PAGESIZE;

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("q2_3 (%d): Accessed memory inside virtual address space, but outside of currently allocated pages:\n", getpid());
  Printf("Accessed memory: %d\n", *mem_addr);

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("q2_3 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("q2_3 (%d): Done!\n", getpid());
}
