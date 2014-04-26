#!/bin/sh
# Copyright (C) 2012, Daniel Kubec <niel@rtfm.cz>

cat << "END" | $@ -x c - -c -o /dev/null >/dev/null 2>&1 && echo "y"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
int main(void)
{
	return 0;
}
END
