/****************************************************************************\
**	g3dSystem.hpp
**
**		The g3dSystem holds and renders the g3dSystemNodes which describe the
**	graphical scene.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SYSTEM_HPP
#error g3dSystem.hpp multiply included
#endif
#define G3D_SYSTEM_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

//============================================================================
//============================================================================
class g3dSystem
{
public:
	//--------------------------------------------------------------------
	//  Create and initialize system
	//--------------------------------------------------------------------
	g3dSystem() {};

	//--------------------------------------------------------------------
	// Clean up and destroy system
	//--------------------------------------------------------------------
	virtual ~g3dSystem() {};

	//------------------------------------------------------------------------
	//	GetVideoAdapterName returns some kind of ANSI C string uniquely
	//	identifying the type of video hardware in the system.  This function
	//	can be called only after calling Init().
	//------------------------------------------------------------------------
	virtual const char* GetVideoAdapterName() = 0;

	//------------------------------------------------------------------------
	// return KB
	//------------------------------------------------------------------------
	virtual float GetVideoMemory() = 0;
};
