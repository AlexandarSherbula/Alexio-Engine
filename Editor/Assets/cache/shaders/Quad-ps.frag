#version 450
layout(column_major) uniform;
layout(column_major) buffer;

#line 18 0
layout(binding = 0)
uniform sampler2D  textures_0[32];


#line 2
vec4 GetSampledTexture_0(vec2 texCoord_0, int texIndex_0)
{

#line 81
    return (texture((textures_0[texIndex_0]), (texCoord_0)));
}


#line 81
layout(location = 0)
out vec4 entryPointParam_PSMain_0;


#line 81
layout(location = 0)
in vec4 output_Color_0;


#line 81
layout(location = 1)
in vec2 output_TexCoord_0;


#line 81
flat layout(location = 2)
in int output_TexIndex_0;


#line 41
void main()
{
    switch(output_TexIndex_0)
    {
    case 0:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 0) * output_Color_0;

#line 43
            return;
        }
    case 1:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 1) * output_Color_0;

#line 43
            return;
        }
    case 2:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 2) * output_Color_0;

#line 43
            return;
        }
    case 3:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 3) * output_Color_0;

#line 43
            return;
        }
    case 4:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 4) * output_Color_0;

#line 43
            return;
        }
    case 5:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 5) * output_Color_0;

#line 43
            return;
        }
    case 6:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 6) * output_Color_0;

#line 43
            return;
        }
    case 7:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 7) * output_Color_0;

#line 43
            return;
        }
    case 8:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 8) * output_Color_0;

#line 43
            return;
        }
    case 9:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 9) * output_Color_0;

#line 43
            return;
        }
    case 10:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 10) * output_Color_0;

#line 43
            return;
        }
    case 11:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 11) * output_Color_0;

#line 43
            return;
        }
    case 12:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 12) * output_Color_0;

#line 43
            return;
        }
    case 13:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 13) * output_Color_0;

#line 43
            return;
        }
    case 14:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 14) * output_Color_0;

#line 43
            return;
        }
    case 15:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 15) * output_Color_0;

#line 43
            return;
        }
    case 16:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 16) * output_Color_0;

#line 43
            return;
        }
    case 17:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 17) * output_Color_0;

#line 43
            return;
        }
    case 18:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 18) * output_Color_0;

#line 43
            return;
        }
    case 19:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 19) * output_Color_0;

#line 43
            return;
        }
    case 20:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 20) * output_Color_0;

#line 43
            return;
        }
    case 21:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 21) * output_Color_0;

#line 43
            return;
        }
    case 22:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 22) * output_Color_0;

#line 43
            return;
        }
    case 23:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 23) * output_Color_0;

#line 43
            return;
        }
    case 24:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 24) * output_Color_0;

#line 43
            return;
        }
    case 25:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 25) * output_Color_0;

#line 43
            return;
        }
    case 26:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 26) * output_Color_0;

#line 43
            return;
        }
    case 27:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 27) * output_Color_0;

#line 43
            return;
        }
    case 28:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 28) * output_Color_0;

#line 43
            return;
        }
    case 29:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 29) * output_Color_0;

#line 43
            return;
        }
    case 30:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 30) * output_Color_0;

#line 43
            return;
        }
    case 31:
        {

#line 43
            entryPointParam_PSMain_0 = GetSampledTexture_0(output_TexCoord_0, 31) * output_Color_0;

#line 43
            return;
        }
    }

#line 43
    entryPointParam_PSMain_0 = vec4(0.5, 0.5, 0.5, 1.0);

#line 43
    return;
}

