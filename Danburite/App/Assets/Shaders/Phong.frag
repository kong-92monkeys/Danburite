#version 460
#pragma shader_stage(fragment)

#extension GL_EXT_nonuniform_qualifier : require

#include <Shaders/Constants.glsl>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Images.glsl>
#include <Shaders/Materials/PhongMaterial.glsl>
#include <Shaders/Utils/LightUtil.glsl>

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
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 worldNormal;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 materialColor = (bool(vertexAttribFlags & VERTEX_ATTRIB_COLOR_BIT) ? inColor : vec4(1.0f, 1.0f, 1.0f, 1.0f));

    if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BIT))
    {
        const int phongMaterialId = instanceInfos[instanceIndex].materialIds[1U];
        const int imageId = phongMaterials[phongMaterialId].imageId;

        if (imageId >= 0)
            materialColor *= texture(sampler2D(sampledImages[imageId], imageSampler), inUV);
    }

    const vec3 lightColor = LightUtil_calcColor(lightMaterials[0], worldNormal);
    outColor = vec4(materialColor.rgb * lightColor, materialColor.a);
}