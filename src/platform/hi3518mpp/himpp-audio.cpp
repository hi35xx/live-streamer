/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <hi_comm_aio.h>
#include <mpi_ai.h>
#include <mpi_aenc.h>
#include <acodec.h>

#include "himpp-audio.h"

//////////////////////////////////////////////////////////////////////////////
// HimppAudioCodec
//////////////////////////////////////////////////////////////////////////////

#define ACODEC_FILE     "/dev/acodec"

#define AUDIO_PTNUMPERFRM   320
#define AUDIO_ADPCM_TYPE    ADPCM_TYPE_DVI4 // ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4
#define G726_BPS            MEDIA_G726_40K  // MEDIA_G726_16K, MEDIA_G726_24K ...

HimppAudioCodec::HimppAudioCodec(AUDIO_SAMPLE_RATE_E sample_rate)
    : HimppObject(NULL), _sample_rate(sample_rate)
{
}

HimppAudioCodec::~HimppAudioCodec()
{
}

HimppAudioCodec::operator MPP_CHN_S* ()
{
    return NULL;
}

uint32_t HimppAudioCodec::getSampleRate()
{
    return _sample_rate;
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

bool HimppAudioCodec::enableObject()
{
    int fd = open(ACODEC_FILE, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Cannot open acodec %s\n", ACODEC_FILE);
        return false;
    }

    if (ioctl(fd, ACODEC_SOFT_RESET_CTRL)) {
        fprintf(stderr, "Failed to reset audio codec");
        close(fd);
        return false;
    }

    unsigned int i2s_fs_sel = 0;
    switch (_sample_rate) {
    case AUDIO_SAMPLE_RATE_8000:
    case AUDIO_SAMPLE_RATE_11025:
    case AUDIO_SAMPLE_RATE_12000:
        i2s_fs_sel = 0x18;
        break;
    case AUDIO_SAMPLE_RATE_16000:
    case AUDIO_SAMPLE_RATE_22050:
    case AUDIO_SAMPLE_RATE_24000:
        i2s_fs_sel = 0x19;
        break;
    case AUDIO_SAMPLE_RATE_32000:
    case AUDIO_SAMPLE_RATE_44100:
    case AUDIO_SAMPLE_RATE_48000:
        i2s_fs_sel = 0x1a;
        break;
    default:
        fprintf(stderr, "Sample rate %d not support\n", _sample_rate);
        close(fd);
        return false;
    }

    if (ioctl(fd, ACODEC_SET_I2S1_FS, &i2s_fs_sel)) {
        fprintf(stderr, "set acodec sample rate failed\n");
        close(fd);
        return false;
    }

    unsigned int gain_mic = 0x16;
    if (ioctl(fd, ACODEC_SET_GAIN_MICL, &gain_mic)) {
        fprintf(stderr, "set acodec micin volume failed\n");
    }
    if (ioctl(fd, ACODEC_SET_GAIN_MICR, &gain_mic)) {
        fprintf(stderr, "set acodec micin volume failed\n");
    }

    ACODEC_VOL_CTRL vol_ctrl;
    vol_ctrl.vol_ctrl_mute = 0;
    vol_ctrl.vol_ctrl = 0x04;
    if (ioctl(fd, ACODEC_SET_ADCL_VOL, &vol_ctrl)) {
        fprintf(stderr, "set acodec adc volume failed\n");
    }
    if (ioctl(fd, ACODEC_SET_ADCR_VOL, &vol_ctrl)) {
        fprintf(stderr, "set acodec adc volume failed\n");
    }

    close (fd);

    return true;
}

bool HimppAudioCodec::disableObject()
{
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// HimppAiDev
//////////////////////////////////////////////////////////////////////////////

HimppAiDev::HimppAiDev(HimppAudioCodec *acodec, AUDIO_DEV devid)
    : HimppObject(acodec), _devid(devid),
      _sample_rate(AUDIO_SAMPLE_RATE_8000)
{
}

HimppAiDev::~HimppAiDev()
{
}

HimppAiDev::operator MPP_CHN_S* ()
{
    return NULL;
}

uint32_t HimppAiDev::getSampleRate()
{
    return _sample_rate;
}

bool HimppAiDev::setSampleRate(uint32_t sample_rate)
{
    switch (sample_rate) {
    case AUDIO_SAMPLE_RATE_8000:
    case AUDIO_SAMPLE_RATE_12000:
    case AUDIO_SAMPLE_RATE_11025:
    case AUDIO_SAMPLE_RATE_16000:
    case AUDIO_SAMPLE_RATE_22050:
    case AUDIO_SAMPLE_RATE_24000:
    case AUDIO_SAMPLE_RATE_32000:
    case AUDIO_SAMPLE_RATE_44100:
    case AUDIO_SAMPLE_RATE_48000:
        break;
    default:
        fprintf(stderr, "Unsupported audio sample rate %d\n", sample_rate);
        return false;
    }

    // set audio codec's sample rate first
    HimppAudioCodec *acodec = dynamic_cast<HimppAudioCodec*>(source());
    if (!acodec || !acodec->setSampleRate(sample_rate)) {
        fprintf(stderr, "Cannot set samplerate for audio codec\n");
        return false;
    }

    _sample_rate = sample_rate;

    return true;
}

bool HimppAiDev::enableObject()
{
    HI_S32 s32Ret;
    AIO_ATTR_S  aio_attr;

    aio_attr.enSamplerate = (AUDIO_SAMPLE_RATE_E)_sample_rate;
    aio_attr.enBitwidth = AUDIO_BIT_WIDTH_16;
    aio_attr.enWorkmode = AIO_MODE_I2S_MASTER;
    aio_attr.enSoundmode = AUDIO_SOUND_MODE_MONO;
    aio_attr.u32EXFlag = 1;
    aio_attr.u32FrmNum = 30;
    aio_attr.u32PtNumPerFrm = AUDIO_PTNUMPERFRM;
    aio_attr.u32ChnCnt = 2;
    aio_attr.u32ClkSel = 1;
    s32Ret = HI_MPI_AI_SetPubAttr(_devid, &aio_attr);
    if (s32Ret) {
        fprintf(stderr, "failed to set AI dev%d attr\n", _devid);
        return false;
    }

    s32Ret = HI_MPI_AI_Enable(_devid);
    if (s32Ret) {
        fprintf(stderr, "failed to enable AI dev%d\n", _devid);
        return false;
    }

    return true;
}

bool HimppAiDev::disableObject()
{
    HI_S32 s32Ret;

    if ((s32Ret = HI_MPI_AI_Disable(_devid)) != HI_SUCCESS) {
        fprintf(stderr, "failed to disable AI dev%d\n", _devid);
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// HimppAiChan
//////////////////////////////////////////////////////////////////////////////

HimppAiChan::HimppAiChan(HimppAiDev *aidev, AI_CHN chnid)
    : HimppObject(aidev), _chnid(chnid)
{
    _mpp_chn.enModId = HI_ID_AI;
    _mpp_chn.s32DevId = aidev->getDeviceId();
    _mpp_chn.s32ChnId = chnid;
}

HimppAiChan::~HimppAiChan()
{
}

HimppAiChan::operator MPP_CHN_S* ()
{
    return &_mpp_chn;
}

bool HimppAiChan::enableObject()
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_AI_EnableChn(_mpp_chn.s32DevId, _chnid);
    if (s32Ret != HI_SUCCESS) {
        fprintf(stderr, "failed to enable AI chn%d-%d\n", _mpp_chn.s32DevId, _chnid);
        return false;
    }

    return true;
}

bool HimppAiChan::disableObject()
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_AI_DisableChn(_mpp_chn.s32DevId, _chnid);
    if (s32Ret != HI_SUCCESS) {
        fprintf(stderr, "failed to disable AI chn%d-%d\n", _mpp_chn.s32DevId, _chnid);
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// HimppAencChan
//////////////////////////////////////////////////////////////////////////////

HimppAencChan::HimppAencChan(HimppAiChan *aichan, AENC_CHN chnid)
    : HimppObject(aichan), _chnid(chnid), _encoding(IAudioEncoder::G711A)
{
}

HimppAencChan::~HimppAencChan()
{
}

HimppAencChan::operator MPP_CHN_S* ()
{
    return NULL;
}

IAudioEncoder::EncodingType HimppAencChan::getEncoding()
{
    return _encoding;
}

bool HimppAencChan::setEncoding(IAudioEncoder::EncodingType encoding)
{
    switch (encoding) {
    case IAudioEncoder::ADPCM:
    case IAudioEncoder::LPCM:
    case IAudioEncoder::G711A:
    case IAudioEncoder::G711U:
    case IAudioEncoder::G726:
        _encoding = encoding;
        break;
    default:
        return false;
    }

    return true;
}

uint32_t HimppAencChan::getBitrate()
{
}

bool HimppAencChan::setBitrate(uint32_t bps)
{
}

bool HimppAencChan::enableObject()
{
    HI_S32 s32Ret;
    AENC_CHN_ATTR_S attr;
    union {
        AENC_ATTR_ADPCM_S   adpcm;
        AENC_ATTR_G711_S    g711;
        AENC_ATTR_G726_S    g726;
        AENC_ATTR_LPCM_S    lpcm;
    } enc_attr;

    attr.u32BufSize = 30;
    attr.pValue = &enc_attr;

    switch (_encoding) {
    case IAudioEncoder::ADPCM:
        attr.enType = PT_ADPCMA;
        enc_attr.adpcm.enADPCMType = AUDIO_ADPCM_TYPE;
        break;
    case IAudioEncoder::G711A:
        attr.enType = PT_G711A;
        enc_attr.g711.resv = 0;
        break;
    case IAudioEncoder::G711U:
        attr.enType = PT_G711U;
        enc_attr.g711.resv = 0;
        break;
    case IAudioEncoder::G726:
        attr.enType = PT_G726;
        enc_attr.g726.enG726bps = G726_BPS;
        break;
    case IAudioEncoder::LPCM:
        attr.enType = PT_LPCM;
        enc_attr.lpcm.resv = 0;
        break;
    }
    s32Ret = HI_MPI_AENC_CreateChn(_chnid, &attr);
    if (s32Ret != HI_SUCCESS) {
        fprintf(stderr, "failed to create VENC chn%d\n", _chnid);
        return false;
    }

    MPP_CHN_S dst_chn;
    dst_chn.enModId = HI_ID_AENC;
    dst_chn.s32DevId = 0;
    dst_chn.s32ChnId = _chnid;
    s32Ret = HI_MPI_SYS_Bind(*source(), &dst_chn);
    if (s32Ret != HI_SUCCESS) {
        fprintf(stderr, "HI_MPI_SYS_Bind AENC chn%d failed [%#x]\n",
                _chnid, s32Ret);
        goto err_destroy_chn;
    }

    return true;

err_destroy_chn:
    HI_MPI_AENC_DestroyChn(_chnid);

    return false;
}

bool HimppAencChan::disableObject()
{
    HI_S32 s32Ret;

    MPP_CHN_S dst_chn;
    dst_chn.enModId = HI_ID_AENC;
    dst_chn.s32DevId = 0;
    dst_chn.s32ChnId = _chnid;
    s32Ret = HI_MPI_SYS_UnBind(*source(), &dst_chn);
    if (s32Ret != HI_SUCCESS) {
        fprintf(stderr, "HI_MPI_SYS_UnBind AENC chn%d failed [%#x]\n",
                _chnid, s32Ret);
    }

    s32Ret = HI_MPI_AENC_DestroyChn(_chnid);
    if (s32Ret != HI_SUCCESS) {
        fprintf(stderr, "failed to destroy VENC chn%d\n", _chnid);
    }

    return true;
}
