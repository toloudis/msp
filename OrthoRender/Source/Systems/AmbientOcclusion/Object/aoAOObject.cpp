/*****************************************************************************
**  aoAOObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Object/aoAOObject.hpp"

// library
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Tool/api3d/api3dScene.hpp"

namespace
{
}

//----------------------------------------------------------------------------
// This object takes ownership of the arguments passed in
//----------------------------------------------------------------------------
aoAOObject::aoAOObject(	)
{
	// Register the properties so they can be displayed to the user
	//
	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AORadius), "SSAO", "radial view space distance cutoff");
	pRFUII->SetMinimum(0.00001f);
	pRFUII->SetMaximum(30.0f);
	pRFUII->SetDecimalPlaces(4);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_AngleBias), "SSAO", "horizon angle bias");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(90.0);
	pRFUII->SetDecimalPlaces(1);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Contrast), "SSAO", "contrast");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(4.0);
	pRFUII->SetDecimalPlaces(2);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Attenuation), "SSAO", "distance attenuation");
	pRFUII->SetMinimum(0.0000);
	pRFUII->SetMaximum(100.0);
	pRFUII->SetDecimalPlaces(2);
	AddProperty(pRFUII);

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BlurWidth), "SSAO", "blur radius");
	pRFUII->SetMinimum(0.1000f);
	pRFUII->SetMaximum(32.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BlurSharpness), "SSAO", "blur sharpness for edge detection");
	pRFUII->SetMinimum(0.0);
	pRFUII->SetMaximum(4.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty(pRFUII);


	m_Data.m_AORadius.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_AngleBias.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_Contrast.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_Attenuation.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_BlurWidth.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
	m_Data.m_BlurSharpness.AddCallback(new prtyCallbackWrapper<aoAOObject>(this, &aoAOObject::UpdateSSAO));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoAOObject::~aoAOObject()
{
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string aoAOObject::GetPick3dName() const
{
	return "AO";
}


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
	p.m_AngleBias = m_Data.m_AngleBias.GetValue();
	p.m_Contrast = m_Data.m_Contrast.GetValue();
	p.m_Attenuation = m_Data.m_Attenuation.GetValue();
	p.m_BlurWidth = m_Data.m_BlurWidth.GetValue();
	p.m_BlurSharpness = m_Data.m_BlurSharpness.GetValue();

	api3dScene::SetSSAO(p);
}

//--------------------------------------------------------------------
// SetName
//--------------------------------------------------------------------
void aoAOObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name.SetValue(this->GetName());
}
