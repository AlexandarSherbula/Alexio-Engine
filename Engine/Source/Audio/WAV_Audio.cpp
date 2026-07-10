#include "aio_pch.hpp"
#include "WAV_Audio.hpp"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace aio
{
	WAV_Audio::WAV_Audio(const std::filesystem::path& filepath)
	{
		mFilePath = filepath;

		mSource = CreateAudioSource();

		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = ma_format_f32;
		deviceConfig.playback.channels = mSource.channels;
		deviceConfig.sampleRate = mSource.sampleRate;
		deviceConfig.dataCallback = WAV_Audio::DataCallback;
		deviceConfig.pUserData = &mSource;

		ma_result result = ma_device_init(NULL, &deviceConfig, &mDevice);
		if (result != MA_SUCCESS)
		{
			AIO_LOG_ERROR("Failed to open playback device.");
		}
	}

	void WAV_Audio::SeekInSeconds(float seconds)
	{
		mSource.currentFrame = (ma_uint64)(seconds * mSource.sampleRate);
		drwav_seek_to_pcm_frame((drwav*)mSource.decoder, mSource.currentFrame);
	}

	void WAV_Audio::SeekInMiliseconds(float miliseconds)
	{
		mSource.currentFrame = (uint32_t)((miliseconds / 1000.0f) * mSource.sampleRate);
		drwav_seek_to_pcm_frame((drwav*)mSource.decoder, mSource.currentFrame);
	}

	AudioSource WAV_Audio::CreateAudioSource()
	{
		drwav_init_file(&mDecoder, mFilePath.string().c_str(), NULL);

		mSource.channels = mDecoder.channels;
		mSource.sampleRate = mDecoder.sampleRate;
		mSource.decoder = &mDecoder;

		return mSource;
	}

	void WAV_Audio::DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
        AudioSource* source = (AudioSource*)pDevice->pUserData;
        float* out = static_cast<float*>(pOutput);

        drwav* wav = static_cast<drwav*>(source->decoder);
        wav->channels;
        wav->sampleRate;

        drwav_uint64 framesRead = drwav_read_pcm_frames_f32(
            wav,
            frameCount,
            out
        );

        for (drwav_uint64 i = 0; i < framesRead * source->channels; ++i)
            out[i] *= source->volume;

        source->currentFrame += framesRead;

        if (source->endPoint != 0.0f)
        {
            drwav_uint64 endFrame = static_cast<drwav_uint64>(source->endPoint * source->sampleRate);

            if (source->currentFrame >= endFrame) {
                framesRead = 0;
            }

            drwav_uint64 framesUntilEnd = endFrame - static_cast<drwav_uint64>(source->currentFrame);
            if (framesUntilEnd < static_cast<drwav_uint64>(frameCount)) {
                frameCount = framesUntilEnd;
            }
        }

        if (framesRead < frameCount)
        {
            if (source->isLooping)
            {
                uint64_t targetFrame = static_cast<uint64_t>(source->loopStartPoint * source->sampleRate);
                if (drwav_seek_to_pcm_frame(wav, targetFrame))
                {
                    source->currentFrame = targetFrame;

                    uint64_t remainingFrames = frameCount - framesRead;
                    uint64_t framesRead2 = drwav_read_pcm_frames_f32(
                        wav,
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