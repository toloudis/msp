/*****************************************************************************
**  pntPointMgr.cpp
**
**      The pntPointMgr handles Points
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/pnt/pntPointMgr.hpp"
#include "Support/pnt/pntPointObject.hpp"
#include "Support/pnt/pntPointSelect.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <map>
#include <string>


namespace pntPointMgr
{

namespace
{
// Use Map from string to list of Points
typedef std::vector<pntPoint*> PointList;
std::map<std::string, PointList> l_Points;

//pntPoint* l_ActivePoint = NULL;

void remove_Point(pntPoint* i_Point)
{
	pntPointSelect::RemovePoint(i_Point);
	std::map<std::string, PointList>::iterator it, end = l_Points.end();
	for (it = l_Points.begin(); it != end; ++it)
		envSTLHelpers::RemoveOneValue(it->second, i_Point);
}

void update_Point(pntPoint* i_Point)
{
	// How do we communicate Point's change to other libs?
	// Add a callback per Point?
}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Clear()
{
	std::map<std::string, PointList>::iterator it, end = l_Points.end();
	for (it = l_Points.begin(); it != end; ++it)
		envSTLHelpers::DeleteContainer(it->second);
	//l_ActivePoint = NULL;
}


//----------------------------------------------------------------------------
//	GetNumPoints
//----------------------------------------------------------------------------
int GetNumPoints(PointGroup i_Group)
{
	return l_Points[i_Group].size();
}

//----------------------------------------------------------------------------
//	GetPoint
//----------------------------------------------------------------------------
pntPoint* GetPoint(PointGroup i_Group, int i_Index)
{
	return l_Points[i_Group][i_Index];
}

//----------------------------------------------------------------------------
// Remove (delete) all Points of given type
//----------------------------------------------------------------------------
void ClearType(PointGroup i_Group)
{
	envSTLHelpers::ForAll(l_Points[i_Group], pntPointSelect::RemovePoint);
	envSTLHelpers::DeleteContainer(l_Points[i_Group]);
}


//----------------------------------------------------------------------------
//	UpdatePoint() - Point's data has changed, update Point's icon
//----------------------------------------------------------------------------
void UpdatePoint(pntPoint* i_Point)
{
	pntPointSelect::UpdatePoint(i_Point);
	//?update_Point(i_Point);
}

//----------------------------------------------------------------------------
//	AddPoint()
//----------------------------------------------------------------------------
pntPointObject* AddPoint(PointGroup i_Group, 
			  pntPoint* i_Point, 
			  const std::string& i_Name)
{
	l_Points[i_Group].push_back(i_Point);
	return pntPointSelect::CreatePoint(i_Point, i_Name);
}

//----------------------------------------------------------------------------
//	RemovePoint()
//----------------------------------------------------------------------------
void RemovePoint(pntPoint* i_Point)
{
	remove_Point(i_Point);
}

//----------------------------------------------------------------------------
//	SetRenderable()
//----------------------------------------------------------------------------
void SetRenderable(pntPoint* i_Point, bool i_Visible)
{
	pntPointSelect::SetRenderable(i_Point, i_Visible);
}
} // end of namespace