/****************************************************************************\
**  sbrdObjectData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Data/sbrdObjectData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/it/itStringUtil.hpp"


//
//		sbrdObjectData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdObjectData::sbrdObjectData()
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Scale",16.0f), 
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Name("Name"),
	m_Filename("File Name"),
	m_Position("Position"),
	m_Orientation("Orientation")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdObjectData::sbrdObjectData(const itString& i_Filename)
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Scale",16.0f), 
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Name("Name", nameString( itStringUtil::GetStdString(i_Filename) )),
	m_Filename("File Name", i_Filename),
	m_Position("Position"),
	m_Orientation("Orientation")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdObjectData::sbrdObjectData(	const itString& i_Filename,
					const maPoint3d& i_Position,
					float i_Scale )
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Scale",i_Scale), 
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Name("Name", nameString( itStringUtil::GetStdString(i_Filename) )),
	m_Filename("File Name", i_Filename),
	m_Position("Position", i_Position),
	m_Orientation("Orientation")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool sbrdObjectData::operator == (const sbrdObjectData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

