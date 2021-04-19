#include "usertraps.h"
#include "misc.h"

#define Q2_1 "q2_1.dlx.obj"
#define Q2_2 "q2_2.dlx.obj"
#define Q2_3 "q2_3.dlx.obj"
#define Q2_4 "q2_4.dlx.obj"
#define Q2_5 "q2_5.dlx.obj"
#define Q2_6 "q2_6.dlx.obj"


void main (int argc, char *argv[])
{
  int test_num;             // Used to store number of processes to create
  int i,j;                               // Loop index variable
  sem_t s_procs;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  if (argc != 2) {
    Printf("Usage: %s <number of test processes to create>\n", argv[0]);
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  test_num = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  if (test_num < 1 || test_num > 6){
    Printf("Invalid test number. Enter a number between 1 and 6\n");
    Exit();
  }
  Printf("makeprocs (%d): Creating %d test processes\n", getpid(), test_num);


  // Create processes
  Printf("-------------------------------------------------------------------------------------\n");

  for(i=1; i < 7; i++) {
    if (i == 1){
      Printf("Running Test # 1\n");
      s_procs = sem_create(0);
      ditoa(s_procs,s_procs_str);
      process_create(Q2_1, s_procs_str, NULL);
      sem_wait(s_procs);
    }
    else if (i == 2){
      Printf("Running Test # 2\n");
      s_procs = sem_create(0);
      ditoa(s_procs, s_procs_str);
      process_create(Q2_2, s_procs_str, NULL);
      sem_wait(s_procs);
    }
    else if (i == 3){
      Printf("Running Test # 3\n");
      s_procs = sem_create(0);
      ditoa(s_procs, s_procs_str);
      process_create(Q2_3, s_procs_str, NULL);
      sem_wait(s_procs);
    }
    else if (i == 4){
      Printf("Running Test # 4\n");
      s_procs = sem_create(0);
      ditoa(s_procs, s_procs_str);
      process_create(Q2_4, s_procs_str, NULL);
      sem_wait(s_procs);
    }
    else if (i == 5){
      Printf("Running Test # 5\n");
      s_procs = sem_create(1-100);
      ditoa(s_procs, s_procs_str);
      for (j = 0; j < 100; j++){
        process_create(Q2_5, s_procs_str, NULL);
      }
      sem_wait(s_procs);
    }
    else if (i == 6){
      Printf("Running Test # 6\n");
      s_procs = sem_create(1-30);
      ditoa(s_procs, s_procs_str);
      Printf("REACHED\n");
      for (j = 0; j < 30; j++){
        process_create(Q2_6, s_procs_str, NULL);
      }
      sem_wait(s_procs);
      Printf("REACHED\n");
    }
    else {
      Printf("Invalid test number\n");
    }
  }

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
