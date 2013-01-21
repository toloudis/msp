#pragma once
#include "testscene.h"

#include "Area18/ogl/oglTypes.hpp"

#include <boost/scoped_ptr.hpp>

class camCamera;
class oglProgram;
class oglShader;
class g3dFragment;

class testDirLt :
	public testScene
{
public:
	testDirLt(void);
	virtual ~testDirLt(void);

	virtual void Setup();
	virtual void Render(camCamera* i_pCamera);
	virtual void draw();
	virtual bool animated() { return true; }
	virtual camCamera* camera() {return mCamera;}

private:
	oglProgram* mProgram;
	oglShader* m_pVertSh;
	oglShader* m_pFragSh;
	oglShader* mLambert;
	oglShader* mDirLt;
	boost::scoped_ptr<g3dFragment> mRect;
	boost::scoped_ptr<g3dFragment> mSphere;
	boost::scoped_ptr<g3dFragment> mCone;
	boost::scoped_ptr<g3dFragment> mCylinder;
	boost::scoped_ptr<g3dFragment> mCube;
	camCamera* mCamera;
	size_t mFrameNumber;
	int64_t mLastTime;

};
