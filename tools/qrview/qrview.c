/*
 * $qrcode.c                                          Daniel Kubec <niel@rtfm.cz>
 *
 * This software may be freely distributed and used according to the terms
 * of the GNU Lesser General Public License.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <qrencode.h>
#include "private.h"

const char *uri;

enum opt_long_e {
	OPT_HELP         = 'h',
	OPT_VERSION      = 'V',
};

static int opt;
static const char *opt_cmd = "hv";
static struct option opt_long[] = {
	{"help"         ,0, 0, OPT_HELP        },
	{"version"      ,0, 0, OPT_VERSION     },
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

	printf("\t--version,      -V\t package version\n");
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
	exit(1);
}

int
main(int argc, char *argv[]) 
{
	const char *uri = argv[1];

	while ((opt = getopt_long(argc, argv, opt_cmd, opt_long, NULL)) >= 0) {
		switch (opt) {
		case OPT_HELP:
		case '?':
			usage(0);
		break;
		case OPT_VERSION:
			version();
			exit(EXIT_SUCCESS);
		break;
		default:
		if (app->num_cmds >= MAX_CMDS)
			giveup("Too many commands specified");
			app->cmd[app->num_cmds] = opt;
			app->arg[app->num_cmds] = optarg;
			app->num_cmds++;
		}
	}

	if (optind < argc)
		usage(2);

	if (!app->num_cmds)
		nothing();

	QRcode *qr = QRcode_encodeString8bit(uri , QR_MODE_8, QR_ECLEVEL_H);
	if (qr == NULL)
		goto exit;

        struct surface surface = (struct surface) {
		.width = qr->width,
		.data  = qr->data
	};

	main_window (argc, argv, &surface);

exit:
	QRcode_free(qr);
	return EXIT_SUCCESS;
}
