/*****************************************************************************
**  entPackage.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entPackage.hpp"

#include "Graphics/ent/entImport.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//------------------------------------------------------------------------
//	Init must be called before you use the mat package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void entPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		entImport::Initialize();

		//	set the error handler file for this package
		//
		//gfErrorHandler::SetErrorFilename( envPackageErrorIndices::e_Mat,itString("matErrors.tsf") );
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the mat package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void entPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		entImport::DeInitialize();

		// clean up packages we depend on
	}
}
