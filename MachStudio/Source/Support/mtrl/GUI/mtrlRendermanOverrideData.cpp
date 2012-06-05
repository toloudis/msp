/********************************************************************************************\
**  mtrlRendermanOverrideData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlRendermanOverrideData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlRendermanOverrideData::mtrlRendermanOverrideData()
:	m_ShaderLocation("Compiled Shader", fsLocator()),
	m_ParamList("Shader Parameters", ""),
	m_AttributeList("Surface Attributes", ""),
	m_bOverrideShader("Override Shader", false),
	m_bOverrideAttributes("Override Attributes", false)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlRendermanOverrideData::mtrlRendermanOverrideData(const mtrlRendermanOverrideData& i_Data)
:	m_ShaderLocation("Compiled Shader", fsLocator()),
	m_ParamList("Optional Parameters", ""),
	m_AttributeList("Surface Attributes", ""),
	m_bOverrideShader("Override Shader", false),
	m_bOverrideAttributes("Override Attributes", false)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlRendermanOverrideData::~mtrlRendermanOverrideData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlRendermanOverrideData& mtrlRendermanOverrideData::operator=(const mtrlRendermanOverrideData& i_Data)
{
	if (this == &i_Data) return *this;

	m_ParamList = i_Data.m_ParamList;
	m_AttributeList = i_Data.m_AttributeList;
	m_ShaderLocation = i_Data.m_ShaderLocation;
	m_bOverrideShader = i_Data.m_bOverrideShader;
	m_bOverrideAttributes = i_Data.m_bOverrideAttributes;

	return *this;
}

