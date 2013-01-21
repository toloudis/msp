#pragma once
#include "testscene.h"

#include "Area18/ogl/oglTypes.hpp"

class oglProgram;
class oglShader;
class g3dFragment;

class testFBO :
	public testScene
{
public:
	testFBO(void);
	virtual ~testFBO(void);

	virtual void Setup();
	virtual void Render(camCamera* i_pCamera);
	virtual void draw();
private:
	virtual void doResize(int w, int h);
	oglProgram* m_pProgram;
	oglShader* m_pVertSh;
	oglShader* m_pFragSh;
	g3dFragment* m_pFrag;
	GLuint m_vboID;
	GLuint m_eboID;
	GLuint m_vaoID;

	GLuint mFBO;
	GLuint mDepthBuf;
	GLuint mColorTex;
};
