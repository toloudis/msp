/****************************************************************************\
**  billData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Data/billData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		billData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billData::billData()
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Uniform Scale",100.0f), 
	m_NonUniformScale("Non-Uniform Scale", maPoint3d(1.0f, 1.0f, 1.0f)),
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Brightness("Brightness", 1),
	m_Name("Name"),
	m_Filename("File Name"),
	m_Position("Position"),
	m_Orientation("Orientation"),
	m_Video("Video"),
	m_bVisible("Visible", true),
	m_bCKActive("Chroma Key Active", false),
	m_CKColor("Chroma Key Color", maFloatRGBA(0,0,0,0)),
	m_CKTolerance("Chroma Key Tolerance", 0.1f),
	m_bCKRemoveSpill("Remove Spill", false),
	m_CKSpillType("Spill Type", 0),
	m_CKSpillBias("Spill Bias", 0.0f),
	m_CKEdgeBlur("Edge Blur Radius", 0),
	m_bSnapToCamera("Snap To View", false),
	m_DistToCamera("Distance to Camera", 0),
	m_CameraIndex("Camera", 0)
{
	m_CKSpillType.SetEnumTag(0, "GreenScreen");
	m_CKSpillType.SetEnumTag(1, "BlueScreen");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billData::billData(const fsLocator& i_Filename)
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Uniform Scale",100.0f), 
	m_NonUniformScale("Non-Uniform Scale", maPoint3d(1.0f, 1.0f, 1.0f)),
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Brightness("Brightness", 1),
	m_Name("Name"),
	m_Filename("File Name", i_Filename),
	m_Position("Position"),
	m_Orientation("Orientation"),
	m_Video("Video"),
	m_bVisible("Visible", true),
	m_bCKActive("Chroma Key Active", false),
	m_CKColor("Chroma Key Color", maFloatRGBA(0,0,0,0)),
	m_CKTolerance("Chroma Key Tolerance", 0.1f),
	m_bCKRemoveSpill("Remove Spill", false),
	m_CKSpillType("Spill Type", 0),
	m_CKSpillBias("Spill Bias", 0.0f),
	m_CKEdgeBlur("Edge Blur Radius", 0),
	m_bSnapToCamera("Snap To View", false),
	m_DistToCamera("Distance to Camera", 0),
	m_CameraIndex("Camera", 0)
{
	m_CKSpillType.SetEnumTag(0, "GreenScreen");
	m_CKSpillType.SetEnumTag(1, "BlueScreen");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billData::billData(	const fsLocator& i_Filename,
					const maPoint3d& i_Position,
					float i_Scale )
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Uniform Scale",i_Scale), 
	m_NonUniformScale("Non-Uniform Scale", maPoint3d(1.0f, 1.0f, 1.0f)),
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Brightness("Brightness", 1),
	m_Name("Name"),
	m_Filename("File Name", i_Filename),
	m_Position("Position", i_Position),
	m_Orientation("Orientation"),
	m_Video("Video"),
	m_bVisible("Visible", true),
	m_bCKActive("Chroma Key Active", false),
	m_CKColor("Chroma Key Color", maFloatRGBA(0,0,0,0)),
	m_CKTolerance("Chroma Key Tolerance", 0.1f),
	m_bCKRemoveSpill("Remove Spill", false),
	m_CKSpillType("Spill Type", 0),
	m_CKSpillBias("Spill Bias", 0.0f),
	m_CKEdgeBlur("Edge Blur Radius", 0),
	m_bSnapToCamera("Snap To View", false),
	m_DistToCamera("Distance to Camera", 0),
	m_CameraIndex("Camera", 0)
{
	m_CKSpillType.SetEnumTag(0, "GreenScreen");
	m_CKSpillType.SetEnumTag(1, "BlueScreen");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool billData::operator == (const billData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

