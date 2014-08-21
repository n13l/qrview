#include <compat/windows.h>
#include <sys/compiler.h>
#include <sys/missing.h>

#define MSG_FLAGS (FORMAT_MESSAGE_ALLOCATE_BUFFER | \
                   FORMAT_MESSAGE_FROM_SYSTEM | \
                   FORMAT_MESSAGE_IGNORE_INSERTS)

#define LANG_ID (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))

void
__compat_perror(void)
{
	LPTSTR errb = NULL;
	DWORD dw = GetLastError();

	if (FormatMessage(MSG_FLAGS, NULL, dw, LANG_ID, (LPTSTR)&errb, 0, NULL ))
		goto failed;

	sys_dbg("sys: %d %d %s", (int)dw, (int)errb, (char *)errb);

failed:
	if (errb)
		LocalFree(errb);
}
