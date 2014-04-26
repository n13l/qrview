#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <png.h>
#include <qrencode.h>
#include <gtk/gtk.h>
#include "private.h"

static int margin = 1;
static int size = 10;

QRcode *qrcode;

struct surface {
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint8_t *pixels;
	int alpha;
};

static cairo_surface_t *
pixbuf_cairo_surface(GdkPixbuf *pix, int m)
{
	struct surface surface = (struct surface) {
		.width  = gdk_pixbuf_get_width(pix),
		.height = gdk_pixbuf_get_height(pix),
		.stride = gdk_pixbuf_get_rowstride(pix),
		.pixels = gdk_pixbuf_get_pixels(pix),
		.alpha  = gdk_pixbuf_get_has_alpha(pix)
	};
/*
	printf("pixbuf: w=%d, h=%d, s=%d, addr=%p alpha: %d\n", 
	       w, h, s, ptr, (int)alpha);
*/
	cairo_surface_t *src = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 
			surface.width, surface.height);

	cairo_surface_t *dst = cairo_image_surface_create_for_data(surface.pixels,
		CAIRO_FORMAT_ARGB32, surface.width, surface.height, surface.stride);
	return dst;
}

void
qr_encode_uri(const char *uri)
{
	char *outfile = "/tmp/qrencode.png";
	FILE *fp = fopen(outfile, "a+");

	unsigned char *row, *p, *q;
	int x, y, xx, yy, bit;
	int realwidth;

	if (!(qrcode = QRcode_encodeString8bit(uri , QR_MODE_8, QR_ECLEVEL_H)))
		exit(EXIT_FAILURE);

	png_structp png_ptr;
	png_infop info_ptr;

	realwidth = (qrcode->width + margin * 2) * size;
	row = (unsigned char *)malloc((realwidth + 7) / 8);

	png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
	                                   NULL, NULL, NULL);

	info_ptr = png_create_info_struct(png_ptr);
	//png_destroy_write_struct(&png_ptr, &info_ptr);

	fseek(fp, 0, SEEK_SET);
	ftruncate(fileno(fp), 0);

	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, realwidth, realwidth, 1,
	             PNG_COLOR_TYPE_GRAY, 
		     PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_DEFAULT,
		     PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	/* top margin */
	memset(row, 0xff, (realwidth + 7) / 8);
	for (y = 0; y < margin * size; y++) {
		png_write_row(png_ptr, row);
	}

	/* data */
	p = qrcode->data;
	for (y = 0; y < qrcode->width; y++) {
		bit = 7;
		memset(row, 0xff, (realwidth + 7) / 8);
		q = row;
		q += margin * size / 8;
		bit = 7 - (margin * size % 8);
		for (x = 0; x < qrcode->width; x++) {
			for (xx = 0; xx < size; xx++) {
				*q ^= (*p & 1) << bit;
				bit--;
				if (bit < 0) {
					q++;
					bit = 7;
				}
			}
			p++;
		}

		for (yy = 0; yy < size; yy++) {
			png_write_row(png_ptr, row);
		}
	}

	/* bottom margin */
	memset(row, 0xff, (realwidth + 7) / 8);
	for (y = 0; y < margin * size; y++) {
		png_write_row(png_ptr, row);
	}
				        
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

//	QRcode_free(qrcode);
}
