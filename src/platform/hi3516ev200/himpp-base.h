/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-base.h
 * Copyright (C) 2017 Watson Xu <xuhuashan@gmail.com>
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

#ifndef _HIMPP_ELEMENT_H_
#define _HIMPP_ELEMENT_H_

#include <mpi_sys.h>
#include <media-element.h>

using namespace Ipcam::Media;

class HimppVideoElement;
class HimppAudioElement;

#define HIMPP_VIDEO_ELEMENT(e)	dynamic_cast<HimppVideoElement*>(e)
#define HIMPP_AUDIO_ELEMENT(e)	dynamic_cast<HimppAudioElement*>(e)

class HimppVideoElement : virtual public VideoElement
{
public:
	HimppVideoElement(HimppVideoElement* source);
	virtual MPP_CHN_S* bindSource();
};

class HimppAudioElement : virtual public AudioElement
{
public:
	HimppAudioElement(HimppAudioElement* source);
	virtual MPP_CHN_S* bindSource();
};

#endif // _HIMPP_ELEMENT_H_

