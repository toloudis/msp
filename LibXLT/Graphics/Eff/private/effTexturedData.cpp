/*****************************************************************************
**	effTexturedData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/mat/matTexture.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTexturedData::effTexturedData()
:	m_Texture(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTexturedData::~effTexturedData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTexturedData::effTexturedData(const effTexturedData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTexturedData& effTexturedData::operator = (const effTexturedData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_Texture = i_CopyFrom.m_Texture;
	m_Color = i_CopyFrom.m_Color;
	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effTexturedData::HasTransparency()
{
	if( m_Color.GetAlpha() != 1.0f )
	{
		return true;
	}

	if (m_Texture)
	{
		if (m_Texture->HasTransparency())
			return true;
	}

	return false;
}
