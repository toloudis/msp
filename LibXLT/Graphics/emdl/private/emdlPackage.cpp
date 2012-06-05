/*****************************************************************************
**  emdlPackage.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/emdl/emdlPackage.hpp"

#include "Graphics/emdl/emdlImport.hpp"


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
void
emdlPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		//	implement an entImport importer,
		//	ownership passes to entImport namespace
		entImport::AddImplementation(new emdlImport());

	}

	//	increment the ref count
	l_RefCount++;
}


//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with this package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void
emdlPackage::CleanUp()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// no cleanup ?  entImport owns pointer?
	}
}
