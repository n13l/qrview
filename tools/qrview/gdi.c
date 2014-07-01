#ifndef NTDDI_VERSION
#define NTDDI_VERSION           NTDDI_WIN2KSP4
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT            0x0500
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
#include <fcntl.h>
#include <gtk/gtk.h>
#include <gdk/gdkwin32.h>
#include <gdk/gdk.h>
#include <cairo/cairo-win32.h>

#include "private.h"

static HBITMAP __bitmap;
static uint8_t*__bits;

void
win32_console_init(void)
{
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();

	freopen("conout$", "w", stdout);
	freopen("conerr$", "w", stderr);
}

void
win32_io_init(void)
{
	int hConHandle;
	long lStdHandle;
	int iVar;

	if (!AttachConsole (ATTACH_PARENT_PROCESS))
		return;

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	FILE *fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
}

char *
win32_error_string(DWORD error)
{
	LPSTR messageBuffer = NULL;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
	                             FORMAT_MESSAGE_IGNORE_INSERTS,
	                             NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				     (LPSTR)&messageBuffer, 0, NULL);

	char *str = malloc(size + 1);
	strcpy(str, messageBuffer);
	str[size] = 0;
	LocalFree(messageBuffer);

	return str;
}

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


	int stride = ((((w * 24) + 31) & ~31) >> 3);
	uint8_t *p = __bits;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			*p++ = 128;
			*p++ = 0;
			*p++ = 0;
		}

		p += stride - (w * 3);
	}

}

void
gdi_init_layer(GtkWidget *w)
{
	GdkWindow *gdk = gtk_widget_get_window(w);
	HWND hwnd = gdk_win32_window_get_impl_hwnd(gdk);

	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED |  WS_EX_TOOLWINDOW);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 128), alpha, 
	                           LWA_ALPHA | LWA_COLORKEY);
	ShowWindow(hwnd, SW_SHOW);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, 
	             SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	BringWindowToTop(hwnd);
}

void
gdi_init(GtkWidget *w)
{
	GdkWindow *gdk = gtk_widget_get_window(w);
	HWND hwnd = gdk_win32_window_get_impl_hwnd(gdk);

	int width, height;
	gtk_window_get_size(GTK_WINDOW(w), &width, &height);

	HDC dc = GetDC(hwnd);
	gdi_bitmap_init(dc, width, height);

	ReleaseDC(hwnd, dc);

//	gdi_init_layer(w);
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

	cairo_surface_finish(surface);

	RECT rect;
	GetClientRect(hwnd, &rect);
	int rc = BitBlt(dc, 0, 0, width, height, dc_new, 0, 0, SRCCOPY);

	SelectObject(dc_new, obj);

	ReleaseDC(hwnd, dc);
	DeleteDC(dc_new);

	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	gdi_init_layer(w);

	return TRUE;
}
