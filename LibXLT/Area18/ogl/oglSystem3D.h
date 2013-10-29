#pragma once

#include "Graphics/g3d/g3dSystem.hpp"
#include <vector>

class matShaderMgrGL;
class matTextureMgrGL;

class oglSystem3D :
	public g3dSystem
{
public:
	oglSystem3D(void);
	virtual ~oglSystem3D(void);

	//------------------------------------------------------------------------
	//	GetVideoAdapterName returns some kind of ANSI C string uniquely
	//	identifying the type of video hardware in the system.  This function
	//	can be called only after calling Init().
	//------------------------------------------------------------------------
	const char* GetVideoAdapterName();

	//------------------------------------------------------------------------
	// return KB
	//------------------------------------------------------------------------
	float GetVideoMemory();

private:
	matShaderMgrGL* mShaderMgr;
	matTextureMgrGL* mTextureMgr;
};
