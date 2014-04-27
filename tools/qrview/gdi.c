#ifndef NTDDI_VERSION
#define NTDDI_VERSION           NTDDI_WIN2KSP4
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT            0x500
#endif
#ifndef _WIN32_IE_
#define _WIN32_IE_              _WIN32_IE_WIN2KSP4
#endif

#include <windows.h>
#include <wingdi.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <gdk/gdkwin32.h>
#include <gdk/gdk.h>
#include <cairo/cairo-win32.h>

#include "private.h"

static HBITMAP __bitmap;
static uint8_t*__bits;

void
gdi_bitmap_init(HDC dc, int w, int h)
{
	if (__bitmap)
		DeleteObject(__bitmap);

	BITMAPINFOHEADER bmi = {
		.biSize          = sizeof(bmi),
		.biWidth         = w,
		.biHeight        = h,
		.biPlanes        = 1,
		.biBitCount      = 24,
		.biCompression   = BI_RGB,
		.biSizeImage     = 0,
		.biXPelsPerMeter = 0,
		.biYPelsPerMeter = 0,
		.biClrUsed       = 0,
		.biClrImportant  = 0
	};

	__bitmap = CreateDIBSection(dc, (LPBITMAPINFO)&bmi, 
	                            DIB_RGB_COLORS, (void *)&__bits, NULL, 0);
	if (!__bitmap) {
		perror("can't create DIBSection");
		exit(EXIT_FAILURE);
	}


	printf("rgb: %d\n", sizeof(RGBQUAD));
	uint8_t *p = __bits;

	for (int x = 0; x < w * h; x++) {
		*p++ = 0;
		*p++ = 0;
		*p++ = 0xff;

	}

}

void
gdi_init_layer(GtkWidget *w)
{
	GdkWindow *gdk = gtk_widget_get_window(w);
	HWND hwnd = gdk_win32_window_get_impl_hwnd(gdk);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 255), 240, 
	                           LWA_ALPHA | LWA_COLORKEY);
	ShowWindow(hwnd, SW_SHOW);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, 
	             SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	BringWindowToTop(hwnd);
}

void
gdi_init(GtkWidget *w)
{
	printf("gdi_init\n");
	GdkWindow *gdk = gtk_widget_get_window(w);
	HWND hwnd = gdk_win32_window_get_impl_hwnd(gdk);

	int width, height;
	gtk_window_get_size(GTK_WINDOW(w), &width, &height);

	HDC dc = GetDC(hwnd);
	gdi_bitmap_init(dc, width, height);

	ReleaseDC(hwnd, dc);

	gdi_init_layer(w);
}

gboolean
gdi_on_draw(GtkWidget *w, cairo_t *ctx, gpointer p)
{
	GdkWindow *gdk = gtk_widget_get_window(w);
	HWND hwnd = gdk_win32_window_get_impl_hwnd(gdk);
	int width, height;
	gtk_window_get_size(GTK_WINDOW(w), &width, &height);

	HDC dc = GetDC(hwnd);
	HDC dc_new = CreateCompatibleDC(dc);

	HGDIOBJ obj = SelectObject(dc_new, __bitmap);

	cairo_surface_t *surface = cairo_win32_surface_create(dc_new);
	cairo_t *cr = cairo_create(surface);

	on_draw(w, cr, p);
/*
	cairo_t *c = cr;
cairo_set_source_rgb (c, 0.0, 0.0, 0.0);
cairo_rectangle(c, 30, 30, width - 60 , height - 60);
cairo_set_line_width(c, 30);
cairo_set_line_join(c, CAIRO_LINE_JOIN_ROUND);
cairo_stroke(c);
*/
	cairo_surface_finish(surface);

	RECT rect;
	GetClientRect(hwnd, &rect);
	int rc = BitBlt(dc, 0, 0, width, height, dc_new, 0, 0, SRCCOPY);

	printf("hwnd: %p cr: %p, surface: %p, dc: %p, newdc: %p __bitmap: %p rc: %d\n", 
			hwnd, cr, surface, dc, dc_new, __bitmap, rc);

	SelectObject(dc_new, obj);

	ReleaseDC(hwnd, dc);
	DeleteDC(dc_new);

	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	return TRUE;
}
