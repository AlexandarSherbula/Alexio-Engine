struct Camera 
{
    float4x4 projection;
}

ConstantBuffer<Camera> camera : register(b0);

struct VS_INPUT
{
    float3 Pos           : aPosition;
    float3 LocalPosition : aLocalPosition;
    float4 Color         : aColor;
    float  Thickness     : aThickness;
    float  Fade          : aFade;
};

struct VS_OUTPUT
{
    float4 Pos           : SV_POSITION;
    float3 LocalPosition : TEXCOORD0;
    float4 Color         : COLOR;
    float  Thickness     : TEXCOORD1;
    float  Fade          : TEXCOORD2;
};

[shader("vertex")]
VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.LocalPosition = input.LocalPosition;
    output.Color = input.Color;
    output.Thickness = input.Thickness;
    output.Fade = input.Fade;

    output.Pos = mul(float4(input.Pos, 1.0), camera.projection);
    return output;
}

[shader("pixel")]
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float distance = 1.0 - length(input.LocalPosition);
	
    float circle = smoothstep(0.0, input.Fade, distance);
    circle *= smoothstep(input.Thickness + input.Fade, input.Thickness, distance);

    if (circle == 0.0)
        discard;
    
    float4 pixelColor = input.Color;
    pixelColor.a *= circle;
    
    return pixelColor;
}