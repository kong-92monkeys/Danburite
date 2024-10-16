#ifndef LIGHT_UTIL_GLSL
#define LIGHT_UTIL_GLSL

#include <Shaders/Materials/LightMaterial.glsl>

const uint LIGHT_TYPE_DIRECTIONAL	= 0U;
const uint LIGHT_TYPE_POINT			= 1U;
const uint LIGHT_TYPE_SPOT			= 2U;

float LightUtil_calcDiffuseFactor(
	in const vec3 lightDir,
	in const vec3 objectNormal)
{
	return max(dot(-lightDir, objectNormal), 0.0f);
}

float LightUtil_calcSpecularFactor(
	const vec3 lightDir,
	const float shininess,
	const vec3 cameraPos,
	const vec3 objectPos,
	const vec3 objectNormal)
{
	const vec3 lightReflectionDir = reflect(lightDir, objectNormal);
	const vec3 viewDir = normalize(objectPos - cameraPos);

	return pow(max(dot(lightReflectionDir, -viewDir), 0.0f), shininess);
}

void LightUtil_calcFactors_directional(
	const LightMaterial light,
	const float shininess,
	const vec3 cameraPos,
	const vec3 objectPos,
	const vec3 objectNormal,
	out float ambientFactor,
	out float diffuseFactor,
	out float specularFactor)
{
	ambientFactor	= light.ambientFactor;
	diffuseFactor	= LightUtil_calcDiffuseFactor(light.direction, objectNormal);
	specularFactor	= LightUtil_calcSpecularFactor(light.direction, shininess, cameraPos, objectPos, objectNormal);
}

void LightUtil_calcFactors_point(
	const LightMaterial light,
	const float shininess,
	const vec3 cameraPos,
	const vec3 objectPos,
	const vec3 objectNormal,
	out float ambientFactor,
	out float diffuseFactor,
	out float specularFactor)
{
	const vec3 lightDir = normalize(objectPos - light.position);

	ambientFactor	= light.ambientFactor;
	diffuseFactor	= LightUtil_calcDiffuseFactor(lightDir, objectNormal);
	specularFactor	= LightUtil_calcSpecularFactor(lightDir, shininess, cameraPos, objectPos, objectNormal);
}

void LightUtil_calcFactors_spot(
	const LightMaterial light,
	const float shininess,
	const vec3 cameraPos,
	const vec3 objectPos,
	const vec3 objectNormal,
	out float ambientFactor,
	out float diffuseFactor,
	out float specularFactor)
{
	ambientFactor	= light.ambientFactor;
	diffuseFactor	= 0.0f;
	specularFactor	= 0.0f;

	const vec3 lightDir		= normalize(objectPos - light.position);
	const float cosAngle	= dot(light.direction, lightDir);

	float cutOffFactor = ((cosAngle - light.cosOuterCutOff) / (light.cosInnerCutOff - light.cosOuterCutOff));
	cutOffFactor = min(cutOffFactor, 1.0f);

	if (cutOffFactor <= 0.0f)
		return;

	diffuseFactor	= LightUtil_calcDiffuseFactor(lightDir, objectNormal);
	specularFactor	= LightUtil_calcSpecularFactor(lightDir, shininess, cameraPos, objectPos, objectNormal);
}

void LightUtil_calcFactors(
	const LightMaterial light,
	const float shininess,
	const vec3 cameraPos,
	const vec3 objectPos,
	const vec3 objectNormal,
	out float ambientFactor,
	out float diffuseFactor,
	out float specularFactor)
{
	switch (light.type)
	{
		case LIGHT_TYPE_DIRECTIONAL:
			LightUtil_calcFactors_directional(
				light, shininess,
				cameraPos, objectPos, objectNormal,
				ambientFactor, diffuseFactor, specularFactor);

			break;

		case LIGHT_TYPE_POINT:
			LightUtil_calcFactors_point(
				light, shininess,
				cameraPos, objectPos, objectNormal,
				ambientFactor, diffuseFactor, specularFactor);

			break;

		case LIGHT_TYPE_SPOT:
			LightUtil_calcFactors_spot(
				light, shininess,
				cameraPos, objectPos, objectNormal,
				ambientFactor, diffuseFactor, specularFactor);

			break;
	}
}

float LightUtil_calcAttenuation(
	const vec3 attenuation,
	const float lightDistance)
{
	const float attConst	= attenuation[0];
	const float attLinear	= attenuation[1];
	const float attQuad		= attenuation[2];
	return (1.0 / (attConst + (attLinear * lightDistance) + (attQuad * lightDistance * lightDistance)));
}

void LightUtil_calcColor(
	const LightMaterial light,
	const float shininess,
	const vec3 cameraPos,
	const vec3 objectPos,
	const vec3 objectNormal,
	out vec3 outAmbient,
	out vec3 outDiffuse,
	out vec3 outSpecular)
{
	float ambientFactor		= 0.0f;
	float diffuseFactor		= 0.0f;
	float specularFactor	= 0.0f;

	const float lightDistance = length(objectPos - light.position);
	if (lightDistance < light.maxDistance)
	{
		LightUtil_calcFactors(
			light, shininess,
			cameraPos, objectPos, objectNormal,
			ambientFactor, diffuseFactor, specularFactor);

		const float attenuation	=
			LightUtil_calcAttenuation(light.attenuation, lightDistance);
		
		ambientFactor	*= attenuation;
		diffuseFactor	*= attenuation;
		specularFactor	*= attenuation;
	}

	outAmbient		= (ambientFactor * light.color);
    outDiffuse		= (diffuseFactor * light.color);
    outSpecular		= (specularFactor * light.color);
}

#endif