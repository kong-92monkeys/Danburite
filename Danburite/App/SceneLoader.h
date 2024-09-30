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

	void load(
		SceneType sceneType);

	void setDisplay(
		Frx::Display *pDisplay) noexcept;

	void onViewKeyDown(
		UINT nChar);

	void onViewKeyUp(
		UINT nChar);

private:
	Frx::RenderSystem &__renderSystem;
	SceneType __sceneType{ SceneType::NOTHING };
	std::unordered_map<SceneType, std::unique_ptr<SceneHandler>> __sceneHandlers;
};