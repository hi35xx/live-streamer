/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-video-vpss-group.h
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

#ifndef _HIMPP_VIDEO_VPSS_GROUP_H_
#define _HIMPP_VIDEO_VPSS_GROUP_H_

#include <himpp-object.h>
#include <mpi_vpss.h>

class HimppVpssGroup: public HimppVideoObject 
{
public:
    HimppVpssGroup(HimppVideoObject *, VPSS_GRP);
    ~HimppVpssGroup();

    //bool setSharpness(int32_t);

    operator MPP_CHN_S* ();
    bool setResolution(ImageResolution &);
    ImageResolution getResolution();
    bool setFramerate(uint32_t);
    uint32_t getFramerate();

    VPSS_GRP getGroupId() { return _grpid; };
protected:
    bool enableObject();
    bool disableObject();
private:
    VPSS_GRP            _grpid;
};

class HimppVpssChan : public HimppVideoObject 
{
public:
    HimppVpssChan(HimppVpssGroup *, VPSS_CHN);
    ~HimppVpssChan();

    operator MPP_CHN_S* ();
    bool setResolution(ImageResolution &);
    ImageResolution getResolution();
    bool setFramerate(uint32_t);
    uint32_t getFramerate();
protected:
    bool enableObject();
    bool disableObject();
private:
    enum VPSS_CHN_TYPE {
        VPSS_CHN_TYPE_PHY,
        VPSS_CHN_TYPE_BYPASS,
        VPSS_CHN_TYPE_EXT
    };
    VPSS_CHN            _chnid;
    VPSS_CHN_TYPE       _type;
    MPP_CHN_S           _mpp_chn;
    HimppVpssGroup*     _vpss_group;
    ImageResolution     _resolution;
    uint32_t            _framerate;
};

#endif // _HIMPP_VIDEO_VPSS_GROUP_H_

