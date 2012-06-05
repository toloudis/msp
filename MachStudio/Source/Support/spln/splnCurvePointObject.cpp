/*****************************************************************************
**  splnCurvePointObject.cpp
**
**      A splnCurvePointObject allows a user to manipulate
**	control point of a curve.
**
**	StudioGPU
**	Copyright(C) 2001 - All Rights Reserved
\****************************************************************************/
#include "Support/spln/splnCurvePointObject.hpp"

#include "Support/spln/splnCurveMgr.hpp"
#include "Support/spln/splnCurveSelect.hpp"

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

#include <algorithm>	// find
#include <sstream>

namespace
{
	const float l_IconsScale = 0.0025f;
	const float l_SphereRadius = 0.7f;
	//const float l_PickRadius = 2.5f;	// pick larger than icon
	const maAxisBox l_SphereBox(-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
splnCurvePointObject::splnCurvePointObject(splnSpline *i_Curve,
										int i_PointIndex)
:	m_Curve(i_Curve),
	m_PointIndex(i_PointIndex)
{
	// Create 3D icon
	api3dObjectSimple *pObjectBase = api3dShape::CreateSphere(maFloatRGBA(0.5f, 0.5f, 0.0f, 1.0f), l_SphereRadius, 8, 8);
	m_pObject = icnIconLayer::CreateIconSet(pObjectBase);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	maPoint3d pos = i_Curve->GetPointPos(i_PointIndex);
	m_pObject->SetPosition(pos);
	m_Position.SetValue( pos );

	// create thread-safe proxy for the icon
	m_pObjectProxy = new gpxIconSet(*m_pObject);

	icnIconScale::RegisterScaleInterest( this );

	// Register the property so it can be displayed to the user
	prtyVector3dEditUpDownUIInfo* pPVUII;
	pPVUII = new prtyVector3dEditUpDownUIInfo(&m_Position, "Transform", "Position");
	pPVUII->SetIncrement(0.1f, 0.1f, 0.1f);
	AddProperty( pPVUII );

	// Register callbacks to update dirty bit when properties change
	m_Position.AddCallback(new prtyCallbackWrapper<splnCurvePointObject>(this, &splnCurvePointObject::PositionChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
splnCurvePointObject::~splnCurvePointObject()
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
//void splnCurvePointObject::GlobalScaleChanged( float i_Scale )
//{
//	if (m_pObject)
//		m_pObject->SetUniformScale( i_Scale );
////	update_world_box();
//}
//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
//virtual 
void splnCurvePointObject::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	if( m_pObject )
	{
		float fScale = i_Scale * icnIconScale::GetIconScaleForPosition(GetPosition(), i_Camera);
		m_pObjectProxy->SetLayerScale( i_IconLayerIndex, maVector3d(fScale,fScale,fScale) );
	}
}

//--------------------------------------------------------------------
//	GetPointIndex - get index of control point it is changing
//--------------------------------------------------------------------
int	splnCurvePointObject::GetPointIndex()
{
	return m_PointIndex;
}

//--------------------------------------------------------------------
//	GetCurve - get curve it is changing
//--------------------------------------------------------------------
splnSpline* splnCurvePointObject::GetCurve()
{
	return m_Curve;
}

//--------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the splnCurvePointObject.
//--------------------------------------------------------------------
maAxisBox splnCurvePointObject::GetWorldBox(int i_IconLayerIndex) const
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
maPoint3d splnCurvePointObject::GetPosition() const
{
	return m_pObjectProxy->GetPosition();
}

//--------------------------------------------------------------------
//	SetPosition sets the position of the object.
//--------------------------------------------------------------------
void splnCurvePointObject::SetPosition(const maPoint3d& i_Position)
{
	m_pObjectProxy->SetPosition(i_Position);
	m_Position.SetValue( i_Position );
}

//--------------------------------------------------------------------
//	UpdatePosition - called when control point is altered from
//	user input.  Need to propagate to spline.
//--------------------------------------------------------------------
void splnCurvePointObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	if (i_Position == m_pObject->GetPosition()) return;

	this->SetPosition( i_Position );

	// hmm, circular dependency created
	splnCurveMgr::AlterCtrlPoint(m_Curve, m_PointIndex, i_Position);

	if (m_Callbacks.size() > 0)
	{
		for (int i = 0; i < m_Callbacks.size(); ++i)
		{
			m_Callbacks[i]->PointChanged(this);
		}
	}
}

//--------------------------------------------------------------------
//	GetOrientation returns the orientation of the object.
//--------------------------------------------------------------------
maRotation splnCurvePointObject::GetOrientation() const
{
	return l_NoRot;
}

//--------------------------------------------------------------------
//	SetOrientation sets the orientation of the object.
//--------------------------------------------------------------------
void splnCurvePointObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	// no need on sphere object?
	//m_pObject->SetPosition(i_Orientation);
}

//--------------------------------------------------------------------
//	Scale
//--------------------------------------------------------------------
maPoint3d splnCurvePointObject::GetScale() const
{
	return m_pObjectProxy->GetScale();
}
void splnCurvePointObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	// no need on sphere object?
	//maPoint3d gScale;
	//gScale.m_X = icnIconScale::Scale( i_Scale.m_X );
	//gScale.m_Y = icnIconScale::Scale( i_Scale.m_Y );
	//gScale.m_Z = icnIconScale::Scale( i_Scale.m_Z );
	//m_pObject->SetScale( gScale );
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool splnCurvePointObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!this->GetRenderable())
		return false;

