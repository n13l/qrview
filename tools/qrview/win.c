#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "private.h"

gboolean supports_alpha = FALSE;
static cairo_surface_t *cimage = NULL;
static uint8_t window_size;

static gboolean
update(gpointer data)
{
	static int count = 15;
	if (!--count)
		gtk_main_quit();

	return TRUE;
}

static void 
screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
	GdkScreen *screen = gtk_widget_get_screen (widget);
	GdkVisual *visual = gdk_screen_get_rgba_visual (screen);

	if (!visual) {
		printf("Your screen does not support alpha channels!\n");
		supports_alpha = FALSE;
		exit(1);
	} else {
		printf("Your screen supports alpha channels!\n");
		gtk_widget_set_visual (GTK_WIDGET (widget), visual);
		supports_alpha = TRUE;
	}

	/* Now we have a colormap appropriate for the screen, use it */
	gtk_widget_set_visual(widget, visual);
}

static gboolean 
expose(GtkWidget *w, cairo_t *ctx, gpointer p)
{
	GdkWindow *gwin = gtk_widget_get_window(w);

	if (supports_alpha)
		cairo_set_source_rgba (ctx, 1.0, 1.0, 1.0, 1); /* transparent */
	else
		cairo_set_source_rgb (ctx, 1.0, 1.0, 1.0); /* opaque white */

	/* draw the background */
	cairo_set_operator (ctx, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_surface(ctx, cimage, 20, 20);
	cairo_paint(ctx);

	return FALSE;
}

static gboolean
on_draw(GtkWidget *w, cairo_t *ctx, gpointer p)
{
	GdkWindow *gwin = gtk_widget_get_window(w);

	cairo_set_operator (ctx, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_surface(ctx, cimage, 20, 20);
	cairo_paint(ctx);

	return TRUE;
}

static void
get_screen_size(GtkWidget *w, int *width, int *height)
{
	GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(w));
	*width = gdk_screen_get_width(screen);
	*height = gdk_screen_get_height(screen);
}

static void
window_set_size(GtkWidget *win)
{
	int w, h;
	get_screen_size(win, &w, &h);

	window_size = w / 4;

	gtk_widget_set_size_request(GTK_WIDGET(win), w / 4, w / 4);
}

static void
window_setup(GtkWidget *w)
{
	gtk_window_set_resizable(GTK_WINDOW(w), FALSE);
	gtk_window_set_decorated(GTK_WINDOW(w), FALSE);
	gtk_window_set_deletable(GTK_WINDOW(w), FALSE);
	gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER);

	gtk_widget_set_app_paintable(GTK_WIDGET(w), TRUE);
	//gtk_widget_set_double_buffered(GTK_WIDGET(w), FALSE);

	g_signal_connect (w, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (w, "draw", G_CALLBACK (expose), NULL);
	g_signal_connect(w, "screen-changed", G_CALLBACK(screen_changed), NULL);
}

struct surface {
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint8_t *pixels;
	int alpha;
};

static void
surface_write_row(uint8_t **ptr, uint8_t *from, int size, int stride)
{
	for (int i = 0; i < size; i++) {
		memcpy(*ptr, from, stride);
		*ptr += stride;
	}
}

static cairo_surface_t *
qrcode_cairo_surface(QRcode *qr, int margin, int size)
{
	cairo_surface_t *cimage;
	margin = 0;
	size = 10;

	int width = (qr->width + margin * 2) * size + (size * 2);
	int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);

	uint8_t *pixels = malloc(stride * width * 10);
	uint8_t *row = malloc(stride);
	uint8_t *pix = pixels;
	uint8_t *src = qr->data;
	uint8_t *dst = row;

	/* top margin */
	memset(row, 0xff, stride);
	surface_write_row(&pix, row, size, stride);

	for (int i = 0; i < qr->width; i++) {
		dst = row;
		memset(row, 0xff, stride);
		dst += size * 4;
		for (int y = 0; y < qr->width; y++) {
			for (int yy = 0; yy < size; yy++) {
				*dst++ = *src & 1 ? 0 : 255;
				*dst++ = *src & 1 ? 0 : 255;
				*dst++ = *src & 1 ? 0 : 255;
				*dst++ = 128; //*src & 1 ? 255 : 0;
			}
			src++;
		}

		surface_write_row(&pix, row, size, stride);
	}

	memset(row, 0xff, stride);
	surface_write_row(&pix, row, size, stride);

        cimage = cairo_image_surface_create_for_data(pixels, CAIRO_FORMAT_ARGB32,
	                                             width, width, stride);

	return cimage;
}

int
main_window(int argc, char *argv[]) 
{
	gtk_init (&argc, &argv);

	GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	window_setup(win);
	window_set_size(win);
	cimage = qrcode_cairo_surface(qrcode, 0, 10);

	g_timeout_add_seconds (1, update, NULL);

	screen_changed(win, NULL, NULL);

	gtk_widget_show (win);
	gtk_widget_queue_draw(win);
	gtk_main ();

	return EXIT_SUCCESS;
}
