/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * himpp-audio.cpp
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

#include <ev++.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <hi_comm_aio.h>
#include <mpi_sys.h>
#include <mpi_ai.h>
#include <mpi_aenc.h>
#include <mpi_ao.h>
#include <mpi_adec.h>
#include <acodec.h>

#include <himpp-common.h>
#include "himpp-audio.h"

//////////////////////////////////////////////////////////////////////////////
// HimppAudioCodec
//////////////////////////////////////////////////////////////////////////////

#define ACODEC_FILE	 "/dev/acodec"

#define AUDIO_PTNUMPERFRM   320
#define AUDIO_ADPCM_TYPE	ADPCM_TYPE_DVI4 // ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4
#define G726_BPS			MEDIA_G726_40K  // MEDIA_G726_16K, MEDIA_G726_24K ...


//////////////////////////////////////////////////////////////////////////////
// HimppAudioCodec
//////////////////////////////////////////////////////////////////////////////

HimppAudioCodec::HimppAudioCodec(AUDIO_SAMPLE_RATE_E sample_rate)
: AudioElement(NULL), HimppAudioElement(NULL), _sample_rate(sample_rate),
  _input_vol(56), _output_vol(0)
{
}

HimppAudioCodec::~HimppAudioCodec()
{
}

uint32_t HimppAudioCodec::getChannels()
{
	return 1;
}

uint32_t HimppAudioCodec::getSampleRate()
{
	return _sample_rate;
}

int32_t HimppAudioCodec::getInputVol()
{
	return _input_vol;
}

void HimppAudioCodec::setInputVol(int32_t value)
{
	_input_vol = value;
}

int32_t HimppAudioCodec::getOutputVol()
{
	return _output_vol;
}

void HimppAudioCodec::setOutputVol(int32_t value)
{
	_output_vol = value;
}

bool HimppAudioCodec::setSampleRate(uint32_t sample_rate)
{
	switch (sample_rate) {
	case AUDIO_SAMPLE_RATE_8000:
	case AUDIO_SAMPLE_RATE_11025:
	case AUDIO_SAMPLE_RATE_12000:
	case AUDIO_SAMPLE_RATE_16000:
	case AUDIO_SAMPLE_RATE_22050:
	case AUDIO_SAMPLE_RATE_24000:
	case AUDIO_SAMPLE_RATE_32000:
	case AUDIO_SAMPLE_RATE_44100:
	case AUDIO_SAMPLE_RATE_48000:
		_sample_rate = (AUDIO_SAMPLE_RATE_E)sample_rate;
		return true;
	}
	fprintf(stderr, "Sample rate %d not support\n", sample_rate);
	return false;
}

uint32_t HimppAudioCodec::samplerate()
{
	return _sample_rate;
}

uint32_t HimppAudioCodec::channels()
{
	return getChannels();
}

