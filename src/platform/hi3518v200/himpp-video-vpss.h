/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <ev++.h>
#include <mpi_vpss.h>
#include <media-common.h>
#include <himpp-base.h>
#include <video-source.h>

#include <himpp-video-isp.h>

class HimppVpssGroup;
class HimppVpssChan;

#define HIMPP_VPSS_GROUP(o) dynamic_cast<HimppVpssGroup*>(o)
#define HIMPP_VPSS_CHAN(o)  dynamic_cast<HimppVpssChan*>(o)

class HimppVpssGroup : public HimppVideoElement, public DefaultVideoSource
{
public:
	class Imaging : public DefaultVideoSource::Imaging
	{
	public:
		// NoiseReduction
		class NoiseReduction : public DefaultVideoSource::Imaging::NoiseReduction
		{
		public:
			NoiseReduction(Imaging& imaging);
			~NoiseReduction();
			// implemention of Ipcam::Media::VideoSource::Imaging::NoiseReduction
			VNRMode			getMode();
			void			setMode(VNRMode value);
			uint32_t		getLevel();
			void			setLevel(uint32_t value);
			NrParamTable&	getParamTable();
			void			setParamTable(NrParamTable& value);
		private:
			VNRMode				_mode;
			uint32_t			_level;
			NrParamTable		_param_table;
		};

	public:
		Imaging(HimppVpssGroup& group);
		~Imaging();

		// implementation of VideoSource::Imaging
		VideoSource::Imaging::NoiseReduction&	noisereduction();

	private:
		NoiseReduction		_noisereduction;
	};
public:
	HimppVpssGroup(HimppVideoElement* source, VPSS_GRP grpid);
	~HimppVpssGroup();

	// implementation of VideoSource
	VideoSource::Imaging&   imaging();

	VPSS_GRP groupId() { return _grpid; };

protected:
	void doEnableElement();
	void doDisableElement();

private:
	void				timeout_handler(ev::timer& w, int revents);
	void				enableNR(bool enable);
	void				enableAutoNR(bool enable);
	void				setNRStrength(int32_t YSFStr, int32_t YTFStr);

private:
	Imaging				_imaging;
	VPSS_GRP			_grpid;
	ev::timer			_timer;
	HimppVideoISP*		_isp;
};

class HimppVpssChan : public HimppVideoElement, public DefaultVideoSource 
{
public:
	HimppVpssChan(HimppVideoElement* source, VPSS_CHN chnid);
	~HimppVpssChan();

	// implementation of HimppVideoElement
	MPP_CHN_S* bindSource();

	// implementation of Ipcam::Media::VideoSource
	//uint32_t		getFrameRate();
	//void			setFrameRate(uint32_t value);
	//Resolution		getResolution();
	//void			setResolution(Resolution value);

protected:
	void doEnableElement();
	void doDisableElement();

private:
	enum VPSS_CHN_TYPE {
		VPSS_CHN_TYPE_PHY,
		VPSS_CHN_TYPE_BYPASS,
		VPSS_CHN_TYPE_EXT
	};
	VPSS_CHN			_chnid;
	VPSS_CHN_TYPE		_type;
	MPP_CHN_S			_mpp_chn;
};

#endif // _HIMPP_VIDEO_VPSS_GROUP_H_
