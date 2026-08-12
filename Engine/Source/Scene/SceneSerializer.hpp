#pragma once

#include "Utils/Utilities.hpp"

#include <filesystem>

namespace aio
{
	class Scene;

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::filesystem::path& filepath);
		void Deserialize(const std::filesystem::path& filepath);

	private:
		Ref<Scene> mScene;
	};
}