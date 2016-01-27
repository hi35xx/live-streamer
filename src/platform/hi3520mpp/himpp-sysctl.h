/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-sysctl.h
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

#ifndef _HIMPP_SYSCTL_H_
#define _HIMPP_SYSCTL_H_

#include <list>
#include <himpp-object.h>

#include <hi_comm_vb.h>
#include <hi_comm_sys.h>

#include <himpp-common.h>

class HimppSysctl: public HimppObject 
{
public:
    HimppSysctl(HI_U32 align_width = HIMPP_SYS_ALIGN_WIDTH, HI_U32 max_pool_cnt = VB_MAX_COMM_POOLS);
    ~HimppSysctl();
    void addVideoBuffer(HI_U32 blksiz, HI_U32 blkcnt);
    void setAlignWidth(HI_U32 value);
    void setMaxPoolCount(HI_U32 value);
protected:
    bool enableObject();
    bool disableObject();
    operator MPP_CHN_S* ();
private:
    struct VideoBuffer
    {
        VideoBuffer(HI_U32 blksiz, HI_U32 blkcnt)
            : blksiz(blksiz), blkcnt(blkcnt) {}
        HI_U32 blksiz;
        HI_U32 blkcnt;
    };
    HI_U32 alignWidth;
    HI_U32 maxPoolCount;
    std::list<VideoBuffer> buffers;
};

#endif // _HIMPP_SYSCTL_H_