void HimppAudioCodec::doEnableElement()
{
	int fd = open(ACODEC_FILE, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Cannot open acodec %s\n", ACODEC_FILE);
		throw IpcamError("Cannot open audio codec");
	}

	if (ioctl(fd, ACODEC_SOFT_RESET_CTRL)) {
		fprintf(stderr, "Failed to reset audio codec");
		close(fd);
		throw IpcamError("Cannot reset audio codec");
	}

	unsigned int i2s_fs_sel = 0;
	switch (_sample_rate) {
	case AUDIO_SAMPLE_RATE_8000:
		i2s_fs_sel = ACODEC_FS_8000;
		break;
	case AUDIO_SAMPLE_RATE_11025:
		i2s_fs_sel = ACODEC_FS_11025;
		break;
	case AUDIO_SAMPLE_RATE_12000:
		i2s_fs_sel = ACODEC_FS_12000;
		break;
	case AUDIO_SAMPLE_RATE_16000:
		i2s_fs_sel = ACODEC_FS_16000;
		break;
	case AUDIO_SAMPLE_RATE_22050:
		i2s_fs_sel = ACODEC_FS_22050;
		break;
	case AUDIO_SAMPLE_RATE_24000:
		i2s_fs_sel = ACODEC_FS_24000;
		break;
	case AUDIO_SAMPLE_RATE_32000:
		i2s_fs_sel = ACODEC_FS_32000;
		break;
	case AUDIO_SAMPLE_RATE_44100:
		i2s_fs_sel = ACODEC_FS_44100;
		break;
	case AUDIO_SAMPLE_RATE_48000:
		i2s_fs_sel = 0x1a;
		break;
	default:
		fprintf(stderr, "Sample rate %d not support\n", _sample_rate);
		close(fd);
		throw IpcamError("Sample rate not support");
	}

	if (ioctl(fd, ACODEC_SET_I2S1_FS, &i2s_fs_sel)) {
		fprintf(stderr, "set acodec sample rate failed\n");
		close(fd);
		throw IpcamError("Cannot set sample rate");
	}

	// select IN or IN_Difference
	ACODEC_MIXER_E input_mode = ACODEC_MIXER_IN;
	if (ioctl(fd, ACODEC_SET_MIXER_MIC, &input_mode)) {
		fprintf(stderr, "set acodec input mode failed\n");
		close(fd);
		throw IpcamError("Cannot set input mode");
	}

#if 0
	unsigned int gain_mic = 0x0A;
	if (ioctl(fd, ACODEC_SET_GAIN_MICL, &gain_mic)) {
		fprintf(stderr, "set acodec micin volume failed\n");
	}
	if (ioctl(fd, ACODEC_SET_GAIN_MICR, &gain_mic)) {
		fprintf(stderr, "set acodec micin volume failed\n");
	}

	ACODEC_VOL_CTRL vol_ctrl;
	vol_ctrl.vol_ctrl_mute = 0;
	vol_ctrl.vol_ctrl = 0x40;
	if (ioctl(fd, ACODEC_SET_ADCL_VOL, &vol_ctrl)) {
		fprintf(stderr, "set acodec adc volume failed\n");
	}
	if (ioctl(fd, ACODEC_SET_ADCR_VOL, &vol_ctrl)) {
		fprintf(stderr, "set acodec adc volume failed\n");
	}
#endif

	int input_vol = _input_vol;
	if (ioctl(fd, ACODEC_SET_INPUT_VOL, &input_vol)) {
		fprintf(stderr, "set acodec micin volume failed\n");
	}

	int output_vol = _output_vol;
	if (ioctl(fd, ACODEC_SET_OUTPUT_VOL, &output_vol)) {
		fprintf(stderr, "set acodec output volume failed\n");
	}

	close (fd);
}

void HimppAudioCodec::doDisableElement()
{
}


//////////////////////////////////////////////////////////////////////////////
// HimppAiDev
//////////////////////////////////////////////////////////////////////////////

HimppAiDev::HimppAiDev(HimppAudioElement* source, AUDIO_DEV devid)
  : AudioElement(AUDIO_ELEMENT(source)),
    HimppAudioElement(source), _devid(devid)
{
}

HimppAiDev::~HimppAiDev()
{
}

uint32_t HimppAiDev::getChannels()
{
	return 1;
}

void HimppAiDev::doEnableElement()
{
	HI_S32 s32Ret;
	AIO_ATTR_S  aio_attr;

	aio_attr.enSamplerate = (AUDIO_SAMPLE_RATE_E)samplerate();
	aio_attr.enBitwidth = AUDIO_BIT_WIDTH_16;
	aio_attr.enWorkmode = AIO_MODE_I2S_MASTER;
	aio_attr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	aio_attr.u32EXFlag = 0;
	aio_attr.u32FrmNum = 30;
	aio_attr.u32PtNumPerFrm = AUDIO_PTNUMPERFRM;
	aio_attr.u32ChnCnt = 1;
	aio_attr.u32ClkSel = 0;
	s32Ret = HI_MPI_AI_SetPubAttr(_devid, &aio_attr);
	if (s32Ret) {
		fprintf(stderr, "failed to set AI dev%d attr [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to set aidev attr");
	}

	s32Ret = HI_MPI_AI_Enable(_devid);
	if (s32Ret) {
		fprintf(stderr, "failed to enable AI dev%d [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to enable aidev");
	}
}

