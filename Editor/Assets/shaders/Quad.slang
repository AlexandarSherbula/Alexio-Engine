float4 GetSampledTexture(float2 texCoord, int texIndex = 0);

struct VS_INPUT
{
    float3 Pos : aPosition;
    float4 Color : aColor;
    float2 TexCoord : aTexCoord;
    int TexIndex : aTexIndex;
    int EntityID : aEntityID;
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
    int EntityID : TEXCOORD2;
};

[shader("vertex")]
VS_OUTPUT VSMain(VS_INPUT vs_input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(vs_input.Pos, 1.0), camera.projection);
    output.Color = vs_input.Color;
    output.TexCoord = vs_input.TexCoord;
    output.TexIndex = vs_input.TexIndex;
    output.EntityID = vs_input.EntityID;
    return output;
}

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
    int EntityID : SV_Target1;
}

[shader("pixel")]
PS_OUTPUT PSMain(VS_OUTPUT vs_output)
{
    float4 sample = float4(0.5, 0.5, 0.5, 1.0);

    switch (vs_output.TexIndex)
    {
        case 0:  sample = GetSampledTexture(vs_output.TexCoord, 0);  break;
        case 1:  sample = GetSampledTexture(vs_output.TexCoord, 1);  break;
        case 2:  sample = GetSampledTexture(vs_output.TexCoord, 2);  break;
        case 3:  sample = GetSampledTexture(vs_output.TexCoord, 3);  break;
        case 4:  sample = GetSampledTexture(vs_output.TexCoord, 4);  break;
        case 5:  sample = GetSampledTexture(vs_output.TexCoord, 5);  break;
        case 6:  sample = GetSampledTexture(vs_output.TexCoord, 6);  break;
        case 7:  sample = GetSampledTexture(vs_output.TexCoord, 7);  break;
        case 8:  sample = GetSampledTexture(vs_output.TexCoord, 8);  break;
        case 9:  sample = GetSampledTexture(vs_output.TexCoord, 9);  break;
        case 10: sample = GetSampledTexture(vs_output.TexCoord, 10); break;
        case 11: sample = GetSampledTexture(vs_output.TexCoord, 11); break;
        case 12: sample = GetSampledTexture(vs_output.TexCoord, 12); break;
        case 13: sample = GetSampledTexture(vs_output.TexCoord, 13); break;
        case 14: sample = GetSampledTexture(vs_output.TexCoord, 14); break;
        case 15: sample = GetSampledTexture(vs_output.TexCoord, 15); break;
        case 16: sample = GetSampledTexture(vs_output.TexCoord, 16); break;
        case 17: sample = GetSampledTexture(vs_output.TexCoord, 17); break;
        case 18: sample = GetSampledTexture(vs_output.TexCoord, 18); break;
        case 19: sample = GetSampledTexture(vs_output.TexCoord, 19); break;
        case 20: sample = GetSampledTexture(vs_output.TexCoord, 20); break;
        case 21: sample = GetSampledTexture(vs_output.TexCoord, 21); break;
        case 22: sample = GetSampledTexture(vs_output.TexCoord, 22); break;
        case 23: sample = GetSampledTexture(vs_output.TexCoord, 23); break;
        case 24: sample = GetSampledTexture(vs_output.TexCoord, 24); break;
        case 25: sample = GetSampledTexture(vs_output.TexCoord, 25); break;
        case 26: sample = GetSampledTexture(vs_output.TexCoord, 26); break;
        case 27: sample = GetSampledTexture(vs_output.TexCoord, 27); break;
        case 28: sample = GetSampledTexture(vs_output.TexCoord, 28); break;
        case 29: sample = GetSampledTexture(vs_output.TexCoord, 29); break;
        case 30: sample = GetSampledTexture(vs_output.TexCoord, 30); break;
        case 31: sample = GetSampledTexture(vs_output.TexCoord, 31); break;
    }

    PS_OUTPUT o;
    o.Color = sample * vs_output.Color;
    o.EntityID = vs_output.EntityID;

    return o;
}