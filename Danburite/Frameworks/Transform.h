#pragma once

#include "Position.h"
#include "Orientation.h"
#include "Scale.h"

namespace Frx
{
	class Transform : public Infra::Stateful<Transform>
	{
	public:
		Transform() noexcept;

		[[nodiscard]]
		constexpr Position &getPosition() noexcept;

		[[nodiscard]]
		constexpr Position const &getPosition() const noexcept;

		[[nodiscard]]
		constexpr Orientation &getOrientation() noexcept;

		[[nodiscard]]
		constexpr Orientation const &getOrientation() const noexcept;

		[[nodiscard]]
		constexpr Scale &getScale() noexcept;

		[[nodiscard]]
		constexpr Scale const &getScale() const noexcept;

		[[nodiscard]]
		constexpr glm::mat4 const &getMatrix() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		Position __position;
		Orientation __orientation;
		Scale __scale;

		glm::mat4 __matrix{ 1.0f };

		Infra::EventListenerPtr<Position *> __pPositionInvalidateListener;
		Infra::EventListenerPtr<Orientation *> __pOrientationInvalidateListener;
		Infra::EventListenerPtr<Scale *> __pScaleInvalidateListener;

		void __onPositionInvalidated() noexcept;
		void __onOrientationInvalidated() noexcept;
		void __onScaleInvalidated() noexcept;
	};

	constexpr Position &Transform::getPosition() noexcept
	{
		return __position;
	}

	constexpr Position const &Transform::getPosition() const noexcept
	{
		return __position;
	}

	constexpr Orientation &Transform::getOrientation() noexcept
	{
		return __orientation;
	}

	constexpr Orientation const &Transform::getOrientation() const noexcept
	{
		return __orientation;
	}

	constexpr Scale &Transform::getScale() noexcept
	{
		return __scale;
	}

	constexpr Scale const &Transform::getScale() const noexcept
	{
		return __scale;
	}

	constexpr glm::mat4 const &Transform::getMatrix() const noexcept
	{
		return __matrix;
	}
}