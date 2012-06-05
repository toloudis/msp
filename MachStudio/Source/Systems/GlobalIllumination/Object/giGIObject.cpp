/*****************************************************************************
**  giGIObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/Object/giGIObject.hpp"
#include "Systems/GlobalIllumination/Data/giDocumentChunk.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

// library
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxGlobalIllumination.hpp"

namespace
{
}

//----------------------------------------------------------------------------
// This object takes ownership of the arguments passed in
//----------------------------------------------------------------------------
giGIObject::giGIObject(	)
{
	// Create thread-safe proxy for the scenes ambient occlusion settings
	m_pGIProxy = new gpxGlobalIllumination();

	// Register the properties so they can be displayed to the user
	//
	prtyComboBoxUIInfo* pCBUII;
	prtyPropertyUIInfo* pPUII;
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Global Illumination", "Color tint of occluded areas");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_GIRadius), "Global Illumination", "Occlusion Radius (Near Plane)");
	pRFUII->SetMinimum(0.00001f);
	pRFUII->SetMaximum(30.0f);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_GIRadiusFar), "Global Illumination", "Occlusion Radius (Far Plane)");
	pRFUII->SetMinimum(0.00001f);
	pRFUII->SetMaximum(30.0f);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AngleBias), "Global Illumination", "horizon angle bias");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(90.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Contrast), "Global Illumination", "contrast");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(4.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Attenuation), "Global Illumination", "distance attenuation");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(100.0);
	pRFUII->SetDecimalPlaces(2);
	AddProperty(pRFUII);

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BlurWidth), "Global Illumination", "blur radius");
	pRFUII->SetMinimum(0.1000f);
	pRFUII->SetMaximum(32.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BlurSharpness), "Global Illumination", "blur sharpness for edge detection");
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

	prtyNumericUpDownUIInfo* pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_OverscanPixels), "Global Illumination", "Widens the depth buffer to reduce border artifacts.");
	pNUDUII->SetMinimum(0.0f);
	pNUDUII->SetMaximum(1000.0f);
	pNUDUII->SetDecimalPlaces(0);
	pNUDUII->SetIncrement( 1.0f );
	pNUDUII->SetReadOnly(true);
	AddProperty(pNUDUII);

	/* RSM GI parameters */
	//pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RSMGIScale), "RSM Global Illumination", "GI intensity");
	//pRFUII->SetMinimum(0.00001f);
	//pRFUII->SetMaximum(30.0f);
	//pRFUII->SetDecimalPlaces(4);
	//AddProperty(pRFUII);

	//pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RSMGISampleRadius), "RSM Global Illumination", "GI sample radius");
	//pRFUII->SetMinimum(0.00001f);
	//pRFUII->SetMaximum(1.0f);
	//pRFUII->SetDecimalPlaces(4);
	//AddProperty(pRFUII);

	//prtyComboBoxUIInfo* pCBUII;
	//pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RSMGISampleNum), "RSM Global Illumination", "GI sample number");
	//AddProperty( pCBUII );

	//pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_RSMSize), "RSM Global Illumination", "GI Map Size");
	//pCBUII->AddItem(std::string("256"), 0);
	//pCBUII->AddItem(std::string("512"), 1);
	//pCBUII->AddItem(std::string("1024"), 2);
	//pCBUII->AddItem(std::string("2048"), 3);
	////pCBUII->SetReadOnly(true);
	//AddProperty( pCBUII );

	// LPV GI properties
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_LPVScale), "LPV Global Illumination", "GI Scale");
	pRFUII->SetMinimum(0.00001f);
	pRFUII->SetMaximum(30.0f);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);

	pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_LPVIteration), "LPV Global Illumination", "Propogation Iteration");
	pNUDUII->SetMinimum(0.0f);
	pNUDUII->SetMaximum(1000.0f);
	pNUDUII->SetDecimalPlaces(0);
	pNUDUII->SetIncrement( 1.0f );
	pNUDUII->SetReadOnly(false);
	AddProperty(pNUDUII);

	pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_LPVVolumeSize), "LPV Global Illumination", "Radiance Volume Size");
	pNUDUII->SetMinimum(0.0f);
	pNUDUII->SetMaximum(1000.0f);
	pNUDUII->SetDecimalPlaces(0);
	pNUDUII->SetIncrement( 1.0f );
	pNUDUII->SetReadOnly(false);
	AddProperty(pNUDUII);

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_LPVGIFalloff), "LPV Global Illumination", "GI Falloff");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);

	pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_LPVRSMSize), "LPV Global Illumination", "RSMap Size");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	/*pCBUII->AddItem(std::string("4096"), 4);
	pCBUII->AddItem(std::string("8192"), 5);*/
	//pCBUII->SetReadOnly(true);
	AddProperty( pCBUII );


	m_Data.m_GIRadius.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_GIRadiusFar.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_AngleBias.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_Contrast.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_Attenuation.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_BlurWidth.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_BlurSharpness.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_OverscanPixels.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateSSGI));

	/*m_Data.m_RSMGIScale.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateRSMGI));
	m_Data.m_RSMGISampleRadius.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateRSMGI));
	m_Data.m_RSMGISampleNum.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateRSMGI));
	m_Data.m_RSMSize.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateRSMGI));*/

	m_Data.m_LPVScale.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateLPVGI));
	m_Data.m_LPVIteration.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateLPVGI));
	m_Data.m_LPVVolumeSize.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateLPVGI));
	m_Data.m_LPVGIFalloff.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateLPVGI));
	m_Data.m_LPVRSMSize.AddCallback(new prtyCallbackWrapper<giGIObject>(this, &giGIObject::UpdateLPVGI));

	ssgiParams p;
	p.m_GIRadius = m_Data.m_GIRadius.GetValue();
	p.m_GIRadiusFar = m_Data.m_GIRadiusFar.GetValue();
	p.m_AngleBias = m_Data.m_AngleBias.GetValue();
	p.m_Contrast = m_Data.m_Contrast.GetValue();
	p.m_Attenuation = m_Data.m_Attenuation.GetValue();
	p.m_BlurWidth = m_Data.m_BlurWidth.GetValue();
	p.m_BlurSharpness = m_Data.m_BlurSharpness.GetValue();
	p.m_OverscanPixels = m_Data.m_OverscanPixels.GetValue();
	p.m_Color = m_Data.m_Color.GetValue();

	/*p.m_RSMGIScale = m_Data.m_RSMGIScale.GetValue();
	p.m_RSMGISampleRadius = m_Data.m_RSMGISampleRadius.GetValue();
	p.m_RSMGISampleNum = m_Data.m_RSMGISampleNum.GetValue();
	p.m_RSMSize = m_Data.m_RSMSize.GetValue();*/

	// Use proxy to set SSGI settings in scene 
	m_pGIProxy->SetSSGI(p);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
