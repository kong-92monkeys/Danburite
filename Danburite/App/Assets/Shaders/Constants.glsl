#ifndef CONSTANTS_GLSL
#define CONSTANTS_GLSL

const uint VERTEX_ATTRIB_POS_LOCATION					= 0U;
const uint VERTEX_ATTRIB_UV_LOCATION					= 1U;
const uint VERTEX_ATTRIB_NORMAL_LOCATION				= 2U;
const uint VERTEX_ATTRIB_COLOR_LOCATION					= 3U;
const uint VERTEX_ATTRIB_TANGENT_LOCATION				= 4U;

const uint VERTEX_ATTRIB_POS_BIT						= (1U << VERTEX_ATTRIB_POS_LOCATION);
const uint VERTEX_ATTRIB_UV_BIT							= (1U << VERTEX_ATTRIB_UV_LOCATION);
const uint VERTEX_ATTRIB_NORMAL_BIT						= (1U << VERTEX_ATTRIB_NORMAL_LOCATION);
const uint VERTEX_ATTRIB_COLOR_BIT						= (1U << VERTEX_ATTRIB_COLOR_LOCATION);
const uint VERTEX_ATTRIB_TANGENT_BIT					= (1U << VERTEX_ATTRIB_TANGENT_LOCATION);

// Descriptor set locations
const uint GLOBAL_DESC_SET_LOCATION						= 0U;
const uint MATERIALS_DESC_SET_LOCATION					= 1U;
const uint SAMPLED_IMAGES_DESC_SET_LOCATION				= 2U;
const uint SUB_LAYER_DESC_SET_LOCATION					= 3U;
const uint RENDERER_DESC_SET_LOCATION					= 4U;

// Descriptor locations

// Global desc set
const uint GLOBAL_DATA_BUFFER_LOCATION				= 0U;

// Material desc set
const uint TRANSFORM_MATERIAL_LOCATION				= 0U;
const uint PHONG_MATERIAL_LOCATION					= 1U;

// Sampled images desc set
const uint SAMPLED_IMAGES_LOCATION					= 0U;

// Sub layer desc set
const uint INSTANCE_INFO_BUFFER_LOCATION			= 0U;

// Renderer desc set
const uint PHONG_RENDERER_IMAGE_SAMLER_LOCATION		= 0U;

#endif