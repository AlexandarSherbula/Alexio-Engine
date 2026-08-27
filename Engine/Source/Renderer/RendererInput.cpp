#include "aio_pch.hpp"
#include "RendererInput.hpp"
#include "Renderer.hpp"

namespace aio
{
    Ref<VertexBuffer> LineRenderer::vertexBuffer = nullptr;
    Ref<VertexInput>  LineRenderer::vertexInput = nullptr;
    Ref<Shader>       LineRenderer::shader = nullptr;

    uint32_t LineRenderer::LineCount = 0;

    Vertex* LineRenderer::CurrentVertexPtr = nullptr;
    Vertex* LineRenderer::baseVertexBuffer = nullptr;

    void LineRenderer::Init()
    {
        const uint32_t maxVertexCount = 2 * MaxLinesPerBatch;

        baseVertexBuffer = new Vertex[maxVertexCount];

        vertexInput = VertexInput::Create();
        vertexBuffer = VertexBuffer::Create(maxVertexCount * sizeof(Vertex));

        BufferLayout layout =
        {
            {ShaderDataType::Float3, "aPosition" },
            {ShaderDataType::Float4, "aColor"    }
        };
        vertexBuffer->SetLayout(layout);
        vertexInput->SetVertexBuffer(vertexBuffer);

        shader = Shader::Create(ASSETS_DIRECTORY / "shaders" / "Basic.slang", vertexInput);
    }

    void LineRenderer::StartNewBatch()
    {
        LineCount = 0;
        CurrentVertexPtr = baseVertexBuffer;
    }

    void LineRenderer::SubmitBatch()
    {
        if (LineCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)CurrentVertexPtr - (uint8_t*)baseVertexBuffer);
            vertexBuffer->SetData(baseVertexBuffer, dataSize);

            vertexInput->Bind();
            shader->Bind();

            Renderer::Backend()->Draw(DrawingMode::Lines, LineCount * 2);
            Renderer::Stats.DrawLine++;

