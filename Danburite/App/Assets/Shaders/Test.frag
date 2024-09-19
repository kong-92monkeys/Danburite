#version 460
#pragma shader_stage(fragment)

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(0.8f, 0.9f, 1.0f, 1.0f);
}