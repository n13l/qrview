#ifndef __PRIVATE_H__
#define __PRIVATE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixdata.h>

#include "autoconf.h"

struct surface {
	uint8_t width;
	uint8_t *data;
	uint8_t alpha;
};

int
main_window(int argc, char *argv[], struct surface *surface);

gboolean
on_draw(GtkWidget *w, cairo_t *ctx, gpointer p);

#ifdef CONFIG_WINDOWS
void gdi_init(GtkWidget *w);
gboolean 
gdi_on_draw(GtkWidget *w, cairo_t *ctx, gpointer p);
#endif

extern unsigned int alpha;

extern const GdkPixdata icon;
#endif
