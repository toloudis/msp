#pragma once
#include "testscene.h"

#include "Area18/ogl/oglTypes.hpp"

class camCamera;
class oglProgram;
class oglShader;
class oglTexture2d;
class g3dFragment;

class testTexture :
	public testScene
{
public:
	testTexture(void);
	virtual ~testTexture(void);

	virtual void Setup();
	virtual void Render(camCamera* i_pCamera);
	virtual void draw();
	virtual camCamera* camera() {return mCamera;}

private:
	oglProgram* m_pProgram;
	oglShader* m_pVertSh;
	oglShader* m_pFragSh;
	g3dFragment* m_pFrag;
	oglTexture2d* mTexture;
	camCamera* mCamera;
};