void HimppAiDev::doDisableElement()
{
	HI_S32 s32Ret;

	if ((s32Ret = HI_MPI_AI_Disable(_devid)) != HI_SUCCESS) {
		fprintf(stderr, "failed to disable AI dev%d\n", _devid);
	}
}

//////////////////////////////////////////////////////////////////////////////
// HimppAiChan
//////////////////////////////////////////////////////////////////////////////

HimppAiChan::HimppAiChan(HimppAudioElement* source, AI_CHN chnid)
  : AudioElement(AUDIO_ELEMENT(source)),
    HimppAudioElement(source), _chnid(chnid)
{
	HimppAiDev* aidev = HIMPP_AI_DEV(source);
	_mpp_chn.enModId = HI_ID_AI;
	_mpp_chn.s32DevId = aidev->deviceId();
	_mpp_chn.s32ChnId = chnid;
}

HimppAiChan::~HimppAiChan()
{
}

uint32_t HimppAiChan::getChannels()
{
	return 1;
}

MPP_CHN_S* HimppAiChan::bindSource()
{
	return &_mpp_chn;
}

void HimppAiChan::doEnableElement()
{
	HI_S32 s32Ret;

	s32Ret = HI_MPI_AI_EnableChn(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "failed to enable AI chn%d-%d\n", _mpp_chn.s32DevId, _chnid);
		throw IpcamError("Failed to enable AI chn");
	}

