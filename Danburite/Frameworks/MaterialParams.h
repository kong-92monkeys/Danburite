#pragma once

#include <cstdint>

namespace Frx
{
	enum class RendererType
	{
		/*
			Flat shading. Shading is done on per-face base,
			diffuse only. Also known as 'faceted shading'.
		*/
		FLAT,

		/*
			Simple Gouraud shading
		*/
		GOURAUD,

		/*
			Phong-Shading
		*/
		PHONG,

		/** Phong-Blinn-Shading
			*/
		BLINN,

		/*
			Toon-Shading per pixel
			Also known as 'comic' shader.
		*/
		TOON,

		/*
			OrenNayar-Shading per pixel
			Extension to standard Lambertian shading, taking the
			roughness of the material into account
		*/
		OREN_NAYAR,

		/*
			Minnaert-Shading per pixel
			Extension to standard Lambertian shading, taking the
			"darkness" of the material into account
		*/
		MINNAERT,

		/*
			CookTorrance-Shading per pixel
			Special shader for metallic surfaces.
		*/
		COOK_TORRANCE,

		/*
			No shading at all. Constant light influence of 1.0.
		*/
		UNLIT,

		/*
			Fresnel shading
		*/
		FRESNEL,

		/*
			Physically-Based Rendering (PBR) shading using
			Bidirectional scattering/reflectance distribution function (BSDF/BRDF)
			There are multiple methods under this banner, and model files may provide
			data for more than one PBR-BRDF method.
			Applications should use the set of provided properties to determine which
			of their preferred PBR rendering methods are likely to be available
			eg:
			- If AI_MATKEY_METALLIC_FACTOR is set, then a Metallic/Roughness is available
			- If AI_MATKEY_GLOSSINESS_FACTOR is set, then a Specular/Glossiness is available
			Note that some PBR methods allow layering of techniques
		*/
		PBR_BRDF
	};

	enum class TextureType : uint32_t
	{
		/*
			No texture, but the value to be used as 'texture semantic'
		*/
		NONE,

		/*
			The texture is combined with the result of the ambient
			lighting equation.
		*/
		AMBIENT,

		/*
			The texture is combined with the result of the diffuse
			lighting equation.
			OR
			PBR Specular/Glossiness
		*/
		DIFFUSE,

		/*
			The texture is combined with the result of the specular
			lighting equation.
			OR
			PBR Specular/Glossiness
		*/
		SPECULAR,

		/*
			The texture is added to the result of the lighting
			calculation. It isn't influenced by incoming light.
		*/
		EMISSIVE,

		/*
			The texture is a height map.
			By convention, higher gray-scale values stand for
			higher elevations from the base height.
		*/
		HEIGHT,

		/*
			The texture is a (tangent space) normal-map.
			Again, there are several conventions for tangent-space
			normal maps.
		*/
		NORMALS,

		/*
			The texture defines the glossiness of the material.
			The glossiness is in fact the exponent of the specular
			(phong) lighting equation. Usually there is a conversion
			function defined to map the linear color values in the
			texture to a suitable exponent.
		*/
		SHININESS,

		/*
			The texture defines per-pixel opacity.
			Usually 'white' means opaque and 'black' means
			'transparency'. Or quite the opposite.
		*/
		OPACITY,

		/*
			Displacement texture
			The exact purpose and format is application-dependent.
			Higher color values stand for higher vertex displacements.
		*/
		DISPLACEMENT,

		/*
			Lightmap texture (aka Ambient Occlusion)
			Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
			covered by this material property. The texture contains a
			scaling value for the final color value of a pixel. Its
			intensity is not affected by incoming light.
		*/
		LIGHTMAP,

		/*
			Reflection texture
			Contains the color of a perfect mirror reflection.
			Rarely used, almost never for real-time applications.
		*/
		REFLECTION,

		/*
			PBR Materials
			PBR definitions from maya and other modelling packages now use this standard.
			This was originally introduced around 2012.
			Support for this is in game engines like Godot, Unreal or Unity3D.
			Modelling packages which use this are very common now.
		*/
		BASE_COLOR,
		NORMAL_CAMERA,
		EMISSION_COLOR,
		METALNESS,
		DIFFUSE_ROUGHNESS,
		AMBIENT_OCCLUSION,

		/*
			Unknown texture
			A texture reference that does not match any of the definitions
			above is considered to be 'unknown'.
		*/
		UNKNOWN,

		/*
			Sheen
			Generally used to simulate textiles that are covered in a layer of microfibers
			eg velvet
			https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_sheen
		*/
		SHEEN,

		/*
			Clearcoat
			Simulates a layer of 'polish' or 'lacquer' layered on top of a PBR substrate
			https://autodesk.github.io/standard-surface/#closures/coating
			https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat
		*/
		CLEARCOAT,

		/*
			Transmission
			Simulates transmission through the surface
			May include further information such as wall thickness
		*/
		TRANSMISSION,

		/*
			Maya material declarations
		*/
		MAYA_BASE,
		MAYA_SPECULAR,
		MAYA_SPECULAR_COLOR,
		MAYA_SPECULAR_ROUGHNESS
	};

	enum class TextureBlendOp : uint32_t
	{
		// T = T1 * T2
		MULTIPLY,

		// T = T1 + T2
		ADD,

		// T = T1 - T2
		SUBTRACT,

		// T = T1 / T2
		DIVIDE,

		// T = (T1 + T2) - (T1 * T2)
		SMOOTH_ADD,

		// T = T1 + (T2-0.5)
		SIGNED_ADD,
	};

	enum class TextureMapMode : uint32_t
	{
		/*
			A texture coordinate u|v is translated to u%1|v%1
		*/
		WRAP,

		/*
			Texture coordinates outside [0...1] are clamped to the nearest valid value.
		*/
		CLAMP,

		/*
			If the texture coordinates for a pixel are outside [0...1]
			the texture is not applied to that pixel
		*/
		DECAL,

		/*
			A texture coordinate u|v becomes u%1|v%1 if (u-(u%1))%2 is zero and
			1-(u%1)|1-(v%1) otherwise
		*/
		MIRROR
	};

	enum class AlphaBlendOp : uint32_t
	{
		/*
			Color = Porter-Duff "Over" op
			Alpha = SourceAlpha + DestAlpha*(1-SourceAlpha)
		*/
		DEFAULT,

		/*
			Color = SourceColor*SourceAlpha + DestColor*1
			Alpha = SourceAlpha + DestAlpha*(1-SourceAlpha)
		*/
		ADDITIVE
	};
}