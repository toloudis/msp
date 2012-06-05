/*****************************************************************************
**  GraphicsLayer.cpp
**
**      GraphicsLayer contains the initialization functions
**	for the all packages within the Graphics Layer.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/GraphicsLayer.hpp"

#include "Graphics/an/anPackage.hpp"
//#include "camPackage.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
#include "Graphics/emdl/emdlPackage.hpp"
#include "Graphics/ent/entPackage.hpp"
#include "Graphics/mat/matPackage.hpp"
//#include "scPackage.hpp"
#include "Graphics/smdl/smdlPackage.hpp"

namespace
{

int l_RefCount = 0;

g2dSystem* l_pSystem2d = NULL;
g3dSystem* l_pSystem3d = NULL;
}

//----------------------------------------------------------------------------
//	Init
//----------------------------------------------------------------------------
void GraphicsLayer::Init()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages in the layer
		anPackage::Init();
		g2dPackage::Init();
		g3dPackage::Init();
		//camPackage::Init();
		matPackage::Init();
		//scPackage::Init();
		entPackage::Init();
		emdlPackage::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp
//----------------------------------------------------------------------------
void GraphicsLayer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		emdlPackage::CleanUp();
		entPackage::CleanUp();
		//scPackage::CleanUp();
		matPackage::CleanUp();
		//camPackage::CleanUp();
		g3dPackage::CleanUp();
		g2dPackage::CleanUp();
		anPackage::CleanUp();

	}
}

//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//------------------------------------------------------------------------
void GraphicsLayer::InitGraphics(g2dSystem* i_pSystem2d, g3dSystem* i_pSystem3d)
{
	l_pSystem2d = i_pSystem2d;
	l_pSystem3d = i_pSystem3d;

	smdlPackage::InitGraphics();
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void GraphicsLayer::CleanUpGraphics()
{
	smdlPackage::CleanUpGraphics();

	l_pSystem3d = NULL;
	l_pSystem2d = NULL;
}

//------------------------------------------------------------------------
//	Implementation specific system accessors
//------------------------------------------------------------------------
g2dSystem* GraphicsLayer::GetSystem2D()
{
	return l_pSystem2d;
}
g3dSystem* GraphicsLayer::GetSystem3D()
{
	return l_pSystem3d;
}
