/*****************************************************************************
**  ptltPointLightObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltPointLightObject.hpp"

#include "Systems/PtLt/GUI/ptltDialogDataUtil.hpp"
#include "Systems/PtLt/Data/ptltDocumentChunk.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"
#include "Systems/PtLt/Object/ptltRangeIcon.hpp"

#include "Support/cmps/cmpsReference.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"

#include "MainApp/mnmApp.hpp"

#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"

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
	m_pLight(NULL),
	m_pParent(NULL),
	m_WorldBox(l_SphereBox),
    m_bRenderable(true),
	m_bLayerVisible(true)
{
	// Create g3d light
	m_pLight = api3dLightMgr::CreatePointLight();

	// Create 3D icon
	m_pObject = api3dShape::CreateSphere(m_Data.m_Color.GetValue(), l_SphereRadius, 8, 8);
	m_pObject->SetUniformScale( api3dScale::GetGlobalScale() );
	api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	m_pRangeIcon = new ptltRangeIcon();

	ltstLightSetMgr::AddLight(this, m_pLight);

	api3dScale::RegisterScaleInterest(this);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_Range), "Light", "Range of the object");
	AddProperty( pPUII );
	pPUII = new prtyColorRGBEditUIInfo(&(m_Data.m_Color), "Light", "Color of the object");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Intensity), "Light", "Brightness multiplier (>1 for HDR only)");
	pRFUII->SetMinimum(1.0f);
	pRFUII->SetMaximum(500.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	// Falloff group
	cmmFalloffAspect::RegisterProperties( this, &(m_Data.m_Falloff));

	// Editor visible is set from other part of GUI
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_Enabled), "Asset", "Is the object enabled") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_ShadowSource), "Light", "Is the object a shadow source") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bDiffuseEnabled), "Light", "Is the diffuse enabled") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bSpecularEnabled), "Light", "Is the specular enabled") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsFur), "Light", "Does this light affect fur") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAffectsGlow), "Light", "Does this light affect glow") );

	// Register callbacks to update particle generator and icons
	// when properties change
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::NameChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::TransformChanged));
	m_Data.m_Range.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Enabled.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_ShadowSource.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_bDiffuseEnabled.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_bSpecularEnabled.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_bAffectsFur.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_bAffectsGlow.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Intensity.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::LightChanged));
	m_Data.m_Falloff.AddCallback(new prtyCallbackWrapper<ptltPointLightObject>(this, &ptltPointLightObject::FalloffChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltPointLightObject::~ptltPointLightObject()
{
	ltstLightSetMgr::RemoveLight(this, m_pLight);

	api3dScale::UnRegisterScaleInterest(this);

	delete m_pRangeIcon;

	api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;

	api3dLightMgr::DestroyLight(m_pLight);
}


//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string ptltPointLightObject::GetPick3dName() const
{
	return GetName().GetString();
}

//--------------------------------------------------------------------
//	GlobalScaleChanged - notification that global scale has changed.
//--------------------------------------------------------------------
//virtual 
void ptltPointLightObject::GlobalScaleChanged( float i_Scale )
{
	m_pObject->SetUniformScale( i_Scale );
	m_WorldBox = m_pObject->GetWorldBox();
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
// EnableFur property access
//--------------------------------------------------------------------
prtyBoolean&	ptltPointLightObject::PropertyEnableFur()
{
	return m_Data.m_bAffectsFur;
}
const prtyBoolean&	ptltPointLightObject::GetPropertyEnableFur() const
{
	return m_Data.m_bAffectsFur;
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
const maAxisBox& ptltPointLightObject::GetWorldBox() const
{
	return m_WorldBox;
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the ptltPointLightObject
//	if its transformations were identity
//----------------------------------------------------------------------------
const maAxisBox& ptltPointLightObject::GetLocalBox() const
{
	return l_SphereBox;
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
	//ptltOperations::ChangePosition( i_Position );
	m_Data.m_Position.SetValue(i_Position, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
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
//	RayPick returns true if the given ray intersects the ptltPointLightObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool ptltPointLightObject::RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T)
{
	if (!m_pObject->GetRenderable())
		return false;

	return geoRayIntersection::IntersectLineSphere(	i_RayStart,
													i_RayEnd - i_RayStart,
													m_Data.m_Position.GetValue(),
													api3dScale::Scale(l_PickRadius),
													o_T);
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
		&& m_bLayerVisible;
	m_pObject->SetRenderable( bRenderable );
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
	m_pObject->SetRenderable( bRenderable );
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
	m_pObject->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//	GPUPickable sets whether the icons should be rendered
//	in pick renders when doing GPU picking
//--------------------------------------------------------------------
void ptltPointLightObject::SetGPUPickable(bool i_Pickable)
{
	m_pObject->SetGPUPickable(i_Pickable);
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
bool ptltPointLightObject::IsOperationEnabled(Operations i_Operation, float i_Time)
{
	return true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dPointLight* ptltPointLightObject::GetLight()
{
	return m_pLight;
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
pick3dPickObject* ptltPointLightObject::GetParentObject() const
{
	return m_pParent;
}
void ptltPointLightObject::SetParentObject(pick3dPickObject* i_pPO)
{
	m_pParent = i_pPO;
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* ptltPointLightObject::GetReference(const char* i_Name)
{
	api3dReference* pRef = new cmpsReference(*this);
	m_References.push_back(pRef);
	return pRef;
}

//--------------------------------------------------------------------
// Get list of references for possible attachment within this object.
//--------------------------------------------------------------------
void ptltPointLightObject::GetReferenceList(std::vector<std::string> &o_List)
{
	m_pObject->GetReferenceList(o_List);
}

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
	const maPoint3d& position = m_Data.m_Position.GetValue();
	m_pLight->SetPosition( position );
	m_pObject->SetPosition( position );

	m_WorldBox = m_pObject->GetWorldBox();
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
	m_pLight->SetIntensity(m_Data.m_Color.GetValue());
	m_pLight->SetIntensityFactor(m_Data.m_Intensity.GetValue());

	m_pLight->SetCastsShadow(m_Data.m_ShadowSource.GetValue());
	m_pLight->SetRange(m_Data.m_Range.GetValue());
	m_pLight->SetEnable( m_Data.m_Enabled.GetValue() );
	m_pLight->SetDiffuseEnabled( m_Data.m_bDiffuseEnabled.GetValue() );
	m_pLight->SetSpecularEnabled( m_Data.m_bSpecularEnabled.GetValue() );
	m_pLight->SetAffectsFur( m_Data.m_bAffectsFur.GetValue() );
	m_pLight->SetAffectsGlow( m_Data.m_bAffectsGlow.GetValue() );

	m_pObject->SetColor(m_Data.m_Color.GetValue());

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
	m_pLight->SetFalloff0(falloff[0]);
	m_pLight->SetFalloff1(falloff[1]);
	m_pLight->SetFalloff2(falloff[2]);

	if (i_bDirty)
		ptltDocumentChunk::ActiveDataChanged();
}
//--------------------------------------------------------------------
// Update icon that displays falloff range
//--------------------------------------------------------------------
void ptltPointLightObject::UpdateFalloffIcon()
{
	m_pRangeIcon->SetRenderable(this->ShouldShowFalloffIcon());

	m_pRangeIcon->Update(m_Data.m_Position.GetValue(), m_FalloffRange.GetValue(), m_FalloffPercent.GetValue());
}
