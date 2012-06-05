/****************************************************************************\
**  aoAOData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Data/aoAOData.hpp"

//============================================================================
//============================================================================
aoAOData::aoAOData()
:	m_AORadius("Scene Scale", 25),
	m_AngleBias("Angle Bias", 0.0f),
	m_Attenuation("Attenuation", 1),
	m_Contrast("Contrast",1),
	m_BlurWidth("Blur width", 7),
	m_BlurSharpness("Blur sharpness", 2),
	m_Name("Name", nameString("AO"))
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
aoAOData::aoAOData(const aoAOData& i_Data)
:	m_AORadius("Scene Scale", 25),
	m_AngleBias("Angle Bias", 0.0f),
	m_Attenuation("Attenuation", 1),
	m_Contrast("Contrast",1),
	m_BlurWidth("Blur width", 7),
	m_BlurSharpness("Blur sharpness", 2),
	m_Name("Name", nameString("AO"))
{
	(*this) = i_Data;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
aoAOData::~aoAOData()
{
}

//-------------------------------------------------
bool aoAOData::operator == (const aoAOData& i_Item)
{
	// name comparison??
	// FIX ME
	// always equal or always unequal? 
	return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
aoAOData& aoAOData::operator=(const aoAOData& i_Data)
{
	if (this == &i_Data) return *this;

	m_AORadius = i_Data.m_AORadius;
	m_AngleBias = i_Data.m_AngleBias;
	m_Attenuation = i_Data.m_Attenuation;
	m_Contrast = i_Data.m_Contrast;
	m_BlurWidth = i_Data.m_BlurWidth;
	m_BlurSharpness = i_Data.m_BlurSharpness;

	m_Name = i_Data.m_Name;

	return *this;
}
