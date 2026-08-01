#include "aio_pch.hpp"
#include "SceneCamera.hpp"

namespace aio
{
	SceneCamera::SceneCamera()
	{
		mOrthographicSize = 10.0f;
		mAspectRatio = 0.0f;
		RecalculateProjection();
	}
	
	void SceneCamera::SetOrthographic(float size)
	{
		mOrthographicSize = size;
		RecalculateProjection();
	}
	
	void SceneCamera::SetViewportSize(float width, float height)
	{
		mAspectRatio = width / height;
		RecalculateProjection();
	}
	
	void SceneCamera::RecalculateProjection()
	{
		float orthoLeft = -mOrthographicSize * mAspectRatio;
		float orthoRight = mOrthographicSize * mAspectRatio;
		float orthoBottom = -mOrthographicSize;
		float orthoTop = mOrthographicSize;
	
		mProjection = glm::ortho(orthoLeft, orthoRight,
			orthoBottom, orthoTop, -100.0f, 100.0f);
	}
}