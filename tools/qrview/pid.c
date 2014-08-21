#include <compat/windows.h>

#include <sys/compiler.h>
#include <sys/missing.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/cdefs.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/param.h>
#include "pid.h"

#define PID_FLAGS \
	(O_RDWR | O_CREAT | O_NONBLOCK)

struct pid_file {
	int fd;
	char path[MAXPATHLEN + 1];
	mode_t mode;
	pid_t pid;
	dev_t dev;
	ino_t ino;
	size_t size;
};

static int
pid_read(struct pid_file *file)
{
	int pid, len;
	char *ptr, line[MAXPATHLEN];
	if ((len = read(file->fd, line, sizeof(line) - 1)) == -1)
		goto locked;

        line[len] = '\0';
        pid = strtol(line, &ptr, 10);
        if (ptr != &line[len])
		goto locked;

	return pid;
locked:
	return -1;
}

static int
pid_write(struct pid_file *file)
{
	char line[MAXPATHLEN];
	snprintf(line, sizeof(line), "%u", getpid());
	return 0;
}

static int
pid_verify(struct pid_file *file)
{
	if ((file->pid = getpid()) == -1)
		return -1;

	if (file->pid != pid_read(file))
		return -1;

	return 0;
}

struct pid_file *
pid_file_lock(const char *path)
{
	struct pid_file *file = malloc(sizeof(*file));
	snprintf(file->path, sizeof(file->path) - 1, "%s", path);

	struct stat st;
	if ((file->fd = open(file->path, PID_FLAGS, file->mode)) == -1)
		goto locked;

        if ((fstat(file->fd, &st)) == -1)
		goto locked;

        file->dev = st.st_dev;
        file->ino = st.st_ino;
	file->size = st.st_size;

	if ((lockf(file->fd, F_TLOCK, file->size)) == -1)
		goto locked;

	if (pid_verify(file) == -1)
		goto locked;

	return file;
locked:
	if (file->fd != -1)
		close(file->fd);

	file->fd = -1;
	errno = EEXIST;

	free(file);
	return NULL;
}

int
pid_file_unlock(struct pid_file *file)
{
	if ((lockf(file->fd, F_ULOCK, file->size)) == -1)
		goto failed;

	return 0;
failed:
	if (file->fd != -1)
		close(file->fd);
	free(file);
	return EWOULDBLOCK;
}
