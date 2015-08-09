#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

char *
__compat_get_home_dir(char *str, int size)
{
	char *home = getenv("HOME");
	snprintf(str, size - 1, "%s", home);
	return str;
}

