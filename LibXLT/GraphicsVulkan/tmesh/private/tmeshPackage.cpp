/*****************************************************************************
**  tmeshPackage.cpp
**
**      tmeshPackage contains the initialization and cleanup functions
**	for the tmesh package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_TMESH_PACKAGE_HPP
#include "GraphicsVulkan/tmesh/tmeshPackage.hpp"
#endif

#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif
//#include "GraphicsVulkan/tmesh/tmeshFragmentCreate.hpp"
//#include "GraphicsVulkan/tmesh/tmeshRenderer.hpp"

#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif

namespace gvk
{


namespace
{

int l_RefCount = 0;
//tmeshRenderer* l_TMeshRenderer = NULL;

}

//------------------------------------------------------------------------
//	Init must be called before you use the tmesh package.  A good place to
//	do this is in your main function, with your other package
//	initializers.
//------------------------------------------------------------------------
void tmeshPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
//		l_TMeshRenderer = new tmeshRenderer;
//		int render_id = g3dRendererMgr::AddRenderer(l_TMeshRenderer);
//		tmeshFrag::SetRendererId(render_id);

	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with the tmesh package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void tmeshPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
//		delete l_TMeshRenderer;
//		l_TMeshRenderer = NULL;

		// clean up packages we depend on
	}
}

}
