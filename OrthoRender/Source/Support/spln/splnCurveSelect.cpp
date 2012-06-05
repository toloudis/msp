/*****************************************************************************
**  splnCurveSelect.cpp
**
**      The splnCurveSelect handles highlight of curves
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#include "Support/spln/splnCurveSelect.hpp"
#include "Support/spln/splnCurveModel.hpp"

#include "Support/spln/splnSpline.hpp"

#include "Core/ma/maFloatRGBA.hpp"

#include <map>


namespace
{
//int l_RenderLayer = g3dPackage::e_World;

typedef std::map<splnSpline*, splnCurveModel*> CurveMap;
CurveMap l_CurveMap;
}


//----------------------------------------------------------------------------
//	Clear
//----------------------------------------------------------------------------
void splnCurveSelect::Clear()
{
	// Delete contents in curve map
	CurveMap::iterator it, end = l_CurveMap.end();
	for ( it = l_CurveMap.begin(); it != end; ++it )
		delete it->second;
	l_CurveMap.clear();

}

//----------------------------------------------------------------------------
//	CreateCurve() - create a graphical representation for the given curve,
//		if it doesn't already exist
//----------------------------------------------------------------------------
void splnCurveSelect::CreateCurve(splnSpline *i_pCurve, 
								  pick3dPickObject* i_pParent)
{
	// First find if the curve is already in the map, if not, create it.
	//
	CurveMap::iterator it= l_CurveMap.find(i_pCurve);
	if (it == l_CurveMap.end())
	{
		splnCurveModel *model = new splnCurveModel(*i_pCurve, i_pParent);
		l_CurveMap[i_pCurve] = model;
	}
}

//----------------------------------------------------------------------------
//	RemoveCurve()
//----------------------------------------------------------------------------
void splnCurveSelect::RemoveCurve(splnSpline *i_pCurve)
{
	CurveMap::iterator it= l_CurveMap.find(i_pCurve);
	if (it != l_CurveMap.end())
	{
		delete it->second;
		l_CurveMap.erase(it);
	}
}

//----------------------------------------------------------------------------
//	UpdateCurve()
//----------------------------------------------------------------------------
void splnCurveSelect::UpdateCurve(splnSpline *i_pCurve)
{
	CurveMap::iterator it= l_CurveMap.find(i_pCurve);
	if (it != l_CurveMap.end())
	{
		if (it->second->UpdateCurve(*i_pCurve))
		{
			return; // updated within current curve
		}
		else
		{
			bool vis = l_CurveMap[i_pCurve]->GetRenderable();
			pick3dPickObject *parent = l_CurveMap[i_pCurve]->GetParentObject();
			RemoveCurve(i_pCurve);
			CreateCurve(i_pCurve, parent);
			SetRenderable( i_pCurve, vis );
			return;
		}
	}
}

//----------------------------------------------------------------------------
//	SetRenderable()
//----------------------------------------------------------------------------
void splnCurveSelect::SetRenderable(splnSpline *i_pCurve, bool i_Visible)
{
	CurveMap::iterator it= l_CurveMap.find(i_pCurve);
	if (it != l_CurveMap.end())
	{
		it->second->SetRenderable( i_Visible );
	}
}

//----------------------------------------------------------------------------
//	GetControlPointObject()
//----------------------------------------------------------------------------
splnCurvePointObject* splnCurveSelect::GetControlPointObject(splnSpline *i_pCurve, int i_Index)
{
	return l_CurveMap[i_pCurve]->GetControlPointObject(i_Index);
}


//----------------------------------------------------------------------------
// Do ray pick on control point objects
//----------------------------------------------------------------------------
bool splnCurveSelect::ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
{
	bool found = false;

	CurveMap::iterator it, end = l_CurveMap.end();
	for ( it = l_CurveMap.begin(); it != end; ++it )
	{
		found |= it->second->ObjectPick(i_Ray, io_PickList);
	}

	return found;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
pick3dPickObject* splnCurveSelect::MatchPickCode(envType::UInt32 i_PickCode)
{
	CurveMap::iterator it, end = l_CurveMap.end();
	for ( it = l_CurveMap.begin(); it != end; ++it )
	{
		pick3dPickObject* pPicked = it->second->MatchPickCode( i_PickCode );
		if (pPicked)
			return pPicked;
	}
	return NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void splnCurveSelect::SetColor( splnSpline *i_pCurve, maFloatRGBA& i_Color )
{
	CurveMap::iterator it= l_CurveMap.find(i_pCurve);
	if (it != l_CurveMap.end())
	{
		it->second->SetColor(i_Color);
	}
}
