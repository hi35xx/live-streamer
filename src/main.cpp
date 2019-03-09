/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cpp
 * Copyright (C) 2015 Watson Xu <xuhuashan@gmail.com>
 * 
 * rtsp-streamer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * rtsp-streamer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <syslog.h>
#include <locale.h>
#include <string>
#include <sstream>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <MediaSink.hh>
#include <BasicUsageEnvironment.hh>
#include <liveMedia.hh>
#include <DynamicRTSPServer.hh>

#include <EvUsageEnvironment.h>

#include <ft2build.h>
#include <freetype/freetype.h>

#include <dbus/dbus.h>
#include <dbus-c++/dbus.h>
#include "dbusxx-libev-integration.h"

#include <ipcam-runtime.h>

#if defined(HAVE_HI3518V100_SUPPORT) || defined(HAVE_HI3518V200_SUPPORT) \
	|| defined(HAVE_HI3516CV300_SUPPORT) \
	|| defined(HAVE_HI3520V100_SUPPORT) || defined(HAVE_HI3520DV200_SUPPORT)
#include <himpp-media.h>
#endif

ev::default_loop mainloop;

static const char *IPCAM_SERVER_NAME = "ipcam.Media";

static std::string config_file;
static unsigned output_packet_buffer_size = 400000;
static int rtsp_port = 554;
static DBusBusType bus_type = DBUS_BUS_SYSTEM;

static void display_usage(char *cmd)
{
	const char *usage = \
		"RTSP/RTP stream server\n"
		"Common Options:\n"
		"  -h, --help                  Show help options\n"
		"  -B, --background            Run daemon in the background\n"
		"  -b, --buffer=SIZE           Output packet buffer size\n"
		"  -S, --syslog                Log output to syslog instead of stdout\n"
		"      --system                Use system message bus\n"
		"      --session               Use session message bus\n"
		"  -c, --configure=FILE        Configuration file\n"
		"      --version               Show version information\n"
		"  -p, --rtsp-port=PORT        RTSP port\n"
		"\n";
	std::cout << "Usage: " << cmd << " [options]...\n";
	std::cout << usage;
	std::cout << "Platform Options:\n";
#if defined(HAVE_HI3518V100_SUPPORT) || defined(HAVE_HI3518V200_SUPPORT) \
	|| defined(HAVE_HI3520V100_SUPPORT) || defined(HAVE_HI3520DV200_SUPPORT)
	const char *plat_options = \
		"  -pipe PIPE-STRING           Add media pipe\n"
		"  -vsrc VIDEOSOURCE           Add video source\n"
		"  -venc VIDEOENCODER          Add video encoder\n"
		"  -asrc AUDIOSOURCE           Add audio source\n"
		"  -aenc AUDIOENCODER          Add audio encoder\n"
		"  -stream PATH:[VENC],[AENC]  Add RTSP stream\n"
		"Example:\n"
		"  -vsrc \"isp0:sensor=ar0130|videv0|vichn0|vpgrp0|vpchn0\"\n"
		"  -venc \"vpchn0|vechn0:encoding=H264,resolution=720P,framerate=25\"\n"
		"  -asrc \"acodec0|aidev0|aichn0\"\n"
		"  -aenc \"aichn0|aechn0:encoding=G711A\"\n"
		"  -stream 0:vechn0,aechn0\n";
	std::cout << plat_options;
#endif
}

static const char *optstr = "Bh?c:p:S";
static const struct option longopts[] = {
	{ "help",        no_argument,        NULL,   'h' },
	{ "buffer",      required_argument,  NULL,   'b' },
	{ "background",  required_argument,  NULL,   'B' },
	{ "version",     no_argument,        NULL,    0  },
	{ "configure",   required_argument,  NULL,   'c' },
	{ "system",      no_argument,        NULL,    0  },
	{ "session",     no_argument,        NULL,    0  },
	{ "port",        required_argument,  NULL,   'p' },
	{ "syslog",      no_argument,        NULL,   'S' },
#if defined(HAVE_HI3518V100_SUPPORT) || defined(HAVE_HI3518V200_SUPPORT) \
	|| defined(HAVE_HI3520V100_SUPPORT) || defined(HAVE_HI3520DV200_SUPPORT) \
	|| defined(HAVE_HI3516CV300_SUPPORT)
	{ "pipe",        required_argument,  NULL,    0  },
	{ "vsrc",        required_argument,  NULL,    0  },
	{ "venc",        required_argument,  NULL,    0  },
	{ "asrc",        required_argument,  NULL,    0  },
	{ "aenc",        required_argument,  NULL,    0  },
	{ "stream",      required_argument,  NULL,    0  },
	{ "aout",        required_argument,  NULL,    0  },
#endif
	{ NULL,          no_argument,        NULL,    0  }
};

bool _terminated = false;