giGIObject::~giGIObject()
{
	// delete proxy
	delete m_pGIProxy;
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string giGIObject::GetDisplayName() const
{
	return "Global Illumination";
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
const giGIData& giGIObject::GetData() const
{
	return this->m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void giGIObject::SetData(const giGIData &i_Data)
{
	m_Data = i_Data;
}


//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void giGIObject::UpdateSSGI(prtyProperty *i_pProperty, bool i_bDirty)
{
	ssgiParams p;
	p.m_GIRadius = m_Data.m_GIRadius.GetValue();
	p.m_GIRadiusFar = m_Data.m_GIRadiusFar.GetValue();
	p.m_AngleBias = m_Data.m_AngleBias.GetValue();
	p.m_Contrast = m_Data.m_Contrast.GetValue();
	p.m_Attenuation = m_Data.m_Attenuation.GetValue();
	p.m_BlurWidth = m_Data.m_BlurWidth.GetValue();
	p.m_BlurSharpness = m_Data.m_BlurSharpness.GetValue();
	p.m_OverscanPixels = m_Data.m_OverscanPixels.GetValue();
	p.m_Color = m_Data.m_Color.GetValue();

	// Use proxy to set SSGI settings in scene 
	m_pGIProxy->SetSSGI(p);
	//api3dScene::SetSSGI(p);
	giDocumentChunk::ActiveDataChanged();
}

//void giGIObject::UpdateRSMGI(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	ssgiParams p;
//	p.m_RSMGIScale = m_Data.m_RSMGIScale.GetValue();
//	p.m_RSMGISampleRadius = m_Data.m_RSMGISampleRadius.GetValue();
//	p.m_RSMGISampleNum = m_Data.m_RSMGISampleNum.GetValue();
//	p.m_GILightScale = m_Data.m_GILightScale.GetValue();
//	p.m_RSMSize = m_Data.m_RSMSize.GetValue();
//
//	// update camera icon
//	//cmraObjectMgr::UpdateGlobalGISetting(p.m_GILightScale);
//	/*int camnum = cmraObjectMgr::GetNumObjects();
//	for (int i = 0; i < camnum; i++)
//	{
//		cmraObjectMgr::GetPickObject(i)->UpdateIcon();
//	}
//	cmraObjectMgr::GetEditorCameraObject()->UpdateIcon();*/
//	m_pGIProxy->SetSSGI(p);
//}

void giGIObject::UpdateLPVGI(prtyProperty *i_pProperty, bool i_bDirty)
{
	ssgiParams p;
	p.m_LPVScale = m_Data.m_LPVScale.GetValue();
	p.m_LPVIteration = m_Data.m_LPVIteration.GetValue();
	p.m_LPVVolumeSize = m_Data.m_LPVVolumeSize.GetValue();
	p.m_LPVGIFalloff = m_Data.m_LPVGIFalloff.GetValue();
	p.m_LPVRSMSize = m_Data.m_LPVRSMSize.GetValue();

	// update camera icon
	//cmraObjectMgr::UpdateGlobalGISetting(p.m_GILightScale);
	/*int camnum = cmraObjectMgr::GetNumObjects();
	for (int i = 0; i < camnum; i++)
	{
		cmraObjectMgr::GetPickObject(i)->UpdateIcon();
	}
	cmraObjectMgr::GetEditorCameraObject()->UpdateIcon();*/
	m_pGIProxy->SetSSGI(p);
}

//--------------------------------------------------------------------
// SetName
//--------------------------------------------------------------------
//void giGIObject::SetName(const nameString& i_Name)
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
prtyFloat&	giGIObject::PropertyRadius()
{
	return m_Data.m_GIRadius;
}
const prtyFloat&	giGIObject::GetPropertyRadius() const
{
	return m_Data.m_GIRadius;
}

//--------------------------------------------------------------------
// RadiusFar property access
//--------------------------------------------------------------------
prtyFloat&	giGIObject::PropertyRadiusFar()
{
	return m_Data.m_GIRadiusFar;
}
const prtyFloat&	giGIObject::GetPropertyRadiusFar() const
{
	return m_Data.m_GIRadiusFar;
}

//--------------------------------------------------------------------
// AngleBias property access
//--------------------------------------------------------------------
prtyFloat&	giGIObject::PropertyAngleBias()
{
	return m_Data.m_AngleBias;
}
const prtyFloat&	giGIObject::GetPropertyAngleBias() const
{
	return m_Data.m_AngleBias;
}

//--------------------------------------------------------------------
// Attenuation property access
//--------------------------------------------------------------------
prtyFloat&	giGIObject::PropertyAttenuation()
{
	return m_Data.m_Attenuation;
}
const prtyFloat&	giGIObject::GetPropertyAttenuation() const
{
	return m_Data.m_Attenuation;
}

//--------------------------------------------------------------------
// Contrast property access
//--------------------------------------------------------------------
prtyFloat&	giGIObject::PropertyContrast()
{
	return m_Data.m_Contrast;
}
const prtyFloat&	giGIObject::GetPropertyContrast() const
{
	return m_Data.m_Contrast;
}

//--------------------------------------------------------------------
// BlurWidth property access
//--------------------------------------------------------------------
prtyFloat&	giGIObject::PropertyBlurWidth()
{
	return m_Data.m_BlurWidth;
}
const prtyFloat&	giGIObject::GetPropertyBlurWidth() const
{
	return m_Data.m_BlurWidth;
}

//--------------------------------------------------------------------
// BlurSharpness property access
//--------------------------------------------------------------------
prtyFloat&	giGIObject::PropertyBlurSharpness()
{
	return m_Data.m_BlurSharpness;
}
const prtyFloat&	giGIObject::GetPropertyBlurSharpness() const
{
	return m_Data.m_BlurSharpness;
}

//--------------------------------------------------------------------
// Overscan Pixels property access
//--------------------------------------------------------------------
prtyInt32& giGIObject::PropertyOverscanPixels()
{
	return m_Data.m_OverscanPixels;
}
const prtyInt32& giGIObject::GetPropertyOverscanPixels() const
{
	return m_Data.m_OverscanPixels;
}

//--------------------------------------------------------------------
// Color property access
//--------------------------------------------------------------------
prtyColor&	giGIObject::PropertyColor()
{
	return m_Data.m_Color;
}
const prtyColor&	giGIObject::GetPropertyColor() const
{
	return m_Data.m_Color;
}
