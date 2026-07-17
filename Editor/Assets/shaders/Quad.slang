float4 GetSampledTexture(float2 texCoord, int texIndex = 0);

struct VS_INPUT
{
    float3 Pos : aPosition;
    float4 Color : aColor;
    float2 TexCoord : aTexCoord;
    int TexIndex : aTexIndex;
};

struct Camera
{
    float4x4 projection;
}

ConstantBuffer<Camera> camera : register(b0);
Texture2D textures[32] : register(t0);
SamplerState samplerState : register(s0);

struct VS_OUTPUT
{
    float4 Pos : SV_Position;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
    int TexIndex : TEXCOORD1;
};

[shader("vertex")]
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0), camera.projection);
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    output.TexIndex = input.TexIndex;
    return output;
}

[shader("pixel")]
float4 PSMain(VS_OUTPUT output) : SV_Target
{
    switch (output.TexIndex)
    {
        case 0: return GetSampledTexture(output.TexCoord, 0) * output.Color;
        case 1: return GetSampledTexture(output.TexCoord, 1) * output.Color;
        case 2: return GetSampledTexture(output.TexCoord, 2) * output.Color;
        case 3: return GetSampledTexture(output.TexCoord, 3) * output.Color;
        case 4: return GetSampledTexture(output.TexCoord, 4) * output.Color;
        case 5: return GetSampledTexture(output.TexCoord, 5) * output.Color;
        case 6: return GetSampledTexture(output.TexCoord, 6) * output.Color;
        case 7: return GetSampledTexture(output.TexCoord, 7) * output.Color;
        case 8: return GetSampledTexture(output.TexCoord, 8) * output.Color;
        case 9: return GetSampledTexture(output.TexCoord, 9) * output.Color;
        case 10: return GetSampledTexture(output.TexCoord, 10) * output.Color;
        case 11: return GetSampledTexture(output.TexCoord, 11) * output.Color;
        case 12: return GetSampledTexture(output.TexCoord, 12) * output.Color;
        case 13: return GetSampledTexture(output.TexCoord, 13) * output.Color;
        case 14: return GetSampledTexture(output.TexCoord, 14) * output.Color;
        case 15: return GetSampledTexture(output.TexCoord, 15) * output.Color;
        case 16: return GetSampledTexture(output.TexCoord, 16) * output.Color;
        case 17: return GetSampledTexture(output.TexCoord, 17) * output.Color;
        case 18: return GetSampledTexture(output.TexCoord, 18) * output.Color;
        case 19: return GetSampledTexture(output.TexCoord, 19) * output.Color;
        case 20: return GetSampledTexture(output.TexCoord, 20) * output.Color;
        case 21: return GetSampledTexture(output.TexCoord, 21) * output.Color;
        case 22: return GetSampledTexture(output.TexCoord, 22) * output.Color;
        case 23: return GetSampledTexture(output.TexCoord, 23) * output.Color;
        case 24: return GetSampledTexture(output.TexCoord, 24) * output.Color;
        case 25: return GetSampledTexture(output.TexCoord, 25) * output.Color;
        case 26: return GetSampledTexture(output.TexCoord, 26) * output.Color;
        case 27: return GetSampledTexture(output.TexCoord, 27) * output.Color;
        case 28: return GetSampledTexture(output.TexCoord, 28) * output.Color;
        case 29: return GetSampledTexture(output.TexCoord, 29) * output.Color;
        case 30: return GetSampledTexture(output.TexCoord, 30) * output.Color;
        case 31: return GetSampledTexture(output.TexCoord, 31) * output.Color;
    }

    return float4(0.5, 0.5, 0.5, 1.0);
}