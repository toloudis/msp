/*****************************************************************************
**  pntPointSelect.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/pnt/pntPointSelect.hpp"
#include "Support/pnt/pntPointObject.hpp"

#include "Support/pnt/pntPoint.hpp"

#include "Core/Geo/geoPickRay.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"

#include <map>


namespace
{
//int l_RenderLayer = g3dPackage::e_World;

typedef std::map<pntPoint*, pntPointObject*> PointMap;
PointMap l_PointMap;
}


//----------------------------------------------------------------------------
//	Clear
//----------------------------------------------------------------------------
void pntPointSelect::Clear()
{
	// Delete contents in point map
	PointMap::iterator it, end = l_PointMap.end();
	for ( it = l_PointMap.begin(); it != end; ++it )
		delete it->second;
	l_PointMap.clear();

}

//----------------------------------------------------------------------------
//	CreatePoint() - create the Point if it doesn't exist
//----------------------------------------------------------------------------
pntPointObject* pntPointSelect::CreatePoint(pntPoint *i_pPoint, 
								 const std::string& i_Name)
{
	// First find if the Point is already in the map, if not, create it.
	//
	PointMap::iterator it= l_PointMap.find(i_pPoint);
	if (it == l_PointMap.end())
	{
		pntPointObject *model = new pntPointObject(i_pPoint, i_Name);
		l_PointMap[i_pPoint] = model;
		return model;
	}
	else 
		return (it->second);
}

//----------------------------------------------------------------------------
//	RemovePoint()
//----------------------------------------------------------------------------
void pntPointSelect::RemovePoint(pntPoint *i_pPoint)
{
	PointMap::iterator it= l_PointMap.find(i_pPoint);
	if (it != l_PointMap.end())
	{
		delete it->second;
		l_PointMap.erase(it);
	}
}

//----------------------------------------------------------------------------
//	UpdatePoint()
//----------------------------------------------------------------------------
void pntPointSelect::UpdatePoint(pntPoint *i_pPoint)
{
	PointMap::iterator it= l_PointMap.find(i_pPoint);
	if (it != l_PointMap.end())
	{
		//it->second->UpdatePoint(*i_pPoint);
		it->second->SetPosition( i_pPoint->GetPosition() );
	}
}

//----------------------------------------------------------------------------
//	SetRenderable()
//----------------------------------------------------------------------------
void pntPointSelect::SetRenderable(pntPoint *i_pPoint, bool i_Visible)
{
	PointMap::iterator it= l_PointMap.find(i_pPoint);
	if (it != l_PointMap.end())
	{
		it->second->SetRenderable( i_Visible );
	}
}

//----------------------------------------------------------------------------
//	GetPointObject()
//----------------------------------------------------------------------------
pntPointObject* pntPointSelect::GetPointObject(pntPoint *i_pPoint)
{
	//return l_PointMap[i_pPoint]->GetPointObject();
	return l_PointMap[i_pPoint];
}


//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* pntPointSelect::MatchPickCode(envType::UInt32 i_PickCode)
{
	PointMap::iterator it, end = l_PointMap.end();
	for ( it = l_PointMap.begin(); it != end; ++it )
	{
		//if (it->second->GetPointObject()->MatchPickCode(i_PickCode))
		//	return it->second->GetPointObject();
		if (it->second->MatchPickCode(i_PickCode))
			return it->second;
	}
	return NULL;
}