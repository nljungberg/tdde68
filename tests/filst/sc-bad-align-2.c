#include "tests/lib.h"
#include "tests/main.h"

#include <stdio.h>
#include <syscall-nr.h>

/**
 * Thanks to Herman Appelgren for suggesting this test case.
 */

/**
 * From threads/vaddr.h:
 */
#define BITMASK(SHIFT, CNT) (((1ul << (CNT)) - 1) << (SHIFT))

#define PGSHIFT 0								  /* Index of first offset bit. */
#define PGBITS	 12							  /* Number of offset bits. */
#define PGSIZE	 (1 << PGBITS)				  /* Bytes in a page. */
#define PGMASK	 BITMASK(PGSHIFT, PGBITS) /* Page offset bits (0:12). */

static inline void* pg_round_up(const void* va)
{
	return (void*) (((uintptr_t) va + PGSIZE - 1) & ~PGMASK);
}

/**
 * External symbol which address is the first address after all data in the BSS segment.
 */
extern int _end_bss;

void test_main(void)
{
	// Get the address of the first unmapped page in the process.
	unsigned page = (unsigned) pg_round_up(&_end_bss);

	// Reserve space for the syscall number and two out of four bytes for the syscall
	// argument.
	unsigned base = page - 4 - 2;

	// Call a syscall.
	asm volatile(
		 "movl %%esp, %%edi;"			// Save the stack pointer in case we survive.
		 "movl %0, %%esp;"				// Set stack pointer.
		 "movl %[number], (%%esp);"	// Set syscall number.
		 "movw $0, 4(%%esp);"			// Set the first byte of the syscall number to zero.
		 "int $0x30;"						// Trigger syscall.
		 "movl %%edi, %%esp;"			// Restore the old stack if we survivied.
		 :
		 : "r"(base), [number] "i"(SYS_EXIT)
		 : "%eax", "%edi");

	fail("should have died.");
}
