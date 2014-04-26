#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "private.h"

int
main(int argc, char *argv[]) 
{
	qr_encode_uri(argv[1]);
	//exit(0);
	main_window (argc, argv);

	return EXIT_SUCCESS;
}
