/****************************************************************************\
**  billData.cpp
**
**		see .hpp
**
**  Extra Large Technology
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
	m_Scale("Scale",1), 
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
billData::billData(const itString& i_Filename)
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Scale",1), 
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Name("Name"),
	m_Filename("File Name", i_Filename),
	m_Position("Position"),
	m_Orientation("Orientation")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billData::billData(	const itString& i_Filename,
					const maPoint3d& i_Position,
					float i_Scale )
:	m_bEditorVisible("Visible in Editor", true),
	m_Scale("Scale",i_Scale), 
	m_bOrientToCamera("Orient to Camera", true), 
	m_bAdditiveMaterial("Additive Material", true),
	m_Color("Color", maFloatRGBA(1,1,1,1)),
	m_Name("Name"),
	m_Filename("File Name", i_Filename),
	m_Position("Position", i_Position),
	m_Orientation("Orientation")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool billData::operator == (const billData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