#if 0
	s32Ret = HI_MPI_AI_EnableReSmp(_mpp_chn.s32DevId, _chnid, AUDIO_SAMPLE_RATE_8000);
	if (s32Ret) {
		fprintf(stderr, "HI_MPI_AI_EnableReSmp(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}
#endif

#if 0
	AI_VQE_CONFIG_S attr;
	memset(&attr, 0, sizeof(attr));
	attr.s32WorkSampleRate    = samplerate();
	attr.s32FrameSample       = AUDIO_PTNUMPERFRM;
	attr.enWorkstate          = VQE_WORKSTATE_COMMON;
	attr.bAecOpen             = HI_FALSE;
	attr.bAgcOpen             = HI_TRUE;
	attr.stAgcCfg.bUsrMode    = HI_FALSE;
	attr.bAnrOpen             = HI_TRUE;
	attr.stAnrCfg.bUsrMode    = HI_FALSE;
	attr.bHpfOpen             = HI_TRUE;
	attr.stHpfCfg.bUsrMode    = HI_TRUE;
	attr.stHpfCfg.enHpfFreq   = AUDIO_HPF_FREQ_150;
	attr.bRnrOpen             = HI_FALSE;
	attr.bEqOpen              = HI_FALSE;
	attr.bHdrOpen             = HI_FALSE;
	attr.stHdrCfg.bUsrMode    = HI_FALSE;
	s32Ret = HI_MPI_AI_SetVqeAttr(_mpp_chn.s32DevId, _chnid, 0, 0, &attr);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AI_SetVqeAttr(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}

	s32Ret = HI_MPI_AI_EnableVqe(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AI_EnableVqe(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}
#endif
}

void HimppAiChan::doDisableElement()
{
	HI_S32 s32Ret;

	s32Ret = HI_MPI_AI_DisableVqe(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AI_DisableVqe(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}

	s32Ret = HI_MPI_AI_DisableChn(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AI_DisableChn(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}
}

//////////////////////////////////////////////////////////////////////////////
// HimppAencChan
//////////////////////////////////////////////////////////////////////////////

extern ev::default_loop mainloop;

HimppAencChan::HimppAencChan
(HimppAudioElement* source, AudioEncodingType encoding, AENC_CHN chnid)
  : AudioElement(AUDIO_ELEMENT(source)), HimppAudioElement(source),
    _io(mainloop), _chnid(chnid), _encoding(encoding), _bitrate(64)
{
}

HimppAencChan::~HimppAencChan()
{
}

MPP_CHN_S* HimppAencChan::bindSource()
{
	return NULL;
}

void HimppAencChan::watch_handler(ev::io& w, int revents)
{
	if (!(revents & ev::READ))
		return;

	int chnid = channelId();

	AUDIO_STREAM_S stStream;
	HI_S32 s32Ret = HI_MPI_AENC_GetStream(chnid, &stStream, 0);
	if (HI_SUCCESS != s32Ret) {
		HIMPP_PRINT("Get audio stream %d failed [%#x]\n", chnid, s32Ret);
		return;
	}

	if (stStream.u32Len > 4) {
		StreamBuffer::Pack stream_data_pack;
		memset(&stream_data_pack, 0, sizeof(stream_data_pack));

		StreamBuffer buffer;
		buffer.pack_count = 1;
		buffer.pack = &stream_data_pack;
		gettimeofday(&buffer.tstamp, NULL);

		// remove the hisilicon header.
		buffer.pack[0].addr = stStream.pStream + 4;
		buffer.pack[0].len = stStream.u32Len - 4;

		streamData(&buffer);
	}

	s32Ret = HI_MPI_AENC_ReleaseStream(chnid, &stStream);
	if (HI_SUCCESS != s32Ret) {
		HIMPP_PRINT("Release audio stream %d failed [%#x]\n", chnid, s32Ret);
	}
}

AudioEncodingType HimppAencChan::encoding()
{
	return _encoding;
}

uint32_t HimppAencChan::bitrate()
{
	return _bitrate;
}

uint32_t HimppAencChan::channels()
{
	return HIMPP_AUDIO_ELEMENT(source())->channels();
}

uint32_t HimppAencChan::samplerate()
{
	return HIMPP_AUDIO_ELEMENT(source())->samplerate();
}

void HimppAencChan::attach(StreamSink* sink)
{
	AudioStreamSource::attach(sink);
}

void HimppAencChan::detach(StreamSink* sink)
{
	AudioStreamSource::detach(sink);
}

void HimppAencChan::play()
{
	enable();
}

void HimppAencChan::stop()
{
	_io.stop();
	disable();
}

void HimppAencChan::resume()
{
	if (!is_enabled()) return;

	_io.set<HimppAencChan, &HimppAencChan::watch_handler>(this);
	_io.set(HI_MPI_AENC_GetFd(channelId()), ev::READ);
	_io.start();
}

void HimppAencChan::pause()
{
	_io.stop();
}


AudioEncodingType HimppAencChan::getEncoding()
{
	return _encoding;
}

uint32_t HimppAencChan::getBitrate()
{
	return _bitrate;
}

void HimppAencChan::setBitrate(uint32_t value)
{
	_bitrate = value;
}

void HimppAencChan::doEnableElement()
{
	HI_S32 s32Ret;
	AENC_CHN_ATTR_S attr;
	union {
		AENC_ATTR_ADPCM_S   adpcm;
		AENC_ATTR_G711_S	g711;
		AENC_ATTR_G726_S	g726;
		AENC_ATTR_LPCM_S	lpcm;
	} enc_attr;

	attr.u32PtNumPerFrm = AUDIO_PTNUMPERFRM;
	attr.u32BufSize = 30;
	attr.pValue = &enc_attr;

	switch (_encoding) {
	case ADPCM:
		attr.enType = PT_ADPCMA;
		enc_attr.adpcm.enADPCMType = AUDIO_ADPCM_TYPE;
		break;
	case G711A:
		attr.enType = PT_G711A;
		break;
	case G711U:
		attr.enType = PT_G711U;
		break;
	case G726:
		attr.enType = PT_G726;
		enc_attr.g726.enG726bps = G726_BPS;
		break;
	case LPCM:
	case AAC: // will be encoded using AACAudioEncoder filter
		attr.enType = PT_LPCM;
		break;
	}
	s32Ret = HI_MPI_AENC_CreateChn(_chnid, &attr);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "failed to create AENC chn%d\n", _chnid);
		throw IpcamError("Failed to create AENC channel");
	}

	MPP_CHN_S dst_chn;
	dst_chn.enModId = HI_ID_AENC;
	dst_chn.s32DevId = 0;
	dst_chn.s32ChnId = _chnid;
	HimppAudioElement* asrc = HIMPP_AUDIO_ELEMENT(source());
	s32Ret = HI_MPI_SYS_Bind(asrc->bindSource(), &dst_chn);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_SYS_Bind AENC chn%d failed [%#x]\n",
				_chnid, s32Ret);
		goto err_destroy_chn;
	}

	return;

err_destroy_chn:
	HI_MPI_AENC_DestroyChn(_chnid);

	throw IpcamError("Failed to bind audio source");
}

void HimppAencChan::doDisableElement()
{
	HI_S32 s32Ret;

	MPP_CHN_S dst_chn;
	dst_chn.enModId = HI_ID_AENC;
	dst_chn.s32DevId = 0;
	dst_chn.s32ChnId = _chnid;
	HimppAudioElement* asrc = HIMPP_AUDIO_ELEMENT(source());
	s32Ret = HI_MPI_SYS_UnBind(asrc->bindSource(), &dst_chn);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_SYS_UnBind AENC chn%d failed [%#x]\n",
				_chnid, s32Ret);
	}

	s32Ret = HI_MPI_AENC_DestroyChn(_chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "failed to destroy AENC chn%d\n", _chnid);
	}
}


