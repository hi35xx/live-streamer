/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-audio.h
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

#ifndef _HIMPP_AUDIO_H_
#define _HIMPP_AUDIO_H_

#include <himpp-object.h>
#include <hi_comm_aio.h>
#include <hi_comm_aenc.h>
#include <mpi_aenc.h>

#include <ipcam-media-iface.h>

using namespace Ipcam::Media;

class HimppAudioCodec : public HimppObject 
{
public:
    HimppAudioCodec(AUDIO_SAMPLE_RATE_E sample_rate = AUDIO_SAMPLE_RATE_8000);
    ~HimppAudioCodec();

    operator MPP_CHN_S* ();

    uint32_t getSampleRate();
    bool setSampleRate(uint32_t sample_rate);
protected:
    virtual bool enableObject();
    virtual bool disableObject();
private:
    AUDIO_SAMPLE_RATE_E _sample_rate;
};

class HimppAiDev : public HimppObject
{
public:
    HimppAiDev(HimppObject*, HimppAudioCodec*, AUDIO_DEV);
    ~HimppAiDev();

    operator MPP_CHN_S* ();

    AUDIO_DEV getDeviceId() { return _devid; }

    uint32_t getSampleRate();
    bool setSampleRate(uint32_t sample_rate);
protected:
    virtual bool enableObject();
    virtual bool disableObject();
private:
    HimppAudioCodec *_audio_codec;
    AUDIO_DEV       _devid;
    uint32_t        _sample_rate;
};

class HimppAiChan : public HimppObject
{
public:
    HimppAiChan(HimppAiDev*, AI_CHN);
    ~HimppAiChan();

    operator MPP_CHN_S* ();
protected:
    virtual bool enableObject();
    virtual bool disableObject();
private:
    AI_CHN      _chnid;
    MPP_CHN_S   _mpp_chn;
};

class HimppAencChan : public HimppObject
{
public:
    HimppAencChan(HimppAiChan *aichan, AENC_CHN chnid);
    ~HimppAencChan();

    operator MPP_CHN_S* ();

    AENC_CHN channelId() { return _chnid; }

    IAudioEncoder::EncodingType getEncoding();
    bool         setEncoding(IAudioEncoder::EncodingType encoding);
    uint32_t     getBitrate();
    bool         setBitrate(uint32_t bps);
protected:
    virtual bool enableObject();
    virtual bool disableObject();
private:
    AENC_CHN            _chnid;
    IAudioEncoder::EncodingType _encoding;
    uint16_t            _bps;
};

#endif // _HIMPP_AUDIO_H_

