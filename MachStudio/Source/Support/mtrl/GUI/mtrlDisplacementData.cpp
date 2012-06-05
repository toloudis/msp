/********************************************************************************************\
**  mtrlDisplacementData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2009 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlDisplacementData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlDisplacementData::mtrlDisplacementData()
:	m_DisplacementMap( "Displacement Map", fsLocator()),
	m_DisplacementScale( "Displacement Scale", 0.0f ),
	m_DisplacementBias( "Displacement Bias", 0.0f ),
	m_DisplacementBlur( "Displacement Smoothness", 0.0f ),
	m_TessellationValue( "Tessellation Amount", 0.0f ),
	m_ObjTextureSize( "Normal Scale", 1.0f )
//	m_ObjTextureAspect( "Object Texture Aspect", 1.0f )
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlDisplacementData::mtrlDisplacementData(const mtrlDisplacementData& i_Data)
:	m_DisplacementMap( "Displacement Map", fsLocator()),
	m_DisplacementScale( "Displacement Scale", 0.0f ),
	m_DisplacementBias( "Displacement Bias", 0.0f ),
	m_DisplacementBlur( "Displacement Smoothness", 0.0f ),
	m_TessellationValue( "Tessellation Amount", 0.0f ),
	m_ObjTextureSize( "Normal Scale", 1.0f )
//	m_ObjTextureAspect( "Object Texture Aspect", 1.0f )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlDisplacementData::~mtrlDisplacementData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlDisplacementData& mtrlDisplacementData::operator=(const mtrlDisplacementData& i_Data)
{
	if (this == &i_Data) return *this;

	m_DisplacementMap = i_Data.m_DisplacementMap;
	m_DisplacementScale = i_Data.m_DisplacementScale;
	m_DisplacementBias = i_Data.m_DisplacementBias;
	m_DisplacementBlur = i_Data.m_DisplacementBlur;
	m_TessellationValue = i_Data.m_TessellationValue;
	m_ObjTextureSize = i_Data.m_ObjTextureSize;
//	m_ObjTextureAspect = i_Data.m_ObjTextureAspect;

	return *this;
}

