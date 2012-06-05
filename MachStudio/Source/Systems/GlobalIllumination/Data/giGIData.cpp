/****************************************************************************\
**  giGIData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/Data/giGIData.hpp"

//============================================================================
//============================================================================
giGIData::giGIData()
:	m_GIRadius("Scene Scale Near", 25),
	m_GIRadiusFar("Scene Scale Far", 50),
	m_AngleBias("Angle Bias", 0.0f),
	m_Attenuation("Attenuation", 1),
	m_Contrast("Contrast",1),
	m_BlurWidth("Blur width", 7),
	m_BlurSharpness("Blur sharpness", 2),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_OverscanPixels("Overscan Pixels", 0 ),
	m_Name("Name", nameString("GI")),
	/*m_RSMGIScale("GI Intensity", 1.0f),
	m_RSMGISampleRadius("GI Sampling Radius", 0.8f),
	m_RSMGISampleNum("GI Bounce Num.", 3),
	m_GILightScale("GI Light Scale", 1.0f),
	m_RSMSize("GI Map Size", 256),*/
	m_LPVScale("GI Scale", 1.0f),
	m_LPVIteration("Propogation Iteration", 2),
	m_LPVVolumeSize("Radiance Volume Size", 32),
	m_LPVGIFalloff("GI Falloff", 1.0f),
	m_LPVRSMSize("GI RSM Size", 512)
{

	/*m_RSMGISampleNum.SetEnumTag(0, "Low");
	m_RSMGISampleNum.SetEnumTag(1, "Medium");
	m_RSMGISampleNum.SetEnumTag(2, "High");
	m_RSMGISampleNum.SetEnumTag(3, "Very High");*/
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
giGIData::giGIData(const giGIData& i_Data)
:	m_GIRadius("Scene Scale Near", 25),
	m_GIRadiusFar("Scene Scale Far", 50),
	m_AngleBias("Angle Bias", 0.0f),
	m_Attenuation("Attenuation", 1),
	m_Contrast("Contrast",1),
	m_BlurWidth("Blur width", 7),
	m_BlurSharpness("Blur sharpness", 2),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_OverscanPixels("Overscan Pixels", 0 ),
	m_Name("Name", nameString("GI")),
	/*m_RSMGIScale("GI Intensity", 1.0f),
	m_RSMGISampleRadius("GI Sampling Radius", 0.8f),
	m_RSMGISampleNum("GI Bounce Num.", 3),
	m_GILightScale("GI Light Scale", 1.0f),
	m_RSMSize("GI Map Size", 256),*/
	m_LPVScale("GI Scale", 1.0f),
	m_LPVIteration("Propogation Iteration", 2),
	m_LPVVolumeSize("Radiance Volume Size", 32),
	m_LPVGIFalloff("GI Falloff", 1.0f),
	m_LPVRSMSize("GI RSM Size", 512)
{
	(*this) = i_Data;

	/*m_RSMGISampleNum.SetEnumTag(0, "Low");
	m_RSMGISampleNum.SetEnumTag(1, "Medium");
	m_RSMGISampleNum.SetEnumTag(2, "High");
	m_RSMGISampleNum.SetEnumTag(3, "Very High");*/
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
giGIData::~giGIData()
{
}

//-------------------------------------------------
bool giGIData::operator == (const giGIData& i_Item)
{
	// name comparison??
	// FIX ME
	// always equal or always unequal? 
	return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
giGIData& giGIData::operator=(const giGIData& i_Data)
{
	if (this == &i_Data) return *this;

	m_GIRadius = i_Data.m_GIRadius;
	m_GIRadiusFar = i_Data.m_GIRadiusFar;
	m_AngleBias = i_Data.m_AngleBias;
	m_Attenuation = i_Data.m_Attenuation;
	m_Contrast = i_Data.m_Contrast;
	m_BlurWidth = i_Data.m_BlurWidth;
	m_BlurSharpness = i_Data.m_BlurSharpness;
	m_OverscanPixels = i_Data.m_OverscanPixels;
	m_Color = i_Data.m_Color;

	m_Name = i_Data.m_Name;

	/*m_RSMGIScale = i_Data.m_RSMGIScale;
	m_RSMGISampleRadius = i_Data.m_RSMGISampleRadius;
	m_RSMGISampleNum = i_Data.m_RSMGISampleNum;
	m_GILightScale = i_Data.m_GILightScale;
	m_RSMSize = i_Data.m_RSMSize;*/

	m_LPVScale = i_Data.m_LPVScale;
	m_LPVIteration = i_Data.m_LPVIteration;
	m_LPVVolumeSize = i_Data.m_LPVVolumeSize;
	m_LPVGIFalloff = i_Data.m_LPVGIFalloff;
	m_LPVRSMSize = i_Data.m_LPVRSMSize;

	return *this;
}
