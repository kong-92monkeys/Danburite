#version 460
#pragma shader_stage(fragment)

#extension GL_EXT_nonuniform_qualifier : require

#include <Shaders/Constants.glsl>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Images.glsl>
#include <Shaders/Materials/PhongMaterial.glsl>

layout(std430, set = MATERIALS_DESC_SET_LOCATION, binding = MATERIAL_DESC_PHONG_MATERIAL_LOCATION) readonly buffer PhongMaterialBuffer
{
	PhongMaterial phongMaterials[];
};

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_DESC_INSTANCE_INFO_LOCATION) readonly buffer InstanceInfoBuffer
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
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = (bool(vertexAttribFlags & VERTEX_ATTRIB_COLOR_BIT) ? inColor : vec4(1.0f, 1.0f, 1.0f, 1.0f));

    if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BIT))
    {
        const int phongMaterialId = instanceInfos[instanceIndex].materialIds[1U];
        const int imageId = phongMaterials[phongMaterialId].imageId;

        if (imageId >= 0)
            outColor *= texture(sampler2D(sampledImages[imageId], imageSampler), inUV);
    }
}