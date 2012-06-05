/*****************************************************************************
**  cmpsSelectMgr.hpp
**
**      The cmpsSelectMgr puts a bounding box highlight around
**	the selected objects.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMPS_SELECTMGR_HPP
#error cmpsSelectMgr.hpp multiply included
#endif
#define CMPS_SELECTMGR_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

class camCamera;

//============================================================================
//============================================================================
namespace cmpsSelectMgr
{
	//----------------------------------------------------------------------------
	//	Initialize
	//----------------------------------------------------------------------------
	void Initialize();

	//----------------------------------------------------------------------------
	//	DeInitialize
	//----------------------------------------------------------------------------
	void DeInitialize();

	//----------------------------------------------------------------------------
	//	SetNumBoxes
	//----------------------------------------------------------------------------
	void SetNumBoxes(int i_Count);

	//----------------------------------------------------------------------------
	//	SetBounds - resize and position bounding box of indexed object
	//----------------------------------------------------------------------------
	void SetBounds(int i_BoxIndex,
					int i_IconLayerIndex,
					const maAxisBox& i_Bounds,
					const maPoint3d& i_WorldPivot, 
					const camCamera& i_Camera,
					float i_ScalingFactor);

	//----------------------------------------------------------------------------
	//	SetRenderable - set the visible state of each visible compass
	//----------------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
};
