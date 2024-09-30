#ifndef LIGHT_MATERIAL_GLSL
#define LIGHT_MATERIAL_GLSL

struct LightMaterial
{
	uint type;
	float ambientFactor;

	vec3 color;
	vec3 position;
	vec3 direction;
};

#endif