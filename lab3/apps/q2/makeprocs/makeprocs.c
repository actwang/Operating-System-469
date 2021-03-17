//#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "reactions.h"

void main (int argc, char *argv[])
{
  mole_struct* molecules;
  mbox_t S2_handle, S_handle, CO_handle, C2_handle, O2_handle, SO4_handle;
  int num_procs = 5;
  // uint32 h_mem;                   // Used to hold handle to shared memory page
  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  //char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes
  char num_S2_str[10]; char num_CO_str[10];
  char num_react1_str[10], num_react2_str[10], num_react3_str[10];
  int num_S2, num_S, num_O2, num_SO4, num_CO, num_C2, i; 
  int num_react1, num_react2, num_react3;
  /*
    1) S2 -> S + S              2) 4 CO -> 2 O2 + 2 C2              3) S + 2 O2 -> SO4
  */

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of S2 molescules> <number of CO molecules>\n");
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  num_S2 = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  num_CO = dstrtol(argv[2], NULL, 10); // the "10" means base 10
  num_S = 2 * num_S2;
  num_O2 = num_CO / 2;
  num_C2 = num_CO / 2;
  Printf("Creating %d S2s and %d COs.\n", num_S2 , num_CO);

  // creat mailbox handle for use
  MboxModuleInit();
  S2_handle = mbox_create(); S_handle = mbox_create(); CO_handle = mbox_create(); C2_handle = mbox_create(); 
  O2_handle = mbox_create(); SO4_handle = mbox_create();

  // What to send? Do we still need struct? 

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  
  if ((s_procs_completed = sem_create(-(num_procs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  // Compute the number of each reactions that can happen
  num_react1 = num_S2;
  num_react2 = num_CO / 4;
  num_react3 = num_S < (num_CO/4)? num_S: (num_CO/4);

  ditoa(s_procs_completed, s_procs_completed_str);
  // ditoa(h_mem, h_mem_str);
  ditoa(num_S2, num_S2_str);
  ditoa(num_CO, num_CO_str);
  ditoa(num_react1, num_react1_str);
  ditoa(num_react2, num_react2_str);
  ditoa(num_react3, num_react3_str);

  for (i = 0; i < num_S2; i++){
    process_create(FILE_CO, CO_handle, s_procs_completed_str, num_CO_str, NULL);
  }
  for (i = 0; i< num_CO; i++){
    process_create(FILE_S2, S2_handle, s_procs_completed_str, num_S2_str, NULL);
  }
  for (i = 0;i < num_react1; i++){
    process_create(FILE_REACT1, S2_handle, S_handle, s_procs_completed_str, num_react1_str, NULL);
  }
  for (i = 0;i < num_react2; i++){
    process_create(FILE_REACT2, CO_handle, O2_handle, C2_handle, s_procs_completed_str, num_react2_str, NULL);
  }
  for (i = 0;i < num_react3; i++){
    process_create(FILE_REACT3, S_handle, O2_handle, SO4_handle, s_procs_completed_str, num_react3_str, NULL);
  }

  // And finally, wait until all five processes have finished. (s_procs_completed back at 1 from -4)
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }
  Printf("%d S2's left over. %d S's left over. %d CO's left over. %d C2's left over. %d O2's left over. %d SO4's created.\n",
  num_S2, num_S, num_CO, num_C2, num_O2, num_SO4);
}