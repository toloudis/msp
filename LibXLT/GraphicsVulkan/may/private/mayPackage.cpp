/*****************************************************************************
**  mayPackage.cpp
**
**      mayPackage contains the initialization and cleanup functions
**	for the may package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MAY_PACKAGE_HPP
#include "GraphicsVulkan/may/mayPackage.hpp"
#endif
#ifndef GVK_MAY_FRAGCREATE_HPP
#include "GraphicsVulkan/may/mayFragCreate.hpp"
#endif

#ifndef AN_PACKAGE_HPP
#include "Graphics/an/anPackage.hpp"
#endif
#ifndef DBG_PACKAGE_HPP
#include "Core/dbg/dbgPackage.hpp"
#endif
#ifndef G3D_PACKAGE_HPP
#include "Graphics/g3d/g3dPackage.hpp"
#endif
#ifndef GEO_PACKAGE_HPP
#include "Core/geo/geoPackage.hpp"
#endif

namespace gvk
{
//#include "scPackage.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{

	int l_RefCount = 0;
	mayFragCreate* l_pFragmentCreator = NULL;

}

//----------------------------------------------------------------------------
//	Init must be called before you use this package.  A good place to
//	do this is in your main function, before you do anything else.
//----------------------------------------------------------------------------
void
mayPackage::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		dbgPackage::Init();
		anPackage::Init();
		g3dPackage::Init();
//		scPackage::Init();
		geoPackage::Init();

		// initialize our internal stuff
		//
		l_pFragmentCreator = new mayFragCreate;
		mdlFragCreate::SetImplementation(l_pFragmentCreator);
	}

	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp should be called after you are done with this package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//----------------------------------------------------------------------------
void
mayPackage::CleanUp()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		delete l_pFragmentCreator;
		l_pFragmentCreator = NULL;

		// clean up packages we depend on
		geoPackage::CleanUp();
//		scPackage::CleanUp();
		g3dPackage::CleanUp();
		anPackage::CleanUp();
		dbgPackage::CleanUp();
	}
}

}
