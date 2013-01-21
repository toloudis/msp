#pragma once
#include "testscene.h"

#include "Area18/ogl/oglTypes.hpp"

class oglProgram;
class oglShader;
class g3dFragment;

class testCamera :
	public testScene
{
public:
	testCamera(void);
	virtual ~testCamera(void);

	virtual void Setup();
	virtual void Render(camCamera* i_pCamera);
	virtual void draw();
	virtual camCamera* camera() {return mCamera;}

private:
	oglProgram* m_pProgram;
	oglShader* m_pVertSh;
	oglShader* m_pFragSh;
	g3dFragment* m_pFrag;
	GLuint m_vboID;
	GLuint m_eboID;
	GLuint m_vaoID;

	camCamera* mCamera;
};
