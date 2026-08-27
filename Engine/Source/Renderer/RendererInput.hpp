#pragma once

#include "Utils/Utilities.hpp"
#include "Utils/Math.hpp"

#include "Renderer/Buffers.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"

namespace aio
{
	struct Vertex
	{
		Vector3 position;
		Vector4 color;
	};

	class LineRenderer
	{
	public:
		static void Init();
		static void StartNewBatch();
		static void SubmitBatch();
		static void End();
	public:
		static uint32_t LineCount;

		static Vertex* CurrentVertexPtr;
		static const size_t MaxLinesPerBatch = 1000;
	private:
		static Ref<VertexBuffer> vertexBuffer;
		static Ref<VertexInput>  vertexInput;
		static Ref<Shader>       shader;
		static Vertex*       baseVertexBuffer;
	};

	class TriangleRenderer
	{
	public:
		static void Init();
		static void StartNewBatch();
		static void SubmitBatch();
		static void End();
	public:
		static uint32_t TriangleCount;

		static Vertex* CurrentVertexPtr;
		static const size_t MaxTrisPerBatch = 1000;
	private:
		static Ref<VertexBuffer> vertexBuffer;
		static Ref<VertexInput>  vertexInput;
		static Ref<Shader>       shader;
		static Vertex* baseVertexBuffer;
	};

	struct QuadVertex
	{
		Vector3 position;
		Vector4 color;
		Vector2 texCoord;
		uint32_t textureIndex;
		uint32_t entityID;
	};

	class QuadRenderer
	{
	public:
		static void Init();
		static void StartNewBatch();
		static void SubmitBatch();
		static void End();
	public:
		static uint32_t QuadCount;
		static uint32_t IndexCount;
		static uint32_t TextureSlotIndex;

		static QuadVertex* CurrentVertexPtr;
		static const size_t MaxQuadsPerBatch = 1000;
		static const uint32_t MaxTextureSlots = 32;
		static std::array<uint32_t, MaxTextureSlots> TextureIDs;

		static Ref<Texture>      WhiteTexture;
	private:
		static Ref<VertexBuffer> vertexBuffer;
		static Ref<IndexBuffer>  indexBuffer;
		static Ref<VertexInput> vertexInput;
		static Ref<Shader>       shader;

		static QuadVertex* baseVertexBuffer;
	};

	struct CircleVertex
	{
		Vector3 position;
		Vector3 localPosition;
		Vector4 color;
		float thickness;
		float fade;
	};

	class CircleRenderer
	{
	public:
		static void Init();
		static void StartNewBatch();
		static void SubmitBatch();
		static void End();
	public:
		static uint32_t CircleCount;
		static uint32_t IndexCount;

		static CircleVertex* CurrentVertexPtr;
		static const size_t MaxCirclesPerBatch = 1000;
	private:
		static Ref<VertexInput>  vertexInput;
		static Ref<VertexBuffer> vertexBuffer;
		static Ref<IndexBuffer>  indexBuffer;
		static Ref<Shader>       shader;

		static CircleVertex* baseVertexBuffer;
	};
}