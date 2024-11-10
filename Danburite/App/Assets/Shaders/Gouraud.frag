#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor.rgb    = inColor;
    outColor.a      = 1.0f;
}