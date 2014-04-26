#!/bin/sh
# Copyright (C) 2012, Daniel Kubec <niel@rtfm.cz>

cat << "END" | $@ -x c - -c -o /dev/null >/dev/null 2>&1 && echo "y"
#include <sys/mmap.h>
#include <sys/stat.h>
int main(void)
{
	mmap(NULL);
	return 0;
}
END
