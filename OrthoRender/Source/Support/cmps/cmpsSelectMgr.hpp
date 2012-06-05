/*****************************************************************************
**  cmpsSelectMgr.hpp
**
**      The cmpsSelectMgr puts a bounding box highlight around
**	the selected objects.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMPS_SELECTMGR_HPP
#error cmpsSelectMgr.hpp multiply included
#endif
#define CMPS_SELECTMGR_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

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
	void SetBounds(int i_Index,
					const maAxisBox& i_Bounds,
					const maPoint3d& i_WorldPivot, 
					const maPoint3d& i_CameraPos  );
};
