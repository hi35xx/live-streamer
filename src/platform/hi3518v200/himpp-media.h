/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-media.h
 * Copyright (C) 2015 Watson Xu <xuhuashan@gmail.com>
 *
 * live-streamer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * live-streamer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _HIMPP_MEDIA_H_
#define _HIMPP_MEDIA_H_

#include <list>
#include <memory>
#include <himpp-sysctl.h>
#include <media-stream.h>
#include <ipcam-runtime.h>

using namespace Ipcam::Media;

class HimppMedia;

class HimppMedia
{
public:
	HimppMedia(IpcamRuntime* runtime, std::unordered_map<std::string, std::string>& args);
	~HimppMedia();

private:
	typedef std::shared_ptr<MediaElement>	MediaElementSPtr;
	std::list<MediaElementSPtr>	_elements;
	IpcamRuntime*				_runtime;
	HimppSysctl					_sysctl;
};

#endif // _HIMPP_MEDIA_H_