            shader->Unbind();
            vertexInput->Unbind();
        }
        StartNewBatch();
    }

    void LineRenderer::End()
    {
        delete[] baseVertexBuffer;
    }

    Ref<VertexBuffer> TriangleRenderer::vertexBuffer = nullptr;
    Ref<VertexInput>  TriangleRenderer::vertexInput = nullptr;
    Ref<Shader>       TriangleRenderer::shader = nullptr;

    uint32_t TriangleRenderer::TriangleCount = 0;

    Vertex* TriangleRenderer::CurrentVertexPtr = nullptr;
    Vertex* TriangleRenderer::baseVertexBuffer = nullptr;

    void TriangleRenderer::Init()
    {
        const uint32_t maxVertexCount = 3 * MaxTrisPerBatch;

        baseVertexBuffer = new Vertex[maxVertexCount];

        vertexInput = VertexInput::Create();
        vertexBuffer = VertexBuffer::Create(maxVertexCount * sizeof(Vertex));

        BufferLayout layout =
        {
            {ShaderDataType::Float3, "aPosition" },
            {ShaderDataType::Float4, "aColor"    }
        };
        vertexBuffer->SetLayout(layout);
        vertexInput->SetVertexBuffer(vertexBuffer);

        shader = Shader::Create(ASSETS_DIRECTORY / "shaders" / "Basic.slang", vertexInput);
    }

    void TriangleRenderer::StartNewBatch()
    {
        TriangleCount = 0;
        CurrentVertexPtr = baseVertexBuffer;
    }

    void TriangleRenderer::SubmitBatch()
    {
        if (TriangleCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)CurrentVertexPtr - (uint8_t*)baseVertexBuffer);
            vertexBuffer->SetData(baseVertexBuffer, dataSize);

            vertexInput->Bind();
            shader->Bind();

            Renderer::Backend()->Draw(DrawingMode::Triangles, TriangleCount * 3);
            Renderer::Stats.DrawLine++;

            shader->Unbind();
            vertexInput->Unbind();
        }
        StartNewBatch();
    }

    void TriangleRenderer::End()
    {
        delete[] baseVertexBuffer;
    }

    Ref<VertexBuffer> QuadRenderer::vertexBuffer = nullptr;
    Ref<IndexBuffer>  QuadRenderer::indexBuffer = nullptr;
    Ref<VertexInput>  QuadRenderer::vertexInput = nullptr;
    Ref<Shader>       QuadRenderer::shader = nullptr;
    Ref<Texture>      QuadRenderer::WhiteTexture = nullptr;

    uint32_t QuadRenderer::QuadCount = 0;
    uint32_t QuadRenderer::IndexCount = 0;
    uint32_t QuadRenderer::TextureSlotIndex = 0;

    std::array<uint32_t, QuadRenderer::MaxTextureSlots> QuadRenderer::TextureIDs = { 0 };

    QuadVertex* QuadRenderer::CurrentVertexPtr = nullptr;
    QuadVertex* QuadRenderer::baseVertexBuffer = nullptr;

    void QuadRenderer::Init()
    {
        const uint32_t maxVertexCount = 4 * MaxQuadsPerBatch;
        const uint32_t maxIndexCount = 6 * MaxQuadsPerBatch;

        baseVertexBuffer = new QuadVertex[maxVertexCount];

        uint32_t* indices = new uint32_t[maxIndexCount];

        int32_t indexOffset = 0;
        for (size_t i = 0; i < MaxQuadsPerBatch; i++)
        {
            indices[i * 6 + 0] = 0 + indexOffset;
            indices[i * 6 + 1] = 1 + indexOffset;
            indices[i * 6 + 2] = 2 + indexOffset;
            indices[i * 6 + 3] = 2 + indexOffset;
            indices[i * 6 + 4] = 3 + indexOffset;
            indices[i * 6 + 5] = 0 + indexOffset;

            indexOffset += 4;
        }
        vertexInput = VertexInput::Create();
        vertexBuffer = VertexBuffer::Create(maxVertexCount * sizeof(QuadVertex));
        indexBuffer = IndexBuffer::Create(indices, maxIndexCount);
        delete[] indices;

        BufferLayout layout =
        {
            {ShaderDataType::Float3, "aPosition" },
            {ShaderDataType::Float4, "aColor"    },
            {ShaderDataType::Float2, "aTexCoord" },
            {ShaderDataType::Int,    "aTexIndex" },
            {ShaderDataType::Int,    "aEntityID" }
        };

        vertexBuffer->SetLayout(layout);
        vertexInput->SetVertexBuffer(vertexBuffer);
        vertexInput->SetIndexBuffer(indexBuffer);

        shader = Shader::Create(ASSETS_DIRECTORY / "shaders" / "Quad.slang", vertexInput);

        TextureSpecification texSpec;

        WhiteTexture = Texture::Create(texSpec);
        TextureIDs[TextureSlotIndex] = WhiteTexture->GetID();
        TextureSlotIndex++;
    }

    void QuadRenderer::StartNewBatch()
    {
        QuadCount = 0;
        IndexCount = 0;
        TextureSlotIndex = 1;

        CurrentVertexPtr = baseVertexBuffer;
    }

    void QuadRenderer::SubmitBatch()
    {
        if (QuadCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)CurrentVertexPtr - (uint8_t*)baseVertexBuffer);
            vertexBuffer->SetData(baseVertexBuffer, dataSize);

            vertexInput->Bind();
            shader->Bind();
            WhiteTexture->Bind(0);

            Renderer::Backend()->DrawIndexed(DrawingMode::Triangles, IndexCount);
            Renderer::Stats.DrawQuad++;

            WhiteTexture->Unbind();
            shader->Unbind();
            vertexInput->Unbind();
        }
        StartNewBatch();
    }

    void QuadRenderer::End()
    {
        delete[] baseVertexBuffer;
    }

    Ref<VertexInput>  CircleRenderer::vertexInput = nullptr;
    Ref<VertexBuffer> CircleRenderer::vertexBuffer = nullptr;
    Ref<IndexBuffer>  CircleRenderer::indexBuffer = nullptr;
    Ref<Shader>       CircleRenderer::shader = nullptr;

    uint32_t CircleRenderer::CircleCount = 0;
    uint32_t CircleRenderer::IndexCount = 0;

    CircleVertex* CircleRenderer::CurrentVertexPtr = nullptr;
    CircleVertex* CircleRenderer::baseVertexBuffer = nullptr;

    void CircleRenderer::Init()
    {
        const uint32_t maxVertexCount = 4 * MaxCirclesPerBatch;
        const uint32_t maxIndexCount = 6 * MaxCirclesPerBatch;

        baseVertexBuffer = new CircleVertex[maxVertexCount];

        uint32_t* indices = new uint32_t[maxIndexCount];

        int32_t indexOffset = 0;
        for (size_t i = 0; i < MaxCirclesPerBatch; i++)
        {
            indices[i * 6 + 0] = 0 + indexOffset;
            indices[i * 6 + 1] = 1 + indexOffset;
            indices[i * 6 + 2] = 2 + indexOffset;
            indices[i * 6 + 3] = 2 + indexOffset;
            indices[i * 6 + 4] = 3 + indexOffset;
            indices[i * 6 + 5] = 0 + indexOffset;

            indexOffset += 4;
        }

        vertexInput = aio::VertexInput::Create();
        vertexBuffer = VertexBuffer::Create(maxVertexCount * sizeof(CircleVertex));
        indexBuffer = IndexBuffer::Create(indices, maxIndexCount);
        delete[] indices;

        BufferLayout layout =
        {
            {ShaderDataType::Float3, "aPosition"      },
            {ShaderDataType::Float3, "aLocalPosition" },
            {ShaderDataType::Float4, "aColor"         },
            {ShaderDataType::Float,  "aThickness"     },
            {ShaderDataType::Float,  "aFade"          }
        };
        vertexBuffer->SetLayout(layout);

        vertexInput->SetVertexBuffer(vertexBuffer);
        vertexInput->SetIndexBuffer(indexBuffer);

        shader = Shader::Create(ASSETS_DIRECTORY / "shaders" / "Circle.slang", vertexInput);
    }

    void CircleRenderer::StartNewBatch()
    {
        CircleCount = 0;
        IndexCount = 0;

        CurrentVertexPtr = baseVertexBuffer;
    }

    void CircleRenderer::SubmitBatch()
    {
        if (CircleCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)CurrentVertexPtr - (uint8_t*)baseVertexBuffer);
            vertexBuffer->SetData(baseVertexBuffer, dataSize);

            vertexInput->Bind();
            shader->Bind();

            Renderer::Backend()->DrawIndexed(DrawingMode::Triangles, IndexCount);
            Renderer::Stats.DrawCircle++;

            shader->Unbind();
            vertexInput->Unbind();
        }
        StartNewBatch();
    }

    void CircleRenderer::End()
    {
        delete[] baseVertexBuffer;
    }
}