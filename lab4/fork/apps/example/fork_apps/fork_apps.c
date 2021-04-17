#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int fork_result;

  if (argc != 2) {
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  fork_result = fork();
  Printf("Before TRAP_ROP_ACCESS PTE:\n");
  if (fork_result == 0) {
    Printf("hello_fork from CHILD (%d): Hello fork!\n", getpid());
  } else {
    Printf("hello_fork from PARENT (%d): Hello fork!\n", getpid());
  }
  //Print the valid PTE of both parent and child


  //Generate TRAP_ROP_ACCESS and print the PTE of parent and child


  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("hello_fork (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("hello_fork (%d): Done!\n", getpid());
}
