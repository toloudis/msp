/*****************************************************************************
**  g2dPackage.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dPackage.hpp"

#include "Core/app/appPackage.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itPackage.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//----------------------------------------------------------------------------
//	Init must be called before you use the g2d package.  A good place to
//	do this is in your main function, before you do anything else.
//----------------------------------------------------------------------------
void g2dPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		envPackage::Init();
		dbgPackage::Init();
		itPackage::Init();
		appPackage::Init();

		// initialize our internal stuff
		//
		g2dFontUtil::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the g2d package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void g2dPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		g2dFontUtil::CleanUp();

		// clean up packages we depend on
		appPackage::CleanUp();
		itPackage::CleanUp();
		dbgPackage::CleanUp();
		envPackage::CleanUp();
	}
}
