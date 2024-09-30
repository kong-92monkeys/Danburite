#ifndef PHONG_MATERIAL_GLSL
#define PHONG_MATERIAL_GLSL

#include <Shaders/Constants.glsl>

struct PhongMaterial
{
	float shininess;
	int albedoTexId;
};

#endif