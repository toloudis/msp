/*****************************************************************************
**  shdwPackage.cpp
**
**      shdwPackage contains the initialization and cleanup functions
**	for the shdw package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_SHDW_PACKAGE_HPP
#include "GraphicsVulkan/shdw/shdwPackage.hpp"
#endif
#ifndef GVK_SHDW_SCENERENDERERCREATE_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererCreate.hpp"
#endif

#include <cstddef>
namespace gvk
{

namespace
{

int l_RefCount = 0;
shdwSceneRendererCreate* l_pRendererCreator = 0;

}

//------------------------------------------------------------------------
//	Init must be called before you use the shdw package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void shdwPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pRendererCreator = new shdwSceneRendererCreate;
		g3dSceneRendererCreate::SetImplementation(l_pRendererCreator);

	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the shdw package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void shdwPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order (DX11 leaves the
		// deleted creator installed; it is removed here)
		//
		g3dSceneRendererCreate::SetImplementation(NULL);
		delete l_pRendererCreator;
		l_pRendererCreator = NULL;

		// clean up packages we depend on
	}
}


//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//	(the InitStates of each pass and renderer joins here, in DX11's order,
//	as it is ported)
//------------------------------------------------------------------------
void shdwPackage::InitGraphics()
{
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void shdwPackage::CleanUpGraphics()
{
}

}
