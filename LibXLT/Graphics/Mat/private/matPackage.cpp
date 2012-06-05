/*****************************************************************************
**  matPackage.cpp
**
**      matPackage contains the initialization and cleanup functions
**	for the mat package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/mat/matPackage.hpp"

#include "Core/app/appPackage.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itPackage.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/an/anPackage.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "Graphics/mat/matShaderParser.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <string>

namespace
{

int l_RefCount = 0;

}

//------------------------------------------------------------------------
//	Init must be called before you use the mat package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void matPackage::Init()
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

		// initialize our internal stuff
		//
		matTextureMgr::Init();
		matShaderParser::Initialize();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the mat package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void matPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		matShaderParser::DeInitialize();
		matTextureMgr::CleanUp();

		// clean up packages we depend on
		g2dPackage::CleanUp();
		anPackage::CleanUp();
		appPackage::CleanUp();
		itPackage::CleanUp();
		dbgPackage::CleanUp();
		envPackage::CleanUp();
	}
}
