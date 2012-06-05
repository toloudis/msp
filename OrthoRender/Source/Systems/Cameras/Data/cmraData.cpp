/********************************************************************************************\
**  cmraData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Data/cmraData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraCameraData::cmraCameraData()
:	m_bEditorVisible("Visible in Editor", true),
	m_Name("Name"),
	m_Description("Description"),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_Orientation("Orientation", maRotation( 0.0f, 0.0f, 0.0f )),
	m_FOV("Field of View",90.0f), 
	m_Near("Near Clip", 0.1f), 
	m_Far("Far Clip", 1000.0f), 
	m_Tilt("Tilt", 0.0f),
	m_Target( "Target", maPoint3d( 0.0f, 0.0f, 1.0f ) ),
	//m_bMatchAspectToWindow("Match Aspect To Window",  true),
	//m_AspectRatio("Aspect Ratio", (4.0f / 3.0f)), 
	m_bOrthographic("Orthographic", false),
	m_OrthoWidth("Ortho Width", 1.0f), 
	m_bEnableDOF("Depth of Field", false),
	m_NearBlurDistance("Near Blur Distance", false),
	m_NearFocalDistance("Near Focal Distance", false),
	m_FarFocalDistance("Far Focal Distance", false),
	m_FarBlurDistance("Far Blur Distance", false),
	m_MaxFarBlur("Maximum Far Blur", 1.0f),
	m_HDRMiddleGray("Middle Gray", 1.0f),	
	m_HDRBloomScale("Bloom Scale", 1.0f),	
	m_HDRStarScale("Star Scale", 0.5f),	
	m_HDRBrightPassThresh("BrightPass Threshold", 5.0f),	
	m_HDRBrightPassOffset("BrightPass Offset", 10.0f),	
	m_HDRWhiteCutoff("White Cutoff", 1.0f),
	m_HDRStarType("Glare Type"),
	m_HDRSceneLuminance("Scene Luminance", 1.0)
{
	m_NearBlurDistance = m_Near.GetValue();
	m_FarBlurDistance = m_Far.GetValue();
	m_NearFocalDistance = (float)(0.5f*(m_Far.GetValue() - m_Near.GetValue()) - 1);
	m_FarFocalDistance = (float)(0.5f*(m_Far.GetValue() - m_Near.GetValue()) + 1);

	m_HDRStarType.SetEnumTag(0,"Disable");
	m_HDRStarType.SetEnumTag(1,"Camera");
	m_HDRStarType.SetEnumTag(2,"Natural Bloom");
	m_HDRStarType.SetEnumTag(3,"Cheap Lens Camera");
	m_HDRStarType.SetEnumTag(4,"Cross Screen Filter");
	m_HDRStarType.SetEnumTag(5,"Spectral Cross Filter");
	m_HDRStarType.SetEnumTag(6,"Snow Cross Filter");
	m_HDRStarType.SetEnumTag(7,"Spectral Snow Cross");
	m_HDRStarType.SetEnumTag(8,"Sunny Cross Filter");
	m_HDRStarType.SetEnumTag(9,"Spectral Sunny Cross");
	m_HDRStarType.SetEnumTag(10,"Cine Camera Vertical Slits");
	m_HDRStarType.SetEnumTag(11,"Cine Camera Horizontal Slits");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraCameraData::cmraCameraData(const cmraCameraData& i_Data)
:	m_bEditorVisible("Visible in Editor", true),
	m_Name("Name"),
	m_Description("Description"),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_Orientation("Orientation", maRotation( 0.0f, 0.0f, 0.0f )),
	m_FOV("Field of View",90.0f), 
	m_Near("Near Clip", 0.1f), 
	m_Far("Far Clip", 1000.0f), 
	m_Tilt("Tilt", 0.0f),
	m_Target( "Target", maPoint3d( 0.0f, 0.0f, 0.0f ) ),
	//m_bMatchAspectToWindow("Match Aspect To Window",  true),
	//m_AspectRatio("Aspect Ratio", (4.0f / 3.0f)), 
	m_bOrthographic("Orthographic", false),
	m_OrthoWidth("Ortho Width", 1.0f), 
	m_bEnableDOF("Depth of Field", false),
	m_NearBlurDistance("Near Blur Distance", false),
	m_NearFocalDistance("Near Focal Distance", false),
	m_FarFocalDistance("Far Focal Distance", false),
	m_FarBlurDistance("Far Blur Distance", false),
	m_MaxFarBlur("Maximum Far Blur", 1.0f),
	m_HDRMiddleGray("Middle Gray", 1.0f),	
	m_HDRBloomScale("Bloom Scale", 1.0f),	
	m_HDRStarScale("Star Scale", 0.5f),	
	m_HDRBrightPassThresh("BrightPass Threshold", 5.0f),	
	m_HDRBrightPassOffset("BrightPass Offset", 10.0f),	
	m_HDRWhiteCutoff("White Cutoff", 1.0f),
	m_HDRStarType("Glare Type"),
	m_HDRSceneLuminance("Scene Luminance", 1.0)
{
	m_HDRStarType.SetEnumTag(0,"Disable");
	m_HDRStarType.SetEnumTag(1,"Camera");
	m_HDRStarType.SetEnumTag(2,"Natural Bloom");
	m_HDRStarType.SetEnumTag(3,"Cheap Lens Camera");
	m_HDRStarType.SetEnumTag(4,"Cross Screen Filter");
	m_HDRStarType.SetEnumTag(5,"Spectral Cross Filter");
	m_HDRStarType.SetEnumTag(6,"Snow Cross Filter");
	m_HDRStarType.SetEnumTag(7,"Spectral Snow Cross");
	m_HDRStarType.SetEnumTag(8,"Sunny Cross Filter");
	m_HDRStarType.SetEnumTag(9,"Spectral Sunny Cross");
	m_HDRStarType.SetEnumTag(10,"Cine Camera Vertical Slits");
	m_HDRStarType.SetEnumTag(11,"Cine Camera Horizontal Slits");

	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraCameraData::~cmraCameraData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool cmraCameraData::operator == (const cmraCameraData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraCameraData& cmraCameraData::operator=(const cmraCameraData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_bEditorVisible	= i_Data.m_bEditorVisible;
	this->m_Name			= i_Data.m_Name;
	this->m_Orientation		= i_Data.m_Orientation;
	this->m_Position		= i_Data.m_Position;
	this->m_Description		= i_Data.m_Description;
	this->m_Target			= i_Data.m_Target;
	this->m_FOV				= i_Data.m_FOV;
	this->m_Near			= i_Data.m_Near;
	this->m_Far				= i_Data.m_Far;
	this->m_Tilt			= i_Data.m_Tilt;
	//this->m_bMatchAspectToWindow	= i_Data.m_bMatchAspectToWindow;
	//this->m_AspectRatio		= i_Data.m_AspectRatio;
	this->m_bOrthographic	= i_Data.m_bOrthographic;
	this->m_OrthoWidth		= i_Data.m_OrthoWidth;
	this->m_bEnableDOF		= i_Data.m_bEnableDOF;
	this->m_NearBlurDistance	= i_Data.m_NearBlurDistance;
	this->m_NearFocalDistance	= i_Data.m_NearFocalDistance;
	this->m_FarFocalDistance	= i_Data.m_FarFocalDistance;
	this->m_FarBlurDistance = i_Data.m_FarBlurDistance;
	this->m_MaxFarBlur		= i_Data.m_MaxFarBlur;
	this->m_HDRMiddleGray 	= i_Data.m_HDRMiddleGray;
	this->m_HDRBloomScale	= i_Data.m_HDRBloomScale;
	this->m_HDRStarScale	= i_Data.m_HDRStarScale;
	this->m_HDRBrightPassThresh	= i_Data.m_HDRBrightPassThresh;
	this->m_HDRBrightPassOffset	= i_Data.m_HDRBrightPassOffset;
	this->m_HDRWhiteCutoff	= i_Data.m_HDRWhiteCutoff;
	this->m_HDRStarType		= i_Data.m_HDRStarType;
	this->m_HDRSceneLuminance	= i_Data.m_HDRSceneLuminance;

	return *this;
}



