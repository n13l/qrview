#ifndef __COMPAT_PORTABLE_H__
#define __COMPAT_PORTABLE_H__

#ifdef CONFIG_WINDOWS
#define NTDDI_VERSION           NTDDI_WIN2KSP4
#define _WIN32_WINNT            0x500
#define _WIN32_IE_              _WIN32_IE_WIN2KSP4
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <sys/decls.h>

#include "../obj/autoconf.h"

#ifndef CONFIG_WINDOWS
#include <sys/mman.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#endif

#define CPU_AFFINITY_DOMAIN   1
#define CPU_AFFINITY_ID       2
#define CPU_AFFINITY_INDEX    3

int cpu_set_affinity(int id, int mode, int cpu);

int sys_uri_scheme_register(const char *id, const char *handler, const char *about);
int sys_uri_scheme_unregister(const char *id);
void sys_uri_scheme_dump(const char *id);

const char *sys_get_progname(void);

int
open_uri(const char *uri);

unsigned long find_window_pid(char *c, char *window);
unsigned long find_proc_pid(const char *name);
unsigned long find_proc_tid(unsigned long pid);
unsigned long find_module(unsigned long pid, const char *name);

#ifdef CONFIG_LINUX
#include <compat/linux/sys.h>
#endif

#ifdef CONFIG_HPUX
#include <compat/hpux/sys.h>
#endif

#ifdef CONFIG_DARWIN
#include <compat/darwin/sys.h>
#include <compat/darwin/dlfcn-add.h>
#endif

#ifdef CONFIG_WINDOWS
#include <compat/windows/api.h>
#endif

#endif
