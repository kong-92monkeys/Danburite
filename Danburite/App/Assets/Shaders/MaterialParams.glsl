//? #version 460
#ifndef MATERIAL_PARAMS_GLSL
#define MATERIAL_PARAMS_GLSL

const uint TEXTURE_BLEND_OP_MULTIPLY	= 0U;
const uint TEXTURE_BLEND_OP_ADD			= 1U;
const uint TEXTURE_BLEND_OP_SUBTRACT	= 2U;
const uint TEXTURE_BLEND_OP_DIVIDE		= 3U;
const uint TEXTURE_BLEND_OP_SMOOTH_ADD	= 4U;
const uint TEXTURE_BLEND_OP_SIGNED_ADD	= 5U;

const uint TEXTURE_MAP_MODE_WRAP		= 0U;
const uint TEXTURE_MAP_MODE_CLAMP		= 1U;
const uint TEXTURE_MAP_MODE_DECAL		= 2U;
const uint TEXTURE_MAP_MODE_MIRROR		= 3U;

const uint ALPHA_BLEND_OP_DEFAULT		= 0U;
const uint ALPHA_BLEND_OP_ADDITIVE		= 1U;

struct TextureParam
{
	int id;
	float strength;
	uint blendOp;
	uint mapModeU;
	uint mapModeV;
	bool inverted;
};

#endif