/*****************************************************************************
**  fbxPackage.cpp
**
**      fbxPackage contains the initialization and cleanup functions
**	for the FBX importer package.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/fbx/fbxPackage.hpp"

#include "ImportExport/fbx/fbxSdkManager.hpp"
#include "ImportExport/fbx/import/fbxImport.hpp"

#include "Graphics/ent/entPackage.hpp"

//------------------------------------------------------------------------
//	library pragmas
//------------------------------------------------------------------------
#ifdef USE_FBX_IMPORTEXPORT
#ifdef _DEBUG
#pragma comment(lib,"fbxsdk_md2008d.lib")
#else
#pragma comment(lib,"fbxsdk_md2008.lib")
#endif
#pragma comment(lib,"wininet.lib")
#endif

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
void fbxPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		entPackage::Init();

#ifdef USE_FBX_IMPORTEXPORT
		// Init our internal stuff
		fbxSdkManager::Init();

		//	implement an entImport importer,
		//	ownership passes to entImport namespace
		entImport::AddImplementation(new fbxImport());
#endif
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the mat package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void fbxPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
#ifdef USE_FBX_IMPORTEXPORT
		fbxSdkManager::CleanUp();
#endif

		// clean up packages we depend on
		entPackage::CleanUp();
	}
}
