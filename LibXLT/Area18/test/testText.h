#pragma once
#include "testscene.h"

#include "Area18/ogl/oglTypes.hpp"
#include "AntTweakBar/include/AntTweakBar.h"

class oglProgram;
class oglShader;
class oglTextDraw;
class g3dFragment;

class testText:
	public testScene
{
public:
	testText(void);
	virtual ~testText(void);

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
	oglTextDraw* mText;


	camCamera* mCamera;
	TwBar* mTweakBar;
};
