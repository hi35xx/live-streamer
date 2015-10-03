## Process this file with automake to produce Makefile.in

if ENABLE_HIMPP

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir).../../../live-streamer../../../live-streamer"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-I$(top_srcdir)/src -I$(top_srcdir)/src/himpp \
	$(LIVE555_CPPFLAGS) \
	$(DBUSXX_CFLAGS) \
	$(HIMPP_CPPFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

noinst_LTLIBRARIES =  \
	libhimpp-media.la

libhimpp_media_la_SOURCES =  \
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
	HimppStreamInput.cpp \
	HimppStreamInput.hh \
	himpp-audio.cpp \
	himpp-audio.h

EXTRA_DIST =


endif
