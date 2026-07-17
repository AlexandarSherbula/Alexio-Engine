#pragma once

#include "Utils/Math.hpp"

#include "RendererBackend.hpp"
#include "GraphicsContext.hpp"
#include "Buffers.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "RendererInput.hpp"

#include "Events/WindowEvent.hpp"

enum class GraphicsAPI
{
	OpenGL,
	DX11
};

#if defined(AIO_WINDOWS)

#define CHECK_API(x, y)                                                    \
        do {                                                               \
            switch (aio::Renderer::CheckAPI())                             \
            {                                                              \
                case GraphicsAPI::OpenGL: { x; break; }                    \
                case GraphicsAPI::DX11:   { y; break; }                    \
                default:     { AIO_ASSERT(false, "Unknown API."); break; } \
            }                                                              \
        } while(0)
#define SET_API(x) aio::Renderer::SetAPI(x)
#else
#define CHECK_API(x, y) do { x; } while(0)
#define SET_API(x)
#endif


namespace aio
{
	class Renderer
	{
	public:
		static void Init();
		static void ClearColor(const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 1.0f));

		static void OnWindowResize(WindowResizeEvent& e);

		static void StartBatches();
		static void Flush();
		static void End();

		inline static const Scope<RendererBackend>& Backend() { return sBackend; }
		inline static const GraphicsAPI CheckAPI() { return sAPI; }
#if defined (AIO_WINDOWS)
		inline static void SetAPI(GraphicsAPI api) { sAPI = api; }
#endif
		static void DrawLine(const Vector2& p1, const Vector2& p2, const Vector4& color = Vector4(1.0f));
		static void DrawTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector4& color = Vector4(1.0f));
		static void DrawLinedTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector4& color = Vector4(1.0f));
		static void DrawLinedQuad(const Vector2& position, const Vector2& size, const Vector4& color = Vector4(1.0f));
		static void DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color = Vector4(1.0f));
		static void DrawQuad(const Mat4x4& transform, const Vector4& color);
		static void DrawCircle(const Vector2& position, const Vector4& color = Vector4(1.0f), float radius = 0.5f, float thickness = 1.0f, float fade = 0.005f);

		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, const Vector4& color = Vector4(1.0f), float angle = 0.0f);
		static void DrawSprite(const Ref<Texture>& texture, const Vector2& position, const Vector2& size, const Vector4& color = Vector4(1.0f));
		static void DrawSubSprite(const Ref<Texture>& texture, const Vector2& position, const Vector2& size, const Vector2& tileOffset, const Vector2& tileSize, const Vector4& colorTint = Vector4(1.0f));
		static void DrawRotatedSprite(const Ref<Texture>& texture, const Vector2& position, const Vector2& size, const Vector4& color = Vector4(1.0f), float angle = 0.0f);
	public:
		struct Statistics
		{
			uint32_t Lines;
			uint32_t DrawLine;

			uint32_t Triangles;
			uint32_t DrawTriangles;

			uint32_t Quads;
			uint32_t DrawQuad;

			uint32_t Circles;
			uint32_t DrawCircle;

			inline void Reset()
			{
				Lines = 0;
				Triangles = 0;
				Quads = 0;
				Circles = 0;
			}
		};

		static Statistics Stats;
	private:
		static GraphicsAPI sAPI;
		static Scope<RendererBackend> sBackend;
	};
}