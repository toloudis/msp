/*****************************************************************************
**  pntPointModel.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/pnt/pntPointModel.hpp"
#include "Support/pnt/pntPointObject.hpp"
#include "Support/pnt/pntPoint.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pntPointModel::pntPointModel(pntPoint &i_Point, 
							 pick3dPickObject* i_pParent,
							 const std::string& i_Name)
: m_pParent(i_pParent)
//	m_bVisible(false)
{
	m_pObject = new pntPointObject( &i_Point, m_pParent, i_Name );

	this->SetRenderable( true );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pntPointModel::~pntPointModel()
{
	//if ( m_bVisible )
	//{
	//	if (m_pObject)	
	//		api3dScene::RemoveObject(m_pObject);
	//}
	delete m_pObject;
}

//----------------------------------------------------------------------------
// UpdatePoint - attempt to use existing objects to represent
//	the given point.  If the number of control points are
//	different, then false is returned.
//----------------------------------------------------------------------------
bool pntPointModel::UpdatePoint(pntPoint &i_Point)
{
	m_pObject->SetPosition( i_Point.GetPosition() );

	return true;
}

//----------------------------------------------------------------------------
//	GetPointObject()
//----------------------------------------------------------------------------
pntPointObject* pntPointModel::GetPointObject()
{
	return m_pObject;
}

//----------------------------------------------------------------------------
// Do ray pick on control point objects
//----------------------------------------------------------------------------
bool pntPointModel::ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
{
	bool found = false;

	float tval = 0.0f;
	if (m_pObject->RayPick(i_Ray.GetRayStart(), i_Ray.GetRayEnd(), tval))
	{
		io_PickList.AddItem(m_pObject, tval);
		found = true;
	}

	return found;
}

//----------------------------------------------------------------------------
//	Render the point or not
//----------------------------------------------------------------------------
void pntPointModel::SetRenderable( bool i_bVisible )
{
	//if ( !m_bVisible && i_bVisible )
	//{
	//	api3dScene::AddObject(m_pObject);
	//}
	//if ( m_bVisible && !i_bVisible )
	//{
	//	api3dScene::RemoveObject(m_pObject);
	//}

	m_pObject->SetRenderable( i_bVisible );
	//m_bVisible = i_bVisible;
}

//----------------------------------------------------------------------------
//	return whether the point is visible or not
//----------------------------------------------------------------------------
bool pntPointModel::GetRenderable()
{
	return m_pObject->GetRenderable();
	//return m_bVisible;
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
pick3dPickObject* pntPointModel::GetParentObject() const
{
	return m_pParent;
}