//////////////////////////////////////////////////////////////////////////////
// HimppAoDev
//////////////////////////////////////////////////////////////////////////////

HimppAoDev::HimppAoDev(HimppAudioElement* source, AUDIO_DEV devid)
: AudioElement(AUDIO_ELEMENT(source)),
  HimppAudioElement(source),
  _devid(devid)
{
}

HimppAoDev::~HimppAoDev()
{
}

void HimppAoDev::doEnableElement()
{
	HI_S32 s32Ret;
	AIO_ATTR_S  aio_attr;

	aio_attr.enSamplerate = (AUDIO_SAMPLE_RATE_E)samplerate();
	aio_attr.enBitwidth = AUDIO_BIT_WIDTH_16;
	aio_attr.enWorkmode = AIO_MODE_I2S_MASTER;
	aio_attr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	aio_attr.u32EXFlag = 0;
	aio_attr.u32FrmNum = 30;
	aio_attr.u32PtNumPerFrm = AUDIO_PTNUMPERFRM;
	aio_attr.u32ChnCnt = 1;
	aio_attr.u32ClkSel = 0;
	s32Ret = HI_MPI_AO_SetPubAttr(_devid, &aio_attr);
	if (s32Ret) {
		fprintf(stderr, "failed to set AO dev%d attr [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to set aodev attr");
	}

	s32Ret = HI_MPI_AO_Enable(_devid);
	if (s32Ret) {
		fprintf(stderr, "failed to enable AO dev%d [%#x]\n", _devid, s32Ret);
		throw IpcamError("Failed to enable aodev");
	}
}

void HimppAoDev::doDisableElement()
{
	HI_S32 s32Ret;

	if ((s32Ret = HI_MPI_AO_Disable(_devid)) != HI_SUCCESS) {
		fprintf(stderr, "failed to disable AO dev%d\n", _devid);
	}
}


//////////////////////////////////////////////////////////////////////////////
// HimppAoChan
//////////////////////////////////////////////////////////////////////////////

