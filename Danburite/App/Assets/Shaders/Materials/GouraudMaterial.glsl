#ifndef GOURAUD_MATERIAL_GLSL
#define GOURAUD_MATERIAL_GLSL

#include <Shaders/Constants.glsl>
#include <Shaders/MaterialParams.glsl>

struct GouraudMaterial
{
	vec3 ambient;
	vec3 diffuse;
	vec3 emissive;

	uint blendOp;
};

#endif