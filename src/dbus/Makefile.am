## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir)/src -I$(srcdir) -I$(builddir) \
	$(DBUSXX_CFLAGS)

noinst_LTLIBRARIES =  \
	libdbus-iface.la

BUILT_SOURCES = \
	audio-source-server-glue.h \
	audio-encoder-server-glue.h \
	video-source-server-glue.h \
	video-encoder-server-glue.h \
	video-osd-server-glue.h

libdbus_iface_la_SOURCES =  \
	dbusxx-libev-integration.cpp \
	dbusxx-libev-integration.h \
	$(BUILT_SOURCES) \
	dbus-audio-source.cpp \
	dbus-audio-source.h \
	dbus-audio-encoder.cpp \
	dbus-audio-encoder.h \
	dbus-video-osd.cpp \
	dbus-video-osd.h \
	dbus-video-source.cpp \
	dbus-video-source.h \
	dbus-video-encoder.cpp \
	dbus-video-encoder.h

%-server-glue.h: %-introspect.xml
	dbusxx-xml2cpp $^ --adaptor=$@

CLEANFILES = $(BUILT_SOURCES)

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)

dbuspolicydir=$(sysconfdir)/dbus-1/system.d
dbuspolicy_DATA = \
	live-streamer.conf

EXTRA_DIST = \
	video-source-introspect.xml \
	video-encoder-introspect.xml \
	audio-source-introspect.xml \
	audio-encoder-introspect.xml \
	live-streamer.conf
