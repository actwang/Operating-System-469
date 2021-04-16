#include "usertraps.h"
#include "misc.h"

#define HELLO_FORK "fork_apps.dlx.obj"


void main (int argc, char *argv[])
{
  int test_num = 0;             // Used to store number of processes to create
  int i,j;                               // Loop index variable
  sem_t s_procs;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  if (argc != 2) {
    Printf("Usage: %s <number of test processes to create>\n", argv[0]);
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  test_num = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  Printf("makeprocs (%d): Creating %d test fork processes\n", getpid(), test_num);


  // Create processes
  Printf("-------------------------------------------------------------------------------------\n");

  for(i=0; i < test_num; i++) {
    Printf("test_fork (%d): Creating hello fork #%d\n", getpid(), i);
    s_procs = sem_create(0);
    ditoa(s_procs,s_procs_str);
    process_create(HELLO_FORK, s_procs_str, NULL);
    if (sem_wait(s_procs) != SYNC_SUCCESS) {
      Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
      Exit();
    }
  }

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
