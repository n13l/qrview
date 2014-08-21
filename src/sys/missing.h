/*
 * $id: sys/missing.h                                Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 */

#ifndef __PORTABLE_SYS_MISSING_H__
#define __PORTABLE_SYS_MISSING_H__

#ifdef CONFIG_WINDOWS
#include <compat/windows.h>
#endif

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>

#include "autoconf.h"

__BEGIN_DECLS

#ifndef O_NOATIME
#define O_NOATIME 0
#endif

#ifndef MAXPATHLEN
#define MAXXPATHLEN 1024
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK 0x0
#endif

#ifndef assert
#define assert(x)
#endif

#ifndef sys_dbg_label
#define sys_dbg_label "init"
#endif
#define sys_dbg(fmt, ...) \
	printf("%5.5s: " fmt "\n", sys_dbg_label , ## __VA_ARGS__);

char *
__compat_get_unix_path(char *f);

char *
__compat_get_home_dir(char *str, int size);

void
die(const char *fmt, ...);

void *
xmalloc(size_t size);

void *
xmalloc_zero(size_t size);

void *
xrealloc(void *addr, size_t size);

void
xfree(void *ptr);

timestamp_t
get_timestamp(void);

unsigned int
get_timer(timestamp_t *timer);

__END_DECLS

#endif/*__MEM_MISSING_H__*/
