#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer_consumer.h"

void main (int argc, char *argv[])
{
  //missile_code *mc;        // Used to access missile codes in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  circ_buffer *buffer2;     // Used to get address of shared memory page
  lock_t buff_lock;			    // Lock for the buffer
  int i;                // index for str[]

  char str[] = "Hello World";
  int length = dstrlen(str);

  cond_t c_empty;
  cond_t c_full;

  if (argc != 6) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <handle_to_lock>\n");
    Exit();
  }

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  buff_lock = dstrtol(argv[3], NULL, 10);
  c_full = dstrtol(argv[4], NULL, 10);
  c_empty = dstrtol(argv[5], NULL, 10);


  // Map shared memory page into this process's memory space
  if ((buffer2 = (circ_buffer*) shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  for(i = 0; i < length; i++) {
	while(lock_acquire(buff_lock) != SYNC_SUCCESS) {
      		Printf("Get buffer lock failed.\n");
    	}
	while(buffer2->head == buffer2->tail) {
		cond_wait(c_empty);
	}
	Printf("Consumer %d removed: %c\n", getpid(), buffer2->buffer[buffer2->tail]);
	buffer2->tail = (buffer2->tail + 1) % BUFFERSIZE;
	cond_signal(c_full);
	while(lock_release(buff_lock) != SYNC_SUCCESS) {
		Printf("Release lock failed.\n");
	}
   }

  // Now print a message to show that everything worked
  // Printf("Producer: This is one of the %d instances you created.  ", buffer2->numprocs);
  // Printf("Producer: Missile code is: %c\n", buffer2->really_important_char);
  // Printf("Producer: My PID is %d\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  Printf("Consumer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
