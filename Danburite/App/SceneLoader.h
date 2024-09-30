#include "../Frameworks/RenderSystem.h"
#include "SceneHandler.h"

enum class SceneType
{
	NOTHING,
	PHONG_TEST
};

class SceneLoader : public Infra::Unique
{
public:
	SceneLoader(
		Frx::RenderSystem &renderSystem);

	[[nodiscard]]
	constexpr SceneType getSceneType() const noexcept;

	void load(
		SceneType sceneType);

	void setDisplay(
		Frx::Display *pDisplay) noexcept;

	void onKeyDown(
		UINT nChar);

	void onKeyUp(
		UINT nChar);

private:
	Frx::RenderSystem &__renderSystem;
	SceneType __sceneType{ SceneType::NOTHING };
	std::unordered_map<SceneType, std::unique_ptr<SceneHandler>> __sceneHandlers;
};

constexpr SceneType SceneLoader::getSceneType() const noexcept
{
	return __sceneType;
}