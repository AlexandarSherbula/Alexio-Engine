#pragma once
#include "Utils/Utilities.hpp"
#include "Utils/FileUtils.hpp"

#include "Renderer/Renderer.hpp"

namespace aio
{
	class Assets
	{
	public:
		template<typename T, typename ... Args>
		static Ref<T> Create(Args&& ... args)
		{
			return T::CreateAsset(std::forward<Args>(args)...);
		}

		template<typename T>
		static Ref<T> Get(const std::string& name)
		{
			return T::Get(name);
		}

		template<typename T>
		static void Add(const Ref<T>& asset, std::string name = "")
		{
			T::Add(asset, name);
		}
	};
}

