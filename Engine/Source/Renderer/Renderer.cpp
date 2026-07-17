#include "aio_pch.hpp"

#include "Renderer.hpp"

namespace aio
{
	GraphicsAPI Renderer::sAPI = GraphicsAPI::OpenGL;
	Scope<RendererBackend> Renderer::sBackend = nullptr;
	Renderer::Statistics Renderer::Stats = { 0 };

	static std::array<Vector3, 4> localPosition =
	{
		Vector3(-0.5f, -0.5f, 0.0f),
		Vector3( 0.5f, -0.5f, 0.0f),
		Vector3( 0.5f,  0.5f, 0.0f),
		Vector3(-0.5f,  0.5f, 0.0f)
	};

	void Renderer::Init()
	{
		sBackend = RendererBackend::Create();
		sBackend->Init();

		LineRenderer::Init();
		TriangleRenderer::Init();
		QuadRenderer::Init();
		CircleRenderer::Init();

		StartBatches();
	}

	void Renderer::ClearColor(const Vector4& color)
	{
		sBackend->Clear(color);
	}

	void Renderer::OnWindowResize(WindowResizeEvent& e)
	{
		if (sBackend != nullptr)
			sBackend->SetViewport(Vector2(0.0f, 0.0f), Vector2(e.GetWidth(), e.GetHeight()));
	}

	void Renderer::StartBatches()
	{
		LineRenderer::StartNewBatch();
		TriangleRenderer::StartNewBatch();
		QuadRenderer::StartNewBatch();
		CircleRenderer::StartNewBatch();
	}

	void Renderer::Flush()
	{
		LineRenderer::SubmitBatch();
		TriangleRenderer::SubmitBatch();
		QuadRenderer::SubmitBatch();
		CircleRenderer::SubmitBatch();
	}

	void Renderer::End()
	{
		LineRenderer::End();
		TriangleRenderer::End();
		QuadRenderer::End();
		CircleRenderer::End();
	}

	void Renderer::DrawLine(const Vector2& p0, const Vector2& p1, const Vector4& color)
	{
		AIO_PROFILE_FUNCTION();

		if (LineRenderer::LineCount >= LineRenderer::MaxLinesPerBatch)
			LineRenderer::SubmitBatch();

		LineRenderer::CurrentVertexPtr->position = Vector3(p0, 0.0f);
		LineRenderer::CurrentVertexPtr->color = color;
		LineRenderer::CurrentVertexPtr++;

		LineRenderer::CurrentVertexPtr->position = Vector3(p1, 0.0f);
		LineRenderer::CurrentVertexPtr->color = color;
		LineRenderer::CurrentVertexPtr++;

		LineRenderer::LineCount++;
		Stats.Lines++;
	}

