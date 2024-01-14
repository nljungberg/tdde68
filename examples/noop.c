#include <stdio.h>
#include <syscall.h>

/*  A program that does nothing, a classic no-op.
 *
 * Compile it, and run it from userprog/ with the following command:
	 pintos -k --filesys-size=2 -p ../examples/noop -a lab1 -- -f -q run 'lab1 arg1 arg2 arg3'
*/

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	return EXIT_SUCCESS;
}
