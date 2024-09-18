#pragma once

#include "../Infra/Stateful.h"
#include "RenderObject.h"

namespace Render
{
	class Layer : public Infra::Unique, public Infra::Stateful<Layer>
	{
	public:
		[[nodiscard]]
		constexpr int getPriority() const noexcept;
		void setPriority(
			int priority) noexcept;

		void addRenderObject(
			RenderObject const *pObject);

		void removeRenderObject(
			RenderObject const *pObject);

		[[nodiscard]]
		bool isEmpty() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<Layer const *, int, int> &
			getPriorityChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<Layer const *> &
			getNeedRedrawEvent() const noexcept;

	private:

	};
}
