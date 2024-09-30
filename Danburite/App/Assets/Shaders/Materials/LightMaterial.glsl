#ifndef LIGHT_MATERIAL_GLSL
#define LIGHT_MATERIAL_GLSL

struct LightMaterial
{
	uint type;
	float ambientFactor;

	vec4 color;
};

#endif