#ifndef BLEND_UTIL_GLSL
#define BLEND_UTIL_GLSL

#include <Shaders/MaterialParams.glsl>

vec3 BlendUtil_blendTex(
    const uint texBlendOp,
    const vec3 src,
    const vec3 dst)
{
    vec3 retVal = vec3(0.0f);

    switch (texBlendOp)
    {
        case TEXTURE_BLEND_OP_MULTIPLY:
            retVal = (dst * src);
            break;

        case TEXTURE_BLEND_OP_ADD:
            retVal = (dst + src);
            break;

        case TEXTURE_BLEND_OP_SUBTRACT:
            retVal = (dst - src);
            break;

        case TEXTURE_BLEND_OP_DIVIDE:
            retVal = (dst / src);
            break;

        case TEXTURE_BLEND_OP_SMOOTH_ADD:
            retVal = ((dst + src) - (dst * src));
            break;

        case TEXTURE_BLEND_OP_SIGNED_ADD:
            retVal = (dst + (src - vec3(0.5f)));
            break;
    }

    return retVal;
}

float BlendUtil_blendTex(
    const uint texBlendOp,
    const float src,
    const float dst)
{
    float retVal = 0.0f;

    switch (texBlendOp)
    {
        case TEXTURE_BLEND_OP_MULTIPLY:
            retVal = (dst * src);
            break;

        case TEXTURE_BLEND_OP_ADD:
            retVal = (dst + src);
            break;

        case TEXTURE_BLEND_OP_SUBTRACT:
            retVal = (dst - src);
            break;

        case TEXTURE_BLEND_OP_DIVIDE:
            retVal = (dst / src);
            break;

        case TEXTURE_BLEND_OP_SMOOTH_ADD:
            retVal = ((dst + src) - (dst * src));
            break;

        case TEXTURE_BLEND_OP_SIGNED_ADD:
            retVal = (dst + (src - 0.5f));
            break;
    }

    return retVal;
}

#endif