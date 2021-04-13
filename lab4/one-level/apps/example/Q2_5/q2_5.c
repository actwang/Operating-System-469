#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  s_procs_completed = dstrtol(argv[1], NULL, 10);

  Printf("q2_5 (%d): Hello world!\n", getpid());

  // Signal the semaphore
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("q2_5 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("q2_5 (%d): Done!\n", getpid());
}
