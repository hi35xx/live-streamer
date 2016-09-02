## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I. -I$(top_srcdir)/src -I$(top_srcdir)/src/rtsp-server \
	-I$(top_srcdir)/src/dbus -I$(top_builddir)/src/dbus \
	$(LIVE555_CPPFLAGS) \
	$(DBUSXX_CFLAGS) \
	$(SDL_CFLAGS) \
	$(HI3518V200MPP_CPPFLAGS)

AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall

noinst_LTLIBRARIES =  \
	libhi3518v200mpp.la

libhi3518v200mpp_la_SOURCES =  \
	himpp-common.h \
	himpp-object.cpp \
	himpp-object.h \
	himpp-sysctl.cpp \
	himpp-sysctl.h \
	himpp-audio.cpp \
	himpp-audio.h \
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
	himpp-video-region.cpp \
	himpp-video-region.h \
	himpp-media.cpp \
	himpp-media.h

libhi3518v200mpp_la_LIBADD = \
	$(top_builddir)/src/dbus/libdbus-iface.la

EXTRA_DIST =

