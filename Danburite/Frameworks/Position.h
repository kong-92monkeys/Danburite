#pragma once

#include "../Infra/Stateful.h"
#include "../Infra/GLM.h"

namespace Frx
{
	class Position : public Infra::Stateful<Position>
	{
	public:
		void set(
			float x,
			float y,
			float z);

		void set(
			glm::vec3 const &value);

		void setX(
			float x);

		void setY(
			float y);

		void setZ(
			float z);

		void add(
			float x,
			float y,
			float z);

		void add(
			glm::vec3 const &value);

		void addX(
			float x);

		void addY(
			float y);

		void addZ(
			float z);

		[[nodiscard]]
		constexpr glm::vec3 const &get() const noexcept;

		[[nodiscard]]
		constexpr glm::mat4 const &getMatrix() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		glm::vec3 __position	{ 0.0f, 0.0f, 0.0f };
		glm::mat4 __matrix		{ 1.0f };
	};

	constexpr glm::vec3 const &Position::get() const noexcept
	{
		return __position;
	}

	constexpr glm::mat4 const &Position::getMatrix() const noexcept
	{
		return __matrix;
	}
}
