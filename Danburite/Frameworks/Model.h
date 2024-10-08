#pragma once

#include "../Infra/Unique.h"
#include "../Infra/Bitmap.h"
#include "../Vulkan/Vulkan.h"
#include "../Infra/GenericBuffer.h"
#include "Vertex.h"
#include <unordered_map>
#include "SceneNode.h"
#include <queue>

namespace Frx
{
	class Model : public Infra::Unique
	{
	public:
		enum class TextureType
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

		enum class TextureOp
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

		enum class TextureMapMode
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

		enum class ShadingModel
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

		enum class BlendMode
		{
			/*
				SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
			*/
			DEFAULT,

			/*
				SourceColor*1 + DestColor*1
			*/
			ADDITIVE
		};

		struct TextureInfo
		{
		public:
			uint32_t index{ };

			/*
				All color components (rgb) are multiplied
				with this factor before any further processing is done.
			*/
			float blend{ 1.0f };

			/*
				Defines the arithmetic operation
				to be used to combine the n¡¯th texture
			*/
			TextureOp op{ TextureOp::MULTIPLY };

			TextureMapMode mapModeU{ TextureMapMode::WRAP };
			TextureMapMode mapModeV{ TextureMapMode::WRAP };

			bool inverted{ };
			bool useAlpha{ };
		};

		struct Material
		{
		public:
			/*
				Ambient color of the material.
				This is typically scaled by the amount of ambient light
			*/
			glm::vec3 ambient{ 0.f, 0.f, 0.f };

			/*
				Diffuse color of the material.
				This is typically scaled by the amount of incoming diffuse light (e.g. using gouraud shading)
			*/
			glm::vec3 diffuse{ 0.f, 0.f, 0.f };

			/*
				Specular color of the material.
				This is typically scaled by the amount of incoming specular light (e.g. using phong shading)
			*/
			glm::vec3 specular{ 0.f, 0.f, 0.f };

			/*
				Emissive color of the material.
				This is the amount of light emitted by the object.

				In real time applications it will usually not affect surrounding objects,
				but raytracing applications may wish to treat emissive objects as light sources.
			*/
			glm::vec3 emissive{ 0.f, 0.f, 0.f };

			/*
				Defines the transparent color of the material,
				this is the color to be multiplied with the color of translucent light
				to construct the final ¡®destination color¡¯ for a particular position in the screen buffer.
			*/
			glm::vec3 transparent{ 0.f, 0.f, 0.f };

			/*
				Defines the reflective color of the material.
				This is typically scaled by the amount of incoming light
				from the direction of mirror reflection.
				
				Usually combined with an environment lightmap of some kind for real-time applications.
			*/
			glm::vec3 reflective{ 0.f, 0.f, 0.f };

			/*
				Scales the reflective color of the material.
			*/
			float reflectivity{ 0.0f };

			/*
				Specifies whether wireframe rendering must be turned on for the material.
			*/
			bool wireframe{ false };

			/*
				Specifies whether meshes using this material must be rendered without backface culling.
			*/
			bool twoSided{ false };

			/*
				Defines the library shading model to use for (real time) rendering
				to approximate the original look of the material as closely as possible.
				The presence of this key might indicate a more complex material.
				If absent, assume phong shading only if a specular exponent is given.
			*/
			ShadingModel shadingModel{ ShadingModel::GOURAUD };

			/*
				Defines how the final color value in the screen buffer is computed
				from the given color at that position and the newly computed color
				from the material.
				
				Simply said, alpha blending settings.
			*/
			BlendMode blendMode{ BlendMode::DEFAULT };

			/*
				Defines the opacity of the material in a range between 0..1.
				Use this value to decide whether you have to activate alpha blending
				for rendering.
				OPACITY !=1 usually also implies TWOSIDED=1 to avoid cull artifacts.
			*/
			float opacity{ 1.0f };

			/*
				Defines the shininess of a phong-shaded material.
				This is actually the exponent of the phong specular equation
				SHININESS=0 is equivalent to SHADING_MODEL = Gouraud
			*/
			float shininess{ 0.0f };

			/*
				Scales the specular color of the material.
				This value is kept separate from the specular color by most modelers.
			*/
			float shininessStrength{ 1.0f };

			/*
				Defines the Index Of Refraction for the material.
				That¡¯s not supported by most file formats.
				Might be of interest for raytracing.
			*/
			float indexOfRefraction{ 1.0f };

			std::unordered_map<TextureType, std::vector<TextureInfo>> textureInfos;
		};

		struct Mesh
		{
		public:
			uint32_t vertexCount{ };
			std::unordered_map<VertexAttribFlagBits, Infra::GenericBuffer> vertexBuffers;

			uint32_t indexCount{ };
			VkIndexType indexType{ VkIndexType::VK_INDEX_TYPE_UINT16 };
			Infra::GenericBuffer indexBuffer;
		};

		struct Node
		{
		public:
			glm::mat4 transform{ 1.0f };

			std::vector<uint32_t> materials;
			std::vector<uint32_t> meshes;
			std::vector<uint32_t> children;
		};

		struct CreateInfo
		{
		public:
			std::vector<std::unique_ptr<Infra::Bitmap>> textures;
			std::vector<Material> materials;
			std::vector<Mesh> meshes;
			std::vector<Node> nodes;
		};

		Model(
			CreateInfo &&createInfo);

		virtual ~Model() noexcept override;

	private:
		std::vector<SceneNode *> __sceneNodes;
		std::queue<std::vector<glm::mat4>> __transformQue;
	};
}
