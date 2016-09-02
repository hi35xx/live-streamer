## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIVE555_CPPFLAGS) \
	$(DBUSXX_CFLAGS) \
	$(FREETYPE_CFLAGS) \
	$(FONTCONFIG_CFLAGS) \
	$(SDL_CFLAGS) \
	-I$(srcdir) -I$(srcdir)/rtsp-server \
	-I$(srcdir)/dbus -I$(builddir)/dbus

AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall

SUBDIRS = \
	rtsp-server \
	dbus \
	platform

bin_PROGRAMS = live-streamer

live_streamer_SOURCES = \
	main.cpp \
	ipcam-media-iface.cpp \
	ipcam-media-iface.h \
	ipcam-runtime.cpp \
	ipcam-runtime.h \
	ipcam-video-osd.cpp \
	ipcam-video-osd.h

live_streamer_LDFLAGS = $(LIVE555_LDFLAGS) \
	$(DBUSXX_LIBS) \
	$(FREETYPE_LIBS) $(FONTCONFIG_LIBS) $(SDL_LIBS) \
	$(LIBEV_LDFLAGS) -ldl

live_streamer_CPPFLAGS = $(AM_CPPFLAGS)
live_streamer_LDADD = \
	$(builddir)/dbus/libdbus-iface.la \
	$(builddir)/rtsp-server/librtsp-server.la

if ENABLE_HI3518V100MPP
live_streamer_CPPFLAGS += $(HI3518V100MPP_CPPFLAGS) -I$(srcdir)/platform/hi3518v100mpp
live_streamer_LDFLAGS += $(HI3518V100MPP_LDFLAGS)
live_streamer_LDADD += $(builddir)/platform/hi3518v100mpp/libhi3518v100mpp.la
endif

if ENABLE_HI3518V200MPP
live_streamer_CPPFLAGS += $(HI3518V200MPP_CPPFLAGS) -I$(srcdir)/platform/hi3518v200mpp
live_streamer_LDFLAGS += $(HI3518V200MPP_LDFLAGS)
live_streamer_LDADD += $(builddir)/platform/hi3518v200mpp/libhi3518v200mpp.la
endif

if ENABLE_HI3520V100MPP
live_streamer_CPPFLAGS += $(HI3520V100MPP_CPPFLAGS) -I$(srcdir)/platform/hi3520v100mpp
live_streamer_LDFLAGS += $(HI3520V100MPP_LDFLAGS)
live_streamer_LDADD += $(builddir)/platform/hi3520v100mpp/libhi3520v100mpp.la
endif
