DIRS+=tools/qrview
qrview=$(o)/tools/qrview/qrview

qrview_OBJECTS=$(addprefix $(o)/tools/qrview/, qrview win asc)

qrview_CFLAGS=$(CONFIG_HAVE_GTK__3_0_CFLAGS) \
	      $(CONFIG_HAVE_LIBQRENCODE_CFLAGS)\
	      $(CONFIG_HAVE_GTK__WIN32_3_0_CFLAGS)\
	      $(CONFIG_HAVE_GTK__WIN32_3_0_STATIC_CFLAGS)

qrview_LDFLAGS=$(CONFIG_HAVE_GTK__3_0_LDFLAGS) \
	       $(CONFIG_HAVE_GTK__WIN32_3_0_LDFLAGS)\
	       $(CONFIG_HAVE_GTK__WIN32_3_0_STATIC_LDFLAGS)\
               $(CONFIG_HAVE_LIBQRENCODE_LDFLAGS) \

$(qrview): $(addsuffix .o, $(qrview_OBJECTS))
$(qrview): LIBS+=$(qrview_LDFLAGS)

$(o)/tools/qrview/qrview.o: CFLAGS+=$(qrview_CFLAGS)
$(o)/tools/qrview/win.o: CFLAGS+=$(qrview_CFLAGS)
$(o)/tools/qrview/enc.o: CFLAGS+=$(qrview_CFLAGS)

PROGS+=$(qrview)
ASM+=$(addsuffix .s, $(qrview_OBJECTS))
