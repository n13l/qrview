#!/bin/sh
# Copyright (C) 2012, Daniel Kubec <niel@rtfm.cz>

cat << "END" | $@ -x c - -c -o /dev/null >/dev/null 2>&1 && echo "y"
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <aio.h>
int main(void)
{
	const struct aioinit init;
	aio_init(&init);
	return 0;
}
END
