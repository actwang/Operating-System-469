#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int fork_result, var = 1183;

  if (argc != 1) {
    Exit();
  }

  //Print the valid PTE of both parent and child
  fork_result = fork();
  Printf("Before TRAP_ROP_ACCESS PTE:\n");
  
  //Generate TRAP_ROP_ACCESS and print the PTE of parent and child
  if (fork_result == 0) {
    // Child process
    Printf("hello_fork from CHILD (%d): Hello fork!\n", getpid());
    Printf("hello fork from CHILD (%d), var was %d\n",getpid(),var);
    var = 2130;
    Printf("hello fork from CHILD (%d), var after is %d\n",getpid(),var);
  } else {
    //parent process
    Printf("hello_fork from PARENT (%d): Hello fork!\n", getpid());
    Printf("hello fork from PARENT (%d), var was %d\n",getpid(),var);
    var = 808;
    Printf("hello fork from PARENT (%d), var after is %d\n",getpid(),var);
  }

  Printf("hello_fork (%d): Done!\n", getpid());
}
