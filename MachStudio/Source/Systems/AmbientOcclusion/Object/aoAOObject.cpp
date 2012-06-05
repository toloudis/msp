/*****************************************************************************
**  aoAOObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Object/aoAOObject.hpp"
#include "Systems/AmbientOcclusion/Data/aoDocumentChunk.hpp"

// library
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxAmbientOcclusion.hpp"

namespace
{
}

//----------------------------------------------------------------------------
// This object takes ownership of the arguments passed in
//----------------------------------------------------------------------------
aoAOObject::aoAOObject(	)
{
	// Create thread-safe proxy for the scenes ambient occlusion settings
	m_pAOProxy = new gpxAmbientOcclusion();

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Ambient Occlusion", "Color tint of occluded areas");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AORadius), "Ambient Occlusion", "Occlusion Radius (Near Plane)");
	pRFUII->SetMinimum(0.00001f);
	pRFUII->SetMaximum(30.0f);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AORadiusFar), "Ambient Occlusion", "Occlusion Radius (Far Plane)");
	pRFUII->SetMinimum(0.00001f);
	pRFUII->SetMaximum(30.0f);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AngleBias), "Ambient Occlusion", "horizon angle bias");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(90.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Contrast), "Ambient Occlusion", "contrast");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(4.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Attenuation), "Ambient Occlusion", "distance attenuation");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(100.0);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetRestrictFlag(true);
	AddProperty(pRFUII);

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BlurWidth), "Ambient Occlusion", "blur radius");
	pRFUII->SetMinimum(0.1000f);
	pRFUII->SetMaximum(32.0);
	pRFUII->SetDecimalPlaces(3);
	pRFUII->SetRestrictFlag(false);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BlurSharpness), "Ambient Occlusion", "blur sharpness for edge detection");
	pRFUII->SetMinimum(0.0);
	pRFUII->SetMaximum(4.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
/*
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_OverscanPixels), "Ambient Occlusion", "Widens the depth buffer to reduce border artifacts.");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1000.0f);
	pRFUII->SetDecimalPlaces(1);
	AddProperty(pRFUII);
*/

	prtyNumericUpDownUIInfo* pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_OverscanPixels), "Ambient Occlusion", "Widens the depth buffer to reduce border artifacts.");
	pNUDUII->SetMinimum(0.0f);
	pNUDUII->SetMaximum(1000.0f);
	pNUDUII->SetDecimalPlaces(0);
	pNUDUII->SetIncrement( 1.0f );
	AddProperty(pNUDUII);

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_ClipPlaneEpsilon), "AO Volumes", "horizon angle bias");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(1);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_NoClipPlaneEpsilon), "AO Volumes", "horizon angle bias");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(1);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AreaRatio), "AO Volumes", "horizon angle bias");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(1);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BehindPlaneEpsilon), "AO Volumes", "horizon angle bias");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(0.1f);
	pRFUII->SetDecimalPlaces(5);
	AddProperty(pRFUII);

	m_Data.m_AORadius.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_AORadiusFar.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_AngleBias.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_Contrast.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_Attenuation.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_BlurWidth.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_BlurSharpness.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_OverscanPixels.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_ClipPlaneEpsilon.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_NoClipPlaneEpsilon.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_AreaRatio.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_BehindPlaneEpsilon.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoAOObject::~aoAOObject()
{
	// delete proxy
	delete m_pAOProxy;
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string aoAOObject::GetDisplayName() const
{
	return "Ambient Occlusion";
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//sel3dObject* aoAOObject::GetParentObject() const
//{
//	return m_pParent;
//}
//void aoAOObject::SetParentObject(sel3dObject* i_pPO)
//{
//	m_pParent = i_pPO;
//}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const aoAOData& aoAOObject::GetData() const
{
	return this->m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void aoAOObject::SetData(const aoAOData &i_Data)
{
	m_Data = i_Data;
}


//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void aoAOObject::UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty)
{
	ssaoParams p;
	p.m_AORadius = m_Data.m_AORadius.GetValue();
	p.m_AORadiusFar = m_Data.m_AORadiusFar.GetValue();
	p.m_AngleBias = m_Data.m_AngleBias.GetValue();
	p.m_Contrast = m_Data.m_Contrast.GetValue();
	p.m_Attenuation = m_Data.m_Attenuation.GetValue();
	p.m_BlurWidth = m_Data.m_BlurWidth.GetValue();
	p.m_BlurSharpness = m_Data.m_BlurSharpness.GetValue();
	p.m_OverscanPixels = m_Data.m_OverscanPixels.GetValue();
	p.m_ClipPlaneEpsilon = m_Data.m_ClipPlaneEpsilon.GetValue();
	p.m_NoClipPlaneEpsilon = m_Data.m_NoClipPlaneEpsilon.GetValue();
	p.m_AreaRatio = m_Data.m_AreaRatio.GetValue();
	p.m_BehindPlaneEpsilon = m_Data.m_BehindPlaneEpsilon.GetValue();

	p.m_Color = m_Data.m_Color.GetValue();

	// Use proxy to set SSAO settings in scene 
	m_pAOProxy->SetSSAO(p);
	//api3dScene::SetSSAO(p);
	aoDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// SetName
//--------------------------------------------------------------------
//void aoAOObject::SetName(const nameString& i_Name)
//{
//    // Set name first, which may change the ID number
//    nameObject::SetName(i_Name);
//
//    // Make sure that the data matches our true name (including
//    // ID number)
//    m_Data.m_Name.SetValue(this->GetName());
//}

//--------------------------------------------------------------------
// Radius property access
//--------------------------------------------------------------------
prtyFloat&	aoAOObject::PropertyRadius()
{
	return m_Data.m_AORadius;
}
const prtyFloat&	aoAOObject::GetPropertyRadius() const
{
	return m_Data.m_AORadius;
}

//--------------------------------------------------------------------
// RadiusFar property access
//--------------------------------------------------------------------
prtyFloat&	aoAOObject::PropertyRadiusFar()
{
	return m_Data.m_AORadiusFar;
}
const prtyFloat&	aoAOObject::GetPropertyRadiusFar() const
{
	return m_Data.m_AORadiusFar;
}

//--------------------------------------------------------------------
// AngleBias property access
//--------------------------------------------------------------------
prtyFloat&	aoAOObject::PropertyAngleBias()
{
	return m_Data.m_AngleBias;
}
const prtyFloat&	aoAOObject::GetPropertyAngleBias() const
{
	return m_Data.m_AngleBias;
}

//--------------------------------------------------------------------
// Attenuation property access
//--------------------------------------------------------------------
prtyFloat&	aoAOObject::PropertyAttenuation()
{
	return m_Data.m_Attenuation;
}
const prtyFloat&	aoAOObject::GetPropertyAttenuation() const
{
	return m_Data.m_Attenuation;
}

//--------------------------------------------------------------------
// Contrast property access
//--------------------------------------------------------------------
prtyFloat&	aoAOObject::PropertyContrast()
{
	return m_Data.m_Contrast;
}
const prtyFloat&	aoAOObject::GetPropertyContrast() const
{
	return m_Data.m_Contrast;
}

//--------------------------------------------------------------------
// BlurWidth property access
//--------------------------------------------------------------------
prtyFloat&	aoAOObject::PropertyBlurWidth()
{
	return m_Data.m_BlurWidth;
}
const prtyFloat&	aoAOObject::GetPropertyBlurWidth() const
{
	return m_Data.m_BlurWidth;
}

//--------------------------------------------------------------------
// BlurSharpness property access
//--------------------------------------------------------------------
prtyFloat&	aoAOObject::PropertyBlurSharpness()
{
	return m_Data.m_BlurSharpness;
}
const prtyFloat&	aoAOObject::GetPropertyBlurSharpness() const
{
	return m_Data.m_BlurSharpness;
}

//--------------------------------------------------------------------
// Overscan Pixels property access
//--------------------------------------------------------------------
prtyInt32& aoAOObject::PropertyOverscanPixels()
{
	return m_Data.m_OverscanPixels;
}
const prtyInt32& aoAOObject::GetPropertyOverscanPixels() const
{
	return m_Data.m_OverscanPixels;
}

//--------------------------------------------------------------------
// Color property access
//--------------------------------------------------------------------
prtyColor&	aoAOObject::PropertyColor()
{
	return m_Data.m_Color;
}
const prtyColor&	aoAOObject::GetPropertyColor() const
{
	return m_Data.m_Color;
}

//--------------------------------------------------------------------
// ClipPlaneEpsilon property access
//--------------------------------------------------------------------
prtyDistance& aoAOObject::PropertyClipPlaneEpsilon()
{
	return m_Data.m_ClipPlaneEpsilon;
}
const prtyDistance& aoAOObject::GetPropertyClipPlaneEpsilon() const
{
	return m_Data.m_ClipPlaneEpsilon;
}

//--------------------------------------------------------------------
// NoClipPlaneEpsilon property access
//--------------------------------------------------------------------
prtyDistance& aoAOObject::PropertyNoClipPlaneEpsilon()
{
	return m_Data.m_NoClipPlaneEpsilon;
}
const prtyDistance& aoAOObject::GetPropertyNoClipPlaneEpsilon() const
{
	return m_Data.m_NoClipPlaneEpsilon;
}

//--------------------------------------------------------------------
// AreaRatio property access
//--------------------------------------------------------------------
prtyFloat& aoAOObject::PropertyAreaRatio()
{
	return m_Data.m_AreaRatio;
}
const prtyFloat& aoAOObject::GetPropertyAreaRatio() const
{
	return m_Data.m_AreaRatio;
}

//--------------------------------------------------------------------
// BehindPlaneEpsilon property access
//--------------------------------------------------------------------
prtyDistance& aoAOObject::PropertyBehindPlaneEpsilon()
{
	return m_Data.m_BehindPlaneEpsilon;
}
const prtyDistance& aoAOObject::GetPropertyBehindPlaneEpsilon() const
{
	return m_Data.m_BehindPlaneEpsilon;
}
