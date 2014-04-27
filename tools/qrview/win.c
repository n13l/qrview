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
	GtkWidget *widget = (GtkWidget *)data;

	static int count = 15;
	/*
	if (!--count)
		gtk_main_quit();
*/
	gtk_widget_queue_draw(widget);

	return TRUE;
}

static void 
screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
	GdkScreen *screen = gtk_widget_get_screen (widget);
	GdkVisual *visual = gdk_screen_get_rgba_visual (screen);

	if (!visual) {
		fprintf(stderr, "Your screen does not support alpha channels!\n");
		supports_alpha = FALSE;
	} else {
		gtk_widget_set_visual (GTK_WIDGET (widget), visual);
		supports_alpha = TRUE;
	}
}

gboolean 
on_draw(GtkWidget *w, cairo_t *ctx, gpointer p)
{
	GdkWindow *gwin = gtk_widget_get_window(w);

	int width, height;
	gtk_window_get_size(GTK_WINDOW(w), &width, &height);

	cairo_set_source_rgb (ctx, 0.0, 0.0, 0.0);
	cairo_rectangle(ctx, 30, 30, width - 60 , height - 60);
	cairo_set_line_width(ctx, 30);
	cairo_set_line_join(ctx, CAIRO_LINE_JOIN_ROUND); 
	cairo_stroke(ctx); 

	if (supports_alpha)
		cairo_set_source_rgba (ctx, 1.0, 1.0, 1.0, 1); 
	else
		cairo_set_source_rgb (ctx, 1.0, 1.0, 1.0);

	/* draw the background */
	//cairo_set_operator (ctx, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_surface(ctx, cimage, 40, 40);
	cairo_paint(ctx);
	return FALSE;
}

static void
get_screen_size(GtkWidget *w, int *width, int *height)
{
	GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(w));
	*width = gdk_screen_get_width(screen);
	*height = gdk_screen_get_height(screen);
}

static void
window_set_size(GtkWidget *win, int width)
{
	int w, h;
	get_screen_size(win, &w, &h);

	int size = width + 100;
	gtk_widget_set_size_request(GTK_WIDGET(win), size, size);
}

static void
window_setup(GtkWidget *w)
{
	gtk_window_set_resizable(GTK_WINDOW(w), FALSE);
	gtk_window_set_decorated(GTK_WINDOW(w), FALSE);
	gtk_window_set_deletable(GTK_WINDOW(w), FALSE);
	gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER);

	gtk_widget_set_app_paintable(GTK_WIDGET(w), TRUE);
#ifdef CONFIG_WINDOWS
	gtk_widget_set_double_buffered(GTK_WIDGET(w), FALSE);
#endif
	g_signal_connect (w, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect(w, "screen-changed", G_CALLBACK(screen_changed), NULL);

#ifdef CONFIG_WINDOWS
	g_signal_connect (w, "draw", G_CALLBACK (gdi_on_draw), NULL);
#else
	g_signal_connect (w, "draw", G_CALLBACK (on_draw), NULL);
#endif
}

static void
surface_write_row(uint8_t **ptr, uint8_t *from, int size, int stride)
{
	for (int i = 0; i < size; i++) {
		memcpy(*ptr, from, stride);
		*ptr += stride;
	}
}

static cairo_surface_t *
qrcode_cairo_surface(struct surface *surface, int margin, int size)
{
	cairo_surface_t *cimage;

	int width = (surface->width + margin * 2) * size + (size * 2);
	int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);

	uint8_t *pixels = malloc(stride * width * 10);
	uint8_t *row = malloc(stride);
	uint8_t *pix = pixels;
	uint8_t *src = surface->data;
	uint8_t *dst = row;

	/* top margin */
	memset(row, 0xff, stride);
	surface_write_row(&pix, row, size, stride);

	for (int i = 0; i < surface->width; i++) {
		dst = row;
		memset(row, 0xff, stride);
		dst += size * 4;
		for (int y = 0; y < surface->width; y++) {
			for (int yy = 0; yy < size; yy++) {
				*dst++ = *src & 1 ? 0 : 255;
				*dst++ = *src & 1 ? 0 : 255;
				*dst++ = *src & 1 ? 0 : 255;
				*dst++ = 0xff;
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
main_window(int argc, char *argv[], struct surface *surface)
{
	gtk_init (&argc, &argv);

	GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	window_set_size(win, surface->width * 10);
	window_setup(win);

	cimage = qrcode_cairo_surface(surface, 0, 10);
	screen_changed(win, NULL, NULL);

	gtk_widget_show (win);

#ifdef CONFIG_WINDOWS
	gdi_init(win);
#endif
	g_timeout_add_seconds (1, update, win);
	gtk_widget_queue_draw(win);
	gtk_main ();

	return EXIT_SUCCESS;
}
