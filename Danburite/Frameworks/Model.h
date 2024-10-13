#pragma once

#include "../Infra/Unique.h"
#include "../Infra/Bitmap.h"
#include "../Infra/Executor.h"
#include "../Infra/GenericBuffer.h"
#include "../Render/Engine.h"
#include "MaterialParams.h"
#include "RendererFactory.h"
#include "VertexAttribute.h"
#include "SceneNode.h"
#include "TransformMaterial.h"
#include <unordered_map>
#include <queue>

namespace Frx
{
	class Model : public Infra::Unique
	{
	public:
		struct TextureInfo
		{
		public:
			uint32_t index{ };

			/*
				All color components (rgb) are multiplied
				with this factor before any further processing is done.
			*/
			float strength{ 1.0f };

			/*
				Defines the arithmetic operation
				to be used to combine the n¡¯th texture
			*/
			TextureBlendOp op{ TextureBlendOp::MULTIPLY };

			TextureMapMode mapModeU{ TextureMapMode::WRAP };
			TextureMapMode mapModeV{ TextureMapMode::WRAP };

			bool inverted{ };
		};

		struct MaterialInfo
		{
		public:
			/*
				Ambient color of the material.
				This is typically scaled by the amount of ambient light
			*/
			glm::vec4 ambient{ 0.f, 0.f, 0.f, 1.0f };

			/*
				Diffuse color of the material.
				This is typically scaled by the amount of incoming diffuse light (e.g. using gouraud shading)
			*/
			glm::vec4 diffuse{ 0.f, 0.f, 0.f, 1.0f };

			/*
				Specular color of the material.
				This is typically scaled by the amount of incoming specular light (e.g. using phong shading)
			*/
			glm::vec4 specular{ 0.f, 0.f, 0.f, 1.0f };

			/*
				Emissive color of the material.
				This is the amount of light emitted by the object.

				In real time applications it will usually not affect surrounding objects,
				but raytracing applications may wish to treat emissive objects as light sources.
			*/
			glm::vec4 emissive{ 0.f, 0.f, 0.f, 1.0f };

			/*
				Defines the transparent color of the material,
				this is the color to be multiplied with the color of translucent light
				to construct the final ¡®destination color¡¯ for a particular position in the screen buffer.
			*/
			glm::vec4 transparent{ 0.f, 0.f, 0.f, 1.0f };

			/*
				Defines the reflective color of the material.
				This is typically scaled by the amount of incoming light
				from the direction of mirror reflection.
				
				Usually combined with an environment lightmap of some kind for real-time applications.
			*/
			glm::vec4 reflective{ 0.f, 0.f, 0.f, 1.0f };

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
			RendererType rendererType{ RendererType::GOURAUD };

			/*
				Defines how the final color value in the screen buffer is computed
				from the given color at that position and the newly computed color
				from the material.
				
				Simply said, alpha blending settings.
			*/
			AlphaBlendOp blendOp{ AlphaBlendOp::DEFAULT };

			/*
				Defines the shininess of a phong-shaded material.
				This is actually the exponent of the phong specular equation
				SHININESS=0 is equivalent to SHADING_MODEL = Gouraud
			*/
			float shininess{ 0.0f };

			/*
				Defines the Index Of Refraction for the material.
				That¡¯s not supported by most file formats.
				Might be of interest for raytracing.
			*/
			float indexOfRefraction{ 1.0f };

			std::unordered_map<TextureType, std::vector<TextureInfo>> textureInfoMap;
		};

		struct MeshInfo
		{
		public:
			uint32_t vertexCount{ };
			std::unordered_map<uint32_t, Infra::GenericBuffer> vertexBuffers;

			uint32_t indexCount{ };
			VkIndexType indexType{ VkIndexType::VK_INDEX_TYPE_UINT16 };
			Infra::GenericBuffer indexBuffer;
		};

		struct DrawInfo
		{
		public:
			uint32_t materialIdx{ };
			uint32_t meshIdx{ };

			uint32_t indexCount{ };
			uint32_t firstIndex{ };
			int32_t vertexOffset{ };
		};

		struct NodeInfo
		{
		public:
			glm::mat4 transform{ 1.0f };
			std::vector<uint32_t> drawInfoIndices;
			std::vector<uint32_t> childIndices;
		};

		struct CreateInfo
		{
		public:
			std::vector<std::shared_ptr<Infra::Bitmap>> textures;
			std::vector<MaterialInfo> materials;
			std::vector<MeshInfo> meshes;
			std::vector<DrawInfo> drawInfos;
			std::vector<NodeInfo> nodes;
		};

		Model(
			CreateInfo &&createInfo,
			Infra::Executor &rcmdExecutor,
			Render::Engine &renderEngine,
			RendererFactory &rendererFactory);

		virtual ~Model() noexcept override;

		void rcmd_addToLayer(
			Render::Layer &layer);

		void rcmd_removeFromLayer(
			Render::Layer &layer);

	private:
		struct __RcmdResources
		{
		public:
			std::vector<std::shared_ptr<Render::Texture>> textures;
			std::vector<std::shared_ptr<Render::Material>> materials;
			std::vector<std::shared_ptr<Render::Mesh>> meshes;
			std::vector<std::shared_ptr<Render::DrawParam>> drawParams;

			std::vector<std::shared_ptr<Render::RenderObject>> renderObjects;
			std::vector<std::shared_ptr<TransformMaterial>> transformMaterials;

			__RcmdResources() noexcept;
		};

		Infra::Executor &__rcmdExecutor;

		std::vector<SceneNode *> __sceneNodes;
		std::queue<std::vector<glm::mat4>> __transformQue;

		__RcmdResources *__pRcmdResources{ new __RcmdResources };

		static void __rcmd_init(
			CreateInfo const &createInfo,
			Render::Engine &renderEngine,
			RendererFactory &rendererFactory,
			__RcmdResources &outResources);
	};
}