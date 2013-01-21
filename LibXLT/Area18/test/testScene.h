#pragma once

#include "Area18/rndr/rndrEngine.h"
#include "Graphics/mat/matShaderEffect.hpp"

class camCamera;

class ITestUI
{
public:
	virtual ~ITestUI() {}
	std::vector<matShaderParamUI> mUIList;
};

class testScene : public rndrEngine
{
public:
	testScene(void);
	virtual ~testScene(void);

	virtual void Setup() {}
	virtual void Render(camCamera* i_pCamera) {}

	virtual camCamera* camera() {return 0;}

	ITestUI& testUI() { return mTestUI; }
protected:
	ITestUI mTestUI;
};
