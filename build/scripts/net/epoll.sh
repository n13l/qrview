#!/bin/sh
# Copyright (C) 2012, Daniel Kubec <niel@rtfm.cz>

cat << "END" | $@ -x c - -c -o /dev/null >/dev/null 2>&1 && echo "y"
#include <sys/epoll.h>
int main(void)
{
	epoll_create(255);
	return 0;
}
END
