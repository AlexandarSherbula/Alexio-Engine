#version 450
layout(column_major) uniform;
layout(column_major) buffer;

#line 3 0
struct Camera_natural_0
{
    mat4x4 projection_0;
};


#line 6
layout(binding = 0)
layout(std140) uniform block_Camera_natural_0
{
    mat4x4 projection_0;
}camera_0;

#line 8
layout(location = 0)
out vec3 entryPointParam_VSMain_LocalPosition_0;


#line 8
layout(location = 1)
out vec4 entryPointParam_VSMain_Color_0;


#line 8
layout(location = 2)
out float entryPointParam_VSMain_Thickness_0;


#line 8
layout(location = 3)
out float entryPointParam_VSMain_Fade_0;


#line 8
layout(location = 0)
in vec3 input_Pos_0;


#line 8
layout(location = 1)
in vec3 input_LocalPosition_0;


#line 8
layout(location = 2)
in vec4 input_Color_0;


#line 8
layout(location = 3)
in float input_Thickness_0;


#line 8
layout(location = 4)
in float input_Fade_0;


#line 17
struct VS_OUTPUT_0
{
    vec4 Pos_0;
    vec3 LocalPosition_0;
    vec4 Color_0;
    float Thickness_0;
    float Fade_0;
};


void main()
{
    VS_OUTPUT_0 output_0;
    output_0.LocalPosition_0 = input_LocalPosition_0;
    output_0.Color_0 = input_Color_0;
    output_0.Thickness_0 = input_Thickness_0;
    output_0.Fade_0 = input_Fade_0;

    output_0.Pos_0 = (((camera_0.projection_0) * (vec4(input_Pos_0, 1.0))));
    VS_OUTPUT_0 _S1 = output_0;

#line 36
    gl_Position = output_0.Pos_0;

#line 36
    entryPointParam_VSMain_LocalPosition_0 = _S1.LocalPosition_0;

#line 36
    entryPointParam_VSMain_Color_0 = _S1.Color_0;

#line 36
    entryPointParam_VSMain_Thickness_0 = _S1.Thickness_0;

#line 36
    entryPointParam_VSMain_Fade_0 = _S1.Fade_0;

#line 36
    return;
}

