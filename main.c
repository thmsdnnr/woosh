#include <stdio.h>
#include "woosh.h"

// main is the entrypoint to the woosh shell.
int main(int argc, char **argv)
{
	FILE *in_file = get_input_file(argc, argv);
	int retval = 0;
	while ((retval = cycle(in_file)) == 0)
		;
	fclose(in_file);
	return retval == STATUS_EXIT ? 0 : retval;
}
