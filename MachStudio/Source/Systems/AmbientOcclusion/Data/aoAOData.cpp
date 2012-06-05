/****************************************************************************\
**  aoAOData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Data/aoAOData.hpp"

//============================================================================
//============================================================================
aoAOData::aoAOData()
:	m_AORadius("Scene Scale Near", 25),
	m_AORadiusFar("Scene Scale Far", 50),
	m_AngleBias("Angle Bias", 0.0f),
	m_Attenuation("Attenuation", 1),
	m_Contrast("Contrast",1),
	m_BlurWidth("Blur width", 7),
	m_BlurSharpness("Blur sharpness", 2),
	m_Color("Color", maFloatRGBA(0,0,0,1)),
	m_OverscanPixels("Overscan Pixels", 0 ),
	m_ClipPlaneEpsilon("ClipPlaneE", 0.01f),
	m_NoClipPlaneEpsilon("NoClipPlaneE", 0.01f),
	m_AreaRatio("AreaRatio", 0.1f),
	m_BehindPlaneEpsilon("BehindPlaneE", 0.0001f),
	m_Name("Name", nameString("AO"))
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
aoAOData::aoAOData(const aoAOData& i_Data)
:	m_AORadius("Scene Scale Near", 25),
	m_AORadiusFar("Scene Scale Far", 50),
	m_AngleBias("Angle Bias", 0.0f),
	m_Attenuation("Attenuation", 1),
	m_Contrast("Contrast",1),
	m_BlurWidth("Blur width", 7),
	m_BlurSharpness("Blur sharpness", 2),
	m_Color("Color", maFloatRGBA(0,0,0,1)),
	m_OverscanPixels("Overscan Pixels", 0 ),
	m_ClipPlaneEpsilon("ClipPlaneE", 0.01f),
	m_NoClipPlaneEpsilon("NoClipPlaneE", 0.01f),
	m_AreaRatio("AreaRatio", 0.1f),
	m_BehindPlaneEpsilon("BehindPlaneE", 0.01f),
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
	m_AORadiusFar = i_Data.m_AORadiusFar;
	m_AngleBias = i_Data.m_AngleBias;
	m_Attenuation = i_Data.m_Attenuation;
	m_Contrast = i_Data.m_Contrast;
	m_BlurWidth = i_Data.m_BlurWidth;
	m_BlurSharpness = i_Data.m_BlurSharpness;
	m_OverscanPixels = i_Data.m_OverscanPixels;
	m_ClipPlaneEpsilon = i_Data.m_ClipPlaneEpsilon;
	m_NoClipPlaneEpsilon = i_Data.m_NoClipPlaneEpsilon;
	m_AreaRatio = i_Data.m_AreaRatio;
	m_BehindPlaneEpsilon = i_Data.m_BehindPlaneEpsilon;
	m_Color = i_Data.m_Color;

	m_Name = i_Data.m_Name;

	return *this;
}
