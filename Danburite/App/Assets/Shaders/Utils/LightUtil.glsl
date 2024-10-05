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

float LightUtil_calcIntensity_directional(
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

float LightUtil_calcIntensity_point(
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

float LightUtil_calcIntensity_spot(
	in const LightMaterial light,
	in const vec3 cameraPos,
	in const vec3 position,
	in const vec3 normal,
	in const float shininess)
{
	const float ambientFactor	= light.ambientFactor;

	const vec3 lightDir		= normalize(position - light.position);
	const float cosAngle	= dot(light.direction, lightDir);

	float cutOffFactor = ((cosAngle - light.cosOuterCutOff) / (light.cosInnerCutOff - light.cosOuterCutOff));
	cutOffFactor = min(cutOffFactor, 1.0f);

	if (cutOffFactor <= 0.0f)
		return ambientFactor;

	const float diffuseFactor	= LightUtil_calcDiffuseFactor(lightDir, normal);
	const float specularFactor	= LightUtil_calcSpecularFactor(lightDir, cameraPos, position, normal, shininess);

	return (ambientFactor + (cutOffFactor * (diffuseFactor + specularFactor)));
}

float LightUtil_calcIntensity(
	in const LightMaterial light,
	in const vec3 cameraPos,
	in const vec3 position,
	in const vec3 normal,
	in const float shininess)
{
	float intensity = 0.0f;

	switch (light.type)
	{
		case LIGHT_TYPE_DIRECTIONAL:
			intensity = LightUtil_calcIntensity_directional(light, cameraPos, position, normal, shininess);
			break;

		case LIGHT_TYPE_POINT:
			intensity = LightUtil_calcIntensity_point(light, cameraPos, position, normal, shininess);
			break;

		case LIGHT_TYPE_SPOT:
			intensity = LightUtil_calcIntensity_spot(light, cameraPos, position, normal, shininess);
			break;
	}

	return intensity;
}

float LightUtil_calcAttenuation(
	in const vec3 attenuation,
	in const float lightDistance)
{
	const float attConst	= attenuation[0];
	const float attLinear	= attenuation[1];
	const float attQuad		= attenuation[2];
	return (1.0 / (attConst + (attLinear * lightDistance) + (attQuad * lightDistance * lightDistance)));
}

vec3 LightUtil_calcColor(
	in const LightMaterial light,
	in const vec3 cameraPos,
	in const vec3 position,
	in const vec3 normal,
	in const float shininess)
{
	vec3 retVal = vec3(0.0f, 0.0f, 0.0f);

	const float lightDistance = length(position - light.position);
	if (lightDistance < light.maxDistance)
	{
		const float intensity		= LightUtil_calcIntensity(light, cameraPos, position, normal, shininess);
		const float attenuation		= LightUtil_calcAttenuation(light.attenuation, lightDistance);

		retVal = ((intensity * attenuation) * light.color);
	}

	return retVal;
}

#endif