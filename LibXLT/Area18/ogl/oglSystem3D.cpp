#include "Area18/ogl/oglSystem3D.h"
#include "Area18/mat/matShaderMgrGL.h"

oglSystem3D::oglSystem3D(void)
{
	mShaderMgr = new matShaderMgrGL();
	matShaderMgr::SetImplementation(mShaderMgr);
	matShaderMgr::Initialize();

}
oglSystem3D::~oglSystem3D(void)
{
	matShaderMgr::DeInitialize();
	matShaderMgr::SetImplementation(NULL);
	delete mShaderMgr;
}

//------------------------------------------------------------------------
//	GetVideoAdapterName returns some kind of ANSI C string uniquely
//	identifying the type of video hardware in the system.  This function
//	can be called only after calling Init().
//------------------------------------------------------------------------
const char* oglSystem3D::GetVideoAdapterName()
{
	return "Unknown";
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
float oglSystem3D::GetVideoMemory()
{
	return 1;
}
