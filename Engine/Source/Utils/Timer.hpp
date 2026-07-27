#pragma once

#include <chrono>

namespace aio
{
	class AppTimer
	{
	public:
		static void Start();
		static void Update();

		inline static float GetElapsedTime() { return sAppTime; }
		inline static float DeltaTime() { return sDeltaTime.count(); }
	private:
		static float sAppTime;
		static std::chrono::duration<float> sDeltaTime;
		static std::chrono::time_point<std::chrono::high_resolution_clock> sCurrentTime, sLastFrame;
	};
}