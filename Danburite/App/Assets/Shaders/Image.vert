#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/Constants.glsl>

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;
layout(location = VERTEX_ATTRIB_TEXCOORD_LOCATION) in vec2 inUV;

layout(location = 0) out flat int instanceIndex;
layout(location = 1) out vec2 outUV;

void main()
{
	gl_Position = vec4(inPos, 1.0f);

	instanceIndex = gl_InstanceIndex;
	outUV = inUV;
}