#version 460
#pragma shader_stage(fragment)

#include <Shaders/Constants.glsl>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Images.glsl>
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

layout(location = 0) in flat int instanceIndex;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main()
{
	const int materialId = instanceInfos[instanceIndex].materialIds[IMAGE_MATERIAL_SLOT];
    const uint imageId = imageMaterials[materialId].imageId;
//    const vec4 texColor = texture(sampler2D(sampledImages[imageId], albedoTexSampler), inUV);
    outColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}