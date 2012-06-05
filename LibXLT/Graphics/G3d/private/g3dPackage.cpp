/*****************************************************************************
**  g3dPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dPackage.hpp"

#include "Core/app/appPackage.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itPackage.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/an/anPackage.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/mat/matPackage.hpp"

#include <string>


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
	std::string l_VideoName;

	int l_FirstShadowLayer = -1;
	int l_LastShadowLayer = -1;
}


//------------------------------------------------------------------------
//	Init must be called before you use the g3d package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void g3dPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		envPackage::Init();
		dbgPackage::Init();
		itPackage::Init();
		appPackage::Init();
		anPackage::Init();
		g2dPackage::Init();
		matPackage::Init();

		g3dRendererMgr::Initialize();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the g3d package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void g3dPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		g3dRendererMgr::DeInitialize();

		// clean up packages we depend on
		matPackage::CleanUp();
		g2dPackage::CleanUp();
		anPackage::CleanUp();
		appPackage::CleanUp();
		itPackage::CleanUp();
		dbgPackage::CleanUp();
		envPackage::CleanUp();
	}
}

