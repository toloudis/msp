/********************************************************************************************\
**  mtrlGlowData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlGlowData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlGlowData::mtrlGlowData()
:	m_GlowMask("Glow Mask", itString("")),
	m_GlowAmount("GlowAmount", 1),
	m_GlowScale("Glow Scale", maVector3d(1,1,1)),
	m_GlowSize("Glow Size", 1),
	m_bConstantGlow("Constant Glow", false)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlGlowData::mtrlGlowData(const mtrlGlowData& i_Data)
:	m_GlowMask("Glow Mask", itString("")),
	m_GlowAmount("GlowAmount", 1),
	m_GlowScale("Glow Scale", maVector3d(1,1,1)),
	m_GlowSize("Glow Size", 1),
	m_bConstantGlow("Constant Glow", false)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlGlowData::~mtrlGlowData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlGlowData& mtrlGlowData::operator=(const mtrlGlowData& i_Data)
{
	if (this == &i_Data) return *this;

	m_GlowAmount = i_Data.m_GlowAmount;
	m_GlowScale = i_Data.m_GlowScale;
	m_GlowSize = i_Data.m_GlowSize;
	m_bConstantGlow = i_Data.m_bConstantGlow;
	m_GlowMask = i_Data.m_GlowMask;

	return *this;
}

