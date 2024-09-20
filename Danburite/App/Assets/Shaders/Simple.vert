#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/Materials/SimpleMaterial.glsl>
#include <Shaders/ShaderData.glsl>

const uint SIMPLE_MATERIAL_SLOT = 0U;

layout(std430, set = GLOBAL_DESC_SET_LOCATION, binding = GLOBAL_DESC_SIMPLE_MATERIAL_LOCATION) readonly buffer SimpleMaterialBuffer
{
	SimpleMaterial simpleMaterials[];
};

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_DESC_INSTANCE_INFO_LOCATION) readonly buffer InstanceInfoBuffer
{
    InstanceInfo instanceInfos[];
};

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;

layout(location = 0) out vec4 outColor;

void main()
{
	gl_Position = vec4(inPos, 1.0f);

	const int materialId = instanceInfos[gl_InstanceIndex].materialIds[SIMPLE_MATERIAL_SLOT];
	outColor = simpleMaterials[materialId].color;
}