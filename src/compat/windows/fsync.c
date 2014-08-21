#include <compat/windows.h>
#include <sys/compiler.h>
#include <sys/missing.h>

int
fsync(int fd)
{
	return (FlushFileBuffers ((HANDLE) _get_osfhandle (fd))) ? 0 : -1;
}