	void Renderer::DrawTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector4& color)
	{
		AIO_PROFILE_FUNCTION();
		if (TriangleRenderer::TriangleCount >= TriangleRenderer::MaxTrisPerBatch)
			TriangleRenderer::SubmitBatch();

		TriangleRenderer::CurrentVertexPtr->position = Vector3(p1, 0.0f);
		TriangleRenderer::CurrentVertexPtr->color = color;
		TriangleRenderer::CurrentVertexPtr++;

		TriangleRenderer::CurrentVertexPtr->position = Vector3(p2, 0.0f);
		TriangleRenderer::CurrentVertexPtr->color = color;
		TriangleRenderer::CurrentVertexPtr++;

		TriangleRenderer::CurrentVertexPtr->position = Vector3(p3, 0.0f);
		TriangleRenderer::CurrentVertexPtr->color = color;
		TriangleRenderer::CurrentVertexPtr++;

		TriangleRenderer::TriangleCount++;
		Stats.Triangles++;
	}

	void Renderer::DrawLinedTriangle(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector4& color)
	{
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p1, color);
	}

	void Renderer::DrawLinedQuad(const Vector2& position, const Vector2& size, const Vector4& color)
	{
		AIO_PROFILE_FUNCTION();
		Vector2 p0 = position;
		Vector2 p1 = { position.x + size.x, position.y };
		Vector2 p2 = { position.x + size.x, position.y + size.y };
		Vector2 p3 = { position.x, position.y + size.y };

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}


	void Renderer::DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color)
	{
		AIO_PROFILE_FUNCTION();
		if (QuadRenderer::QuadCount >= QuadRenderer::MaxQuadsPerBatch)
			QuadRenderer::SubmitBatch();

		QuadRenderer::CurrentVertexPtr->position = { position.x, position.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x + size.x, position.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x + size.x, position.y + size.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x, position.y + size.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::IndexCount += 6;
		QuadRenderer::QuadCount++;
		Stats.Quads++;
	}


	void Renderer::DrawQuad(const Mat4x4& transform, const Vector4& color)
	{
		AIO_PROFILE_FUNCTION();
		if (QuadRenderer::QuadCount >= QuadRenderer::MaxQuadsPerBatch)
			QuadRenderer::SubmitBatch();

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[0], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[1], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[2], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[3], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::IndexCount += 6;
		QuadRenderer::QuadCount++;
		Stats.Quads++;
	}

	void Renderer::DrawRotatedQuad(const Vector2& position, const Vector2& size, const Vector4& color, float angle)
	{
		AIO_PROFILE_FUNCTION();
		if (QuadRenderer::QuadCount >= QuadRenderer::MaxQuadsPerBatch)
			QuadRenderer::SubmitBatch();

		Vector2 originPoint = position + size * 0.5f;
		Mat4x4 transform = glm::translate(Mat4x4(1.0f), Vector3(originPoint, 1.0f)) *
			glm::rotate(Mat4x4(1.0f), angle, Vector3(0, 0, 1)) * glm::scale(Mat4x4(1.0f), Vector3(size, 1.0f));

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[0], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[1], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[2], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[3], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = 0;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::IndexCount += 6;
		QuadRenderer::QuadCount++;
		Stats.Quads++;
	}

	void Renderer::DrawSprite(const Ref<Texture>& texture, const Vector2& position, const Vector2& size, const Vector4& color)
	{
		AIO_PROFILE_FUNCTION();
		if (QuadRenderer::QuadCount >= QuadRenderer::MaxQuadsPerBatch ||
			QuadRenderer::TextureSlotIndex >= QuadRenderer::MaxTextureSlots)
			QuadRenderer::SubmitBatch();

		uint32_t texIndex = QuadRenderer::TextureSlotIndex;
		for (int i = 1; i < QuadRenderer::TextureSlotIndex; i++)
		{
			if (QuadRenderer::TextureIDs[i] == texture->GetID())
			{
				texIndex = i;
				break;
			}
		}

		QuadRenderer::CurrentVertexPtr->position = { position.x, position.y, 1.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x + size.x, position.y, 1.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x + size.x, position.y + size.y, 1.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x, position.y + size.y, 1.0f };
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::IndexCount += 6;
		QuadRenderer::QuadCount++;
		Stats.Quads++;

		texture->Bind(texIndex);
		if (texIndex == QuadRenderer::TextureSlotIndex)
		{
			QuadRenderer::TextureIDs[QuadRenderer::TextureSlotIndex] = texture->GetID();
			QuadRenderer::TextureSlotIndex++;
		}
	}

	void Renderer::DrawSubSprite(const Ref<Texture>& texture, const Vector2& position, const Vector2& size, const Vector2& tileOffset, const Vector2& tileSize, const Vector4& colorTint)
	{
		AIO_PROFILE_FUNCTION();

		if (QuadRenderer::QuadCount >= QuadRenderer::MaxQuadsPerBatch ||
			QuadRenderer::TextureSlotIndex >= QuadRenderer::MaxTextureSlots)
			QuadRenderer::SubmitBatch();

		uint32_t texIndex = QuadRenderer::TextureSlotIndex;
		for (int i = 1; i < QuadRenderer::TextureSlotIndex; i++)
		{
			if (QuadRenderer::TextureIDs[i] == texture->GetID())
			{
				texIndex = i;
				break;
			}
		}

		const std::array<Vector2, 4> textureCoordinates =
		{
			Vector2((tileOffset.x * tileSize.x) / texture->GetWidth(), (tileOffset.y * tileSize.y) / texture->GetHeight()),
			Vector2(((tileOffset.x + 1) * tileSize.x) / texture->GetWidth(), (tileOffset.y * tileSize.y) / texture->GetHeight()),
			Vector2(((tileOffset.x + 1) * tileSize.x) / texture->GetWidth(), ((tileOffset.y + 1) * tileSize.y) / texture->GetHeight()),
			Vector2(((tileOffset.x) * tileSize.x) / texture->GetWidth(), ((tileOffset.y + 1) * tileSize.y) / texture->GetHeight())
		};

		const Vector2 finalSize = Vector2(size.x * tileSize.x / texture->GetWidth(), size.y * tileSize.y / texture->GetHeight());

		QuadRenderer::CurrentVertexPtr->position = { position.x, position.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = colorTint;
		QuadRenderer::CurrentVertexPtr->texCoord = textureCoordinates[0];
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x + finalSize.x, position.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = colorTint;
		QuadRenderer::CurrentVertexPtr->texCoord = textureCoordinates[1];
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x + finalSize.x, position.y + finalSize.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = colorTint;
		QuadRenderer::CurrentVertexPtr->texCoord = textureCoordinates[2];
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = { position.x, position.y + finalSize.y, 0.0f };
		QuadRenderer::CurrentVertexPtr->color = colorTint;
		QuadRenderer::CurrentVertexPtr->texCoord = textureCoordinates[3];
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::IndexCount += 6;
		QuadRenderer::QuadCount++;
		Stats.Quads++;

		texture->Bind(texIndex);
		if (texIndex == QuadRenderer::TextureSlotIndex)
		{
			QuadRenderer::TextureIDs[QuadRenderer::TextureSlotIndex] = texture->GetID();
			QuadRenderer::TextureSlotIndex++;
		}
	}

	void Renderer::DrawRotatedSprite(const Ref<Texture>& texture, const Vector2& position, const Vector2& size, const Vector4& color, float angle)
	{
		AIO_PROFILE_FUNCTION();
		if (QuadRenderer::QuadCount >= QuadRenderer::MaxQuadsPerBatch ||
			QuadRenderer::TextureSlotIndex >= QuadRenderer::MaxTextureSlots)
			QuadRenderer::SubmitBatch();

		uint32_t texIndex = QuadRenderer::TextureSlotIndex;
		for (int i = 1; i < QuadRenderer::TextureSlotIndex; i++)
		{
			if (QuadRenderer::TextureIDs[i] == texture->GetID())
			{
				texIndex = i;
				break;
			}
		}

		Vector2 originPoint = position + size * 0.5f;
		Mat4x4 transform = glm::translate(Mat4x4(1.0f), Vector3(originPoint, 1.0f)) *
			glm::rotate(Mat4x4(1.0f), angle, Vector3(0, 0, 1)) * glm::scale(Mat4x4(1.0f), Vector3(size, 1.0f));

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[0], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[1], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 0.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[2], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 1.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::CurrentVertexPtr->position = transform * Vector4(localPosition[3], 1.0f);
		QuadRenderer::CurrentVertexPtr->color = color;
		QuadRenderer::CurrentVertexPtr->texCoord = { 0.0f, 1.0f };
		QuadRenderer::CurrentVertexPtr->textureIndex = texIndex;
		QuadRenderer::CurrentVertexPtr++;

		QuadRenderer::IndexCount += 6;
		QuadRenderer::QuadCount++;
		Stats.Quads++;

		texture->Bind(texIndex);
		if (texIndex == QuadRenderer::TextureSlotIndex)
		{
			QuadRenderer::TextureIDs[QuadRenderer::TextureSlotIndex] = texture->GetID();
			QuadRenderer::TextureSlotIndex++;
		}
	}

	void Renderer::DrawCircle(const Vector2& position, const Vector4& color, float radius, float thickness, float fade)
	{
		AIO_PROFILE_FUNCTION();
		if (CircleRenderer::CircleCount >= CircleRenderer::MaxCirclesPerBatch)
			CircleRenderer::SubmitBatch();

		CircleRenderer::CurrentVertexPtr->position = Vector4(position.x - radius, position.y - radius, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->localPosition = Vector4(-1.0f, -1.0f, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->color = color;
		CircleRenderer::CurrentVertexPtr->thickness = thickness;
		CircleRenderer::CurrentVertexPtr->fade = fade;
		CircleRenderer::CurrentVertexPtr++;

		CircleRenderer::CurrentVertexPtr->position = Vector4(position.x + radius, position.y - radius, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->localPosition = Vector4(1.0f, -1.0f, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->color = color;
		CircleRenderer::CurrentVertexPtr->thickness = thickness;
		CircleRenderer::CurrentVertexPtr->fade = fade;
		CircleRenderer::CurrentVertexPtr++;

		CircleRenderer::CurrentVertexPtr->position = Vector4(position.x + radius, position.y + radius, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->localPosition = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->color = color;
		CircleRenderer::CurrentVertexPtr->thickness = thickness;
		CircleRenderer::CurrentVertexPtr->fade = fade;
		CircleRenderer::CurrentVertexPtr++;

		CircleRenderer::CurrentVertexPtr->position = Vector4(position.x - radius, position.y + radius, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->localPosition = Vector4(-1.0f, 1.0f, 0.0f, 1.0f);
		CircleRenderer::CurrentVertexPtr->color = color;
		CircleRenderer::CurrentVertexPtr->thickness = thickness;
		CircleRenderer::CurrentVertexPtr->fade = fade;
		CircleRenderer::CurrentVertexPtr++;

		CircleRenderer::IndexCount += 6;
		CircleRenderer::CircleCount++;
		Stats.Circles++;
	}
}