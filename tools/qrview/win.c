#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "private.h"

GtkWidget *win;
GtkStatusIcon *status;

gboolean supports_alpha = FALSE;
static cairo_surface_t *cimage = NULL;
static uint8_t window_size;
static int bar_size;

unsigned int alpha = 0;
static enum state {
	STARTING  = 0,
	RUNNING   = 1,
	STOPPING  = 2
} state;

static time_t start;
static int ms = 1000;

static gboolean
update(gpointer data)
{
	GtkWidget *widget = (GtkWidget *)data;

	if (!start) start = time(NULL);

	time_t now = time(NULL);
	time_t rest = now - start;

	if (rest > timeout && state != STOPPING) {
		state = STOPPING;
		gtk_widget_queue_draw(win);
	}

	return TRUE;
}

static gboolean                                                                 
press_event(GtkWidget *w, GdkEventButton *e)                             
{
	if (e->button != 1)
		return TRUE;

	gtk_main_quit();
	return TRUE;
}

static void 
screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
	GdkScreen *screen = gtk_widget_get_screen (widget);
	GdkVisual *visual = gdk_screen_get_rgba_visual (screen);

	if (!visual) {
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

	double d;

	if (supports_alpha)
		d = ((float)alpha / (float)255);

	if (supports_alpha)
		cairo_set_source_rgba (ctx, 0.0, 0.0, 0.0, d);
	else
		cairo_set_source_rgb (ctx, 0.0, 0.0, 0.0);

	cairo_save(ctx);
	cairo_rectangle(ctx, 30, 30, width - 60 , height - 60);
	cairo_set_line_width(ctx, 30);
	cairo_set_line_join(ctx, CAIRO_LINE_JOIN_ROUND); 
	cairo_stroke(ctx);
	cairo_restore(ctx);

	if (supports_alpha)
		cairo_set_source_rgba (ctx, 1.0, 1.0, 1.0, d);
	else
		cairo_set_source_rgb (ctx, 1.0, 1.0, 1.0);

	double scale = (double)(width - 90) / (double)bar_size;

	cairo_save(ctx);
	cairo_translate (ctx, 45, 45);
	cairo_scale (ctx, scale, scale);
        cairo_set_source_surface(ctx, cimage, 0, 0);

	if (supports_alpha)
		cairo_paint_with_alpha (ctx, d);
	else		               
		cairo_paint(ctx);

	cairo_restore(ctx);

	switch (state) {
	case STARTING:
		if (alpha >= alpha_max)
			state = RUNNING;
		alpha += 2;
		if (alpha > alpha_max)
			alpha = alpha_max;
		
		gtk_widget_queue_draw(win);
		break;
	case RUNNING:
		break;
	case STOPPING:
		if (alpha < 3)
			gtk_main_quit();
		alpha -= 2;
		if (alpha < 0)
			alpha = 0;
		gtk_widget_queue_draw(win);
		break;
	}

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

	int size = w / 3;
	gtk_widget_set_size_request(GTK_WIDGET(win), size, size);
}

static void
window_setup(GtkWidget *w)
{
	gtk_window_set_resizable(GTK_WINDOW(w), FALSE);
	gtk_window_set_decorated(GTK_WINDOW(w), FALSE);
	gtk_window_set_deletable(GTK_WINDOW(w), FALSE);
	gtk_window_set_position(GTK_WINDOW(w), GTK_WIN_POS_CENTER);
	gtk_window_set_type_hint(GTK_WINDOW(w), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN); 
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(w), FALSE);

	gtk_widget_set_app_paintable(GTK_WIDGET(w), TRUE);
#ifndef CONFIG_LINUX
	gtk_widget_set_double_buffered(GTK_WIDGET(w), FALSE);
#endif
	//g_signal_connect(w, "show", G_CALLBACK (realize), NULL);
	g_signal_connect(w, "button_press_event", G_CALLBACK (press_event), NULL);
	g_signal_connect(w, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect(w, "screen-changed", G_CALLBACK(screen_changed), NULL);

#ifdef CONFIG_WINDOWS
	g_signal_connect(w, "draw", G_CALLBACK (gdi_on_draw), NULL);
#else
	g_signal_connect(w, "draw", G_CALLBACK (on_draw), NULL);
#endif

        gtk_widget_set_events(w, GDK_EXPOSURE_MASK |                       
	                      GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |   
	                      GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
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

static
gboolean
on_second_click(GtkStatusIcon *i, GdkEvent *e, gpointer ur)
{
	if (e->type != GDK_BUTTON_PRESS || ((GdkEventButton *)e)->button != 1)
		return FALSE;

	return TRUE;
}

int
main_window(int argc, char *argv[], struct surface *surface)
{
	gtk_init (&argc, &argv);

#ifdef MAC_INTEGRATION
        GtkosxApplication *osx = g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
#endif

	GdkPixbuf *icon_pix = gdk_pixbuf_from_pixdata((const GdkPixdata *)&icon, FALSE, NULL);

	status = gtk_status_icon_new();

	if (status) {
		char *tip  = "LGPL QRView (c) 2014 Daniel Kubec <niel@rtfm.cz>";
		gtk_status_icon_set_from_pixbuf(status, icon_pix);
		gtk_status_icon_set_tooltip_text(status,tip);
		gtk_status_icon_set_visible(status, TRUE);
		gboolean emb = gtk_status_icon_is_embedded(status);
	}

	win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	bar_size = (surface->width * 10) + 20;

	window_set_size(win, bar_size);
	window_setup(win);

	cimage = qrcode_cairo_surface(surface, 0, 10);
	screen_changed(win, NULL, NULL);

	alpha = 0;

	gtk_widget_show (win);
	//gtk_widget_hide (win);

#ifdef CONFIG_WINDOWS
	gdi_init(win);
#endif
#ifdef CONFIG_DARWIN
	quartz_init(win);
#endif
	//gtk_widget_show (win);
	gtk_window_set_keep_above(GTK_WINDOW(win), TRUE);

	if (timeout)
		g_timeout_add(ms, update, win);
	gtk_widget_queue_draw(win);

	gtk_main ();

	if (status)
		gtk_status_icon_set_visible(status, FALSE);

	return EXIT_SUCCESS;
}
