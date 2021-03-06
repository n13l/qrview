#include <windows.h>
#include <stdint.h>
#include <sys/time.h>

/* FILETIME of Jan 1 1970 00:00:00. */
static const uint64_t epoch = (uint64_t)(116444736000000000);

/*
* timezone information is stored outside the kernel so tzp isn't used anymore.
*
* Note: this function is not for Win32 high precision timing purpose. See
* elapsed_time().
*/

int
gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	FILETIME    file_time;
	SYSTEMTIME  system_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long) (system_time.wMilliseconds * 1000);

	return 0;
}
