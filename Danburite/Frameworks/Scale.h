#pragma once

#include "../Infra/Stateful.h"
#include "../Infra/GLM.h"

namespace Frx
{
	class Scale : public Infra::Stateful<Scale>
	{
	public:
		void set(
			float x,
			float y,
			float z);

		void set(
			glm::vec3 const &value);

		void set(
			float value);

		void setX(
			float x);

		void setY(
			float y);

		void setZ(
			float z);

		[[nodiscard]]
		constexpr glm::vec3 const &get() const noexcept;

		[[nodiscard]]
		constexpr glm::mat4 const &getMatrix() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		glm::vec3 __scale	{ 1.0f, 1.0f, 1.0f };
		glm::mat4 __matrix	{ 1.0f };
	};

	constexpr const glm::vec3 &Scale::get() const noexcept
	{
		return __scale;
	}

	constexpr const glm::mat4 &Scale::getMatrix() const noexcept
	{
		return __matrix;
	}
}