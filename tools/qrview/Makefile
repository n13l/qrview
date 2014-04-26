DIRS+=tools/qrview
qrview=$(o)/tools/qrview/qrview

qrview_OBJECTS=$(addprefix $(o)/tools/qrview/, qrview win enc asc)

qrview_CFLAGS=$(CONFIG_HAVE_GTK__3_0_CFLAGS) $(CONFIG_HAVE_LIBQRENCODE_CFLAGS)
qrview_LDFLAGS=$(CONFIG_HAVE_GTK__3_0_LDFLAGS) \
	      $(CONFIG_HAVE_LIBQRENCODE_LDFLAGS) \
	      $(CONFIG_HAVE_LIBPNG_LDFLAGS)

$(qrview): $(addsuffix .o, $(qrview_OBJECTS))
$(qrview): $(LIBSYS_STATIC) $(LIBqrview_STATIC) $(LIBS)
$(qrview): LDFLAGS+=$(qrview_LDFLAGS)
$(o)/tools/qrview/win.o: CFLAGS+=$(CONFIG_HAVE_GTK__3_0_CFLAGS)
$(o)/tools/qrview/enc.o: CFLAGS+=$(CONFIG_HAVE_LIBQRENCODE_CFLAGS)
$(o)/tools/qrview/enc.o: CFLAGS+=$(CONFIG_HAVE_LIBPNG_CFLAGS)
$(o)/tools/qrview/enc.o: CFLAGS+=$(CONFIG_HAVE_GTK__3_0_CFLAGS)

PROGS+=$(qrview)
ASM+=$(addsuffix .s, $(qrview_OBJECTS))
