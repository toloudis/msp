/*****************************************************************************
**  GraphicsDX11Layer.cpp
**
**      GraphicsDX11Layer contains the initialization functions
**	for the all packages within the GraphicsDX11 Layer.
**
**	Studio GPU 
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/GraphicsDX11Layer.hpp"

#include "GraphicsDX11/g2d/g2dSystemDX11.hpp"
#include "GraphicsDX11/g3d/g3dSystemDX11.hpp"
#include "GraphicsDX11/bump/bumpPackage.hpp"
#include "GraphicsDX11/tmesh/tmeshPackage.hpp"
#include "GraphicsDX11/shdw/shdwPackage.hpp"
//#include "GraphicsDX11/sprt/sprtPackage.hpp"
#include "GraphicsDX11/may/mayPackage.hpp"
#include "GraphicsDX11/hair/hairPackage.hpp"
#ifdef MSP_GFX_PROFILING
#include "GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp"
#endif

namespace
{

int l_RefCount = 0;

g2dSystemDX11* l_pSystem2D = NULL;
g3dSystemDX11* l_pSystem3D = NULL;
#ifdef MSP_GFX_PROFILING
g3dProfilerGpuDX11* l_pProfilerGpu = NULL;
#endif
}

//----------------------------------------------------------------------------
//	Init
//----------------------------------------------------------------------------
void GraphicsDX11Layer::Init(void* i_hWnd)
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		l_pSystem2D = new g2dSystemDX11;
		l_pSystem3D = new g3dSystemDX11;

		// initialize packages in the layer
		shdwPackage::Init();
		tmeshPackage::Init();
		bumpPackage::Init();

		mayPackage::Init();
//		sprtPackage::Init();
		hairPackage::Init();
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp
//----------------------------------------------------------------------------
void GraphicsDX11Layer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		hairPackage::CleanUp();
//		sprtPackage::CleanUp();
		mayPackage::CleanUp();
		bumpPackage::CleanUp();
		tmeshPackage::CleanUp();
		shdwPackage::CleanUp();

		delete l_pSystem3D;
		delete l_pSystem2D;
	}
}


//------------------------------------------------------------------------
//	InitGraphics - should be called after device is created
//------------------------------------------------------------------------
void GraphicsDX11Layer::InitGraphics()
{
	shdwPackage::InitGraphics();
	bumpPackage::InitGraphics();
//	sprtPackage::InitGraphics();

#ifdef MSP_GFX_PROFILING
	l_pProfilerGpu = new g3dProfilerGpuDX11;
	g3dProfiler::SetGpu(l_pProfilerGpu);
#endif
}

//------------------------------------------------------------------------
//	CleanUpGraphics - should be called before device is destroyed
//------------------------------------------------------------------------
void GraphicsDX11Layer::CleanUpGraphics()
{
#ifdef MSP_GFX_PROFILING
	g3dProfiler::SetGpu(NULL);
	delete l_pProfilerGpu;
	l_pProfilerGpu = NULL;
#endif

//	sprtPackage::CleanUpGraphics();
	bumpPackage::CleanUpGraphics();
	shdwPackage::CleanUpGraphics();

}

//------------------------------------------------------------------------
//	Accessors for the major graphics system objects
//------------------------------------------------------------------------
g2dSystem* GraphicsDX11Layer::GetSystem2D()
{
	return l_pSystem2D;
}
g3dSystem* GraphicsDX11Layer::GetSystem3D()
{
	return l_pSystem3D;
}
