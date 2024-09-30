#include "SceneHandler.h"

class SceneHandler_Nothing : public SceneHandler
{
public:
	virtual ~SceneHandler_Nothing() noexcept override = default;

protected:
	virtual void _onActivated() override;
};