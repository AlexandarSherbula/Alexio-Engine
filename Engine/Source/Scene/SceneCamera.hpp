#pragma once

#include "Renderer/Camera.hpp"

namespace aio
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;
	
		void SetOrthographic(float size);
	
		void SetViewportSize(float width, float height);
	
		float GetOrthographicSize() const { return mOrthographicSize; }
		void SetOrthographicSize(float size) { mOrthographicSize = size; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		float mOrthographicSize = 10.0f;
	
		float mAspectRatio = 0.0f;
	};
}