#version 450
layout(column_major) uniform;
layout(column_major) buffer;

#line 8 0
layout(location = 0)
out vec4 entryPointParam_PSMain_0;


#line 8
layout(location = 0)
in vec3 input_LocalPosition_0;


#line 8
layout(location = 1)
in vec4 input_Color_0;


#line 8
layout(location = 2)
in float input_Thickness_0;


#line 8
layout(location = 3)
in float input_Fade_0;


#line 40
void main()
{
    float distance_0 = 1.0 - length(input_LocalPosition_0);


    float circle_0 = smoothstep(0.0, input_Fade_0, distance_0) * smoothstep(input_Thickness_0 + input_Fade_0, input_Thickness_0, distance_0);

    if(circle_0 == 0.0)
    {

#line 48
        discard;

#line 47
    }


    vec4 pixelColor_0 = input_Color_0;
    pixelColor_0[3] = pixelColor_0[3] * circle_0;

#line 51
    entryPointParam_PSMain_0 = pixelColor_0;

#line 51
    return;
}

