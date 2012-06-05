/*****************************************************************************
**	api3dObject.cpp
**
**	Base class for all objects that can be placed into the scene
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
api3dObject::~api3dObject()
{
}

//--------------------------------------------------------------------
// Pivot point information - default implementation
//--------------------------------------------------------------------
//virtual 
maPoint3d api3dObject::GetPivotPoint() const
{
	return maPoint3d(0,0,0);
}
//virtual 
void  api3dObject::SetPivotPoint(const maPoint3d &i_Position, 
						   bool i_bPreserveTransformation)
{
}
//virtual 
maVector3d api3dObject::GetPivotCompensation() const
{
	return maPoint3d(0,0,0);
}
//virtual 
void  api3dObject::SetPivotCompensation(const maVector3d &i_Compensation)
{

}

//----------------------------------------------------------------------------
// Is the pick code given within the high and low pick codes assigned
// to this node?
//----------------------------------------------------------------------------
bool api3dObject::ContainsPickCode(envType::UInt32 i_PickCode) const
{
	return false;
}

//--------------------------------------------------------------------
//	Returns the node with a fragment with the given pick code
//--------------------------------------------------------------------
const g3dSceneNode* api3dObject::GetPickedNode(envType::UInt32 i_PickCode) const
{
	return NULL;
}

