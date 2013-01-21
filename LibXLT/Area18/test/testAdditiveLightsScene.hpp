#pragma once

#include "Area18/test/testScene.h"
#include "Area18/rndr/rndrPass.h"
#include "Area18/ogl/oglTexture2D.h"
#include "Graphics/g3d/g3dPointLight.hpp"

class camCamera;
class maAxisBox;
class meshTriMeshFrag;
class rndrNode;

class testBlendRenderPass : public rndrPass
{
public:
	testBlendRenderPass(oglContext* iDevice);
	virtual ~testBlendRenderPass();
protected:
	virtual void pushGraphicsState();
	virtual void popGraphicsState();
};


class testAdditiveLightsScene : public testScene
{
public:
	testAdditiveLightsScene(oglContext* i_pDevice);
	virtual ~testAdditiveLightsScene(void);

	virtual void Setup();
	virtual void Render(camCamera* i_pCamera);

	camCamera* m_pCam;
	meshTriMeshFrag* m_pSphere;
	g3dPointLight mPointLight0;
	g3dPointLight mPointLight1;

	oglTexture2dHandle mTexture;

	testBlendRenderPass mDefaultRenderer;
	rndrNode* mRoot;

	void GetBounds(maAxisBox& o_Bounds);

};
