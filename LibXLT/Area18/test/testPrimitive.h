#pragma once
#include "testscene.h"

#include "Area18/ogl/oglTypes.hpp"

class camCamera;
class oglProgram;
class oglShader;
class g3dFragment;

class testPrimitive :
	public testScene
{
public:
	testPrimitive(void);
	virtual ~testPrimitive(void);

	virtual void Setup();
	virtual void Render(camCamera* i_pCamera);
	virtual void draw();
	virtual bool animated() { return true; }
	virtual camCamera* camera() {return mCamera;}

private:
	oglProgram* m_pProgram;
	oglShader* m_pVertSh;
	oglShader* m_pFragSh;
	g3dFragment* mRect;
	g3dFragment* mSphere;
	g3dFragment* mCone;
	g3dFragment* mCylinder;
	g3dFragment* mCube;
	GLuint m_vboID;
	GLuint m_eboID;
	GLuint m_vaoID;
	camCamera* mCamera;
	size_t mFrameNumber;
	int64_t mLastTime;

};
