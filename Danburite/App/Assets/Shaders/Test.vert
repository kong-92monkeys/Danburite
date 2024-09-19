#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/Constants.glsl>

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;

void main()
{
    gl_Position = vec4(inPos, 1.0f);
}