#include "aio_pch.hpp"
#include "OpenGL_Backend.hpp"

namespace aio
{
	OpenGL_Backend* OpenGL_Backend::sInstance = nullptr;

	GLenum OpenGLDrawingMode(DrawingMode mode)
	{
		switch (mode)
		{
		case DrawingMode::Point: return GL_POINT;
		case DrawingMode::Lines: return GL_LINES;
		case DrawingMode::Triangles: return GL_TRIANGLES;
		default:
			return 0;	// this is also return GL_POINTS;
		}
	}

	static void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         AIO_LOG_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       AIO_LOG_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          AIO_LOG_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: AIO_LOG_TRACE(message); return;
		}

		AIO_ASSERT(false, "Unknown severity level!");
	}

	OpenGL_Backend::OpenGL_Backend()
	{
		AIO_ASSERT(!sInstance, "OpenGL backend has already been made");
		sInstance = this;
	}

	OpenGL_Backend::~OpenGL_Backend()
	{

	}

	void OpenGL_Backend::Init()
	{
		AIO_PROFILE_FUNCTION();

#ifdef AIO_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
		AIO_LOG_INFO("OpenGL Info:");
		AIO_LOG_INFO(" Vendor: {0}",   (const char*)glGetString(GL_VENDOR));
		AIO_LOG_INFO(" Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		AIO_LOG_INFO(" Version: {0}",  (const char*)glGetString(GL_VERSION));

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	void OpenGL_Backend::SetViewport(const Vector2& position, const Vector2& size)
	{
		glViewport(position.x, position.y, size.x, size.y);
	}

	void OpenGL_Backend::Draw(DrawingMode mode, uint32_t vertexCount)
	{
		AIO_PROFILE_FUNCTION();

		glDrawArrays(OpenGLDrawingMode(mode), 0, vertexCount);
	}

	void OpenGL_Backend::DrawIndexed(DrawingMode mode, uint32_t indexCount)
	{
		AIO_PROFILE_FUNCTION();

		glDrawElements(OpenGLDrawingMode(mode), indexCount, GL_UNSIGNED_INT, 0);
	}

	void OpenGL_Backend::Clear(const Vector4& color)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(color.r, color.g, color.b, color.a);
	}
}

