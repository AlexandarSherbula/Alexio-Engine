#include "aio_pch.hpp"
#include "MP3_Audio.hpp"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

namespace aio
{
	MP3_Audio::MP3_Audio(const std::filesystem::path& filepath)
	{
		mFilePath = filepath;

		mSource = CreateAudioSource();

		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = ma_format_f32;
		deviceConfig.playback.channels = mSource.channels;
		deviceConfig.sampleRate = mSource.sampleRate;
		deviceConfig.dataCallback = MP3_Audio::DataCallback;
		deviceConfig.pUserData = &mSource;

		ma_result result = ma_device_init(NULL, &deviceConfig, &mDevice);
		if (result != MA_SUCCESS)
		{
			AIO_LOG_ERROR("Failed to open playback device.");
		}
	}

	void MP3_Audio::SeekInSeconds(float seconds)
	{
		mSource.currentFrame = (uint32_t)(seconds * mSource.sampleRate);
		drmp3_seek_to_pcm_frame((drmp3*)mSource.decoder, mSource.currentFrame);
	}

	void MP3_Audio::SeekInMiliseconds(float miliseconds)
	{
		mSource.currentFrame = (uint32_t)((miliseconds / 1000.0f) * mSource.sampleRate);
		drmp3_seek_to_pcm_frame((drmp3*)mSource.decoder, mSource.currentFrame);
	}

	AudioSource MP3_Audio::CreateAudioSource()
	{
        drmp3_init_file(&mDecoder, mFilePath.string().c_str(), NULL);
		mSource.channels = mDecoder.channels;
		mSource.sampleRate = mDecoder.sampleRate;
		mSource.decoder = &mDecoder;

		return mSource;
	}

	void MP3_Audio::DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, drmp3_uint32 frameCount)
	{
        AudioSource* source = (AudioSource*)pDevice->pUserData;
        float* out = static_cast<float*>(pOutput);

        drmp3* mp3 = static_cast<drmp3*>(source->decoder);
        mp3->channels;
        mp3->sampleRate;

        drmp3_uint64 framesRead = drmp3_read_pcm_frames_f32(
            mp3,
            frameCount,
            out
        );

        for (drmp3_uint64 i = 0; i < framesRead * source->channels; ++i)
            out[i] *= source->volume;

        source->currentFrame += framesRead;

        if (source->endPoint != 0.0f)
        {
            drmp3_uint64 endFrame = static_cast<drmp3_uint64>(source->endPoint * source->sampleRate);

            if (source->currentFrame >= endFrame) {
                framesRead = 0;
            }

            drmp3_uint64 framesUntilEnd = endFrame - static_cast<drmp3_uint64>(source->currentFrame);
            if (framesUntilEnd < static_cast<drmp3_uint64>(frameCount)) {
                frameCount = framesUntilEnd;
            }
        }

        if (framesRead < frameCount)
        {
            if (source->isLooping)
            {
                uint64_t targetFrame = static_cast<uint64_t>(source->loopStartPoint * source->sampleRate);
                if (drmp3_seek_to_pcm_frame(mp3, targetFrame))
                {
                    source->currentFrame = targetFrame;

                    uint64_t remainingFrames = frameCount - framesRead;
                    uint64_t framesRead2 = drmp3_read_pcm_frames_f32(
                        mp3,
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


