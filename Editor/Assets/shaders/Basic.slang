struct VS_INPUT
{
    float3 Pos : aPosition;
    float4 Color : aColor;
};

struct Camera
{
    float4x4 projection;
}

ConstantBuffer<Camera> camera : register(b0);

struct VS_OUTPUT
{
    float4 Pos : SV_Position;
    float4 Color : COLOR;
};

[shader("vertex")]
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0), camera.projection);
    output.Color = input.Color;
    return output;
}

[shader("pixel")]
float4 PSMain(VS_OUTPUT output) : SV_Target
{
    return output.Color;
}