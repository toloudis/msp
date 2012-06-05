/*****************************************************************************
**  splnCurveSelect.cpp
**
**      The splnCurveSelect handles highlight of curves
**
**	StudioGPU
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
splnCurveModel* splnCurveSelect::CreateCurve(splnSpline *i_pCurve)
{
	// First find if the curve is already in the map, if not, create it.
	//
	CurveMap::iterator it= l_CurveMap.find(i_pCurve);
	if (it == l_CurveMap.end())
	{
		splnCurveModel *model = new splnCurveModel(*i_pCurve);
		l_CurveMap[i_pCurve] = model;
		return model;
	}
	else
		return (it->second);
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
		it->second->UpdateCurve(*i_pCurve);
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
