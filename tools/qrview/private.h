#ifndef __PRIVATE_H__
#define __PRIVATE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

struct surface {
	uint8_t width;
	uint8_t *data;
	uint8_t alpha;
};

int
main_window(int argc, char *argv[], struct surface *surface);

#endif
