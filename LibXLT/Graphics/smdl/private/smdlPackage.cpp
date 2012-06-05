/*****************************************************************************
**	smdlPackage.cpp
**
**		smdlPackage contains the initialization functions
**	for the smdl (Scene Model) package.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/smdlPackage.hpp"

#include "Graphics/smdl/private/smdlBoneFragment.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//----------------------------------------------------------------------------
//	Init must be called before you use this package.  A good place to
//	do this is in your main function, before you do anything else.
//----------------------------------------------------------------------------
void smdlPackage::InitGraphics()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		smdlBoneFragment::Initialize();
	}

	//	increment the ref count
	l_RefCount++;
}


//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with this package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void smdlPackage::CleanUpGraphics()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		smdlBoneFragment::DeInitialize();
	}
}
