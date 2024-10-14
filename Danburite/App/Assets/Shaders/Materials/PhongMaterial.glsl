#ifndef PHONG_MATERIAL_GLSL
#define PHONG_MATERIAL_GLSL

#include <Shaders/Constants.glsl>
#include <Shaders/MaterialParams.glsl>

struct PhongMaterial
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emissive;

	uint blendOp;
	float opacity;
	float shininess;

	TextureParam ambientTexs[MAX_TEX_CHANNEL_COUNT];
	TextureParam diffuseTexs[MAX_TEX_CHANNEL_COUNT];
	TextureParam specularTexs[MAX_TEX_CHANNEL_COUNT];
	TextureParam emissiveTexs[MAX_TEX_CHANNEL_COUNT];
	TextureParam normalTexs[MAX_TEX_CHANNEL_COUNT];
	TextureParam opacityTexs[MAX_TEX_CHANNEL_COUNT];
	TextureParam aoTexs[MAX_TEX_CHANNEL_COUNT];
};

#endif