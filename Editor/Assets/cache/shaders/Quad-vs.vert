#version 450
layout(column_major) uniform;
layout(column_major) buffer;

#line 14 0
struct Camera_natural_0
{
    mat4x4 projection_0;
};


#line 17
layout(binding = 0)
layout(std140) uniform block_Camera_natural_0
{
    mat4x4 projection_0;
}camera_0;

#line 12140 1
layout(location = 0)
out vec4 entryPointParam_VSMain_Color_0;


#line 4 0
layout(location = 1)
out vec2 entryPointParam_VSMain_TexCoord_0;


#line 4
layout(location = 2)
out int entryPointParam_VSMain_TexIndex_0;


#line 4
layout(location = 0)
in vec3 input_Pos_0;


#line 4
layout(location = 1)
in vec4 input_Color_0;


#line 4
layout(location = 2)
in vec2 input_TexCoord_0;


#line 4
layout(location = 3)
in int input_TexIndex_0;


#line 21
struct VS_OUTPUT_0
{
    vec4 Pos_0;
    vec4 Color_0;
    vec2 TexCoord_0;
    int TexIndex_0;
};


void main()
{
    VS_OUTPUT_0 output_0;
    output_0.Pos_0 = (((camera_0.projection_0) * (vec4(input_Pos_0, 1.0))));
    output_0.Color_0 = input_Color_0;
    output_0.TexCoord_0 = input_TexCoord_0;
    output_0.TexIndex_0 = input_TexIndex_0;
    VS_OUTPUT_0 _S1 = output_0;

#line 37
    gl_Position = output_0.Pos_0;

#line 37
    entryPointParam_VSMain_Color_0 = _S1.Color_0;

#line 37
    entryPointParam_VSMain_TexCoord_0 = _S1.TexCoord_0;

#line 37
    entryPointParam_VSMain_TexIndex_0 = _S1.TexIndex_0;

#line 37
    return;
}

