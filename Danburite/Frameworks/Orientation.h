#pragma once

#include "../Infra/Stateful.h"
#include "../Infra/GLM.h"
#include <glm/gtc/quaternion.hpp>

namespace Frx
{
	class Orientation : public Infra::Stateful<Orientation>
	{
	public:
		void set(
			float w,
			float x,
			float y,
			float z);

		void set(
			glm::quat const &value);

		void rotate(
			glm::quat const &value);

		void rotate(
			float const angle,
			glm::vec3 const &axis);

		[[nodiscard]]
		constexpr glm::quat const &get() const noexcept;

		[[nodiscard]]
		constexpr glm::mat4 const &getMatrix() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		glm::quat __orientation	{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::mat4 __matrix		{ 1.0f };
	};

	constexpr glm::quat const &Orientation::get() const noexcept
	{
		return __orientation;
	}

	constexpr glm::mat4 const &Orientation::getMatrix() const noexcept
	{
		return __matrix;
	}
}