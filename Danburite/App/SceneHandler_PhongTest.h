#include "SceneHandler.h"
#include "PhongTestScene.h"

class SceneHandler_PhongTest : public SceneHandler
{
public:
	virtual ~SceneHandler_PhongTest() noexcept override = default;

	virtual void onKeyDown(
		UINT nChar) override;

	virtual void onKeyUp(
		UINT nChar) override;

protected:
	virtual void _onActivated() override;
	virtual void _onDeactivated() override;

	virtual void _onDisplayChanged(
		Frx::Display *pDisplay) override;

private:
	std::unique_ptr<PhongTestScene> __pScene;
	size_t __fpsUpdateTick{ };

	static constexpr size_t __fpsUpdateTickCount{ 20ULL };
};