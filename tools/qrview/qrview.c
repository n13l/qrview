/*
 * $qrview.c                                          Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 */

#include <sys/missing.h>
#include <sys/compiler.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <qrencode.h>
#include "private.h"
#include "pid.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

char home[MAXPATHLEN];

const char *uri;
static const char *name;

uint32_t qr_size = 10;
uint32_t fade = 1;
uint32_t fade_interval = 10;
int timeout = 30;
int alpha_max = 255;

enum opt_long_e {
	OPT_HELP     = 'h',
	OPT_NAME     = 'n',
	OPT_VERSION  = 'V',
	OPT_TIMEOUT  = 't',
	OPT_QR_SIZE  = 's',
	OPT_ALPHA    = 'a',
};

static int opt;
static const char *opt_cmd = "hVt:s:a:n:";
static struct option opt_long[] = {
	{"help"    ,0, 0, OPT_HELP    },
	{"name"    ,1, 0, OPT_NAME    },
	{"timeout" ,0, 0, OPT_TIMEOUT },
	{"version" ,0, 0, OPT_VERSION },
	{"qr-size" ,1, 0, OPT_QR_SIZE },
	{"alpha"   ,1, 0, OPT_ALPHA   },
	{NULL, 0, 0, 0}
};

struct app {
	const char *uri;
	int verbose;
#define MAX_CMDS 16
	unsigned int cmd[MAX_CMDS];
	const char *arg[MAX_CMDS];
	unsigned int num_cmds;
} app_data;

struct app *app = &app_data;

void
usage(int code)
{
	printf("Usage: qrview <commands-and-parameters> <uri>\n\n");
	printf("\t--help,            -h\t help\n");
	printf("\t--name,            -n\t instance name\n");
	printf("\t--version,         -V\t package version\n");
	printf("\t--timeout,         -t\t exit after number of seconds\n");
	printf("\t--alpha,           -a\t final alpha channel value [0-255]\n");
	printf("\t--qr-size=[pixels] -s\t qrcode module size in pixels (default: 10)\n");
	printf("\n");

	exit(EXIT_SUCCESS);
}

static void
version(void)
{
	printf("qrview (QRVIEW) %s %04d%02d%02dT%02d:%02d:%02d\n",
	       STR_VALUE(PACKAGE_VERSION_STRING),
	       BUILD_YEAR, BUILD_MONTH, BUILD_DAY, BUILD_HOUR, BUILD_MIN, BUILD_SEC);

	printf("Copyright (C) 2014  Daniel Kubec <niel@rtfm.cz> .\n"
	       "This is free software; see the source for copying conditions.  There is NO\n"
	       "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

static void
nothing(void)
{
	printf("Nothing to do.\n");
	exit(EXIT_SUCCESS);
}

void
giveup(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, ": ");
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}

static struct pid_file *pid_file;

static int
lock_instance(void)
{
	char path[MAXPATHLEN];

	if (!name)
		return 0;

	__compat_get_home_dir(home, sizeof(home) - 1);
	snprintf(path, sizeof(path) - 1, "%s/.qrview-%s.pid", home, name);

	if (!(pid_file = pid_file_lock(path))) {
		die("cant lock pid file=%s\n", path);
		return -1;
	}

	return 0;
}

static int
unlock_instance(void)
{
	if (!pid_file)
		return 1;
	return pid_file_unlock(pid_file);
}

int
main(int argc, char *argv[]) 
{
	QRcode *qr = NULL;
#ifdef CONFIG_WINDOWS
	win32_io_init();
#endif

	while ((opt = getopt_long(argc, argv, opt_cmd, opt_long, NULL)) >= 0) {
		switch (opt) {
		case OPT_HELP:
		case '?':
			usage(0);
			break;
                case OPT_NAME:
                        name = strdup(optarg);
			break;
		case OPT_VERSION:
			version();
			exit(EXIT_SUCCESS);
		break;
		case OPT_QR_SIZE:
			qr_size = atoi(optarg);
		break;
		case OPT_TIMEOUT:
			timeout = atoi(optarg);
			break;
		case OPT_ALPHA:
			alpha_max = atoi(optarg);
			if (alpha_max > 255)
				alpha_max = 255;
			if (alpha_max < 0)
				alpha_max = 0;
		break;
		default:
		if (app->num_cmds >= MAX_CMDS)
			giveup("Too many commands specified");
			app->cmd[app->num_cmds] = opt;
			app->arg[app->num_cmds] = optarg;
			app->num_cmds++;
		}
	}

	if (optind + 1 != argc)
		usage(2);

	if (!(uri = argv[argc - 1]))
		usage(2);

	if (lock_instance())
		goto exit;

	qr = QRcode_encodeString8bit(uri , QR_MODE_8, QR_ECLEVEL_H);
	if (qr == NULL)
		goto exit;

        struct surface surface = (struct surface) {
		.width = qr->width,
		.data  = qr->data
	};

	main_window (argc, argv, &surface);

exit:
	if (qr)
		QRcode_free(qr);
	unlock_instance();
	return EXIT_SUCCESS;
}
