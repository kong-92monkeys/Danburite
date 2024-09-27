#include "../Frameworks/Scene.h"
#include "../Frameworks/Display.h"
#include "../Frameworks/PhongRenderer.h"
#include "../Frameworks/TransformMaterial.h"
#include "../Frameworks/PhongMaterial.h"

class PhongTestScene : public Frx::Scene
{
public:
	virtual ~PhongTestScene() noexcept override;

	void setDisplay(
		Frx::Display *pDisplay);

protected:
	[[nodiscard]]
	virtual std::any _scmd_onUpdate(
		Time const &time) override;

	virtual void _rcmd_onInit() override;
	virtual void _rcmd_onUpdate(
		std::any const &updateParam) override;

private:
	std::unique_ptr<Render::Mesh> __rcmd_pMesh;
	std::unique_ptr<Render::DrawParam> __rcmd_pDrawParam;
	std::unique_ptr<Frx::PhongRenderer> __rcmd_pRenderer;
	std::unique_ptr<Render::Texture> __rcmd_pAlbedoTexture;
	std::unique_ptr<Frx::TransformMaterial> __rcmd_pTransformMaterial;
	std::unique_ptr<Frx::PhongMaterial> __rcmd_pPhongMaterial;
	std::unique_ptr<Render::RenderObject> __rcmd_pObject;
	std::unique_ptr<Render::Layer> __rcmd_pLayer;

	glm::mat4 __scmd_transform{ 1.0f };

	Frx::Display *__rcmd_pDisplay{ };
};