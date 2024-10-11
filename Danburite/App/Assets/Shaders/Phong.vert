#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/VertexInput.vert>
#include <Shaders/ShaderData.glsl>
#include <Shaders/Materials/TransformMaterial.glsl>

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

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = INSTANCE_INFO_BUFFER_LOCATION) readonly buffer InstanceInfoBuffer
{
    InstanceInfo instanceInfos[];
};

layout(push_constant) uniform PushConstants
{
	uint vertexAttribFlags;
};

layout(location = 0) out flat int instanceIndex;
layout(location = 1) out vec3 worldPos;
layout(location = 2) out vec3 worldNormal;
layout(location = 3) out vec4 outColor;
layout(location = 4) out vec2 outUV0;
layout(location = 5) out vec2 outUV1;
layout(location = 6) out vec2 outUV2;
layout(location = 7) out vec2 outUV3;

void main()
{
	const int transformMaterialId				= instanceInfos[gl_InstanceIndex].materialIds[0];
	const TransformMaterial transformMaterial	= transformMaterials[transformMaterialId];

	const mat4 modelMatrix		= transformMaterial.transform;
	const mat3 normalMatrix		= transformMaterial.normalTransform;

	const mat4 mvp = (globalData.projMatrix * globalData.viewMatrix * modelMatrix);
	gl_Position = (mvp * vec4(inPos, 1.0f));

	instanceIndex = gl_InstanceIndex;
	worldPos = (modelMatrix * vec4(inPos, 1.0f)).xyz;
	worldNormal = (normalMatrix * inNormal);

	if (bool(vertexAttribFlags & VERTEX_ATTRIB_COLOR_BIT))
		outColor = inColor;

	if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[0]))
		outUV0 = inUV0;

	if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[1]))
		outUV1 = inUV1;

	if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[2]))
		outUV2 = inUV2;

	if (bool(vertexAttribFlags & VERTEX_ATTRIB_UV_BITS[3]))
		outUV3 = inUV3;
}