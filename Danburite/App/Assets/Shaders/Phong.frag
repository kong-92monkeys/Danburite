#version 460
#pragma shader_stage(fragment)

#extension GL_EXT_nonuniform_qualifier : require

#include <Shaders/ShaderData.glsl>
#include <Shaders/Images.glsl>
#include <Shaders/Materials/PhongMaterial.glsl>
#include <Shaders/Utils/BlendUtil.glsl>
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

vec4 Phong_getVertexColor()
{
    if (bool(vertexAttribFlags & VERTEX_ATTRIB_COLOR_BIT))
        return inColor;

    return vec4(1.0f);
}

void Phong_blendMaterialColor(
    const PhongMaterial phongMaterial,
    out vec3 outAmbient,
    out vec3 outDiffuse,
    out vec3 outSpecular,
    out float outAlpha)
{
    const vec4 vertexColor = Phong_getVertexColor();

    const vec3 materialAmbient      = phongMaterial.ambient;
    const vec3 materialDiffuse      = phongMaterial.diffuse;
    const vec3 materialSpecular     = phongMaterial.specular;
    const vec3 dstColor             = vertexColor.rgb;

    const float srcAlpha = phongMaterial.opacity;
    const float dstAlpha = vertexColor.a;

	const float backAlpha = (dstAlpha * (1.0f - srcAlpha));
    outAlpha = (srcAlpha + backAlpha);

    if (phongMaterial.blendOp == ALPHA_BLEND_OP_DEFAULT)
    {
        const vec3 dstColorAdj = (dstColor * backAlpha);

        outAmbient   = ((materialAmbient * srcAlpha) + dstColorAdj);
        outDiffuse   = ((materialDiffuse * srcAlpha) + dstColorAdj);
        outSpecular  = ((materialSpecular * srcAlpha) + dstColorAdj);

        outAmbient   /= outAlpha;
        outDiffuse   /= outAlpha;
        outSpecular  /= outAlpha;
    }
    else
    {
        outAmbient   = ((materialAmbient * srcAlpha) + dstColor);
        outDiffuse   = ((materialDiffuse * srcAlpha) + dstColor);
        outSpecular  = ((materialSpecular * srcAlpha) + dstColor);
    }
}

vec3 Phong_blendTex(
    const TextureParam texParam,
    const vec2 uv,
    const vec3 dst)
{
    const int texId = texParam.id;
    if (texId < 0)
        return dst;

    vec3 src = texture(sampler2D(sampledImages[texId], imageSampler), uv).rgb;
    src *= texParam.strength;
    return BlendUtil_blendTex(texParam.blendOp, src, dst);
}

float Phong_blendTex(
    const TextureParam texParam,
    const vec2 uv,
    const float dst)
{
    const int texId = texParam.id;
    if (texId < 0)
        return dst;

    float src = texture(sampler2D(sampledImages[texId], imageSampler), uv).r;
    src *= texParam.strength;
    return BlendUtil_blendTex(texParam.blendOp, src, dst);
}

void Phong_blendTexChannel(
    const PhongMaterial phongMaterial,
    const uint channel,
    const vec2 uv,
    inout vec3 objectAmbient,
    inout vec3 objectDiffuse,
    inout vec3 objectSpecular,
    inout vec3 objectEmissive,
    inout float objectAlpha,
    inout float occlusion)
{
    objectAmbient   = Phong_blendTex(phongMaterial.ambientTexs[channel], uv, objectAmbient);
    objectDiffuse   = Phong_blendTex(phongMaterial.diffuseTexs[channel], uv, objectDiffuse);
    objectSpecular  = Phong_blendTex(phongMaterial.specularTexs[channel], uv, objectSpecular);
    objectEmissive  = Phong_blendTex(phongMaterial.emissiveTexs[channel], uv, objectEmissive);
    objectAlpha     = Phong_blendTex(phongMaterial.opacityTexs[channel], uv, objectAlpha);
    occlusion       = Phong_blendTex(phongMaterial.aoTexs[channel], uv, occlusion);
}

void Phong_calcObjectColors(
    const PhongMaterial phongMaterial,
    out vec3 outAmbient,
    out vec3 outDiffuse,
    out vec3 outSpecular,
    out vec3 outEmissive,
    out float outAlpha)
{
    Phong_blendMaterialColor(
        phongMaterial,
        outAmbient, outDiffuse, outSpecular,
        outAlpha);

    outEmissive = phongMaterial.emissive;

    float occlusion = 1.0f;

    if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[0U]))
    {
        Phong_blendTexChannel(
            phongMaterial, 0U, inUV0,
            outAmbient, outDiffuse, outSpecular, outEmissive,
            outAlpha, occlusion);
    }

    if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[1U]))
    {
        Phong_blendTexChannel(
            phongMaterial, 1U, inUV1,
            outAmbient, outDiffuse, outSpecular, outEmissive,
            outAlpha, occlusion);
    }

    if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[2U]))
    {
        Phong_blendTexChannel(
            phongMaterial, 2U, inUV2,
            outAmbient, outDiffuse, outSpecular, outEmissive,
            outAlpha, occlusion);
    }

    if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[3U]))
    {
        Phong_blendTexChannel(
            phongMaterial, 3U, inUV3,
            outAmbient, outDiffuse, outSpecular, outEmissive,
            outAlpha, occlusion);
    }

    outAmbient   *= occlusion;
    outDiffuse   *= occlusion;
    outSpecular  *= occlusion;
}

void Phong_calcLightColors(
	const float shininess,
    out vec3 outAmbient,
    out vec3 outDiffuse,
    out vec3 outSpecular)
{
    outAmbient = vec3(0.0f);
    outDiffuse = vec3(0.0f);
    outSpecular = vec3(0.0f);

    for (uint lightIter = 0U; lightIter < globalData.lightCount; ++lightIter)
    {
        uint lightIndex = globalData.lightIndices[lightIter];

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        LightUtil_calcColor(
            lightMaterials[lightIndex], shininess,
            globalData.cameraPos, worldPos, worldNormal,
            ambient, diffuse, specular);

        outAmbient      += ambient;
        outDiffuse      += diffuse;
        outSpecular     += specular;
    }
}

void main()
{
    const int phongMaterialId = instanceInfos[instanceIndex].materialIds[PHONG_RENDERER_PHONG_MATERIAL_SLOT_IDX];
    const PhongMaterial phongMaterial = phongMaterials[phongMaterialId];

    vec3 objectAmbient;
    vec3 objectDiffuse;
    vec3 objectSpecular;
    vec3 objectEmissive;
    float objectAlpha;

    Phong_calcObjectColors(
        phongMaterial,
        objectAmbient, objectDiffuse, objectSpecular, objectEmissive,
        objectAlpha);
    
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;

    Phong_calcLightColors(
        phongMaterial.shininess,
        lightAmbient, lightDiffuse, lightSpecular);

    outColor.rgb    = (objectAmbient * lightAmbient);
    outColor.rgb    += (objectDiffuse * lightDiffuse);
    outColor.rgb    += (objectSpecular * lightSpecular);
    outColor.rgb    += objectEmissive;
    outColor.a      = objectAlpha;
}