#pragma once

#include "Audio.hpp"

#include "dr_flac.h"

namespace aio
{
	class FLAC_Audio : public Audio
	{
	public:
		FLAC_Audio(const std::filesystem::path& filepath);

		void SeekInSeconds(float seconds) override;
		void SeekInMiliseconds(float miliseconds) override;
	private:
		AudioSource CreateAudioSource() override;

		static void DataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
	};
}