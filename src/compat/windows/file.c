#include <compat/windows.h>
#include <sys/compiler.h>
#include <sys/missing.h>

char *
__compat_get_unix_path(char *f)
{
	for (char *p = f; *p; p++)
		if (*p == '\\') *p = '/';
			return f;
}

char *
__compat_get_home_dir(char *str, int size)
{
	const char *home = getenv("LOCALAPPDATA");
	if (!home)
		home = getenv("USERPROFILE");
	if (!home)
		home = getenv("ALLUSERSPROFILE");
	if (!home)
		return NULL;

	snprintf(str, size, "%s", home);
	__compat_get_unix_path((char *)str);
	return str;
}
