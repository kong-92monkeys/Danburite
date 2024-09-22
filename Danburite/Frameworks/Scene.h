#pragma once

namespace Frx
{
	class Scene
	{
	public:
		struct Time
		{
		public:
			float deltaTime;
		};

		void init();
		void update();

	protected:
		virtual void _onInit() = 0;
		virtual void _onUpdate();

		[[nodiscard]]
		constexpr Time const &_getTime() const noexcept;

	private:
		Time __time;
	};

	constexpr Scene::Time const &Scene::_getTime() const noexcept
	{
		return __time;
	}
}