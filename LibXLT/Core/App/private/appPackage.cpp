/*****************************************************************************
**  appPackage.cpp
**
**      appPackage contains the initialization functions
**	for the app package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appPackage.hpp"

#include "Core/app/appApplication.hpp"
#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/it/itPackage.hpp"
#include "Core/it/itString.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//----------------------------------------------------------------------------
//	Init must be called before you use the fs package.  A good place to
//	do this is in your main function, before you do anything else.
//----------------------------------------------------------------------------
void appPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		//
		envPackage::Init();
		dbgPackage::Init();
		itPackage::Init();

		// initialize our internal stuff
		//
		appTime::Init();
		appApplication::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the fs package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void appPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		appApplication::CleanUp();
		appTime::CleanUp();

		// clean up packages we depend on
		//
		itPackage::CleanUp();
		dbgPackage::CleanUp();
		envPackage::CleanUp();
	}
}
