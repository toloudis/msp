#include "Area18/ogl/oglSystem3D.h"
#include "Area18/mat/matShaderMgrGL.h"
#include "Area18/mat/matTextureMgrGL.hpp"

oglSystem3D::oglSystem3D(void)
{
	mTextureMgr = new matTextureMgrGL();
	matTextureMgr::SetImplementation(mTextureMgr);
	matTextureMgr::Init();

	mShaderMgr = new matShaderMgrGL();
	matShaderMgr::SetImplementation(mShaderMgr);
	matShaderMgr::Initialize();

}
oglSystem3D::~oglSystem3D(void)
{
	matShaderMgr::DeInitialize();
	matShaderMgr::SetImplementation(NULL);
	delete mShaderMgr;

	matTextureMgr::CleanUp();
	matTextureMgr::SetImplementation(NULL);
	delete mTextureMgr;
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
