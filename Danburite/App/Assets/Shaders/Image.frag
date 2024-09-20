#version 460
#pragma shader_stage(fragment)

layout(location = 0) in flat int instanceIndex;
layout(location = 1) in vec2 outTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor;
}