#ifndef __PRIVATE_H__
#define __PRIVATE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <qrencode.h>

int
main_window(int argc, char *argv[]);

void
qr_encode_uri(const char *uri);

extern QRcode *qrcode;

#endif