HimppAoChan::HimppAoChan(HimppAudioElement* source, AO_CHN chnid)
  : AudioElement(AUDIO_ELEMENT(source)),
    HimppAudioElement(source), _chnid(chnid)
{
	HimppAoDev* aidev = HIMPP_AO_DEV(source);
	_mpp_chn.enModId = HI_ID_AO;
	_mpp_chn.s32DevId = aidev->deviceId();
	_mpp_chn.s32ChnId = chnid;
}

HimppAoChan::~HimppAoChan()
{
}

MPP_CHN_S* HimppAoChan::bindSource()
{
	return &_mpp_chn;
}

void HimppAoChan::doEnableElement()
{
	HI_S32 s32Ret;

	s32Ret = HI_MPI_AO_EnableChn(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "failed to enable AO chn%d-%d\n", _mpp_chn.s32DevId, _chnid);
		throw IpcamError("Failed to enable aochn");
	}

#if 0
	s32Ret = HI_MPI_AO_EnableReSmp(_mpp_chn.s32DevId, _chnid, AUDIO_SAMPLE_RATE_8000);
	if (s32Ret) {
		fprintf(stderr, "HI_MPI_AI=O_EnableReSmp(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}
#endif

#if 0
	AI_TALKVQE_CONFIG_S attr;
	memset(&attr, 0, sizeof(attr));
	attr.u32OpenMask = \
		AI_TALKVQE_MASK_HPF | AI_TALKVQE_MASK_AEC | \
		AI_TALKVQE_MASK_AGC | /*AI_TALKVQE_MASK_HDR |*/ \
		AI_TALKVQE_MASK_AGC | AI_TALKVQE_MASK_EQ | \
		AI_TALKVQE_MASK_ANR;
	attr.s32WorkSampleRate    = samplerate();
	attr.s32FrameSample       = AUDIO_PTNUMPERFRM;
	attr.enWorkstate          = VQE_WORKSTATE_COMMON;
	attr.stAgcCfg.bUsrMode    = HI_FALSE;
	attr.stAnrCfg.bUsrMode    = HI_FALSE;
	attr.stHpfCfg.bUsrMode    = HI_FALSE;
	s32Ret = HI_MPI_AI_SetTalkVqeAttr(_mpp_chn.s32DevId, _chnid, 0, 0, &attr);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AI_SetVqeAttr(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}

	s32Ret = HI_MPI_AO_EnableVqe(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AI_EnableVqe(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}
#endif
}

void HimppAoChan::doDisableElement()
{
	HI_S32 s32Ret;

	s32Ret = HI_MPI_AO_DisableVqe(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AO_DisableVqe(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}

	s32Ret = HI_MPI_AO_DisableChn(_mpp_chn.s32DevId, _chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_AO_DisableChn(%d,%d) failed [%#x]\n",
		        _mpp_chn.s32DevId, _chnid, s32Ret);
	}
}


//////////////////////////////////////////////////////////////////////////////
// HimppAdecChan
//////////////////////////////////////////////////////////////////////////////

HimppAdecChan::HimppAdecChan
(HimppAudioElement* source, AudioEncodingType encoding, ADEC_CHN chnid)
: AudioElement(AUDIO_ELEMENT(source)), HimppAudioElement(source),
  _chnid(chnid), _encoding(encoding)

{
}

HimppAdecChan::~HimppAdecChan()
{
}

MPP_CHN_S* HimppAdecChan::bindSource()
{
}

void HimppAdecChan::doEnableElement()
{
	HI_S32 s32Ret;
	ADEC_CHN_ATTR_S attr;
	union {
		ADEC_ATTR_ADPCM_S   adpcm;
		ADEC_ATTR_G711_S	g711;
		ADEC_ATTR_G726_S	g726;
		ADEC_ATTR_LPCM_S	lpcm;
	} dec_attr;

	attr.u32BufSize = 20;
	attr.pValue = &dec_attr;
	attr.enMode = ADEC_MODE_STREAM;

	switch (_encoding) {
	case ADPCM:
		attr.enType = PT_ADPCMA;
		dec_attr.adpcm.enADPCMType = AUDIO_ADPCM_TYPE;
		break;
	case G711A:
		attr.enType = PT_G711A;
		break;
	case G711U:
		attr.enType = PT_G711U;
		break;
	case G726:
		attr.enType = PT_G726;
		dec_attr.g726.enG726bps = G726_BPS;
		break;
	case LPCM:
		attr.enType = PT_LPCM;
		attr.enMode = ADEC_MODE_PACK;
		break;
	}
	s32Ret = HI_MPI_ADEC_CreateChn(_chnid, &attr);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "failed to create ADEC chn%d\n", _chnid);
		throw IpcamError("Failed to create ADEC channel");
	}

	MPP_CHN_S src_chn;
	src_chn.enModId = HI_ID_ADEC;
	src_chn.s32DevId = 0;
	src_chn.s32ChnId = _chnid;
	HimppAudioElement* asrc = HIMPP_AUDIO_ELEMENT(source());
	s32Ret = HI_MPI_SYS_Bind(&src_chn, asrc->bindSource());
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_SYS_Bind ADEC chn%d failed [%#x]\n",
				_chnid, s32Ret);
		goto err_destroy_chn;
	}

	return;

err_destroy_chn:
	HI_MPI_ADEC_DestroyChn(_chnid);

	throw IpcamError("Failed to bind audio source");
}