static void redirect_io_to_syslog(FILE **pfp)
{
	cookie_io_functions_t io_funcs = {
		.read = [](void*, char*, size_t) -> ssize_t { return 0; },
		.write = [](void* cookie, const char* buf, size_t size) -> ssize_t {
			syslog(LOG_INFO, "%.*s", (int)size, buf);
			return size;
		},
		.seek = [](void*, off64_t*, int) -> int { return 0; },
		.close = [](void*) -> int { return 0; }
	};
	setvbuf(*pfp = fopencookie(NULL, "w", io_funcs), NULL, _IOLBF, 1024);
}

static void sigterm_handler(ev::sig &w, int revents)
{
	std::cout << std::endl;
	_terminated = true;
	mainloop.break_loop(ev::ALL);
}

static DBus::Ev::BusDispatcher dispatcher;

int main(int argc, char *argv[])
{
	bool to_syslog = false;
	std::vector<std::pair<std::string, std::string>> plat_args;

	setlocale(LC_ALL, "");

	int c, loptind = 0;
	while ((c = getopt_long_only(argc, argv, optstr, longopts, &loptind)) != -1) {
		switch (c) {
		case 'B':
			if (fork() != 0)
				return 0;
			break;
		case 'b':
			output_packet_buffer_size = strtoul(optarg, NULL, 0);
		case 'p':
			rtsp_port = strtoul(optarg, NULL, 0);
			break;
		case 'h':
			display_usage(argv[0]);
			return 0;
		case 'c':
			config_file = optarg;
			break;
		case 'S':
			to_syslog = true;
			break;
		case 0:
			if (strcmp(longopts[loptind].name, "version") == 0) {
				std::cout << PACKAGE_STRING << std::endl;
				return 0;
			} else if (strcmp(longopts[loptind].name, "system") == 0) {
				bus_type = DBUS_BUS_SYSTEM;
			} else if (strcmp(longopts[loptind].name, "session") == 0) {
				bus_type = DBUS_BUS_SESSION;
			} else {
				plat_args.emplace_back(std::string(longopts[loptind].name),
				                       std::string(optarg));
			}
			break;
		default:
			display_usage(argv[0]);
			return -1;
		}
	}

	if (to_syslog) {
		openlog(PACKAGE_NAME, LOG_CONS, LOG_USER);
		redirect_io_to_syslog(&stdout);
		redirect_io_to_syslog(&stderr);
	}

	/* install SIGINT handler */
	ev::sig sigintw(mainloop);
	sigintw.set<sigterm_handler>();
	sigintw.set(SIGINT);
	sigintw.start();
	/* install SIGTERM handler */
	ev::sig sigtermw(mainloop);
	sigtermw.set<sigterm_handler>();
	sigtermw.set(SIGTERM);
	sigtermw.start();
	/* install SIGQUIT handler */
	ev::sig sigquitw(mainloop);
	sigquitw.set<sigterm_handler>();
	sigquitw.set(SIGQUIT);
	sigquitw.start();
	/* install SIGHUP handler */
	ev::sig sighupw(mainloop);
	sighupw.set<sigterm_handler>();
	sighupw.set(SIGHUP);
	sighupw.start();

	TaskScheduler* scheduler = EvTaskScheduler::createNew(mainloop);
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase *authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1");
#endif

	// allow for some possibly large H.264 frames
	OutPacketBuffer::maxSize = output_packet_buffer_size;

	// Create the RTSP server
	DynamicRTSPServer *rtspServer;
	portNumBits rtspServerPortNum = rtsp_port;
	rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server:" << env->getResultMsg() << "\n";
		exit(1);
	}

	DBus::default_dispatcher = &dispatcher;
	dispatcher.attach(&mainloop);

	DBus::Connection conn = (bus_type == DBUS_BUS_SYSTEM) ?
		DBus::Connection::SystemBus() : DBus::Connection::SessionBus();
	conn.request_name(IPCAM_SERVER_NAME);

	FT_Library freetype;
	if (FT_Init_FreeType(&freetype)) {
		fprintf(stderr, "FT_Init_FreeType failed\n");
		return 1;
	}

	IpcamRuntime *runtime = new IpcamRuntime(config_file, mainloop, rtspServer, &conn);

#if defined(HAVE_HI3518V100_SUPPORT) || defined(HAVE_HI3518V200_SUPPORT) \
	|| defined(HAVE_HI3516CV300_SUPPORT) \
	|| defined(HAVE_HI3520V100_SUPPORT) || defined(HAVE_HI3520DV200_SUPPORT)
	HimppMedia himpp_media(runtime, plat_args);
#endif

#ifdef HAVE_JSONCPP_SUPPORT
	runtime->LoadConfig();
#endif

	mainloop.run();

	delete runtime;

	FT_Done_FreeType(freetype);

	Medium::close(rtspServer);

	env->reclaim();
	delete scheduler;

	if (to_syslog) {
		closelog();
	}

	return 0;
}
