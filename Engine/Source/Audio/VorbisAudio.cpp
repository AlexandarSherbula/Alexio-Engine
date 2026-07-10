#include "aio_pch.hpp"
#include "VorbisAudio.hpp"

#include "../../vendor/miniaudio/stb_vorbis.c"

namespace aio
{
	Vorbis_Audio::Vorbis_Audio(const std::filesystem::path& filepath)
	{
		mFilePath = filepath;

		mSource = CreateAudioSource();

		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = ma_format_s16;
		deviceConfig.playback.channels = mSource.channels;
		deviceConfig.sampleRate = mSource.sampleRate;
		deviceConfig.dataCallback = Vorbis_Audio::DataCallback;
		deviceConfig.pUserData = &mSource;

		ma_result result = ma_device_init(NULL, &deviceConfig, &mDevice);
		if (result != MA_SUCCESS)
		{
			printf("Failed to open playback device.\n");
		}
	}

	void Vorbis_Audio::SeekInSeconds(float seconds)
	{
		mSource.currentFrame = (uint32_t)(seconds * mSource.sampleRate);
		stb_vorbis_seek((stb_vorbis*)mSource.decoder, mSource.currentFrame);
	}

	void Vorbis_Audio::SeekInMiliseconds(float miliseconds)
	{
		mSource.currentFrame = (uint32_t)((miliseconds / 1000.0f) * mSource.sampleRate);
		stb_vorbis_seek((stb_vorbis*)mSource.decoder, mSource.currentFrame);
	}

	AudioSource Vorbis_Audio::CreateAudioSource()
	{
		STBVorbisError error = VORBIS__no_error;
		stb_vorbis* vorbis = stb_vorbis_open_filename(mFilePath.string().c_str(), (int32_t*)&error, NULL);
		stb_vorbis_info info = stb_vorbis_get_info(vorbis);

		mSource.channels = info.channels;
		mSource.sampleRate = info.sample_rate;
		mSource.decoder = vorbis;

		return mSource;
	}

	void Vorbis_Audio::DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		AudioSource* source = static_cast<AudioSource*>(pDevice->pUserData);
		int16_t* out = static_cast<int16_t*>(pOutput);

		uint64_t framesRead = stb_vorbis_get_samples_short_interleaved(
			static_cast<stb_vorbis*>(source->decoder),
			source->channels,
			out,
			static_cast<uint64_t>(frameCount) * source->channels
		);

		for (uint64_t i = 0; i < framesRead * source->channels; ++i)
			out[i] = static_cast<int16_t>(out[i] * source->volume);

		if (source->endPoint != 0.0f)
		{
			source->currentFrame = stb_vorbis_get_sample_offset(static_cast<stb_vorbis*>(source->decoder));
			uint64_t endFrame = static_cast<uint64_t>(source->endPoint * source->sampleRate);

			if (source->currentFrame >= endFrame)
			{
				framesRead = 0;
			}

			uint64_t framesUntilEnd = endFrame - source->currentFrame;
			if (framesUntilEnd < frameCount)
			{
				frameCount = framesUntilEnd;
			}
		}


		if (framesRead < frameCount) 
		{
			if (source->isLooping)
			{
				int32_t targetFrame = static_cast<int32_t>(source->loopStartPoint * source->sampleRate);
				stb_vorbis_seek(static_cast<stb_vorbis*>(source->decoder), targetFrame);

				uint64_t remainingFrames = frameCount - framesRead;
				uint64_t framesRead2 = stb_vorbis_get_samples_short_interleaved(
					static_cast<stb_vorbis*>(source->decoder),
					source->channels,
					out + framesRead * source->channels,
					remainingFrames * source->channels
				);
				framesRead += framesRead2;
			}
			
			memset(out + framesRead * source->channels, 0,
				(frameCount - framesRead) * source->channels * sizeof(short));
		}
	}
}


