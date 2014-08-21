#ifndef __COMPAT_FILE_PID_H__
#define __COMPAT_FILE_PID_H__

struct pid_file;

struct pid_file *
pid_file_lock(const char *file);

int
pid_file_unlock(struct pid_file *file);

#endif
