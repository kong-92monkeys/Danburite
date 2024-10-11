#version 460
#pragma shader_stage(fragment)

#extension GL_EXT_nonuniform_qualifier : require

#include <Shaders/Constants.glsl>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Images.glsl>
#include <Shaders/Materials/PhongMaterial.glsl>
#include <Shaders/Utils/LightUtil.glsl>

layout(std430, set = GLOBAL_DESC_SET_LOCATION, binding = GLOBAL_DATA_BUFFER_LOCATION) readonly buffer GlobalDataBuffer
{
    mat4 viewMatrix;
    mat4 projMatrix;
	vec3 cameraPos;
	uint lightCount;
	uint lightIndices[];
} globalData;

layout(std430, set = MATERIALS_DESC_SET_LOCATION, binding = PHONG_MATERIAL_LOCATION) readonly buffer PhongMaterialBuffer
{
	PhongMaterial phongMaterials[];
};

layout(std430, set = MATERIALS_DESC_SET_LOCATION, binding = LIGHT_MATERIAL_LOCATION) readonly buffer LightMaterialBuffer
{
	LightMaterial lightMaterials[];
};

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = INSTANCE_INFO_BUFFER_LOCATION) readonly buffer InstanceInfoBuffer
{
    InstanceInfo instanceInfos[];
};

layout(set = RENDERER_DESC_SET_LOCATION, binding = 0) uniform sampler imageSampler;

layout(push_constant) uniform PushConstants
{
	uint vertexAttribFlags;
};

layout(location = 0) in flat int instanceIndex;
layout(location = 1) in vec3 worldPos;
layout(location = 2) in vec3 worldNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec2 inUV0;
layout(location = 5) in vec2 inUV1;
layout(location = 6) in vec2 inUV2;
layout(location = 7) in vec2 inUV3;

layout(location = 0) out vec4 outColor;

void main()
{
    const int phongMaterialId = instanceInfos[instanceIndex].materialIds[1U];
    const PhongMaterial material = phongMaterials[phongMaterialId];

    vec4 materialColor = (bool(vertexAttribFlags & VERTEX_ATTRIB_COLOR_BIT) ? inColor : vec4(1.0f, 1.0f, 1.0f, 1.0f));
    if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[0]))
    {
        const int albedoTexId = material.albedoTexId;

        if (albedoTexId >= 0)
            materialColor *= texture(sampler2D(sampledImages[albedoTexId], imageSampler), inUV0);
    }

    vec3 lightColor = vec3(0.0f, 0.0f, 0.0f);
    for (uint lightIter = 0U; lightIter < globalData.lightCount; ++lightIter)
    {
        uint lightIndex = globalData.lightIndices[lightIter];

        lightColor += LightUtil_calcColor(
            lightMaterials[lightIndex], globalData.cameraPos,
            worldPos, worldNormal, material.shininess);
    }

    outColor = vec4(materialColor.rgb * lightColor, 1.0f);
}