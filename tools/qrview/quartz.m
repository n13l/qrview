#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include "private.h"
#include <gdk/gdkquartz.h>

void
quartz_init(GtkWidget *win)
{
	GdkWindow *gwin = gtk_widget_get_window(win);
	NSWindow *nswin = gdk_quartz_window_get_nswindow (gwin);

	[nswin setBackgroundColor:[NSColor clearColor]];
	[nswin setOpaque:NO];
	[nswin setAlphaValue:1.0];

	[nswin display];

}
