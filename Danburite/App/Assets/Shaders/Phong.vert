#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/Constants.glsl>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Materials/TransformMaterial.glsl>

layout(std430, set = GLOBAL_DESC_SET_LOCATION, binding = GLOBAL_DATA_BUFFER_LOCATION) readonly buffer GlobalDataBuffer
{
    mat4 viewMatrix;
    mat4 projMatrix;
	vec3 cameraPosition;
	int lightIdx;
} globalData;

layout(std430, set = MATERIALS_DESC_SET_LOCATION, binding = TRANSFORM_MATERIAL_LOCATION) readonly buffer TransformMaterialBuffer
{
	TransformMaterial transformMaterials[];
};

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = INSTANCE_INFO_BUFFER_LOCATION) readonly buffer InstanceInfoBuffer
{
    InstanceInfo instanceInfos[];
};

layout(push_constant) uniform PushConstants
{
	uint vertexAttribFlags;
};

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;
layout(location = VERTEX_ATTRIB_UV_LOCATION) in vec2 inUV;
layout(location = VERTEX_ATTRIB_NORMAL_LOCATION) in vec3 inNormal;
layout(location = VERTEX_ATTRIB_COLOR_LOCATION) in vec4 inColor;

layout(location = 0) out flat int instanceIndex;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 worldPos;
layout(location = 3) out vec3 worldNormal;
layout(location = 4) out vec4 outColor;

void main()
{
	const int transformMaterialId				= instanceInfos[gl_InstanceIndex].materialIds[0];
	const TransformMaterial transformMaterial	= transformMaterials[transformMaterialId];

	const mat4 modelMatrix		= transformMaterial.transform;
	const mat3 normalMatrix		= transformMaterial.normalTransform;

	const mat4 mvp = (globalData.projMatrix * globalData.viewMatrix * modelMatrix);
	gl_Position = (mvp * vec4(inPos, 1.0f));

	worldPos = (modelMatrix * vec4(inPos, 1.0f)).xyz;
	worldNormal = (normalMatrix * inNormal);

	if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BIT))
	{
		instanceIndex = gl_InstanceIndex;
		outUV = inUV;
	}

	if (bool(vertexAttribFlags & VERTEX_ATTRIB_COLOR_BIT))
		outColor = inColor;
}