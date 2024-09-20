#ifndef IMAGES_GLSL
#define IMAGES_GLSL

#include <Shaders/Constants.glsl>

layout(set = GLOBAL_DESC_SET_LOCATION, binding = GLOBAL_DESC_SAMPLED_IMAGES_LOCATION) uniform texture2D textures[];

#endif