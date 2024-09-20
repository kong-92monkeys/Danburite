#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/Constants.glsl>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Materials/ImageMaterial.glsl>

const uint IMAGE_MATERIAL_SLOT = 0U;

layout(std430, set = MATERIALS_DESC_SET_LOCATION, binding = MATERIAL_DESC_IMAGE_MATERIAL_LOCATION) readonly buffer ImageMaterialBuffer
{
	ImageMaterial imageMaterials[];
};

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_DESC_INSTANCE_INFO_LOCATION) readonly buffer InstanceInfoBuffer
{
    InstanceInfo instanceInfos[];
};

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;
layout(location = VERTEX_ATTRIB_TEXCOORD_LOCATION) in vec2 inTexCoord;

layout(location = 0) out flat int instanceIndex;
layout(location = 1) out vec2 outTexCoord;

void main()
{
	gl_Position = vec4(inPos, 1.0f);

	instanceIndex = gl_InstanceIndex;
	outTexCoord = inTexCoord;
}