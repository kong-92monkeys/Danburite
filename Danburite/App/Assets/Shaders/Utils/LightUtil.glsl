#ifndef LIGHT_UTIL_GLSL
#define LIGHT_UTIL_GLSL

#include <Shaders/Materials/LightMaterial.glsl>

const uint LIGHT_TYPE_DIRECTIONAL	= 0U;
const uint LIGHT_TYPE_POINT			= 1U;
const uint LIGHT_TYPE_SPOT			= 2U;

float LightUtil_calcDiffuseFactor(
	in const vec3 lightDir,
	in const vec3 normal)
{
	return max(dot(-lightDir, normal), 0.0f);
}

float LightUtil_calcSpecularFactor(
	in const vec3 lightDir,
	in const vec3 cameraPos,
	in const vec3 position,
	in const vec3 normal,
	in const float shininess)
{
	const vec3 lightReflectionDir = reflect(lightDir, normal);
	const vec3 viewDir = normalize(position - cameraPos);

	return pow(max(dot(lightReflectionDir, -viewDir), 0.0f), shininess);
}

float LightUtil_calcFactor_directional(
	in const LightMaterial light,
	in const vec3 cameraPos,
	in const vec3 position,
	in const vec3 normal,
	in const float shininess)
{
	const float ambientFactor	= light.ambientFactor;
	const float diffuseFactor	= LightUtil_calcDiffuseFactor(light.direction, normal);
	const float specularFactor	= LightUtil_calcSpecularFactor(light.direction, cameraPos, position, normal, shininess);

	return (ambientFactor + diffuseFactor + specularFactor);
}

float LightUtil_calcFactor_point(
	in const LightMaterial light,
	in const vec3 cameraPos,
	in const vec3 position,
	in const vec3 normal,
	in const float shininess)
{
	const vec3 lightDir = normalize(position - light.position);

	const float ambientFactor	= light.ambientFactor;
	const float diffuseFactor	= LightUtil_calcDiffuseFactor(lightDir, normal);
	const float specularFactor	= LightUtil_calcSpecularFactor(lightDir, cameraPos, position, normal, shininess);

	return (ambientFactor + diffuseFactor + specularFactor);
}

vec3 LightUtil_calcColor(
	in const LightMaterial light,
	in const vec3 cameraPos,
	in const vec3 position,
	in const vec3 normal,
	in const float shininess)
{
	float factor = 0.0f;

	switch (light.type)
	{
		case LIGHT_TYPE_DIRECTIONAL:
			factor = LightUtil_calcFactor_directional(light, cameraPos, position, normal, shininess);
			break;

		case LIGHT_TYPE_POINT:
			factor = LightUtil_calcFactor_point(light, cameraPos, position, normal, shininess);
			break;

		case LIGHT_TYPE_SPOT:
			break;
	}

	return (light.color * factor);
}

#endif