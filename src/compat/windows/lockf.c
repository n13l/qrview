
#include <compat/windows.h>
#include <sys/compiler.h>
#include <sys/missing.h>

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/locking.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int
lockf(int fd, int cmd, off_t len)
{
	int stat;
	switch (cmd) {
	case F_LOCK:
		do     stat = _locking(fd, _LK_LOCK, len);
		while (stat == -1 && errno == EDEADLOCK);
		return stat;
	case F_TLOCK:
		return _locking(fd, _LK_NBLCK, len);
	case F_ULOCK:
		return _locking(fd, _LK_UNLCK, len);
	default:
		errno = EINVAL;
		return -1;
	}
}
