#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  sem_struct* all_sems;
  int num_procs = 5;
  uint32 h_mem;                   // Used to hold handle to shared memory page
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char num_H2O_str[10]; char num_SO4_str[10];
  char num_react1_str[10], num_react2_str[10], num_react3_str[10];
  int num_H2O, num_SO4, num_H2, num_O2, num_SO2, tmp, num_react1, num_react2, num_react3, num_H2SO4;

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of H2O molescules> <number of SO4 molecules>\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  num_H2O = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  num_SO4 = dstrtol(argv[2], NULL, 10); // the "10" means base 10
  num_H2 = (num_H2O / 2) * 2;
  num_O2 = num_SO4 + num_H2O / 2;
  num_SO2 = num_SO4;
  Printf("Creating %d H2Os and %d SO4s.\n", num_H2O , num_SO4);

  if ((h_mem = shmget()) == 0) {
    Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // Map shared memory page into this process's memory space
  if ((all_sems = (sem_struct*) shmat(h_mem)) == NULL) {
    Printf("Could not map the shared page to virtual address in "); Printf(argv[0]); Printf(", exiting..\n");
    Exit();
  }

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  
  if ((s_procs_completed = sem_create(-(num_procs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Compute the number of each reactions that can happen
  num_react1 = num_H2O / 2;
  num_react2= num_SO4;
  tmp = (num_H2<num_O2?num_H2:num_O2);
  num_react3 = tmp < num_SO2 ? tmp: num_SO2;
  num_H2SO4 = num_react3;

  // Create semaphores for each of the struct's sem_t
  all_sems->numH2O = sem_create(num_H2O);
  all_sems->numSO4 = sem_create(num_SO4);
  all_sems->numH2 = sem_create(num_H2);
  all_sems->numReact1 = sem_create(num_react1);
  all_sems->numReact2 = sem_create(num_react2);
  all_sems->numReact3 = sem_create(num_react3);
  all_sems->numO2 = sem_create(num_O2);
  all_sems->numSO2 = sem_create(num_SO2);
  all_sems->numH2SO4 = sem_create(num_H2SO4);
  Printf("all_sems->numH2O\n",all_sems->numH2O);

  ditoa(s_procs_completed, s_procs_completed_str);
  ditoa(h_mem, h_mem_str);
  ditoa(num_H2O, num_H2O_str);
  ditoa(num_SO4, num_SO4_str);
  ditoa(num_react1, num_react1_str);
  ditoa(num_react2, num_react2_str);
  ditoa(num_react3, num_react3_str);

  process_create(FILE_H2O, h_mem_str, s_procs_completed_str, num_H2O_str, NULL);
  process_create(FILE_SO4, h_mem_str, s_procs_completed_str, num_SO4_str, NULL);

  // wait for semaphore reactants inside react.c
  process_create(FILE_REACT1, h_mem_str, s_procs_completed_str, num_react1_str, NULL);
  process_create(FILE_REACT2, h_mem_str, s_procs_completed_str, num_react2_str, NULL);
  process_create(FILE_REACT3, h_mem_str, s_procs_completed_str, num_react3_str, NULL);

  // And finally, wait until all five processes have finished. (s_procs_completed back at 1 from -4)
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  num_H2O -= (num_react1 *2);
  num_H2 -= num_react3;
  num_O2 -= num_react3;
  num_SO2 -=num_react3;
  Printf("%d H2O's left over. %d H2's left over. %d O2's left over. ", num_H2O, num_H2, num_O2);
  Printf("%d SO2's left over. %d H2SO4's created. \n",num_SO2, num_H2SO4);
}
