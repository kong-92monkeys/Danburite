#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"

namespace Frx
{
	struct SimpleMaterialData
	{
	public:
		glm::vec4 color;
	};

	class SimpleMaterial : public Render::TypedMaterial<SimpleMaterialData>
	{
	public:
		[[nodiscard]]
		constexpr glm::vec4 const &getColor() const noexcept;
		void setColor(
			glm::vec4 const &color) noexcept;
	};

	constexpr glm::vec4 const &SimpleMaterial::getColor() const noexcept
	{
		return _getTypedData().color;
	}
}