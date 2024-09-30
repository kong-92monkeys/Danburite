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

	void activate();
	void deactivate();

	virtual void onKeyDown(
		UINT nChar);

	virtual void onKeyUp(
		UINT nChar);

protected:
	virtual void _onInit();
	virtual void _onActivated();
	virtual void _onDeactivated();

	virtual void _onDisplayChanged(
		Frx::Display *pDisplay);

	[[nodiscard]]
	CMainFrame *_getMainFrame() const;

	[[nodiscard]]
	constexpr Frx::RenderSystem &_getRenderSystem() const noexcept;

	[[nodiscard]]
	constexpr Frx::Display *_getDisplay() const noexcept;

private:
	Frx::RenderSystem *__pRenderSystem{ };
	Frx::Display *__pDisplay{ };

	bool __activated{ };
};

constexpr Frx::RenderSystem &SceneHandler::_getRenderSystem() const noexcept
{
	return *__pRenderSystem;
}

constexpr Frx::Display *SceneHandler::_getDisplay() const noexcept
{
	return __pDisplay;
}