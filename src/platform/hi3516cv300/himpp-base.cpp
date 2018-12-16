/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-element.cc
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

#include <mpi_sys.h>

#include "himpp-base.h"

HimppVideoElement::HimppVideoElement(HimppVideoElement* source)
  : VideoElement(VIDEO_ELEMENT(source))
{
}

MPP_CHN_S* HimppVideoElement::bindSource()
{
	return NULL;
}

HimppAudioElement::HimppAudioElement(HimppAudioElement* source)
  : AudioElement(AUDIO_ELEMENT(source))
{
}

MPP_CHN_S* HimppAudioElement::bindSource()
{
	return NULL;
}
