/*****************************************************************************
**  pntPointSelect.hpp
**
**      The pntPointSelect handles highlight of points
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_POINTSELECT_HPP
#error pntPointSelect.hpp multiply included
#endif
#define PNT_POINTSELECT_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif


#include <string>

//============================================================================
//	forward references
//============================================================================
class pntPoint;
class pntPointObject;
class geoPickRay;
class pick3dPickList;
class pick3dPickObject;
class sel3dObject;

//============================================================================
//============================================================================
namespace pntPointSelect
{
	//----------------------------------------------------------------------------
	//	Clear
	//----------------------------------------------------------------------------
	void Clear();

	//----------------------------------------------------------------------------
	//	CreatePoint() - create graphical representation for this Point,
	//		if it doesn't already exist
	//----------------------------------------------------------------------------
	pntPointObject* CreatePoint(pntPoint *i_pPoint, 
					 const std::string& i_Name);

	//----------------------------------------------------------------------------
	//	RemovePoint()
	//----------------------------------------------------------------------------
	void RemovePoint(pntPoint *i_pPoint);

	//----------------------------------------------------------------------------
	//	UpdatePoint()
	//----------------------------------------------------------------------------
	void UpdatePoint(pntPoint *i_pPoint);

	//----------------------------------------------------------------------------
	//	SetRenderable()
	//----------------------------------------------------------------------------
	void SetRenderable(pntPoint *i_pPoint, bool i_Visible);

	//----------------------------------------------------------------------------
	//	GetPointObject()
	//----------------------------------------------------------------------------
	pntPointObject* GetPointObject(pntPoint *i_pPoint);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);
}
