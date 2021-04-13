#ifndef	_memory_constants_h_
#define	_memory_constants_h_

//------------------------------------------------
// #define's that you are given:
//------------------------------------------------

// We can read this address in I/O space to figure out how much memory
// is available on the system.
#define	DLX_MEMSIZE_ADDRESS	0xffff0000

// Return values for success and failure of functions
#define MEM_SUCCESS 1
#define MEM_FAIL -1

//--------------------------------------------------------
// Put your constant definitions related to memory here.
// Be sure to prepend any constant names with "MEM_" so 
// that the grader knows they are defined in this file.

#define MEM_L1FIELD_FIRST_BITNUM 12         // Bit position of LSB of level 1 page number bit in a virtual addr
#define MEM_MAX_VIRTUAL_ADDRESS 0xFFFFF     // 1024 KB = 1046576 bytes = 0xfffff, 2^20
#define MEM_PTE_READONLY 0x4
#define MEM_PTE_DIRTY 0x2
#define MEM_PTE_VALID 0x1
#define MEM_L1TABLE_SIZE (MEM_MAX_VIRTUAL_ADDRESS + 1) >> MEM_L1FIELD_FIRST_BITNUM      // Size of array of level 1 pagetable
#define MEM_PAGESIZE (0x1 << MEM_L1FIELD_FIRST_BITNUM)          // Size of a page 

#define MEM_ADDRESS_OFFSET_MASK (MEM_PAGESIZE - 1)  //page offset mask 0xFFF
#define MEM_PTE2PAGE_MASK (~(MEM_PTE_READONLY | MEM_PTE_DIRTY | MEM_PTE_VALID))     // Mask to convert from PTE to page address

#define MEM_MAX_PHYS_MEM (1<<21) //2MB = 2097152 bytes = 0x1FFFF, 2^21
#define MEM_MAX_PAGES (MEM_MAX_PHYS_MEM / MEM_PAGESIZE)     //number of pages in physcial memory

// num_pages = size_of_memory / size_of_one_page
#define NUM_PAGES MEM_MAX_PAGES/32          // number of pages / 32(bits) so that each bit can represent a page

//--------------------------------------------------------

#endif	// _memory_constants_h_
