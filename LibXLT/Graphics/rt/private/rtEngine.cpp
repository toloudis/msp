/*****************************************************************************
**	rtEngine.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/rt/rtEngine.hpp"


//--------------------------------------------------------------------
// rtEngine()
//--------------------------------------------------------------------
rtEngine::rtEngine( World * i_world )
:	world(i_world)
{
	AllocateResources();
	Render();
}

//--------------------------------------------------------------------
// ~rtEngine()
//--------------------------------------------------------------------
rtEngine::~rtEngine()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
// AllocateResources()
//--------------------------------------------------------------------
void rtEngine::AllocateResources()
{
	canvas = new maFloatRGBA*[world->viewplane.hres];
	int i;
	for ( i = 0; i < world->viewplane.hres; ++i) 
	{
		canvas[i] = new maFloatRGBA[world->viewplane.vres];
	}

}

//--------------------------------------------------------------------
// ReleaseResources()
//--------------------------------------------------------------------
void rtEngine::ReleaseResources()
{
	int i;
	for ( i = 0; i < world->viewplane.hres; ++i )
	{
		delete [] canvas[i];
	}
    delete [] canvas;
}

//--------------------------------------------------------------------
// Render()
//--------------------------------------------------------------------
void rtEngine::Render() {

	Ray ray;	
	maPoint2d pp;
	ray.o = world->camera.eye;
	ray.d = maVector3d(0,0,-1);

	// Loop through pixels of 2D viewplane
	for ( int i = 0 ; i < world->viewplane.hres; i++ ) {
		for ( int j = 0 ; j < world->viewplane.vres; j++ ) {

			pp.SetX( i - 0.5 * (world->viewplane.hres - 1.0));
			pp.SetY( j - 0.5 * (world->viewplane.vres - 1.0));
		
			ray.d = world->camera.u*pp.GetX() + world->camera.v*pp.GetY() - world->camera.w*VIEW_DIST;
			ray.d.Normalize();
			canvas[i][j] = TraceRay(ray,1);	
		}
	}
}

//--------------------------------------------------------------------
// TraceRay()
//--------------------------------------------------------------------
maFloatRGBA rtEngine::TraceRay( Ray ray, int depth )
{
	return maFloatRGBA(0,0,0,0);
}