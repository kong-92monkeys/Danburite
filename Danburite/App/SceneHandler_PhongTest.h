#include "SceneHandler.h"
#include "PhongTestScene.h"

class SceneHandler_PhongTest : public SceneHandler
{
public:
	virtual ~SceneHandler_PhongTest() noexcept override = default;

	virtual void onDisplayRegistered(
		Frx::Display *pDisplay) override;

	virtual void onDisplayUnregistered(
		Frx::Display *pDisplay) override;

	virtual void onDisplaySync(
		Frx::Display const *pDisplay) override;

	virtual void onKeyDown(
		UINT nChar) override;

	virtual void onKeyUp(
		UINT nChar) override;

	virtual void onTick() override;

protected:
	virtual void _onActivated() override;
	virtual void _onDeactivated() override;

private:
	std::unique_ptr<PhongTestScene> __pScene;
	size_t __fpsUpdateTick{ };

	static constexpr size_t __fpsUpdateTickCount{ 100'000ULL };
};