void HimppAdecChan::doDisableElement()
{
	HI_S32 s32Ret;

	MPP_CHN_S src_chn;
	src_chn.enModId = HI_ID_AENC;
	src_chn.s32DevId = 0;
	src_chn.s32ChnId = _chnid;
	HimppAudioElement* asrc = HIMPP_AUDIO_ELEMENT(source());
	s32Ret = HI_MPI_SYS_UnBind(&src_chn, asrc->bindSource());
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "HI_MPI_SYS_UnBind ADEC chn%d failed [%#x]\n",
				_chnid, s32Ret);
	}

	s32Ret = HI_MPI_ADEC_DestroyChn(_chnid);
	if (s32Ret != HI_SUCCESS) {
		fprintf(stderr, "failed to destroy AENC chn%d\n", _chnid);
	}
}

AudioEncodingType HimppAdecChan::encoding()
{
	return _encoding;
}

uint32_t HimppAdecChan::bitrate()
{
	return 64;
}

uint32_t HimppAdecChan::channels()
{
	return 1;
}

uint32_t HimppAdecChan::samplerate()
{
	return 8000;
}

void HimppAdecChan::streamData(StreamBuffer* buffer)
{
	AUDIO_STREAM_S stStream;
	HI_S32 s32Ret;

	for (int i = 0; i < (int)buffer->pack_count; i++) {
		uint8_t* addr = buffer->pack[i].addr;
		uint32_t len = buffer->pack[i].len;
		uint8_t frame[len + 4];
		if (_encoding == LPCM) {
			stStream.pStream = addr;
			stStream.u32Len = len;
		} else {
			*((uint16_t*)&frame[0]) = 0x0100;
			*((uint16_t*)&frame[2]) = len / 2;
			memmove(&frame[4], addr, len);
			stStream.pStream = frame;
			stStream.u32Len = len + 4;
		}
		s32Ret = HI_MPI_ADEC_SendStream(_chnid, &stStream, HI_TRUE);
		if (s32Ret != HI_SUCCESS) {
			fprintf(stderr, "HI_MPI_ADEC_SendFrame(%d) failed with %#x\n",
			        _chnid, s32Ret);
			break;
		}
	}
}

void HimppAdecChan::play()
{
	enable();
}

void HimppAdecChan::stop()
{
	disable();
}

void HimppAdecChan::pause()
{
}

void HimppAdecChan::resume()
{
}
