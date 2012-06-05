/*****************************************************************************
**  hairPackage.cpp
**
**      hairPackage contains the initialization and cleanup functions
**	for the hair package.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/hair/hairPackage.hpp"

#include "GraphicsDX11/hair/hairRendererDX11.hpp"
//#include "GraphicsDX11/hair/hairFragmentCreateDX11.hpp"
#include "GraphicsDX11/hair/hairModelFrag.hpp"

#include "Graphics/g3d/g3dRendererMgr.hpp"

namespace
{

int l_RefCount = 0;
//hairFragmentCreateDX11* l_pFragmentCreator = NULL;
HairRendererDX11* l_HairRenderer = NULL;
}

//------------------------------------------------------------------------
//	Init must be called before you use the hair package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void hairPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		l_HairRenderer = new HairRendererDX11;
		int render_id = g3dRendererMgr::AddRenderer(l_HairRenderer);
		hairModelFrag::SetRendererId(render_id);

		//l_pFragmentCreator = new hairFragmentCreateDX11;
		//hairFragmentCreate::SetImplementation(l_pFragmentCreator);
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the hair package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void hairPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		//delete l_pFragmentCreator;
		delete l_HairRenderer;
		l_HairRenderer = NULL;

		// clean up packages we depend on
	}
}
