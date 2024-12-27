/*****************************************************************************
**  gltfPackage.cpp
**
**      gltfPackage contains the initialization and cleanup functions
**	for the FBX importer package.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/gltf/gltfPackage.hpp"

#include "ImportExport/gltf/import/gltfImport.hpp"

#include "Graphics/ent/entPackage.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
namespace
{

int l_RefCount = 0;

}

//------------------------------------------------------------------------
//	Init must be called before you use the mat package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void gltfPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		entPackage::Init();

		//	implement an entImport importer,
		//	ownership passes to entImport namespace
		entImport::AddImplementation(new gltfImport());
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the mat package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void gltfPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//

		// clean up packages we depend on
		entPackage::CleanUp();
	}
}
