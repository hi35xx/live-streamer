## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I. -I$(top_srcdir)/src -I$(top_srcdir)/src/rtsp-server \
	-I$(top_srcdir)/src/dbus -I$(top_builddir)/src/dbus \
	$(LIVE555_CPPFLAGS) \
	$(DBUSXX_CFLAGS) \
	$(HI3520MPP_CPPFLAGS)

AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall

noinst_LTLIBRARIES =  \
	libhi3520mpp.la

libhi3520mpp_la_SOURCES =  \
	himpp-common.h \
	himpp-object.cpp \
	himpp-object.h \
	himpp-sysctl.cpp \
	himpp-sysctl.h \
	himpp-video-codec.cpp \
	himpp-video-codec.h \
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

libhi3520mpp_la_LIBADD = \
	$(top_builddir)/src/dbus/libdbus-iface.la

EXTRA_DIST =
