#pragma once

#include "Audio.hpp"

namespace aio
{
	class Vorbis_Audio : public Audio
	{
	public:
		Vorbis_Audio(const std::filesystem::path& filepath);

		void SeekInSeconds(float seconds) override;
		void SeekInMiliseconds(float miliseconds) override;
	private:
		AudioSource CreateAudioSource() override;

		static void DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
	};
}

