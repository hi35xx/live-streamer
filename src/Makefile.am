## Process this file with automake to produce Makefile.in

## Created by Anjuta

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I$(top_srcdir)/src -I$(top_srcdir)/src/himpp \
	$(LIVE555_CPPFLAGS) \
	$(DBUSXX_CFLAGS) \
	$(HIMPP_CPPFLAGS)

SUBDIRS = \
	himpp

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = live-streamer

live_streamer_SOURCES = \
	main.cpp \
	EvUsageEnvironment.cpp \
	EvUsageEnvironment.h \
	DynamicRTSPServer.cpp \
	DynamicRTSPServer.hh \
	dbusxx-libev-integration.cpp \
	dbusxx-libev-integration.h \
	video-source-server-glue.h \
	video-encoder-server-glue.h \
	ipcam-media.cpp \
	ipcam-media.h \
	ipcam-video-encoder.cpp \
	ipcam-video-encoder.h \
	ipcam-video-source.cpp \
	ipcam-video-source.h

live_streamer_LDFLAGS = $(LIVE555_LDFLAGS) \
	$(DBUSXX_LIBS) \
	$(HIMPP_LDFLAGS) \
	$(LIBEV_LDFLAGS) -ldl

if ENABLE_HIMPP
live_streamer_LDADD = $(builddir)/himpp/libhimpp-media.la
endif

video-source-server-glue.h: video-source-introspect.xml
	dbusxx-xml2cpp $^ --adaptor=$@
video-encoder-server-glue.h: video-encoder-introspect.xml
	dbusxx-xml2cpp $^ --adaptor=$@

BUILT_SOURCES = \
	video-source-server-glue.h \
	video-encoder-server-glue.h

CLEANFILES = $(BUILT_SOURCES)

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)

EXTRA_DIST = \
	video-source-introspect.xml \
	video-encoder-introspect.xml
