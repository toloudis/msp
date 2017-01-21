/********************************************************************************************\
**  cmraData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Data/cmraData.hpp"

#include "Graphics/g3d/g3dPassBuffers.hpp"

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
	m_Near("Near Clip", 1.0f), 
	m_Far("Far Clip", 10000.0f), 
	m_Tilt("Tilt", 0.0f),
	m_Target( "Target", maPoint3d( 0.0f, 0.0f, 1.0f ) ),
	//m_bMatchAspectToWindow("Match Aspect To Window",  true),
	//m_AspectRatio("Aspect Ratio", (4.0f / 3.0f)), 
	m_bOrthographic("Orthographic", false),
	m_OrthoWidth("Ortho Width", 200.0f), 
	m_bEnableDOF("Depth of Field", false),
	m_NearBlurDistance("Near Blur Distance", 10),
	m_NearFocalDistance("Near Focal Distance", 50),
	m_FarFocalDistance("Far Focal Distance", 200),
	m_FarBlurDistance("Far Blur Distance", 400),
	m_MaxFarBlur("Maximum Far Blur", 1.0f),
	m_MaxCoC("Focus Blur", 5.0f),
	m_bEnableALP("Advanced Lens Properties", false),
	m_FocalLength("Focal Length", 35.0f),
	m_Fstop("FStop", 5.6f),
	m_CoC("Circle of Confusion", 0.025f),
	m_HorizontalAperture("Horizontal Aperture",1.417f),
	m_FocalDistance("Focus Distance", 5),
	m_HDRMiddleGray("Middle Gray", 1.0f),	
	m_HDRBloomScale("Bloom Scale", 1.0f),	
	m_HDRStarScale("Star Scale", 0.5f),	
	m_HDRBrightPassThresh("BrightPass Threshold", 5.0f),	
	m_HDRBrightPassOffset("BrightPass Offset", 10.0f),	
	m_HDRWhiteCutoff("White Cutoff", 1.0f),
	m_HDRStarType("Glare Type"),
	m_HDRSceneLuminance("Scene Luminance", 1.0f),
	m_StereoFD("Zero Parallax",75),
	m_StereoFilterColor("Anaglyph Colors"),
	m_StereoType("Output Type"),
	m_StereoProjection("Projection Type",1),
	m_StereoIOD("Interaxial Separation",1.0f),
	m_TextureFilenameAO("Render Pass: AO"),
	m_IntensityAO("Render Pass: AO",1.0f),
	m_BlendOpAO("Render Pass: AO",g3dPassBuffers::e_MUL),
	m_TextureFilenameGI("Render Pass: GI"),
	m_IntensityGI("Render Pass: GI",1.0f),
	m_BlendOpGI("Render Pass: GI",g3dPassBuffers::e_ADD),
	m_TextureFilenameRefl("Render Pass: Reflections"),
	m_IntensityRefl("Render Pass: Reflections",1.0f),
	m_BlendOpRefl("Render Pass: Reflections",g3dPassBuffers::e_ADD),
	m_TextureFilenameShadowMask("Render Pass: Shadow Mask"),
	m_IntensityShadowMask("Render Pass: Shadow Mask",1.0f),
	m_BlendOpShadowMask("Render Pass: Shadow Mask",g3dPassBuffers::e_MUL),
	m_TextureFilenameBeauty("Render Pass: Beauty"),
	m_IntensityBeauty("Render Pass: Beauty",1.0f),
	m_BlendOpBeauty("Render Pass: Beauty",g3dPassBuffers::e_ADD)
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

	m_StereoType.SetEnumTag(0,"None");
	m_StereoType.SetEnumTag(1,"Anaglyph");
	m_StereoType.SetEnumTag(2,"Dual Cameras");
	m_StereoType.SetEnumTag(3,"Left Camera");
	m_StereoType.SetEnumTag(4,"Right Camera");

	m_StereoFilterColor.SetEnumTag(0,"Red - Cyan");
	m_StereoFilterColor.SetEnumTag(1,"Red - Blue");
	m_StereoFilterColor.SetEnumTag(2,"Red - Green");
	m_StereoFilterColor.SetEnumTag(3,"Cyan - Red");
	m_StereoFilterColor.SetEnumTag(4,"Blue - Red");
	m_StereoFilterColor.SetEnumTag(5,"Green - Red");

	m_StereoProjection.SetEnumTag(0,"Toe-in");
	m_StereoProjection.SetEnumTag(1,"Off-axis");

	m_TextureFilenameAO.SetValue(fsLocator());
	m_TextureFilenameGI.SetValue(fsLocator());
	m_TextureFilenameRefl.SetValue(fsLocator());
	m_TextureFilenameShadowMask.SetValue(fsLocator());
	m_TextureFilenameBeauty.SetValue(fsLocator());

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
	m_Near("Near Clip", 1.0f), 
	m_Far("Far Clip", 10000.0f), 
	m_Tilt("Tilt", 0.0f),
	m_Target( "Target", maPoint3d( 0.0f, 0.0f, 0.0f ) ),
	//m_bMatchAspectToWindow("Match Aspect To Window",  true),
	//m_AspectRatio("Aspect Ratio", (4.0f / 3.0f)), 
	m_bOrthographic("Orthographic", false),
	m_OrthoWidth("Ortho Width", 200.0f), 
	m_bEnableDOF("Depth of Field", false),
	m_NearBlurDistance("Near Blur Distance", 10),
	m_NearFocalDistance("Near Focal Distance", 50),
	m_FarFocalDistance("Far Focal Distance", 200),
	m_FarBlurDistance("Far Blur Distance", 400),
	m_MaxFarBlur("Maximum Far Blur", 1.0f),
	m_MaxCoC("Focus Blur", 5.0f),
	m_bEnableALP("Advanced Lens Properties", false),
	m_FocalLength("Focal Length", 35.0f),
	m_Fstop("FStop", 5.6f),
	m_CoC("Circle of Confusion", 0.025f),
	m_HorizontalAperture("Horizontal Aperture",1.417f),
	m_FocalDistance("Focus Distance", 5),
	m_HDRMiddleGray("Middle Gray", 1.0f),	
	m_HDRBloomScale("Bloom Scale", 1.0f),	
	m_HDRStarScale("Star Scale", 0.5f),	
	m_HDRBrightPassThresh("BrightPass Threshold", 5.0f),	
	m_HDRBrightPassOffset("BrightPass Offset", 10.0f),	
	m_HDRWhiteCutoff("White Cutoff", 1.0f),
	m_HDRStarType("Glare Type"),
	m_HDRSceneLuminance("Scene Luminance", 1.0f),
	m_StereoFD("Zero Parallax",75),
	m_StereoFilterColor("Anaglyph Colors"),
	m_StereoType("Output Type"),
	m_StereoProjection("Projection Type",1),
	m_StereoIOD("Interaxial Separation",1.0),
	m_TextureFilenameAO("Render Pass: AO"),
	m_IntensityAO("Render Pass: AO",1.0f),
	m_BlendOpAO("Render Pass: AO",g3dPassBuffers::e_MUL),
	m_TextureFilenameGI("Render Pass: GI"),
	m_IntensityGI("Render Pass: GI",1.0f),
	m_BlendOpGI("Render Pass: GI",g3dPassBuffers::e_ADD),
	m_TextureFilenameRefl("Render Pass: Reflections"),
	m_IntensityRefl("Render Pass: Reflections",1.0f),
	m_BlendOpRefl("Render Pass: Reflections",g3dPassBuffers::e_ADD),
	m_TextureFilenameShadowMask("Render Pass: Shadow Mask"),
	m_IntensityShadowMask("Render Pass: Shadow Mask",1.0f),
	m_BlendOpShadowMask("Render Pass: Shadow Mask",g3dPassBuffers::e_MUL),
	m_TextureFilenameBeauty("Render Pass: Beauty"),
	m_IntensityBeauty("Render Pass: Beauty",1.0f),
	m_BlendOpBeauty("Render Pass: Beauty",g3dPassBuffers::e_ADD)
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

	m_StereoType.SetEnumTag(0,"None");
	m_StereoType.SetEnumTag(1,"Anaglyph");
	m_StereoType.SetEnumTag(2,"Dual Cameras");
	m_StereoType.SetEnumTag(3,"Left Camera");
	m_StereoType.SetEnumTag(4,"Right Camera");

	m_StereoFilterColor.SetEnumTag(0,"Red - Cyan");
	m_StereoFilterColor.SetEnumTag(1,"Red - Blue");
	m_StereoFilterColor.SetEnumTag(2,"Red - Green");
	m_StereoFilterColor.SetEnumTag(3,"Cyan - Red");
	m_StereoFilterColor.SetEnumTag(4,"Blue - Red");
	m_StereoFilterColor.SetEnumTag(5,"Green - Red");

	m_StereoProjection.SetEnumTag(0,"Toe-in");
	m_StereoProjection.SetEnumTag(1,"Off-axis");

	m_TextureFilenameAO.SetValue(fsLocator());
	m_TextureFilenameGI.SetValue(fsLocator());
	m_TextureFilenameRefl.SetValue(fsLocator());
	m_TextureFilenameShadowMask.SetValue(fsLocator());
	m_TextureFilenameBeauty.SetValue(fsLocator());

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
	this->m_MaxCoC			= i_Data.m_MaxCoC;
	this->m_bEnableALP		= i_Data.m_bEnableALP;
	this->m_FocalLength		= i_Data.m_FocalLength;
	this->m_CoC				= i_Data.m_CoC;
	this->m_HorizontalAperture = i_Data.m_HorizontalAperture;
	this->m_Fstop			= i_Data.m_Fstop;
	this->m_FocalDistance	= i_Data.m_FocalDistance;
	this->m_HDRMiddleGray 	= i_Data.m_HDRMiddleGray;
	this->m_HDRBloomScale	= i_Data.m_HDRBloomScale;
	this->m_HDRStarScale	= i_Data.m_HDRStarScale;
	this->m_HDRBrightPassThresh	= i_Data.m_HDRBrightPassThresh;
	this->m_HDRBrightPassOffset	= i_Data.m_HDRBrightPassOffset;
	this->m_HDRWhiteCutoff	= i_Data.m_HDRWhiteCutoff;
	this->m_HDRStarType		= i_Data.m_HDRStarType;
	this->m_HDRSceneLuminance	= i_Data.m_HDRSceneLuminance;
	this->m_StereoFD		= i_Data.m_StereoFD;
	this->m_StereoFilterColor	= i_Data.m_StereoFilterColor;
	this->m_StereoType	= i_Data.m_StereoType;
	this->m_StereoProjection	= i_Data.m_StereoProjection;
	this->m_StereoIOD		= i_Data.m_StereoIOD;

	this->m_TextureFilenameAO		= i_Data.m_TextureFilenameAO;
	this->m_TextureFilenameGI		= i_Data.m_TextureFilenameGI;
	this->m_TextureFilenameRefl		= i_Data.m_TextureFilenameRefl;
	this->m_TextureFilenameShadowMask = i_Data.m_TextureFilenameShadowMask;
	this->m_TextureFilenameBeauty		= i_Data.m_TextureFilenameBeauty;

	return *this;
}



