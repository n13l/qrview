#!/bin/sh
# Copyright (C) 2012, Daniel Kubec <niel@rtfm.cz>

cat << "END" | $@ -x c - -c -o /dev/null >/dev/null 2>&1 && echo "y"
#include <stdlib.h>
#include <sys/poll.h>
int main(void)
{
	return poll(NULL,0,0);
}
END
