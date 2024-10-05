#include "../Infra/Looper.h"
#include "../Frameworks/RenderSystem.h"
#include "SceneHandler.h"

enum class SceneType
{
	NOTHING,
	PHONG_TEST,
	MODEL_LOADING
};

class SceneManager : public Infra::Unique
{
public:
	SceneManager(
		Infra::Executor &uiExecutor,
		Frx::RenderSystem &renderSystem);

	virtual ~SceneManager() noexcept override;

	void registerDisplay(
		Frx::Display *pDisplay);

	void unregisterDisplay(
		Frx::Display *pDisplay);

	[[nodiscard]]
	constexpr SceneType getSceneType() const noexcept;

	void load(
		SceneType sceneType);

	void onKeyDown(
		UINT nChar);

	void onKeyUp(
		UINT nChar);

private:
	Infra::Looper __scmdExecutor;

	SceneType __scmd_sceneType{ SceneType::NOTHING };
	std::unordered_map<SceneType, std::unique_ptr<SceneHandler>> __scmd_handlers;

	std::unordered_set<Frx::Display *> __displays;
	Infra::EventListenerPtr<Frx::Display const *> __pDisplaySyncListener;

	Infra::EventListenerPtr<Infra::Looper *> __scmd_pIdleListener;

	void __onDisplaySync(
		Frx::Display const *pDisplay);

	[[nodiscard]]
	SceneHandler &__scmd_getHandler();

	void __scmd_onIdle();
};

constexpr SceneType SceneManager::getSceneType() const noexcept
{
	return __scmd_sceneType;
}