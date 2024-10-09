#pragma once

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
}