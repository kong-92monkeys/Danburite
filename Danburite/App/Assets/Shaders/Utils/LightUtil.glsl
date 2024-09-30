#ifndef LIGHT_UTIL_GLSL
#define LIGHT_UTIL_GLSL

#include <Shaders/Materials/LightMaterial.glsl>

const uint LIGHT_TYPE_DIRECTIONAL	= 0U;
const uint LIGHT_TYPE_POINT			= 1U;
const uint LIGHT_TYPE_SPOT			= 2U;

vec3 LightUtil_calcAmbientColor(
	in const LightMaterial light)
{
	return (light.color * light.ambientFactor);
}

vec3 LightUtil_calcDiffuseColor_directional(
	in const LightMaterial light, in const vec3 normal)
{
	const float diffuseFactor = max(dot(-(light.direction), normal), 0.0f);
	return (light.color * diffuseFactor);
}

vec3 LightUtil_calcDiffuseColor_point(
	in const LightMaterial light, in const vec3 normal)
{
	return vec3(0.0f, 0.0f, 0.0f);
}

vec3 LightUtil_calcDiffuseColor_spot(
	in const LightMaterial light, in const vec3 normal)
{
	return vec3(0.0f, 0.0f, 0.0f);
}

vec3 LightUtil_calcDiffuseColor(
	in const LightMaterial light, in const vec3 normal)
{
	switch (light.type)
	{
		case LIGHT_TYPE_DIRECTIONAL:
			return LightUtil_calcDiffuseColor_directional(light, normal);

		case LIGHT_TYPE_POINT:
			return LightUtil_calcDiffuseColor_point(light, normal);

		case LIGHT_TYPE_SPOT:
			return LightUtil_calcDiffuseColor_spot(light, normal);
	}

	return vec3(0.0f, 0.0f, 0.0f);
}

vec3 LightUtil_calcColor(
	in const LightMaterial light, in const vec3 normal)
{
	vec3 retVal = vec3(0.0f, 0.0f, 0.0f);

	retVal += LightUtil_calcAmbientColor(light);
	retVal += LightUtil_calcDiffuseColor(light, normal);

	return retVal;
}

#endif