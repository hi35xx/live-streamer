/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <hi_comm_aio.h>
#include <hi_comm_aenc.h>
#include <mpi_aenc.h>

#include <himpp-base.h>
#include <audio-source.h>
#include <audio-encoder.h>
#include <media-stream.h>

using namespace Ipcam::Media;

class HimppAudioCodec;
class HimppAiDev;
class HimppAiChan;
class HimppAencChan;

#define HIMPP_AUDIO_CODEC(o)	dynamic_cast<HimppAudioCodec*>(o)
#define HIMPP_AI_DEV(o)			dynamic_cast<HimppAiDev*>(o)
#define HIMPP_AI_CHAN(o)		dynamic_cast<HimppAiChan*>(o)
#define HIMPP_AENC_CHAN(o)		dynamic_cast<HimppAencChan*>(o)

class HimppAudioCodec : public HimppAudioElement, public AudioSource 
{
public:
	HimppAudioCodec(AUDIO_SAMPLE_RATE_E sample_rate = AUDIO_SAMPLE_RATE_8000);
	~HimppAudioCodec();

	// implementation of AudioSource
	uint32_t getChannels();
	uint32_t getSampleRate();
	bool setSampleRate(uint32_t sample_rate);
protected:
	virtual void doEnableElement();
	virtual void doDisableElement();

private:
	AUDIO_SAMPLE_RATE_E _sample_rate;
};

class HimppAiDev : public HimppAudioElement, public AudioSource
{
public:
	HimppAiDev(HimppAudioElement* source, AUDIO_DEV devid);
	~HimppAiDev();

	// implementation of AudioSource
	uint32_t getChannels();

	AUDIO_DEV deviceId() { return _devid; }

protected:
	virtual void doEnableElement();
	virtual void doDisableElement();

private:
	AUDIO_DEV	   _devid;
	uint32_t		_sample_rate;
};

class HimppAiChan : public HimppAudioElement, public AudioSource
{
public:
	HimppAiChan(HimppAudioElement* source, AI_CHN chnid);
	~HimppAiChan();

	// implementation of AudioSource
	uint32_t getChannels();

	MPP_CHN_S* bindSource();

protected:
	virtual void doEnableElement();
	virtual void doDisableElement();

private:
	AI_CHN		_chnid;
	MPP_CHN_S	_mpp_chn;
};

class HimppAencChan :
	public HimppAudioElement,
	public AudioEncoder,
	public AudioStreamSource
{
public:
	HimppAencChan(HimppAudioElement* source, AudioEncodingType encoding, AENC_CHN chnid);
	~HimppAencChan();

	// implementation of HimppElement
	MPP_CHN_S* bindSource();

	// implementation of AudioStreamSource
	AudioEncodingType   encoding();
	uint32_t			bitrate();
	uint32_t			channels();
	uint32_t			samplerate();
	// implementation of StreamSource
	void				attach(StreamSink* sink);
	void				detach(StreamSink* sink);
	void				play();
	void				stop();
	void				resume();
	void				pause();

	// implementation of AudioEncoder
	AudioEncodingType	getEncoding();
	uint32_t			getBitrate();
	void				setBitrate(uint32_t value);

	AENC_CHN channelId() { return _chnid; }

protected:
	// implementation of MediaElement
	virtual void		doEnableElement();
	virtual void		doDisableElement();

private:
	ev::io				_io;
	AENC_CHN			_chnid;
	AudioEncodingType	_encoding;
	uint16_t			_bitrate;

	void watch_handler(ev::io& w, int revents);
};

#endif // _HIMPP_AUDIO_H_

