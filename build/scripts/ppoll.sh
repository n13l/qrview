#!/bin/sh
# Copyright (C) 2012, Daniel Kubec <niel@rtfm.cz>

compiler="$*"
cat << "END" | $compiler -x c - -c -o /dev/null >/dev/null 2>&1 && echo "y"
#define _GNU_SOURCE
#include <stdlib.h>
#include <poll.h>
int main(void)
{
	return ppoll(NULL, 0, NULL, NULL);
}
END
