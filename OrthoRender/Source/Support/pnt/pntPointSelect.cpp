/*****************************************************************************
**  pntPointSelect.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/pnt/pntPointSelect.hpp"
#include "Support/pnt/pntPointModel.hpp"

#include "Support/pnt/pntPoint.hpp"
#include "Support/pnt/pntPointObject.hpp"

#include <map>


namespace
{
//int l_RenderLayer = g3dPackage::e_World;

typedef std::map<pntPoint*, pntPointModel*> PointMap;
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
void pntPointSelect::CreatePoint(pntPoint *i_pPoint, 
								 pick3dPickObject* i_pParent,
								 const std::string& i_Name)
{
	// First find if the Point is already in the map, if not, create it.
	//
	PointMap::iterator it= l_PointMap.find(i_pPoint);
	if (it == l_PointMap.end())
	{
		pntPointModel *model = new pntPointModel(*i_pPoint, i_pParent, i_Name);
		l_PointMap[i_pPoint] = model;
	}
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
		if (it->second->UpdatePoint(*i_pPoint))
		{
			return; // updated within current point
		}
		else
		{
			bool vis = l_PointMap[i_pPoint]->GetRenderable();
			pick3dPickObject *parent = l_PointMap[i_pPoint]->GetParentObject();
			std::string name = l_PointMap[i_pPoint]->GetPointObject()->GetPick3dName();
			RemovePoint(i_pPoint);
			CreatePoint(i_pPoint, parent, name);
			SetRenderable( i_pPoint, vis );
			return;
		}
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
	return l_PointMap[i_pPoint]->GetPointObject();
}


//----------------------------------------------------------------------------
// Do ray pick on control point objects
//----------------------------------------------------------------------------
bool pntPointSelect::ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
{
	bool found = false;

	PointMap::iterator it, end = l_PointMap.end();
	for ( it = l_PointMap.begin(); it != end; ++it )
	{
		found |= it->second->ObjectPick(i_Ray, io_PickList);
	}

	return found;
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
		if (it->second->GetPointObject()->MatchPickCode(i_PickCode))
			return it->second->GetPointObject();
	}
	return NULL;
}