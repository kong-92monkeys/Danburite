#pragma once

#include "../Frameworks/RenderSystem.h"
#include "MainFrm.h"

class SceneHandler : public Infra::Unique
{
public:
	virtual ~SceneHandler() noexcept override = default;

	void init(
		Infra::Executor &mainExecutor,
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
	constexpr Infra::Executor &_getMainExecutor() const noexcept;

	[[nodiscard]]
	constexpr Frx::RenderSystem &_getRenderSystem() const noexcept;

	[[nodiscard]]
	constexpr Frx::Display *_getDisplay() const noexcept;

private:
	Infra::Executor *__pMainExecutor{ };
	Frx::RenderSystem *__pRenderSystem{ };
	Frx::Display *__pDisplay{ };

	bool __activated{ };
};

constexpr Infra::Executor &SceneHandler::_getMainExecutor() const noexcept
{
	return *__pMainExecutor;
}

constexpr Frx::RenderSystem &SceneHandler::_getRenderSystem() const noexcept
{
	return *__pRenderSystem;
}

constexpr Frx::Display *SceneHandler::_getDisplay() const noexcept
{
	return __pDisplay;
}