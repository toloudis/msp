/*****************************************************************************
**  snPackage.cpp
**
**      snPackage contains the initialization and cleanup functions
**	for the Sn package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snPackage.hpp"

#include "Core/dbg/dbgPackage.hpp"
#include "Core/it/itString.hpp"
#include "AudioDS/sn/snSoundUtil.hpp"


namespace
{
	int l_RefCount = 0;
}


//============================================================================
//	Init must be called before you use this package.  A good place to
//	do this is in your main function, before you do anything else.
//============================================================================
void 
snPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		//
		dbgPackage::Init();

		// initialize our internal stuff
		//
		snSoundUtil::Init();

	}

	//	increment the ref count
	l_RefCount++;
}


//============================================================================
//	CleanUp should be called after you are done with this package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//============================================================================
void 
snPackage::CleanUp()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	//
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		snSoundUtil::CleanUp();

		// clean up packages we depend on
		//
		dbgPackage::CleanUp();
	}
}
