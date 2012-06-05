/*****************************************************************************
**  pntPointObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/pnt/pntPointObject.hpp"
#include "Support/pnt/pntPoint.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/geo/geoRayIntersection.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Graphics/Cam/camCamera.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"
#include "Tool/gpx/gpxIconSet.hpp"


namespace
{
	const float l_IconsScale = 0.0025f;
	const float l_SphereRadius	= 0.7f;
	//const float l_PickRadius	= 0.4f;	// pick larger than icon
	const maAxisBox l_SphereBox(-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
pntPointObject::pntPointObject( pntPoint *i_Point, 
								const std::string &i_Name )
:	m_pPoint(i_Point),
    m_Name( i_Name )
{
	// Create 3D icon
	api3dObjectSimple *pObjectBase = api3dShape::CreateSphere(maFloatRGBA(1,1,0,1), l_SphereRadius, 8, 8);
	m_pObject = icnIconLayer::CreateIconSet(pObjectBase);
	m_pObject->SetRenderable(true);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	maPoint3d pos = i_Point->GetPosition();
	m_pObject->SetPosition( pos );
	m_Position.SetValue( pos );
	
	// create thread-safe proxy for the icon
	m_pObjectProxy = new gpxIconSet(*m_pObject);

	icnIconScale::RegisterScaleInterest( this );
	
	// Register the property so it can be displayed to the user
	prtyVector3dEditUpDownUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&m_Position, "Transform", "Position");
	pPUII->SetIncrement(0.1f, 0.1f, 0.1f);
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Position.AddCallback(new prtyCallbackWrapper<pntPointObject>(this, &pntPointObject::PositionChanged));

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pntPointObject::~pntPointObject()
{
	sel3dMgr::RemoveFromSelection(this); // safe to call even if not in selection list

	icnIconScale::UnRegisterScaleInterest(this);

	delete m_pObjectProxy;
	//api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
//void pntPointObject::GlobalScaleChanged( float i_Scale )
//{
//	if (m_pObject)
//		m_pObject->SetUniformScale( i_Scale );
////	update_world_box();
//}

//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
//virtual 
void pntPointObject::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	if( m_pObject )
	{
		float fScale = i_Scale * icnIconScale::GetIconScaleForPosition(GetPosition(), i_Camera);
		m_pObjectProxy->SetLayerScale( i_IconLayerIndex, maVector3d(fScale,fScale,fScale) );
	}
}


//--------------------------------------------------------------------
//	GetPoint - get point it is changing
//--------------------------------------------------------------------
pntPoint* pntPointObject::GetPoint()
{
	return m_pPoint;
}

//--------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the pntPointObject.
//--------------------------------------------------------------------
maAxisBox pntPointObject::GetWorldBox(int i_IconLayerIndex) const
{
	//return m_WorldBox;

	// Since the point icon scales with the camera view,
	// there is no one definite world box. Have to compute it from 
	// the size of the icon in a given layer.
	float icon_radius = l_SphereRadius * m_pObjectProxy->GetLayerScale(i_IconLayerIndex).GetX();
	maAxisBox icon_box(-icon_radius, icon_radius,
						-icon_radius, icon_radius,
						-icon_radius, icon_radius);
	icon_box.Translate( m_Position.GetValue() );
	return icon_box;
}

//--------------------------------------------------------------------
//	GetPosition returns the position of the object.
//--------------------------------------------------------------------
maPoint3d pntPointObject::GetPosition() const
{
	return m_pObjectProxy->GetPosition();
}

//--------------------------------------------------------------------
//	SetPosition sets the position of the object.
//--------------------------------------------------------------------
void pntPointObject::SetPosition(const maPoint3d& i_Position)
{
	m_pObjectProxy->SetPosition(i_Position);
	m_Position.SetValue( i_Position );
}

//--------------------------------------------------------------------
//	UpdatePosition - called when the point is altered from
//	user input.
//--------------------------------------------------------------------
void pntPointObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	if (i_Position == m_pObject->GetPosition()) return;

	m_pPoint->SetPosition( i_Position );
	this->SetPosition( i_Position );
}

//--------------------------------------------------------------------
//	GetOrientation returns the orientation of the object.
//--------------------------------------------------------------------
maRotation pntPointObject::GetOrientation() const
{
	return m_pObjectProxy->GetOrientation();
}

//--------------------------------------------------------------------
//	SetOrientation sets the orientation of the object.
//--------------------------------------------------------------------
void pntPointObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	// no need on sphere object?
	//m_pObject->SetPosition(i_Orientation);
}

//--------------------------------------------------------------------
//	Scale
//--------------------------------------------------------------------
maPoint3d pntPointObject::GetScale() const
{
	return m_pObjectProxy->GetScale();
}
void pntPointObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	// no need on sphere object?
	//maPoint3d gScale;
	//gScale.m_X = icnIconScale::Scale( i_Scale.m_X );
	//gScale.m_Y = icnIconScale::Scale( i_Scale.m_Y );
	//gScale.m_Z = icnIconScale::Scale( i_Scale.m_Z );
	//m_pObject->SetScale( i_Scale );
}


//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool pntPointObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!this->GetRenderable())
		return false;

	return m_pObject->ContainsPickCode(i_PickCode);
}

//--------------------------------------------------------------------
//	Renderable sets whether the mtObject can be selected.
//--------------------------------------------------------------------
void pntPointObject::SetRenderable(bool i_Renderable)
{
	m_pObjectProxy->SetRenderable(i_Renderable);
}
bool pntPointObject::GetRenderable() const
{
	return m_pObjectProxy->GetRenderable();
}

//--------------------------------------------------------------------
//	GetDefaultTerrainOffset is the desired offset from
//	the terrain for this object.  This can be altered
//	by the user during placement
//--------------------------------------------------------------------
float pntPointObject::GetDefaultTerrainOffset() const
{
	return 0.0f;
}


//--------------------------------------------------------------------
// Flags for how object can be modified
//--------------------------------------------------------------------
mnmObject::RotateFlags	pntPointObject::GetRotateFlags()
{
	return mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags	pntPointObject::GetScaleFlags()
{
	return mnmObject::e_ScaleNone;
}
mnmObject::TranslateFlags	pntPointObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//----------------------------------------------------------------------------
// Get parent object of the spline in order for selection to trace from
//	this spline back to the controlled object.
//----------------------------------------------------------------------------
//sel3dObject* pntPointObject::GetParentObject() const
//{
//	return m_pParent;
//}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
std::string pntPointObject::GetDisplayName() const
{
	return m_Name;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pntPointObject::PositionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Use a threshold for changing the 3D icon in order to prevent infinite loops
	if ( (m_Position.GetValue() - m_pPoint->GetPosition()).LengthSqr() > maConstants::c_fEpsilon)
	{
		// Alter 3D point icon's position
		maPoint3d pos = m_Position.GetValue();
		m_pPoint->SetPosition( pos );
		this->SetPosition( pos );
	}
}
