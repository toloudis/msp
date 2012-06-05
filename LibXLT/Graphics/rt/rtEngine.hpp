/*****************************************************************************
**	rtEngine.hpp
**
**		Ray tracing engine
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef RT_ENGINE_HPP
#error rtEngine.hpp multiply included
#endif
#define RT_ENGINE_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef RT_STRUCTURES_HPP
#include "Graphics/rt/rtStructures.hpp"
#endif


//============================================================================
//============================================================================
class rtEngine
{
public:
	//--------------------------------------------------------------------
	// rtEngine()
	//--------------------------------------------------------------------
	rtEngine( World * i_world );

	//--------------------------------------------------------------------
	// ~rtEngine()
	//--------------------------------------------------------------------
	~rtEngine();

	//--------------------------------------------------------------------
	// render()
	//--------------------------------------------------------------------
	void Render();

	//--------------------------------------------------------------------
	// AllocateResources()
	//--------------------------------------------------------------------
	void AllocateResources();

	//--------------------------------------------------------------------
	// ReleaseResources()
	//--------------------------------------------------------------------
	void ReleaseResources();

	//--------------------------------------------------------------------
	// traceRay()
	//--------------------------------------------------------------------
	maFloatRGBA TraceRay( Ray ray, int depth );

protected:
	maFloatRGBA**	canvas;
	World*			world;
};

