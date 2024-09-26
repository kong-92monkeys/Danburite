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
	virtual void _scmd_onInit() override;

private:
	std::unique_ptr<Render::Mesh> __rcmd_pMesh;
	std::unique_ptr<Render::DrawParam> __rcmd_pDrawParam;
	std::unique_ptr<Frx::PhongRenderer> __rcmd_pRenderer;
	std::unique_ptr<Render::Texture> __rcmd_pAlbedoTexture;
	std::unique_ptr<Frx::TransformMaterial> __rcmd_pTransformMaterial;
	std::unique_ptr<Frx::PhongMaterial> __rcmd_pPhongMaterial;
	std::unique_ptr<Render::RenderObject> __rcmd_pObject;
	std::unique_ptr<Render::Layer> __rcmd_pLayer;

	Frx::Display *__rcmd_pDisplay{ };
};