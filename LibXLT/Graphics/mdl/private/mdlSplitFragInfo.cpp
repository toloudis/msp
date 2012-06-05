/****************************************************************************\
**	mdlSplitFragInfo.cpp
**
**		Contains structures for single material fragments.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlSplitFragInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlSplitFragInfo::mdlSplitFragInfo()
:	m_WeldIndex(0)
{
	m_Flags.m_bCastsShadow = true;
	m_Flags.m_bReceivesShadow = true;
	m_Flags.m_bBumpMap = false;
	m_Flags.m_bShadowHull = false;
	m_Flags.m_bComponentSort = false;
	m_Flags.m_bDoubleSided = false;
}