	return m_pObject->ContainsPickCode(i_PickCode);
}

//--------------------------------------------------------------------
//	Renderable sets whether the mtObject can be selected.
//--------------------------------------------------------------------
void splnCurvePointObject::SetRenderable(bool i_Renderable)
{
	m_pObjectProxy->SetRenderable(i_Renderable);
}
bool splnCurvePointObject::GetRenderable() const
{
	return m_pObjectProxy->GetRenderable();
}

//--------------------------------------------------------------------
//	GetDefaultTerrainOffset is the desired offset from
//	the terrain for this object.  This can be altered
//	by the user during placement
//--------------------------------------------------------------------
float splnCurvePointObject::GetDefaultTerrainOffset() const
{
	return 0.0f;
}


//--------------------------------------------------------------------
// Flags for how object can be modified
//--------------------------------------------------------------------
mnmObject::RotateFlags	splnCurvePointObject::GetRotateFlags()
{
	return mnmObject::e_RotateNone;
}
mnmObject::ScaleFlags	splnCurvePointObject::GetScaleFlags()
{
	return mnmObject::e_ScaleNone;
}
mnmObject::TranslateFlags	splnCurvePointObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//sel3dObject* splnCurvePointObject::GetParentObject() const
//{
//	return m_pParent;
//}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
std::string splnCurvePointObject::GetDisplayName() const
{
	std::ostringstream str;
	str << "Spline Point " << m_PointIndex+1 << " out of " << m_Curve->GetNumPoints();
	return str.str();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void splnCurvePointObject::SetColor( maFloatRGBA& i_Color )
{
	m_pObject->SetColor(i_Color);
}

//--------------------------------------------------------------------
// Set callback for when point changes value.
//--------------------------------------------------------------------
void splnCurvePointObject::AddCallback(PointChangedCallback *i_pCallback)
{
	DBG_ASSERT(i_pCallback != NULL, "Cannot add a NULL callback");
	m_Callbacks.clear();	// temp
	m_Callbacks.push_back(i_pCallback);
}
void splnCurvePointObject::RemoveCallback(PointChangedCallback *i_pCallback)
{
	DBG_ASSERT(i_pCallback != NULL, "Cannot remove a NULL callback");
	std::vector<PointChangedCallback *>::iterator it;
	it = std::find(m_Callbacks.begin(), m_Callbacks.end(), i_pCallback);
	if( it != m_Callbacks.end() )
	{
		m_Callbacks.erase( it );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void splnCurvePointObject::PositionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Use a threshold for changing the 3D icon in order to prevent infinite loops
	if ( (m_Position.GetValue() - m_pObject->GetPosition()).LengthSqr() > maConstants::c_fEpsilon)
	{
		// Alter 3D point icon's position
		maPoint3d pos = m_Position.GetValue();
		this->SetPosition( pos );

		splnCurveMgr::AlterCtrlPoint(m_Curve, m_PointIndex, pos);

		if (m_Callbacks.size() > 0)
		{
			for (int i = 0; i < m_Callbacks.size(); ++i)
			{
				m_Callbacks[i]->PointChanged(this);
			}
		}
	}
}

