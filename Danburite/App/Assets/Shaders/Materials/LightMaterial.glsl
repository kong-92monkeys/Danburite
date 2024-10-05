#ifndef LIGHT_MATERIAL_GLSL
#define LIGHT_MATERIAL_GLSL

struct LightMaterial
{
	uint type;
	float ambientFactor;
	float maxDistance;

	vec3 color;
	vec3 position;
	vec3 direction;
	vec3 attenuation;

	float cosInnerCutOff;
	float cosOuterCutOff;
};

#endif