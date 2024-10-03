#pragma once

#include "../Frameworks/RenderSystem.h"
#include "MainFrm.h"

class SceneHandler : public Infra::Unique
{
public:
	virtual ~SceneHandler() noexcept override = default;

	void init(
		Infra::Executor &uiExecutor,
		Frx::RenderSystem &renderSystem);

	void activate();
	void deactivate();

	virtual void onDisplayRegistered(
		Frx::Display *pDisplay);

	virtual void onDisplayUnregistered(
		Frx::Display *pDisplay);

	virtual void onDisplaySync(
		Frx::Display const *pDisplay);

	virtual void onKeyDown(
		UINT nChar);

	virtual void onKeyUp(
		UINT nChar);

	virtual void onTick();

protected:
	virtual void _onInit();
	virtual void _onActivated();
	virtual void _onDeactivated();

	[[nodiscard]]
	CMainFrame *_ui_getMainFrame() const;

	[[nodiscard]]
	constexpr Infra::Executor &_getUIExecutor() const noexcept;

	[[nodiscard]]
	constexpr Frx::RenderSystem &_getRenderSystem() const noexcept;

private:
	Infra::Executor *__pUIExecutor{ };
	Frx::RenderSystem *__pRenderSystem{ };

	bool __activated{ };
};

constexpr Infra::Executor &SceneHandler::_getUIExecutor() const noexcept
{
	return *__pUIExecutor;
}

constexpr Frx::RenderSystem &SceneHandler::_getRenderSystem() const noexcept
{
	return *__pRenderSystem;
}