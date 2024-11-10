#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/VertexInput.vert>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Materials/TransformMaterial.glsl>
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

layout(std430, set = MATERIALS_DESC_SET_LOCATION, binding = TRANSFORM_MATERIAL_LOCATION) readonly buffer TransformMaterialBuffer
{
	TransformMaterial transformMaterials[];
};

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

layout(push_constant) uniform PushConstants
{
	uint vertexAttribFlags;
};

layout(location = 0) out vec3 outColor;

vec3 Gouraud_getVertexColor()
{
    if (bool(vertexAttribFlags & VERTEX_ATTRIB_COLOR_BIT))
        return inColor.rgb;

    return vec3(1.0f);
}

void Gouraud_blendMaterialColor(
    const PhongMaterial phongMaterial,
    out vec3 outAmbient,
    out vec3 outDiffuse)
{
    const vec3 materialAmbient      = phongMaterial.ambient;
    const vec3 materialDiffuse      = phongMaterial.diffuse;
    const vec3 dstColor             = Gouraud_getVertexColor();

    if (phongMaterial.blendOp == ALPHA_BLEND_OP_DEFAULT)
    {
        outAmbient   = (materialAmbient * dstColor);
        outDiffuse   = (materialDiffuse * dstColor);
    }
    else
    {
        outAmbient   = (materialAmbient + dstColor);
        outDiffuse   = (materialDiffuse + dstColor);
    }
}

void Gouraud_calcObjectColors(
    const PhongMaterial phongMaterial,
    out vec3 outAmbient,
    out vec3 outDiffuse,
    out vec3 outEmissive)
{
    Gouraud_blendMaterialColor(phongMaterial, outAmbient, outDiffuse);
    outEmissive = phongMaterial.emissive;
}

void Gouraud_calcLightColors(
	const float shininess,
    const vec3 worldPos,
    const vec3 worldNormal,
    out vec3 outAmbient,
    out vec3 outDiffuse)
{
    outAmbient = vec3(0.0f);
    outDiffuse = vec3(0.0f);

    for (uint lightIter = 0U; lightIter < globalData.lightCount; ++lightIter)
    {
        uint lightIndex = globalData.lightIndices[lightIter];

        vec3 ambient;
        vec3 diffuse;

        LightUtil_calcColor(
            lightMaterials[lightIndex], shininess,
            globalData.cameraPos, worldPos, worldNormal,
            ambient, diffuse);

        outAmbient      += ambient;
        outDiffuse      += diffuse;
    }
}

void main()
{
	const InstanceInfo instanceInfo				= instanceInfos[gl_InstanceIndex];

	const int transformMaterialId				= instanceInfo.materialIds[PHONG_RENDERER_TRANSFORM_MATERIAL_SLOT_IDX];
	const TransformMaterial transformMaterial	= transformMaterials[transformMaterialId];

	const mat4 modelMatrix		= transformMaterial.transform;
	const mat3 normalMatrix		= transformMaterial.normalTransform;

	const mat4 mvp = (globalData.projMatrix * globalData.viewMatrix * modelMatrix);
	gl_Position = (mvp * vec4(inPos, 1.0f));

	const vec3 worldPos = (modelMatrix * vec4(inPos, 1.0f)).xyz;
	const vec3 worldNormal = (normalMatrix * inNormal);

	const int phongMaterialId					= instanceInfo.materialIds[PHONG_RENDERER_PHONG_MATERIAL_SLOT_IDX];
    const PhongMaterial phongMaterial			= phongMaterials[phongMaterialId];

    vec3 objectAmbient;
    vec3 objectDiffuse;
    vec3 objectEmissive;

    Gouraud_calcObjectColors(
        phongMaterial,
        objectAmbient, objectDiffuse, objectEmissive);

    vec3 lightAmbient;
    vec3 lightDiffuse;

    Gouraud_calcLightColors(
        phongMaterial.shininess, worldPos, worldNormal,
        lightAmbient, lightDiffuse);

    outColor = (objectAmbient * lightAmbient);
    outColor += (objectDiffuse * lightDiffuse);
}