## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIVE555_CPPFLAGS) \
	$(DBUSXX_CFLAGS) \
	$(FREETYPE_CFLAGS) \
	$(FONTCONFIG_CFLAGS) \
	$(SDL_CFLAGS) \
	-I$(srcdir) -I$(srcdir)/common \
	-I$(srcdir)/rtsp-server \
	-I$(srcdir)/dbus -I$(builddir)/dbus

AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall

SUBDIRS = \
	common \
	rtsp-server \
	dbus \
	platform

bin_PROGRAMS = live-streamer

live_streamer_SOURCES = \
	main.cpp \
	ipcam-runtime.cpp \
	ipcam-runtime.h

live_streamer_LDFLAGS = $(LIVE555_LDFLAGS) \
	$(DBUSXX_LIBS) \
	$(FREETYPE_LIBS) $(FONTCONFIG_LIBS) $(SDL_LIBS) \
	$(LIBEV_LDFLAGS) -ldl

live_streamer_CPPFLAGS = $(AM_CPPFLAGS)
live_streamer_LDADD = \
	$(builddir)/common/libcommon.la \
	$(builddir)/dbus/libdbus-iface.la \
	$(builddir)/rtsp-server/librtsp-server.la

if ENABLE_HI3518V100
live_streamer_CPPFLAGS += $(HI3518V100_CPPFLAGS) -I$(srcdir)/platform/hi3518v100
live_streamer_LDFLAGS += $(HI3518V100_LDFLAGS)
live_streamer_LDADD += $(builddir)/platform/hi3518v100/libhi3518v100.la
endif

if ENABLE_HI3518V200
live_streamer_CPPFLAGS += $(HI3518V200_CPPFLAGS) -I$(srcdir)/platform/hi3518v200
live_streamer_LDFLAGS += $(HI3518V200_LDFLAGS)
live_streamer_LDADD += $(builddir)/platform/hi3518v200/libhi3518v200.la
endif

if ENABLE_HI3520V100
live_streamer_CPPFLAGS += $(HI3520V100_CPPFLAGS) -I$(srcdir)/platform/hi3520v100
live_streamer_LDFLAGS += $(HI3520V100_LDFLAGS)
live_streamer_LDADD += $(builddir)/platform/hi3520v100/libhi3520v100.la
endif

if ENABLE_HI3520DV200
live_streamer_CPPFLAGS += $(HI3520DV200_CPPFLAGS) -I$(srcdir)/platform/hi3520v100
live_streamer_LDFLAGS += $(HI3520DV200_LDFLAGS)
live_streamer_LDADD += $(builddir)/platform/hi3520v100/libhi3520v100.la
endif
