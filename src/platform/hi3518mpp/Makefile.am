## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I. -I$(top_srcdir)/src -I$(top_srcdir)/src/rtsp-server \
	-I$(top_srcdir)/src/dbus -I$(top_builddir)/src/dbus \
	$(LIVE555_CPPFLAGS) \
	$(DBUSXX_CFLAGS) \
	$(HI3518MPP_CPPFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

noinst_LTLIBRARIES =  \
	libhi3518mpp.la

libhi3518mpp_la_SOURCES =  \
	himpp-common.h \
	himpp-object.cpp \
	himpp-object.h \
	himpp-sysctl.cpp \
	himpp-sysctl.h \
	himpp-video-sensor.cpp \
	himpp-video-sensor.h \
	himpp-video-isp.cpp \
	himpp-video-isp.h \
	himpp-video-viu.cpp \
	himpp-video-viu.h \
	himpp-video-vpss.cpp \
	himpp-video-vpss.h \
	himpp-video-venc.cpp \
	himpp-video-venc.h \
	himpp-media.cpp \
	himpp-media.h \
	himpp-audio.cpp \
	himpp-audio.h

libhi3518mpp_la_LIBADD = \
	$(top_builddir)/src/dbus/libdbus-iface.la

EXTRA_DIST =

