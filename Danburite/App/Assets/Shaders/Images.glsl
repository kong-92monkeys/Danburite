//? #version 460
#ifndef IMAGES_GLSL
#define IMAGES_GLSL

#include <Shaders/Constants.glsl>

layout(set = SAMPLED_IMAGES_DESC_SET_LOCATION, binding = SAMPLED_IMAGES_LOCATION) uniform texture2D sampledImages[];

#endif