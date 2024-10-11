//? #version 460
#ifndef VERTEX_INPUT_GLSL
#define VERTEX_INPUT_GLSL

#include <Shaders/Constants.glsl>

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;
layout(location = VERTEX_ATTRIB_NORMAL_LOCATION) in vec3 inNormal;
layout(location = VERTEX_ATTRIB_TANGENT_LOCATION) in vec3 inTangent;
layout(location = VERTEX_ATTRIB_COLOR_LOCATION) in vec4 inColor;

layout(location = VERTEX_ATTRIB_UV_LOCATIONS[0]) in vec2 inUV0;
layout(location = VERTEX_ATTRIB_UV_LOCATIONS[1]) in vec2 inUV1;
layout(location = VERTEX_ATTRIB_UV_LOCATIONS[2]) in vec2 inUV2;
layout(location = VERTEX_ATTRIB_UV_LOCATIONS[3]) in vec2 inUV3;

#endif