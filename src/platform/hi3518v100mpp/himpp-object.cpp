/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-object.cpp
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

#include <assert.h>
#include "himpp-object.h"


HimppObject::HimppObject(HimppObject *source)
    : _source(source), enableCount(0)
{
}

HimppObject::~HimppObject()
{
}

bool HimppObject::enable()
{
    assert(enableCount >= 0);

    if (enableCount++ == 0) {
        if (_source) {
            if (!_source->enable()) {
                enableCount--;
                return false;
            }
        }

        if (!enableObject()) {
            enableCount--;

            if (_source)
                _source->disable();

            return false;
        }
    }

    return true;
}

bool HimppObject::disable()
{
    assert(enableCount > 0);

    if (enableCount-- == 1) {
        if (!disableObject()) {
            enableCount++;
            return false;
        }

        if (_source) {
            if (!_source->disable()) {
                enableCount++;
                return false;
            }
        }
    }

    return true;
}

bool HimppObject::isEnabled()
{
    return (enableCount > 0);
}
