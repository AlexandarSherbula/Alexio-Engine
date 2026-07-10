#include "aio_pch.hpp"
#include "FLAC_Audio.hpp"

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

namespace aio
{
	FLAC_Audio::FLAC_Audio(const std::filesystem::path& filepath)
	{
		mFilePath = filepath;

		mSource = CreateAudioSource();

		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = ma_format_f32;
		deviceConfig.playback.channels = mSource.channels;
		deviceConfig.sampleRate = mSource.sampleRate;
		deviceConfig.dataCallback = FLAC_Audio::DataCallback;
		deviceConfig.pUserData = &mSource;

		ma_result result = ma_device_init(NULL, &deviceConfig, &mDevice);
		if (result != MA_SUCCESS)
		{
			AIO_LOG_ERROR("Failed to open playback device.");
		}
	}

	void FLAC_Audio::SeekInSeconds(float seconds)
	{
		mSource.currentFrame = (uint32_t)(seconds * mSource.sampleRate);
		drflac_seek_to_pcm_frame((drflac*)mSource.decoder, mSource.currentFrame);
	}

	void FLAC_Audio::SeekInMiliseconds(float miliseconds)
	{
		mSource.currentFrame = (uint32_t)((miliseconds /1000.0f) * mSource.sampleRate);
		drflac_seek_to_pcm_frame((drflac*)mSource.decoder, mSource.currentFrame);
	}

	AudioSource FLAC_Audio::CreateAudioSource()
	{
		drflac* pFlac = drflac_open_file(mFilePath.string().c_str(), NULL);
		mSource.channels = pFlac->channels;
		mSource.sampleRate = pFlac->sampleRate;
		mSource.decoder = pFlac;
		return mSource;
	}

	void FLAC_Audio::DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
        AudioSource* source = (AudioSource*)pDevice->pUserData;
        float* out = static_cast<float*>(pOutput);

        drflac* flac = static_cast<drflac*>(source->decoder);
        flac->channels;
        flac->sampleRate;

        drflac_uint64 framesRead = drflac_read_pcm_frames_f32(
            flac,
            frameCount,
            out
        );

        for (drflac_uint64 i = 0; i < framesRead * source->channels; ++i)
            out[i] *= source->volume;

        source->currentFrame += framesRead;

        if (source->endPoint != 0.0f)
        {
            drflac_uint64 endFrame = static_cast<drflac_uint64>(source->endPoint * source->sampleRate);

            if (source->currentFrame >= endFrame) {
                framesRead = 0;
            }

            drflac_uint64 framesUntilEnd = endFrame - static_cast<drflac_uint64>(source->currentFrame);
            if (framesUntilEnd < static_cast<drflac_uint64>(frameCount)) {
                frameCount = framesUntilEnd;
            }
        }

        if (framesRead < frameCount)
        {
            if (source->isLooping)
            {
                uint64_t targetFrame = static_cast<uint64_t>(source->loopStartPoint * source->sampleRate);
                if (drflac_seek_to_pcm_frame(flac, targetFrame))
                {
                    source->currentFrame = targetFrame;

                    uint64_t remainingFrames = frameCount - framesRead;
                    uint64_t framesRead2 = drflac_read_pcm_frames_f32(
                        flac,
                        remainingFrames,
                        out + framesRead * source->channels
                    );
                    framesRead += framesRead2;
                    source->currentFrame += framesRead2;
                }
            }

            memset(out + framesRead * source->channels, 0,
                (frameCount - framesRead) * source->channels * sizeof(float));
        }
	}
}