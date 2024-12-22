/*****************************************************************************
**  ptltPointLightObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltPointLightObject.hpp"

#include "Systems/PtLt/Data/ptltDocumentChunk.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"
#include "Systems/PtLt/Object/ptltRangeIcon.hpp"

#include "Support/mnm/mnmPickMask.hpp"
#include "Support/mnm/mnmReference.hpp"
#include "Support/ltst/ltstIsolateMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

#include "Core/geo/geoRayIntersection.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxPointLight.hpp"
#include "Tool/gpx/gpxIconSet.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"

namespace
{
	//============================================================================
	//============================================================================
	// FIX: this is temporary.  need to set this box
	const float l_PickRadius = 1.25f;	// pick larger than icon
	const float l_SphereRadius = 0.6f;
	const maAxisBox l_SphereBox(-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;

} // end of namespace

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltPointLightObject::ptltPointLightObject()
:	m_pObject(NULL),
	m_pPointLight(NULL),
	m_pParent(NULL),
	//m_WorldBox(l_SphereBox),
    m_bRenderable(true),
	m_bLayerVisible(true),
	m_bLayerPickable(true)
{
	// Create g3d light
	m_pPointLight = api3dLightMgr::CreatePointLight();
	m_pPointLight->SetCastsShadow(true);
	// ensure current defaults are set in light.
	m_pPointLight->SetIntensity(m_Data.m_Color.GetValue());
	m_pPointLight->SetEnable(m_Data.m_Enabled.GetValue());
	m_pPointLight->SetPosition( m_Data.m_Position.GetValue() );
	m_pPointLight->SetRange(m_Data.m_Range.GetValue());
	m_pPointLight->SetIntensityFactor(m_Data.m_Intensity.GetValue());
	m_pPointLight->SetFalloff0(m_Data.m_Falloff.GetValue()[0]);
	m_pPointLight->SetFalloff1(m_Data.m_Falloff.GetValue()[1]);
	m_pPointLight->SetFalloff2(m_Data.m_Falloff.GetValue()[2]);
	m_pPointLight->SetDiffuseEnabled( m_Data.m_bDiffuseEnabled.GetValue() );
	m_pPointLight->SetSpecularEnabled( m_Data.m_bSpecularEnabled.GetValue() );
	m_pPointLight->SetAffectsGlow(m_Data.m_bAffectsGlow.GetValue());

	// After all of the values have been set into the point light, 
	// create the proxy for this light. All changes to the light that occur
	// in property callbacks in the gui thread should go through the proxy instead.
	m_pLightProxy = new gpxPointLight(*m_pPointLight);

	// Create 3D icon
	api3dObjectSimple *pObjectBase = api3dShape::CreateSphere(m_Data.m_Color.GetValue(), l_SphereRadius, 8, 8);
	pObjectBase->SetPickMask(mnmPickMask::c_Light);
	m_pObject = icnIconLayer::CreateIconSet(pObjectBase);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	// create thread-safe proxy for the icon
	m_pObjectProxy = new gpxIconSet(*m_pObject);

	m_pRangeIcon = new ptltRangeIcon();

	ltstLightSetMgr::AddLight(this, m_pPointLight);
	ltstIsolateMgr::AddLight(this, this);
	// give our name to transform manager with a callback for notifying
	// when the parent transformation changes.
	xfrmTransformMgr::AddObject(this, this); 

	icnIconScale::RegisterScaleInterest(this);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_Enabled), "Asset", "Is the object enabled") );

	// Falloff group
	cmmFalloffAspect::RegisterProperties( this, &(m_Data.m_Falloff));

	pPUII = new prtyColorRGBEditUIInfo(&(m_Data.m_Color), "Light", "Color of the object");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Intensity), "Light", "Brightness multiplier (>1 for HDR only)");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	//pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_Range), "Light", "Range of the object");
	//AddProperty( pPUII );

	// Took out the light range interface. might need it back if we reintroduce the light range cut off
	//pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Range), "Light", "Range of the object");
	//pRFUII->SetMinimum(0.0f); //0.1f);
	//pRFUII->SetMaximum(3000.0f);
	//pRFUII->SetDecimalPlaces(2);
	//pRFUII->SetNumTicks(100);
	//pRFUII->SetExponent(3);
	//AddProperty( pRFUII );
	

	// Editor visible is set from other part of GUI
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
//	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_ShadowSource), "Light", "Is the object a shadow source") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDiffuseEnabled), "Light", "Is the diffuse enabled") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bSpecularEnabled), "Light", "Is the specular enabled") );
//	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsFur), "Light", "Does this light affect fur") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsGlow), "Light", "Does this light affect glow") );

	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::NameChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::TransformChanged));
	m_Data.m_Range.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Enabled.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
//	m_Data.m_ShadowSource.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_bDiffuseEnabled.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_bSpecularEnabled.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
//	m_Data.m_bAffectsFur.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_bAffectsGlow.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Intensity.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Falloff.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::FalloffChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltPointLightObject::~ptltPointLightObject()
{
	ltstLightSetMgr::RemoveLight(this, m_pPointLight);
	ltstIsolateMgr::RemoveLight(this, this);
	xfrmTransformMgr::RemoveObject(this); 

	icnIconScale::UnRegisterScaleInterest(this);

	delete m_pRangeIcon;

	delete m_pObjectProxy;	// delete proxy
	//api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;

	delete m_pLightProxy; // delete proxy
	api3dLightMgr::DestroyLight(m_pPointLight);
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string ptltPointLightObject::GetDisplayName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
//void ptltPointLightObject::GlobalScaleChanged( float i_Scale )
//{
//	m_pObjectProxy->SetUniformScale( i_Scale );
//	m_WorldBox = m_pObjectProxy->GetWorldBox();
//}

//--------------------------------------------------------------------
//	UpdateIconScale - function that sets the icons scale.
//--------------------------------------------------------------------
//virtual 
void ptltPointLightObject::UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale )
{
	if ( m_pObjectProxy )
	{
		float fScale = i_Scale * icnIconScale::GetIconScaleForPosition(GetPosition(), i_Camera);
		m_pObjectProxy->SetLayerScale( i_IconLayerIndex, maVector3d(fScale,fScale,fScale) );
		//m_WorldBox = m_pObject->GetWorldBox();
	}
}


//--------------------------------------------------------------------
// Get values as a light  data structure
//--------------------------------------------------------------------
const ptltData& ptltPointLightObject::GetData() const
{
	return m_Data;
}

//--------------------------------------------------------------------
// Set from light  data structure
//--------------------------------------------------------------------
void ptltPointLightObject::SetData(const ptltData &i_Data)
{
	m_Data = i_Data;

	cmmFalloffAspect::InitializeFalloffType();
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	ptltPointLightObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	ptltPointLightObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	ptltPointLightObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	ptltPointLightObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Color property access
//--------------------------------------------------------------------
prtyColor&	ptltPointLightObject::PropertyColor()
{
	return m_Data.m_Color;
}
const prtyColor&	ptltPointLightObject::GetPropertyColor() const
{
	return m_Data.m_Color;
}

//--------------------------------------------------------------------
// Enabled property access
//--------------------------------------------------------------------
prtyBoolean&	ptltPointLightObject::PropertyEnabled()
{
	return m_Data.m_Enabled;
}
const prtyBoolean&	ptltPointLightObject::GetPropertyEnabled() const
{
	return m_Data.m_Enabled;
}

//--------------------------------------------------------------------
// Range property access
//--------------------------------------------------------------------
prtyFloat&	ptltPointLightObject::PropertyRange()
{
	return m_Data.m_Range;
}
const prtyFloat&	ptltPointLightObject::GetPropertyRange() const
{
	return m_Data.m_Range;
}

//--------------------------------------------------------------------
// Intensity property access
//--------------------------------------------------------------------
prtyFloat&	ptltPointLightObject::PropertyIntensity()
{
	return m_Data.m_Intensity;
}
const prtyFloat&	ptltPointLightObject::GetPropertyIntensity() const
{
	return m_Data.m_Intensity;
}

//--------------------------------------------------------------------
// ShadowSource property access
//--------------------------------------------------------------------
prtyBoolean&	ptltPointLightObject::PropertyShadowSource()
{
	return m_Data.m_ShadowSource;
}
const prtyBoolean&	ptltPointLightObject::GetPropertyShadowSource() const
{
	return m_Data.m_ShadowSource;
}

//--------------------------------------------------------------------
// EnableDiffuse property access
//--------------------------------------------------------------------
prtyBoolean&	ptltPointLightObject::PropertyEnableDiffuse()
{
	return m_Data.m_bDiffuseEnabled;
}
const prtyBoolean&	ptltPointLightObject::GetPropertyEnableDiffuse() const
{
	return m_Data.m_bDiffuseEnabled;
}

//--------------------------------------------------------------------
// EnableSpecular property access
//--------------------------------------------------------------------
prtyBoolean&	ptltPointLightObject::PropertyEnableSpecular()
{
	return m_Data.m_bSpecularEnabled;
}
const prtyBoolean&	ptltPointLightObject::GetPropertyEnableSpecular() const
{
	return m_Data.m_bSpecularEnabled;
}

//--------------------------------------------------------------------
// AffectsGlow property access
//--------------------------------------------------------------------
prtyBoolean&	ptltPointLightObject::PropertyAffectsGlow()
{
	return m_Data.m_bAffectsGlow;
}
const prtyBoolean&	ptltPointLightObject::GetPropertyAffectsGlow() const
{
	return m_Data.m_bAffectsGlow;
}

//--------------------------------------------------------------------
// Falloff property access
//--------------------------------------------------------------------
prtyPoint3d&	ptltPointLightObject::PropertyFalloff()
{
	return m_Data.m_Falloff;
}
const prtyPoint3d&	ptltPointLightObject::GetPropertyFalloff() const
{
	return m_Data.m_Falloff;
}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the ptltPointLightObject.
//----------------------------------------------------------------------------
//virtual
maAxisBox ptltPointLightObject::GetWorldBox(int i_IconLayerIndex) const
{
	//return m_WorldBox;

	// Since the point light icon scales with the camera view,
	// there is no one definite world box. Have to compute it from 
	// the size of the icon in a given layer.
	float icon_radius = l_SphereRadius * m_pObjectProxy->GetLayerScale(i_IconLayerIndex).GetX();
	maAxisBox icon_box(-icon_radius, icon_radius,
						-icon_radius, icon_radius,
						-icon_radius, icon_radius);
	//icon_box.Translate( m_Data.m_Position.GetValue() );
	icon_box.Translate( this->get_world_position() ); // put world box around the world position 
	return icon_box;
}

//--------------------------------------------------------------------
//	GetWorldPivot returns the point in world space that this
//		object will rotate around. Used to center rotation and
//		scale compasses.
//--------------------------------------------------------------------
//virtual 
maPoint3d ptltPointLightObject::GetWorldPivot() const
{	
	// pivot is world position for point lights
	return get_world_position();
}


//--------------------------------------------------------------------
//  Get sum of matrices of all parents of this node. 
//--------------------------------------------------------------------
void ptltPointLightObject::GetParentMatrix(maMatrix4x4 &o_Transformation) const
{
	// let transform manager compute the sum of the parent matrices
	xfrmTransformMgr::ComputeExclusiveMatrixForNode(this->GetName(), o_Transformation);
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
void ptltPointLightObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());

	// Update LightSetMgr
	ltstLightSetMgr::LightRenamed(this);
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
maPoint3d ptltPointLightObject::GetPosition() const
{
	return m_Data.m_Position.GetValue();
}

//----------------------------------------------------------------------------
//	UpdatePosition - compass interaction has altered the position of
//	of this light
//----------------------------------------------------------------------------
void ptltPointLightObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	if (i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_Position);

	const bool bSetDirty = true;
	m_Data.m_Position.SetValue(i_Position, bSetDirty);
}


//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
maRotation ptltPointLightObject::GetOrientation() const
{
	return l_NoRot;
}
void ptltPointLightObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	// do nothing
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
maPoint3d ptltPointLightObject::GetScale() const
{
	return l_One;

}
void ptltPointLightObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	// do nothing
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool ptltPointLightObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_pObject->GetRenderable())
		return false;

	return (m_pObject->ContainsPickCode(i_PickCode));
}

//----------------------------------------------------------------------------
//	Renderable sets whether the icon is visible
//----------------------------------------------------------------------------
void ptltPointLightObject::SetRenderable(bool i_bRenderable)
{
	m_bRenderable = i_bRenderable;

	// icon is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable
		&& m_bLayerVisible
		&& this->GetIsolationVisible();
	m_pObjectProxy->SetRenderable( bRenderable );
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void ptltPointLightObject::SetActiveRenderLayer(bool i_bActive)
{
}

//--------------------------------------------------------------------
//  Changes visible state of light based on GUI
//--------------------------------------------------------------------
void  ptltPointLightObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible = i_bVisible;

	// icon is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable
		&& m_bLayerVisible;
	m_pObjectProxy->SetRenderable( bRenderable );
}
bool ptltPointLightObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}
 
//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void ptltPointLightObject::SetLayerVisible(bool i_bVisible)
{
	m_bLayerVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_bRenderable
		&& m_bLayerVisible;
	m_pObjectProxy->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the icons should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void ptltPointLightObject::SetGPUPickable(bool i_Pickable)
{
	m_bLayerPickable = i_Pickable;
	bool bParentPickable = xfrmTransformMgr::GetParentPickable(this->GetName());
	bool bPickable = (m_bLayerPickable && bParentPickable);
	if (bPickable != m_pObjectProxy->GetGPUPickable())
		m_pObjectProxy->SetGPUPickable(bPickable);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
ptltPointLightObject::RotateFlags	ptltPointLightObject::GetRotateFlags()
{
	return ptltPointLightObject::e_RotateNone;
}
ptltPointLightObject::ScaleFlags	ptltPointLightObject::GetScaleFlags()
{
	return ptltPointLightObject::e_ScaleNone;
}
ptltPointLightObject::TranslateFlags	ptltPointLightObject::GetTranslateFlags()
{
	return ptltPointLightObject::e_TranslateAll;
}

//--------------------------------------------------------------------
// For polling if an manipulation operation is currently enabled
//--------------------------------------------------------------------
//virtual 
bool ptltPointLightObject::IsOperationEnabled(Operations i_Operation, const maTime& i_Time)
{
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dPointLight* ptltPointLightObject::GetLight()
{
	return m_pPointLight;
}

//--------------------------------------------------------------------
// Return this light's range icon
//--------------------------------------------------------------------
ptltRangeIcon* ptltPointLightObject::GetRangeIcon()
{
	return m_pRangeIcon;
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
//sel3dObject* ptltPointLightObject::GetParentObject() const
//{
//	return m_pParent;
//}
//void ptltPointLightObject::SetParentObject(sel3dObject* i_pPO)
//{
//	m_pParent = i_pPO;
//}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void ptltPointLightObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		ptltDialogDataUtil::UpdateListDialog();
		ptltDocumentChunk::ActiveDataChanged();
	}
}
void ptltPointLightObject::TransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This callback is triggered from position changes
	//const maPoint3d& position = m_Data.m_Position.GetValue();
	maPoint3d position = get_world_position();
	m_pLightProxy->SetPosition( position );
	m_pObjectProxy->SetPosition( position );

	//m_WorldBox = m_pObjectProxy->GetWorldBox();
	//m_WorldBox = l_SphereBox;
	//m_WorldBox.Translate( position );

	UpdateFalloffIcon();

	if (i_bDirty)
		ptltDocumentChunk::ActiveDataChanged();
}
void ptltPointLightObject::LightChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// This callback is triggered when attributes of the light besides position 
	// are changed. The changes are passed on to the light
	//
	m_pLightProxy->SetIntensity(m_Data.m_Color.GetValue());
	m_pLightProxy->SetIntensityFactor(m_Data.m_Intensity.GetValue() * this->GetIsolationAlpha());

//	m_pLightProxy->SetCastsShadow(m_Data.m_ShadowSource.GetValue());
	m_pLightProxy->SetRange(m_Data.m_Range.GetValue());
	m_pLightProxy->SetEnable( m_Data.m_Enabled.GetValue() && this->GetIsolationVisible() );
	m_pLightProxy->SetDiffuseEnabled( m_Data.m_bDiffuseEnabled.GetValue() );
	m_pLightProxy->SetSpecularEnabled( m_Data.m_bSpecularEnabled.GetValue() );
	m_pLightProxy->SetAffectsGlow( m_Data.m_bAffectsGlow.GetValue() );

	m_pObjectProxy->SetColor(m_Data.m_Color.GetValue());

	if (i_bDirty)
		ptltDocumentChunk::ActiveDataChanged();
}
void ptltPointLightObject::FalloffChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// This callback is triggered when the 3-value D3D falloff property is changed.
	// Note: It is also called when the falloff percentage changes because this
	// causes a change to the falloff range computation.

	// Set the light's values
	maVector3d falloff = m_Data.m_Falloff.GetValue();
	m_pLightProxy->SetFalloff0(falloff[0]);
	m_pLightProxy->SetFalloff1(falloff[1]);
	m_pLightProxy->SetFalloff2(falloff[2]);

	if (i_bDirty)
		ptltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	IsolationChanged - notification that this object's isolation
//	state has changed (fading on or out the light's intensity)
//--------------------------------------------------------------------
void ptltPointLightObject::IsolationChanged()
{
	// Combine intensity with the isolation alpha for fades in and out
	m_pLightProxy->SetIntensityFactor(m_Data.m_Intensity.GetValue() * this->GetIsolationAlpha());

	// Combine the isoalted and enabled states to determine if the light is "on"
	m_pLightProxy->SetEnable( m_Data.m_Enabled.GetValue() && this->GetIsolationVisible() );
	
	// Redo the renderable calculation that uses GetIsolationVisible()
	// by just calling it with the current value
	this->SetRenderable(m_bRenderable);
}

//--------------------------------------------------------------------
// Called from transformation manager once per frame, update light
//	position based on parent transformation matrix.
//--------------------------------------------------------------------
void ptltPointLightObject::UpdateParentTransform()
{
	// This function is called once per frame in order to avoid
	// multiple updates everytime any property of any parent transformation
	// node changes. Update light and icons based on parent transformation.

	// Give the properties the transformation matrix and let them compute
	// the object and world space as needed.
	maMatrix4x4 parent_matrix;
	this->GetParentMatrix(parent_matrix);
	m_Data.m_Position.SetTransformation(parent_matrix);

	maPoint3d position = get_world_position();
	if ((m_pLightProxy->GetPosition() - position).LengthSqr() > maConstants::c_fEpsilon)
	{
		m_pLightProxy->SetPosition( position );
		m_pObjectProxy->SetPosition( position );
		UpdateFalloffIcon();
	}

	// Also update pickable flag here, once per frame,
	// because it is inherited from the parent transforms
	bool bParentPickable = xfrmTransformMgr::GetParentPickable(this->GetName());
	bool bPickable = (m_bLayerPickable && bParentPickable);
	if (bPickable != m_pObjectProxy->GetGPUPickable())
		m_pObjectProxy->SetGPUPickable(bPickable);
}

//--------------------------------------------------------------------
// Called from transformation manager when the parenting of this 
// object changes in a way that we need to alter our values to
// saty in the same world position.
//--------------------------------------------------------------------
void ptltPointLightObject::ApplyTransformation(const maMatrix4x4& i_Matrix)
{
	maPoint3d position = m_Data.m_Position.GetValue();
	i_Matrix.Transform(position);
	m_Data.m_Position.SetValue(position);
}

//--------------------------------------------------------------------
// Return true if this object has a pivot point based on its icon.
// If so, return pivot point in the o_Pivot argument.
//--------------------------------------------------------------------
bool ptltPointLightObject::HasIconPivotPoint(maPoint3d& o_Pivot) 
{ 
	o_Pivot = get_world_position();
	return true; 
}

//--------------------------------------------------------------------
// Update icon that displays falloff range
//--------------------------------------------------------------------
void ptltPointLightObject::UpdateFalloffIcon()
{
	m_pRangeIcon->SetRenderable(this->ShouldShowFalloffIcon());

	m_pRangeIcon->Update(get_world_position(), m_FalloffRange.GetValue(), m_FalloffPercent.GetValue());
}

//--------------------------------------------------------------------
// compute world position of light using parent transformations
//--------------------------------------------------------------------
maPoint3d ptltPointLightObject::get_world_position() const
{
	// New method incorporates transformation matrix into property and lets
	// the property compute and cache both world and object space
	return m_Data.m_Position.GetWorldSpaceValue();
	
	// Old method was to transform to word space here
	//maPoint3d pos(this->GetPosition());
	//// transform to world space
	//maMatrix4x4 parent_matrix;
	//this->GetParentMatrix(parent_matrix);
	//parent_matrix.Transform(pos);
	//return pos;

}
