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

#if defined(HAVE_HI3518MPP_SUPPORT) || defined(HAVE_HI3520MPP_SUPPORT)
#include <himpp-media.h>
#endif

ev::default_loop mainloop;

static const char *IPCAM_SERVER_NAME = "ipcam.Media";

static const char* sensor_type = "ov9712";
static int rtsp_port = 554;
static DBusBusType bus_type = DBUS_BUS_SYSTEM;
static std::list<std::string> stream_dirs;

static void list_supported_sensors(void)
{
	std::cout << "RTSP/RTP stream server\n";
	std::cout << "Supported sensor:\n";
#ifdef HAVE_HI3518MPP_SUPPORT
	for (auto s : himpp_video_sensor_map) {
		HimppVideoSensor &sensor = s.second;
		ISP_IMAGE_ATTR_S *attr = sensor;
		std::string resolution = "["
			+ std::to_string(attr->u16Width) + "x" 
			+ std::to_string(attr->u16Height) + "@"
			+ std::to_string(attr->u16FrameRate)
			+ "]";
		printf("  %-10s %-16s: %s\n",
		       sensor.getSensorName().c_str(),
		       resolution.c_str(),
		       sensor.getModulePath().c_str());
	}
#endif
	std::cout << std::endl;
}

static void display_usage(char *cmd)
{
	const char *usage = \
		"RTSP/RTP stream server\n"
		"Options:\n"
		"  -h, --help                  Show help options\n"
		"  -B  --background            Run daemon in the background\n"
		"      --system                Use system message bus\n"
		"      --session               Use session message bus\n"
		"  -p, --rtsp-port=PORT        RTSP port\n"
		"  -F, --file-stream=PATH:LOC  Add path for file streams\n"
		"  -S, --sensor=SENSOR_TYPE    Choose video sensor type\n"
		"  -l, --list-sensor           List supported video sensors\n"
		"\n";
	std::cout << "Usage: " << cmd << " [options]...\n";
	std::cout << usage;
}

static const char *optString = "Bh?p:S:lF:";
static const struct option longOpts[] = {
	{ "background",  required_argument,  NULL,   'B' },
	{ "system",      no_argument,        NULL,    0  },
	{ "session",     no_argument,        NULL,    0  },
	{ "port",        required_argument,  NULL,   'p' },
	{ "sensor",	     required_argument,  NULL,   'S' },
	{ "list-sensor", no_argument,        NULL,   'l' },
	{ "file-stream", no_argument,        NULL,   'F' },
	{ "help",        no_argument,        NULL,   'h' },
	{ NULL,          no_argument,        NULL,    0  }
};

static void sigterm_handler(ev::sig &w, int revents)
{
	std::cout << std::endl;
	mainloop.break_loop(ev::ALL);
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> sv;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		sv.push_back(item);
	}
	return sv;
}

static DBus::Ev::BusDispatcher dispatcher;

int main(int argc, char *argv[])
{
	int opt = 0;
	int longIndex;

	setlocale(LC_ALL, "");

	opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
	while (opt != -1) {
		switch (opt) {
		case 'B':
			if (fork() != 0)
				return 0;
			break;
		case 'p':
			rtsp_port = strtoul(optarg, NULL, 0);
			break;
		case 'S':
			sensor_type = optarg;
			break;
		case 'l':
			list_supported_sensors();
			return 0;
		case 'F':
			stream_dirs.push_back(optarg);
			break;
		case 'h':
			display_usage(argv[0]);
			return 0;
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

	OutPacketBuffer::increaseMaxSizeTo(256 * 1024);

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

	std::list<std::string>::const_iterator it;
	for (it = stream_dirs.begin(); it != stream_dirs.end(); it++) {
		std::vector<std::string> sv = split(*it, ':');
		if (sv.size() != 2)
			continue;
		*env << "  add stream path " 
			<< sv[0].c_str() << " => "
			<< sv[1].c_str() << "\n";
		rtspServer->addStreamPath(sv[0], sv[1]);
	}

#if defined(HAVE_HI3518MPP_SUPPORT)
	Hi3518mppMedia hi3518media(runtime, sensor_type);
#endif

#if defined(HAVE_HI3520MPP_SUPPORT)
	Hi3520mppMedia hi3520media(runtime, sensor_type);
#endif

	mainloop.run();

	delete runtime;

	FT_Done_FreeType(freetype);

	Medium::close(rtspServer);

	env->reclaim();
	delete scheduler;

	return 0;
}

