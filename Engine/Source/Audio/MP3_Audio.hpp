#pragma once

#include "Audio.hpp"

#include "dr_mp3.h"

namespace aio
{
	class MP3_Audio : public Audio
	{
	public:
		MP3_Audio(const std::filesystem::path& filepath);

		void SeekInSeconds(float seconds) override;
		void SeekInMiliseconds(float miliseconds) override;
	private:
		AudioSource CreateAudioSource() override;

		static void DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, drmp3_uint32 frameCount);
	private:
		drmp3 mDecoder;
	};
}
