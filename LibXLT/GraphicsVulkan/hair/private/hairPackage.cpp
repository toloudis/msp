/*****************************************************************************
**  hairPackage.cpp
**
**	GraphicsVulkan port of GraphicsDX11/hair/private/hairPackage.cpp
**	(namespace gvk).
**
**      hairPackage contains the initialization and cleanup functions
**	for the hair package.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_HAIR_PACKAGE_HPP
#include "GraphicsVulkan/hair/hairPackage.hpp"
#endif

#ifndef GVK_HAIR_RENDERERVK_HPP
#include "GraphicsVulkan/hair/HairRendererVk.hpp"
#endif
#ifndef GVK_HAIR_MODELFRAGVK_HPP
#include "GraphicsVulkan/hair/hairModelFragVk.hpp"
#endif

#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif

namespace gvk
{

namespace
{

int l_RefCount = 0;
HairRendererVk* l_HairRenderer = NULL;
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
		l_HairRenderer = new HairRendererVk;
		int render_id = g3dRendererMgr::AddRenderer(l_HairRenderer);
		hairModelFragVk::SetRendererId(render_id);
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
		delete l_HairRenderer;
		l_HairRenderer = NULL;

		// clean up packages we depend on
	}
}

}
