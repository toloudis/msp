/*****************************************************************************
**  ImportExportLayer.cpp
**
**      ImportExportLayer contains the initialization functions
**	for the all packages within the Importer Layer.
**
**	StudioGPU
**	Copyright(C) 20038 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/ImportExportLayer.hpp"

#include "ImportExport/fbx/fbxPackage.hpp"

namespace
{

int l_RefCount = 0;

}

//----------------------------------------------------------------------------
//	Init
//----------------------------------------------------------------------------
void ImportExportLayer::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages in the layer
		fbxPackage::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp
//----------------------------------------------------------------------------
void ImportExportLayer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		fbxPackage::CleanUp();

	}
}

//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//------------------------------------------------------------------------
void ImportExportLayer::InitGraphics()
{
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void ImportExportLayer::CleanUpGraphics()
{

}
