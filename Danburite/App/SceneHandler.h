#pragma once

#include "../Frameworks/RenderSystem.h"
#include "MainFrm.h"

class SceneHandler
{
public:
	void init(
		Frx::RenderSystem &renderSystem);

	void setDisplay(
		Frx::Display *pDisplay) noexcept;

	virtual void activate();
	virtual void deactivate();

	virtual void onKeyDown(
		UINT nChar);

	virtual void onKeyUp(
		UINT nChar);

protected:
	virtual void _onInit();
	virtual void _onDisplayChanged();

	[[nodiscard]]
	constexpr Frx::Display *_getDisplay() const noexcept;

private:
	Frx::RenderSystem *__pRenderSystem{ };
	Frx::Display *__pDisplay{ };
};

constexpr Frx::Display *SceneHandler::_getDisplay() const noexcept
{
	return __pDisplay;
}