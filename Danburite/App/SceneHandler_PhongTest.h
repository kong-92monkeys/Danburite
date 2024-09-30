#include "SceneHandler.h"
#include "PhongTestScene.h"

class SceneHandler_PhongTest : public SceneHandler
{
protected:
	virtual void _onActivated() override;
	virtual void _onDeactivated() override;

	virtual void onKeyDown(
		UINT nChar) override;

	virtual void onKeyUp(
		UINT nChar) override;

	virtual void _onDisplayChanged(
		Frx::Display *pDisplay) override;

private:
	std::unique_ptr<PhongTestScene> __pScene;
};