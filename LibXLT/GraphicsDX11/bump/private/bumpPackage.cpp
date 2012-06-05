/*****************************************************************************
**  bumpPackage.cpp
**
**      bumpPackage contains the initialization and cleanup functions
**	for the bump package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/bump/bumpPackage.hpp"

#include "GraphicsDX11/bump/bumpBumpRenderer.hpp"
#include "GraphicsDX11/bump/bumpFragmentCreate.hpp"
#include "GraphicsDX11/bump/bumpTriMeshBumpFrag.hpp"
#include "GraphicsDX11/bump/private/bumpVertexDecl.hpp"

#include "Graphics/g3d/g3dRendererMgr.hpp"


namespace
{

int l_RefCount = 0;
bumpFragmentCreate* l_pFragmentCreator = NULL;
bumpBumpRenderer* l_BumpRenderer = NULL;
}

//------------------------------------------------------------------------
//	Init must be called before you use the bump package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void bumpPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_BumpRenderer = new bumpBumpRenderer;
		int render_id = g3dRendererMgr::AddRenderer(l_BumpRenderer);
		bumpTriMeshBumpFrag::SetRendererId(render_id);

		l_pFragmentCreator = new bumpFragmentCreate;
		g3dFragmentCreate::SetImplementation(l_pFragmentCreator);

		//	set the error handler file for this package
		//
		//gfErrorHandler::SetErrorFilename( envPackageErrorIndices::e_Mat,itString("bumpErrors.tsf") );
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the bump package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void bumpPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		delete l_pFragmentCreator;
		delete l_BumpRenderer;
		l_BumpRenderer = NULL;

		// clean up packages we depend on
	}
}


//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//------------------------------------------------------------------------
void bumpPackage::InitGraphics()
{
	bumpVertexDecl::Initialize();
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void bumpPackage::CleanUpGraphics()
{
	bumpVertexDecl::DeInitialize();
}
