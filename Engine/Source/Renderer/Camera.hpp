#pragma once

#include "Utils/Utilities.hpp"
#include "Utils/Math.hpp"
#include "Events/Events.hpp"
#include "Buffers.hpp"

namespace aio
{
	class Camera
	{
	public:
		Camera() = default;
	
		void SetProjection(Mat4x4 projection) { mProjection = projection; }
		Mat4x4& GetProjection()  { return mProjection; }
	protected:
		Mat4x4 mProjection;
	};
}