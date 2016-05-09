/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-object.h
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

#ifndef _HIMPP_OBJECT_H_
#define _HIMPP_OBJECT_H_

#include <atomic>
#include "ipcam-media-iface.h"
#include <mpi_sys.h>

using namespace Ipcam::Media;

class HimppObject
{
public:
    HimppObject(HimppObject *source);
    ~HimppObject();

    virtual operator MPP_CHN_S* () = 0; // bind source only

    virtual bool enable();
    virtual bool disable();
    bool isEnabled();
    HimppObject *source() { return _source; }
protected:
    virtual bool enableObject() = 0;
    virtual bool disableObject() = 0;
private:
    std::atomic<int> enableCount;
    HimppObject *_source;
};

class HimppVideoObject : public HimppObject
{
public:
    HimppVideoObject(HimppVideoObject *base) : HimppObject(base) {}
    HimppVideoObject* videoSource() { return dynamic_cast<HimppVideoObject*>(source()); }
    virtual bool setResolution(ImageResolution &res) = 0;
    virtual ImageResolution getResolution() = 0;
    virtual bool setFramerate(uint32_t fps) = 0;
    virtual uint32_t getFramerate() = 0;
};

#endif // _HIMPP_OBJECT_H_

