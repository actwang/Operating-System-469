#include "usertraps.h"
#include "misc.h"


void main (int argc, char argv[])
{
  int i,j;                               // Loop index variable
  int* test1, *test2, *test3;
  int r1, r2, r3;

  Printf("test heap (%d): Creating heap tests\n", getpid());

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("Malloc with memsize=80\n");
  test1 = malloc(80);
  if (test1 == NULL) {
    Printf("Fail to malloc for test1 in test_heap\n");
  }
  Printf("test1 allocated\n");

  r1 = mfree((void*)test1);
  Printf("test1 freed (%d bytes freed)\n", r1);
  Printf("-------------------------------------------------------------------------------------\n");

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("Malloc with memsize=256\n");
  test2 = malloc(256);
  if (test2 == NULL) {
    Printf("Fail to malloc for test2 in test_heap\n");
  }
  Printf("test2 allocated\n");
  r2 = mfree((void*)test2);
  Printf("test2 freed (%d bytes freed)\n", r2);
  Printf("-------------------------------------------------------------------------------------\n");

  Printf("-------------------------------------------------------------------------------------\n");
  Printf("Malloc with memsize=\n");
  test3 = malloc(2048);
  if (test3 == NULL) {
    Printf("Fail to malloc for test3 in test_heap\n");
  }
  Printf("test3 allocated\n");
  r3 = mfree((void*)test3);
  Printf("test3 freed (%d bytes freed)\n", r3);
  Printf("-------------------------------------------------------------------------------------\n");

  Printf("DONE! test_heap (%d): All heap tests completed, exiting...\n", getpid());
}