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
	|| defined(HAVE_HI3520V100_SUPPORT) || defined(HAVE_HI3520DV200_SUPPORT)
#include <himpp-media.h>
#endif

ev::default_loop mainloop;

static const char *IPCAM_SERVER_NAME = "ipcam.Media";

static int rtsp_port = 554;
static DBusBusType bus_type = DBUS_BUS_SYSTEM;

static void display_usage(char *cmd)
{
	const char *usage = \
		"RTSP/RTP stream server\n"
		"Common Options:\n"
		"  -h, --help                  Show help options\n"
		"  -B, --background            Run daemon in the background\n"
		"      --system                Use system message bus\n"
		"      --session               Use session message bus\n"
		"  -v, --version               Show version information\n"
		"  -s, --syslog                Log output to syslog instead of stdout\n"
		"  -p, --rtsp-port=PORT        RTSP port\n"
		"\n";
	std::cout << "Usage: " << cmd << " [options]...\n";
	std::cout << usage;
	std::cout << "Platform Options:\n";
#if defined(HAVE_HI3518V100_SUPPORT) || defined(HAVE_HI3518V200_SUPPORT) \
	|| defined(HAVE_HI3520V100_SUPPORT) || defined(HAVE_HI3520DV200_SUPPORT)
#endif
	std::cout << std::endl;
}

static const char *optString = "Bh?vp:s";
static const struct option longOpts[] = {
	{ "help",        no_argument,        NULL,   'h' },
	{ "background",  required_argument,  NULL,   'B' },
	{ "version",     no_argument,        NULL,   'v' },
	{ "system",      no_argument,        NULL,    0  },
	{ "session",     no_argument,        NULL,    0  },
	{ "port",        required_argument,  NULL,   'p' },
	{ "syslog",      no_argument,        NULL,   's' },
	{ NULL,          no_argument,        NULL,    0  }
};

bool _terminated = false;

static void sigterm_handler(ev::sig &w, int revents)
{
	std::cout << std::endl;
	_terminated = true;
	mainloop.break_loop(ev::ALL);
}

static DBus::Ev::BusDispatcher dispatcher;

int main(int argc, char *argv[])
{
	int opt = 0;
	int longIndex;
	std::unordered_map<std::string, std::string> plat_args;

	setlocale(LC_ALL, "");

	opt = getopt_long_only(argc, argv, optString, longOpts, &longIndex);
	while (opt != -1) {
		switch (opt) {
		case 'B':
			if (fork() != 0)
				return 0;
			break;
		case 'p':
			rtsp_port = strtoul(optarg, NULL, 0);
			break;
		case 'h':
			display_usage(argv[0]);
			return 0;
		case 'v':
			std::cout << PACKAGE_STRING << std::endl;
			return 0;
		case 's':
			break;
		case 0:
			if (strcmp(longOpts[longIndex].name, "system") == 0)
				bus_type = DBUS_BUS_SYSTEM;
			else if(strcmp(longOpts[longIndex].name, "session") == 0)
				bus_type = DBUS_BUS_SESSION;
			break;
		default:
			display_usage(argv[0]);
			return -1;
		}
		opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
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

	IpcamRuntime *runtime = new IpcamRuntime(mainloop, rtspServer, &conn);

#if defined(HAVE_HI3518V100_SUPPORT) || defined(HAVE_HI3518V200_SUPPORT) \
	|| defined(HAVE_HI3520V100_SUPPORT) || defined(HAVE_HI3520DV200_SUPPORT)
	HimppMedia himpp_media(runtime, plat_args);
#endif

	mainloop.run();

	delete runtime;

	FT_Done_FreeType(freetype);

	Medium::close(rtspServer);

	env->reclaim();
	delete scheduler;

	return 0;
}

