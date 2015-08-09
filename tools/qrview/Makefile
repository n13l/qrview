DIRS+=tools/qrview
DIRS+=src/sys
DIRS+=src/compat
DIRS+=src/compat/windows
DIRS+=src/compat/darwin

qrview=$(o)/tools/qrview/qrview

CFLAGS+=-I$(s)/tools/qrview
# workarround macos bug
#CFLAGS+=-DNS_RETURNS_INNER_POINTER=

qrview_OBJECTS=$(addprefix $(o)/tools/qrview/, qrview win asc icon pid)
qrview_OBJECTS+=$(addprefix $(o)/src/sys/, exit)

ifdef CONFIG_WINDOWS
qrview_OBJECTS+=$(addprefix $(o)/src/compat/windows/, gdi lockf fsync kill file errno)
endif
ifdef CONFIG_DARWIN
qrview_OBJECTS+=$(addprefix $(o)/src/compat/darwin/, home)
qrview_OBJECTS+=$(addprefix $(o)/tools/qrview/, quartz)
endif

qrview_CFLAGS=$(CONFIG_HAVE_GTK__3_0_CFLAGS) \
	      $(CONFIG_HAVE_LIBQRENCODE_CFLAGS)\
	      $(CONFIG_HAVE_GTK__WIN32_3_0_CFLAGS)\
	      $(CONFIG_HAVE_GTK__WIN32_3_0_STATIC_CFLAGS)\
              $(CONFIG_HAVE_GTK__QUARTZ_3_0_CFLAGS)\
              $(CONFIG_HAVE_GTK_MAC_INTEGRATION_GTK3_CFLAGS)

qrview_LDFLAGS=$(CONFIG_HAVE_GTK__3_0_LDFLAGS) \
	       $(CONFIG_HAVE_GTK__WIN32_3_0_LDFLAGS)\
	       $(CONFIG_HAVE_GTK__WIN32_3_0_STATIC_LDFLAGS)\
               $(CONFIG_HAVE_LIBQRENCODE_LDFLAGS) \
              $(CONFIG_HAVE_GTK__QUARTZ_3_0_LDFLAGS)\
              $(CONFIG_HAVE_GTK_MAC_INTEGRATION_GTK3_LDFLAGS)

$(qrview): $(addsuffix .o, $(qrview_OBJECTS))
$(qrview): LIBS+=$(qrview_LDFLAGS)

$(o)/tools/qrview/qrview.o: CFLAGS+=$(qrview_CFLAGS)
$(o)/tools/qrview/icon.o: CFLAGS+=$(qrview_CFLAGS)
$(o)/tools/qrview/win.o: CFLAGS+=$(qrview_CFLAGS)
$(o)/tools/qrview/enc.o: CFLAGS+=$(qrview_CFLAGS)
$(o)/src/compat/windows/gdi.o: CFLAGS+=$(qrview_CFLAGS)
$(o)/tools/qrview/quartz.o: CFLAGS+=$(qrview_CFLAGS)

PROGS+=$(qrview)
ASM+=$(addsuffix .s, $(qrview_OBJECTS))
