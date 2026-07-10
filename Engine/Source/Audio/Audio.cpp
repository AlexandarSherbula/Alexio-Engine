#include "aio_pch.hpp"
#include "Audio.hpp"

#include "VorbisAudio.hpp"
#include "MP3_Audio.hpp"
#include "WAV_Audio.hpp"
#include "FLAC_Audio.hpp"

#include "Utils/FileReading.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace aio
{
	std::unordered_map<std::string, Ref<Audio>> Audio::sAudios;

	Ref<Audio> Audio::Create(const std::filesystem::path& filepath)
	{
		if (std::filesystem::exists(filepath))
		{
			if (filepath.extension() == ".ogg")
				return CreateRef<Vorbis_Audio>(filepath);
			else if (filepath.extension() == ".mp3")
				return std::make_shared<MP3_Audio>(filepath);
			else if (filepath.extension() == ".wav")
				return std::make_shared<WAV_Audio>(filepath);
			else if (filepath.extension() == ".flac")
				return std::make_shared<FLAC_Audio>(filepath);

			AIO_LOG_ERROR("Unsupported audio file type: {0}", filepath.extension().string());
		}
		else
		{
			AIO_LOG_ERROR("File or Filepath '{0}' not found", filepath.string());
		}
		
		return nullptr;
	}

	Ref<Audio> Audio::CreateAsset(const std::string& audioFile, std::string name)
	{
		std::filesystem::path audioFilePath = ASSETS_DIRECTORY / "audio" / audioFile;

		if (name == "")
			name = GetFileName(audioFilePath);

		auto audio = Create(audioFilePath);
		Audio::Add(audio, name);
		return audio;
	}

	Ref<Audio> Audio::Get(const std::string& name)
	{
		AIO_ASSERT(Exists(name), "Audio doesn't exist!");
		return sAudios[name];
	}

	void Audio::Add(const Ref<Audio>& audio, const std::string& name)
	{
		AIO_ASSERT(!Exists(name), "Audio already exists!");
		sAudios[name] = audio;
	}

	bool Audio::Exists(const std::string& name)
	{
		return sAudios.find(name) != sAudios.end();
	}

	Audio::~Audio()
	{
		ma_device_uninit(&mDevice);
	}

	void Audio::Play()
	{
		ma_device_start(&mDevice);
	}

	void Audio::Stop()
	{
		ma_device_stop(&mDevice);
	}

	void Audio::LoopInSeconds(float startPoint, float endPoint)
	{
		mSource.isLooping = true;
		mSource.loopStartPoint = startPoint;

		if (endPoint != 0.0f)
		{
			mSource.endPoint = endPoint;
		}
	}

	void Audio::LoopInMiliseconds(float startPoint, float endPoint)
	{
		mSource.isLooping = true;
		mSource.loopStartPoint = startPoint / 1000.0f;

		if (endPoint != 0.0f)
		{
			mSource.endPoint = endPoint / 1000.0f;
		}
	}

	void Audio::SetVolume(float volume)
	{
		if (this != nullptr)
		{
			if (volume > 1.0f)
				mSource.volume = 1.0f;
			else if (volume < 0.0f)
				mSource.volume = 0.0f;

			mSource.volume = volume;
		}
	}
}


