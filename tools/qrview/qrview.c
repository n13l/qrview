#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <qrencode.h>
#include "private.h"

int
main(int argc, char *argv[]) 
{
	const char *uri = argv[1];

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
