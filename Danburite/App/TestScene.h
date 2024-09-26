#include "../Frameworks/Scene.h"
#include "../Frameworks/Display.h"
#include "../Frameworks/ImageRenderer.h"
#include "../Frameworks/ImageMaterial.h"

class TestScene : public Frx::Scene
{
public:
	virtual ~TestScene() noexcept override;

	void setDisplay(
		Frx::Display *pDisplay);

protected:
	virtual void _scmd_onInit() override;

private:
	std::unique_ptr<Render::Mesh> __rcmd_pMesh;
	std::unique_ptr<Render::DrawParam> __rcmd_pDrawParam;
	std::unique_ptr<Frx::ImageRenderer> __rcmd_pRenderer;
	std::unique_ptr<Render::Texture> __rcmd_pTexture;
	std::unique_ptr<Frx::ImageMaterial> __rcmd_pMaterial;
	std::unique_ptr<Render::RenderObject> __rcmd_pObject;
	std::unique_ptr<Render::Layer> __rcmd_pLayer;

	Frx::Display *__rcmd_pDisplay{ };
};