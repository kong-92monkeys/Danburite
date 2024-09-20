#ifndef IMAGES_GLSL
#define IMAGES_GLSL

#include <Shaders/Constants.glsl>

layout(set = MATERIAL_DESC_IMAGE_MATERIAL_LOCATION, binding = 0) uniform texture2D sampledImages[];

#endif